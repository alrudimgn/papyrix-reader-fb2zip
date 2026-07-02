#pragma once

#include <cstring>

constexpr int FOOTNOTE_NUMBER_LEN = 32;
constexpr int FOOTNOTE_HREF_LEN = 96;

struct FootnoteEntry {
  char number[FOOTNOTE_NUMBER_LEN];
  char href[FOOTNOTE_HREF_LEN];

  FootnoteEntry() {
    number[0] = '\0';
    href[0] = '\0';
  }
};
