# Localization

Papyrix includes English and Russian UI strings compiled into firmware. The active UI language can be selected on the device, and a custom locale file can still be placed on the SD card or uploaded via the web interface to override built-in strings.

## How it works

1. Built-in English and Russian strings are compiled into firmware (Flash, zero RAM cost)
2. At boot, the saved UI language selects the built-in string table
3. If `/.papyrix/locale.txt` exists on SD, it overrides the selected built-in language in RAM
4. All `tr()` calls resolve to a single pointer dereference - no SD access after boot

## Built-in language selection

On the device, open **Settings > Device > Language** and select **English** or **Русский**. The UI language is applied immediately and saved in the device settings.

The custom `/.papyrix/locale.txt` file remains an overlay. If it is installed, only keys present in that file override the selected built-in language; missing keys fall back to the selected built-in language.

## File format

Place a locale file at `/.papyrix/locale.txt` on the SD card.

```
# Comment lines start with #
_language_name=Francais

BACK=Retour
OPEN=Ouvrir
SETTINGS=Parametres
LOADING=Chargement...
```

- Keys match the StrId enum names exactly (case-sensitive)
- Missing keys fall back to the selected built-in language
- Unknown keys are silently ignored
- Keys starting with `_` are reserved for metadata (see below)
- Comments: lines starting with `#` or `;`
- UTF-8 encoding
- Max line length: 255 characters (longer lines are truncated)
- Override buffer: 4096 bytes total for all translated strings combined

### Metadata keys

Keys starting with `_` are not loaded into the translation table but may be used by the system:

- `_language_name` - displayed in the web interface as the current language name

## Managing translations

### Via SD card

1. Copy one of the examples from `docs/examples/locale/` to `/.papyrix/locale.txt` on your SD card
2. Replace values with your translations
3. Restart the device, or switch the UI language on the device to reload the locale overlay

### Via web interface

1. Connect to the device via WiFi (Join Network or Create Hotspot)
2. Open the web interface and go to the **Locale** tab
3. Upload a `.txt` locale file
4. Restart the device, or switch the UI language on the device to reload the locale overlay

The web interface shows the current language name (from `_language_name`) and file size, and allows deleting the locale file to revert to built-in translations.

See `docs/examples/locale/` for complete examples (en, de, fr, es, uk).

## String keys reference

See `lib/I18n/src/I18nDefaults.h` for the complete list of all string keys and their English defaults. The built-in Russian table is in `lib/I18n/src/I18nRussian.h`.

Format strings (keys starting with `FMT_`) contain `%s` or `%d` placeholders that must be preserved in translations:

- `FMT_IP` - `IP: %s` (%s = IP address)
- `FMT_RECEIVED_BOOKS` - `Received %d book(s)` (%d = book count)
- `FMT_PAGE_OF` - `of %d` (%d = total pages)
