#include "Fb2Provider.h"

#include <FsHelpers.h>
#include <Logging.h>
#include <SDCardManager.h>
#include <Utf8.h>
#include <ZipFile.h>

#include <cstring>
#include <functional>
#include <string>

#define TAG "FB2PROV"

namespace papyrix {

namespace {

struct PreparedFb2Path {
  std::string actualPath;
  std::string forcedCachePath;
  std::string originalPath;
};

Result<PreparedFb2Path> prepareFb2Path(const char* path, const char* cacheDir) {
  PreparedFb2Path prepared;
  prepared.actualPath = path ? path : "";
  prepared.originalPath = path ? path : "";

  if (!FsHelpers::isZipFile(prepared.actualPath)) {
    return Ok(std::move(prepared));
  }

  ZipFile zip(prepared.originalPath);
  if (!zip.loadAllFileStatSlims()) {
    LOG_ERR(TAG, "Failed to read ZIP central directory: %s", prepared.originalPath.c_str());
    return Err<PreparedFb2Path>(Error::ParseFailed);
  }

  std::string fb2EntryName;
  uint32_t fb2EntrySize = 0;
  uint32_t fb2EntryCrc32 = 0;
  const auto& entries = zip.getFileStatSlimCache();
  for (const auto& entry : entries) {
    if (FsHelpers::isFb2File(entry.first) && (fb2EntryName.empty() || entry.first < fb2EntryName)) {
      fb2EntryName = entry.first;
      fb2EntrySize = entry.second.uncompressedSize;
      fb2EntryCrc32 = entry.second.crc32;
    }
  }

  if (fb2EntryName.empty()) {
    LOG_ERR(TAG, "No FB2 entry found in ZIP: %s", prepared.originalPath.c_str());
    return Err<PreparedFb2Path>(Error::ParseFailed);
  }

  const auto zipHash = std::hash<std::string>{}(prepared.originalPath);
  prepared.forcedCachePath = std::string(cacheDir) + "/fb2_zip_" + std::to_string(zipHash);
  prepared.actualPath = prepared.forcedCachePath + "/source.fb2";
  const std::string tempPath = prepared.actualPath + ".tmp";
  const std::string markerPath = prepared.actualPath + ".ok";
  const std::string expectedMarker =
      fb2EntryName + "\n" + std::to_string(fb2EntrySize) + "\n" + std::to_string(fb2EntryCrc32) + "\n";

  FsFile existingFile;
  if (SdMan.openFileForRead("FB2ZIP", prepared.actualPath, existingFile)) {
    bool markerMatches = false;
    FsFile markerFile;
    if (SdMan.openFileForRead("FB2ZIP", markerPath, markerFile)) {
      std::string marker;
      marker.resize(markerFile.size());
      if (!marker.empty()) {
        markerFile.read(reinterpret_cast<uint8_t*>(&marker[0]), marker.size());
      }
      markerFile.close();
      markerMatches = marker == expectedMarker;
    }
    const bool complete = existingFile.size() == fb2EntrySize && markerMatches;
    existingFile.close();
    if (complete) {
      return Ok(std::move(prepared));
    }
    SdMan.remove(prepared.actualPath.c_str());
    SdMan.remove(markerPath.c_str());
  }

  if (!SdMan.ensureDirectoryExists(prepared.forcedCachePath.c_str())) {
    LOG_ERR(TAG, "Failed to create ZIP FB2 cache dir: %s", prepared.forcedCachePath.c_str());
    return Err<PreparedFb2Path>(Error::IOError);
  }

  SdMan.remove(tempPath.c_str());

  FsFile outFile;
  if (!SdMan.openFileForWrite("FB2ZIP", tempPath, outFile)) {
    LOG_ERR(TAG, "Failed to create temporary extracted FB2: %s", tempPath.c_str());
    return Err<PreparedFb2Path>(Error::IOError);
  }

  constexpr size_t kZipExtractChunkSize = 4096;
  const bool extracted = zip.readFileToStream(fb2EntryName.c_str(), outFile, kZipExtractChunkSize);
  outFile.close();

  if (!extracted) {
    SdMan.remove(tempPath.c_str());
    LOG_ERR(TAG, "Failed to extract FB2 entry '%s' from ZIP: %s", fb2EntryName.c_str(), prepared.originalPath.c_str());
    return Err<PreparedFb2Path>(Error::ParseFailed);
  }

  FsFile tmpFile;
  if (!SdMan.openFileForRead("FB2ZIP", tempPath, tmpFile)) {
    SdMan.remove(tempPath.c_str());
    LOG_ERR(TAG, "Failed to reopen temporary extracted FB2: %s", tempPath.c_str());
    return Err<PreparedFb2Path>(Error::IOError);
  }
  const bool complete = tmpFile.size() == fb2EntrySize;
  tmpFile.close();
  if (!complete) {
    SdMan.remove(tempPath.c_str());
    LOG_ERR(TAG, "Extracted FB2 size mismatch for '%s'", fb2EntryName.c_str());
    return Err<PreparedFb2Path>(Error::ParseFailed);
  }

  SdMan.remove(prepared.actualPath.c_str());
  SdMan.remove(markerPath.c_str());
  if (!SdMan.rename(tempPath.c_str(), prepared.actualPath.c_str())) {
    SdMan.remove(tempPath.c_str());
    LOG_ERR(TAG, "Failed to move extracted FB2 into place: %s", prepared.actualPath.c_str());
    return Err<PreparedFb2Path>(Error::IOError);
  }

  FsFile markerFile;
  if (!SdMan.openFileForWrite("FB2ZIP", markerPath, markerFile)) {
    SdMan.remove(prepared.actualPath.c_str());
    LOG_ERR(TAG, "Failed to create extracted FB2 marker: %s", markerPath.c_str());
    return Err<PreparedFb2Path>(Error::IOError);
  }
  const size_t markerWritten =
      markerFile.write(reinterpret_cast<const uint8_t*>(expectedMarker.data()), expectedMarker.size());
  markerFile.close();
  if (markerWritten != expectedMarker.size()) {
    SdMan.remove(prepared.actualPath.c_str());
    SdMan.remove(markerPath.c_str());
    LOG_ERR(TAG, "Failed to write extracted FB2 marker: %s", markerPath.c_str());
    return Err<PreparedFb2Path>(Error::IOError);
  }

  LOG_INF(TAG, "Extracted FB2 entry '%s' to %s", fb2EntryName.c_str(), prepared.actualPath.c_str());
  return Ok(std::move(prepared));
}

}  // namespace

Result<void> Fb2Provider::open(const char* path, const char* cacheDir) {
  close();

  TRY_VAL(prepared, prepareFb2Path(path, cacheDir));

  fb2.reset(new Fb2(prepared.actualPath, cacheDir, prepared.forcedCachePath, prepared.originalPath));

  if (!fb2->load()) {
    fb2.reset();
    return ErrVoid(Error::ParseFailed);
  }

  // Populate metadata
  meta.clear();
  meta.type = ContentType::Fb2;

  const std::string& title = fb2->getTitle();
  utf8SafeCopy(meta.title, sizeof(meta.title), title.c_str());

  const std::string& author = fb2->getAuthor();
  utf8SafeCopy(meta.author, sizeof(meta.author), author.c_str());

  const std::string& cachePath = fb2->getCachePath();
  strncpy(meta.cachePath, cachePath.c_str(), sizeof(meta.cachePath) - 1);
  meta.cachePath[sizeof(meta.cachePath) - 1] = '\0';

  // Cover path
  std::string coverPath = fb2->getCoverBmpPath();
  strncpy(meta.coverPath, coverPath.c_str(), sizeof(meta.coverPath) - 1);
  meta.coverPath[sizeof(meta.coverPath) - 1] = '\0';

  // FB2 uses file size, not pages (pages calculated during rendering)
  meta.totalPages = 1;  // Will be updated by reader
  meta.currentPage = 0;
  meta.progressPercent = 0;

  return Ok();
}

void Fb2Provider::close() {
  fb2.reset();
  meta.clear();
}

uint32_t Fb2Provider::pageCount() const {
  if (!fb2) return 0;

  // Estimate pages based on file size
  constexpr size_t BYTES_PER_PAGE = 2048;
  size_t fileSize = fb2->getFileSize();
  return (fileSize + BYTES_PER_PAGE - 1) / BYTES_PER_PAGE;
}

uint16_t Fb2Provider::tocCount() const {
  if (!fb2) return 0;
  return fb2->tocCount();
}

Result<TocEntry> Fb2Provider::getTocEntry(uint16_t index) const {
  if (!fb2 || index >= fb2->tocCount()) {
    return Err<TocEntry>(Error::InvalidState);
  }

  const Fb2::TocItem item = fb2->getTocItem(index);

  TocEntry entry;
  utf8SafeCopy(entry.title, sizeof(entry.title), item.title.c_str());
  entry.pageIndex = item.sectionIndex;
  entry.depth = 0;

  return Ok(entry);
}

int Fb2Provider::getSectionCount() const {
  if (!fb2) return 0;
  return fb2->getSectionCount();
}

std::string Fb2Provider::getSectionPath(int sectionIndex) const {
  if (!fb2) return "";
  return fb2->getSectionPath(sectionIndex);
}

std::string Fb2Provider::getSectionCachePath(int sectionIndex) const {
  if (!fb2) return "";
  return fb2->getCachePath() + "/pages_" + std::to_string(sectionIndex) + ".bin";
}

int Fb2Provider::getSectionForTocEntry(int tocIndex) const {
  if (!fb2) return 0;
  return fb2->getSectionForTocEntry(tocIndex);
}

}  // namespace papyrix
