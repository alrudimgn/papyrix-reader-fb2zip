#pragma once

#include "I18n.h"

namespace i18n {

// clang-format off
static constexpr const char* RUSSIAN[static_cast<int>(StrId::STR__COUNT)] = {
    // Button labels
    "Назад",                  // BACK
    "Открыть",                // OPEN
    "Выбрать",                // SELECT
    "Отмена",                 // CANCEL
    "Подтвердить",            // CONFIRM
    "Готово",                 // DONE
    "Повторить",              // RETRY
    "Стоп",                   // STOP
    "Поиск",                  // SCAN
    "Запуск",                 // RUN
    "Перейти",                // GO
    "Перезапуск",             // RESTART
    "Подключить",             // CONNECT
    "Читать",                 // READ
    "Удалить",                // DELETE_BTN
    "Файлы",                  // FILE
    "Прилож.",                // APPS
    "Домой",                  // HOME
    "Да",                     // YES
    "Нет",                    // NO

    // Menu items
    "Настройки",              // SETTINGS
    "Чтение",                 // READER
    "Устройство",             // DEVICE
    "Очистка",                // CLEANUP
    "О системе",              // SYSTEM_INFO
    "Главы",                  // CHAPTERS
    "Закладки",               // BOOKMARKS
    "Сноски",                 // FOOTNOTES
    "Сеть WiFi",              // JOIN_NETWORK
    "Точка доступа",          // CREATE_HOTSPOT
    "Передача WiFi",          // WIFI_TRANSFER
    "Синхр. Calibre",         // CALIBRE_SYNC
    "Язык",                   // LANGUAGE

    // Screen titles
    "Настройки чтения",       // READER_SETTINGS
    "Настройки устройства",   // DEVICE_SETTINGS
    "Файлы",                  // FILES
    "Режим сети",             // NETWORK_MODE
    "Выбор сети",             // SELECT_NETWORK
    "Подключение",            // CONNECTING_TITLE
    "Веб-сервер",             // WEB_SERVER
    "Перейти к странице",     // GO_TO_PAGE
    "Меню",                   // MENU

    // Settings labels
    "Тема",                   // THEME
    "Размер шрифта",          // FONT_SIZE
    "Макет текста",           // TEXT_LAYOUT
    "Межстрочный интервал",   // LINE_SPACING
    "Сглаживание текста",     // TEXT_ANTI_ALIASING
    "Выравнивание",           // PARAGRAPH_ALIGNMENT
    "Переносы",               // HYPHENATION
    "Показывать картинки",    // SHOW_IMAGES
    "Строка состояния",       // STATUS_BAR
    "Ориентация чтения",      // READING_ORIENTATION
    "Автосон",                // AUTO_SLEEP_TIMEOUT
    "Экран сна",              // SLEEP_SCREEN
    "При запуске",            // STARTUP_BEHAVIOR
    "Короткая кнопка питания", // SHORT_POWER_BUTTON
    "Страниц до обновления",  // PAGES_PER_REFRESH
    "Защита от солнца",       // SUNLIGHT_FADING_FIX
    "Передние кнопки",        // FRONT_BUTTONS
    "Боковые кнопки",         // SIDE_BUTTONS
    "Обработка всей книги",   // FULL_BOOK_PROCESS

    // Settings enum values
    "Вкл",                    // ON
    "Выкл",                   // OFF
    "Очень мелкий",           // XSMALL
    "Мелкий",                 // SMALL
    "Обычный",                // NORMAL
    "Крупный",                // LARGE
    "Компактный",             // COMPACT
    "Стандартный",            // STANDARD
    "Свободный",              // RELAXED
    "По ширине",              // JUSTIFIED
    "Слева",                  // LEFT
    "По центру",              // CENTER
    "Справа",                 // RIGHT
    "Нет",                    // NONE_VAL
    "Название",               // TITLE_VAL
    "Глава",                  // CHAPTER_VAL
    "Портрет",                // PORTRAIT
    "Альбом CW",              // LANDSCAPE_CW
    "Перевернуто",            // INVERTED
    "Альбом CCW",             // LANDSCAPE_CCW
    "Темный",                 // DARK
    "Светлый",                // LIGHT
    "Свой",                   // CUSTOM
    "Обложка",                // COVER
    "Последняя книга",        // LAST_DOCUMENT
    "Игнорировать",           // IGNORE
    "Сон",                    // SLEEP_VAL
    "Листать",                // PAGE_TURN
    "Никогда",                // NEVER
    "5 мин",                  // MIN_5
    "10 мин",                 // MIN_10
    "15 мин",                 // MIN_15
    "30 мин",                 // MIN_30
    "Назад/Вперед",           // PREV_NEXT
    "Вперед/Назад",           // NEXT_PREV
    "B/C/L/R",                // FRONT_BCLR
    "L/R/B/C",                // FRONT_LRBC

    // Cleanup
    "Очистить кэш книг",      // CLEAR_BOOK_CACHE
    "Очистить память",        // CLEAR_DEVICE_STORAGE
    "Сброс настроек",         // FACTORY_RESET
    "Очистить кэш?",          // CLEAR_CACHES_Q
    "Будет удален кэш книг",  // CLEAR_CACHES_MSG1
    "и прогресс чтения.",     // CLEAR_CACHES_MSG2
    "Очистить устройство?",   // CLEAR_DEVICE_Q
    "Будет очищена внутренняя", // CLEAR_DEVICE_MSG1
    "память. Устройство перезапустится.", // CLEAR_DEVICE_MSG2
    "Сбросить настройки?",    // FACTORY_RESET_Q
    "Будут удалены все данные,", // FACTORY_RESET_MSG1
    "настройки и пароли WiFi!", // FACTORY_RESET_MSG2
    "Очистка кэша...",        // CLEARING_CACHE
    "Кэш очищен",             // CACHE_CLEARED
    "Кэш не найден",          // NO_CACHE_TO_CLEAR
    "Очистка памяти...",      // CLEARING_STORAGE
    "Сброс устройства...",    // RESETTING_DEVICE
    "Готово. Перезапуск...",  // DONE_RESTARTING

    // Status messages
    "Поиск...",               // SCANNING
    "Подключение...",         // CONNECTING
    "Получение IP...",        // GETTING_IP
    "Подключено!",            // CONNECTED
    "Запуск...",              // STARTING
    "Загрузка...",            // LOADING
    "Индексация...",          // INDEXING
    "Открытие книги...",      // OPENING_BOOK
    "Удаление...",            // DELETING
    "Книга не открыта",       // NO_BOOK_OPEN
    "Нажмите \"Файлы\" для выбора", // PRESS_FILE_TO_EXPLORE
    "Книги не найдены",       // NO_BOOKS_FOUND
    "Конец книги",            // END_OF_BOOK
    "Не удалось загрузить страницу", // FAILED_TO_LOAD_PAGE
    "Нельзя удалить открытую книгу", // CANNOT_DELETE_ACTIVE
    "Удалено",                // DELETED
    "Ошибка удаления",        // DELETE_FAILED
    "Сервер остановлен",      // SERVER_STOPPED
    "Возврат в библиотеку...", // RETURNING_TO_LIBRARY
    "СОН",                    // SLEEPING

    // Network
    "Подключиться к WiFi",    // CONNECT_WIFI
    "Создать точку WiFi",     // CREATE_WIFI_HOTSPOT
    "Сети не найдены",        // NO_NETWORKS_FOUND
    "Confirm: повторить поиск", // PRESS_CONFIRM_SCAN
    "Отключено. Перезапуск?", // DISCONNECTED_RESTART

    // Calibre
    "Ожидание Calibre...",    // WAITING_FOR_CALIBRE
    "Подключение к Calibre...", // CONNECTING_TO_CALIBRE
    "В Calibre: Подключение/общий доступ > Беспроводное устройство", // CALIBRE_HELP

    // Format strings
    "IP: %s",                 // FMT_IP
    "Получено книг: %d",      // FMT_RECEIVED_BOOKS
    "из %d",                  // FMT_PAGE_OF

    // System info
    "Версия",                 // VERSION
    "Время работы",           // UPTIME
    "Батарея",                // BATTERY
    "Чип",                    // CHIP
    "CPU",                    // CPU
    "Свободная память",       // FREE_MEMORY
    "Внутренний диск",        // INTERNAL_DISK
    "SD-карта",               // SD_CARD
    "Готово",                 // READY
    "Недоступно",             // NOT_AVAILABLE

    // Keyboard
    "Стереть",                // BACKSPACE
    "Пробел",                 // SPACE

    // Error
    "Ошибка",                 // ERROR
    "Нажмите любую кнопку",   // PRESS_ANY_BUTTON

    // Confirm dialog
    "Подтвердите удаление",   // CONFIRM_DELETE

    // Misc
    "Введите текст",          // ENTER_TEXT
    "Нет обложки",            // NO_COVER
    "Papyrix",                // PAPYRIX
    "Добавить",               // ADD
    "Закладок пока нет",      // NO_BOOKMARKS
    "На этой странице нет сносок", // NO_FOOTNOTES
    "[ссылка]",               // LINK
    "Книги",                  // BOOKS
    "Удалить этот файл?",     // DELETE_FILE_Q
    "Удалить эту папку?",     // DELETE_FOLDER_Q
    "Введите пароль",         // ENTER_PASSWORD
    "Сохранить пароль?",      // SAVE_PASSWORD_Q
    "Сохранить пароль этой сети?", // SAVE_PASSWORD_MSG
    "Ошибка подключения",     // CONNECTION_FAILED
    "Не удалось создать точку", // HOTSPOT_FAILED
    "Инициализация WiFi...",  // INITIALIZING_WIFI
    "Неверный файл",          // INVALID_FILE
    "Ошибка памяти",          // MEMORY_ERROR
    "Ошибка загрузки страницы", // PAGE_LOAD_ERROR
    "Прием...",               // RECEIVING
    "ЗАГРУЗКА",               // BOOTING

    // Firmware Update (from SD card)
    "Обновление прошивки",    // FIRMWARE_UPDATE
    "Не выключайте питание и не извлекайте SD-карту во время обновления", // FIRMWARE_WARNING
    "Найдено",                // FIRMWARE_FILE_FOUND
    "Прошивка...",            // FLASHING_UPDATE
    "Обновление завершено! Перезапуск...", // UPDATE_COMPLETE
    "Ошибка обновления",      // UPDATE_FAILED
    "Файл прошивки на SD-карте не найден", // NO_FIRMWARE_FILE
    "Проверка...",            // VALIDATING_FIRMWARE
    "Закладка",               // BOOKMARK_VAL
    "Закладка добавлена",     // BOOKMARK_ADDED
    "Недавняя",               // RECENT_NETWORK
};
// clang-format on

static_assert(sizeof(RUSSIAN) / sizeof(RUSSIAN[0]) == static_cast<int>(StrId::STR__COUNT),
              "RUSSIAN array size must match StrId::STR__COUNT");

}  // namespace i18n
