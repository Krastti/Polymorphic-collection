#include "web_interface.h"
#include "../Логирование/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <windows.h>
    #define PATH_SEPARATOR "\\"
#else
    #include <unistd.h>
    #define PATH_SEPARATOR "/"
#endif

// Проверка существования файла
static bool file_exists(const char* filename)
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// Получение абсолютного пути к файлу
static bool get_absolute_path(const char* relative_path, char* absolute_path, size_t size)
{
#ifdef _WIN32
    if (_fullpath(absolute_path, relative_path, size) != NULL) {
        return true;
    }
#else
    if (realpath(relative_path, absolute_path) != NULL) {
        strncpy(absolute_path, absolute_path, size - 1);
        absolute_path[size - 1] = '\0';
        return true;
    }
#endif

    strncpy(absolute_path, relative_path, size - 1);
    absolute_path[size - 1] = '\0';
    return false;
}

bool web_interface_open(int element_type)
{
    LOG_INFO("Открытие веб-интерфейса, тип элементов: %d", element_type);

    // Выбираем правильный файл в зависимости от типа
    const char* html_file = (element_type == 2)
        ? HTML_FILENAME_COMPLEX
        : HTML_FILENAME_REAL;

    const char* type_name = (element_type == 2)
        ? "Комплексные"
        : "Вещественные";

    // Проверяем существование HTML файла
    if (!file_exists(html_file)) {
        LOG_ERROR("HTML файл не найден: %s", html_file);
        fprintf(stderr, "[ОШИБКА] Файл %s не найден!\n", html_file);
        fprintf(stderr, "[ИНФО] Убедитесь, что файл существует в папке WebInterface/\n");
        return false;
    }

    LOG_DEBUG("HTML файл найден: %s", html_file);

    // Получаем абсолютный путь
    char absolute_path[1024];
    get_absolute_path(html_file, absolute_path, sizeof(absolute_path));

    LOG_DEBUG("Абсолютный путь: %s", absolute_path);

    // Формируем URL
    char url[2048];
    snprintf(url, sizeof(url), "file:///%s", absolute_path);

    // Заменяем обратные слеши на прямые (для Windows)
#ifdef _WIN32
    for (char* p = url; *p; p++) {
        if (*p == '\\') {
            *p = '/';
        }
    }
#endif

    LOG_INFO("Открытие URL: %s", url);

    // Открываем в браузере
#ifdef _WIN32
    HINSTANCE result = ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);

    if ((INT_PTR)result <= 32) {
        LOG_WARN("Не удалось открыть браузер автоматически (код: %d)", (int)(INT_PTR)result);
        printf("[ПРЕДУПРЕЖДЕНИЕ] Не удалось автоматически открыть браузер\n");
        printf("[ИНФО] Откройте файл вручную: %s\n", absolute_path);
        return false;
    }
#else
    #ifdef __APPLE__
        char command[2048];
        snprintf(command, sizeof(command), "open \"%s\"", url);
    #else
        char command[2048];
        snprintf(command, sizeof(command), "xdg-open \"%s\"", url);
    #endif

    int result = system(command);

    if (result != 0) {
        LOG_WARN("Не удалось открыть браузер автоматически (код: %d)", result);
        printf("[ПРЕДУПРЕЖДЕНИЕ] Не удалось автоматически открыть браузер\n");
        printf("[ИНФО] Откройте файл вручную: %s\n", absolute_path);
        return false;
    }
#endif

    LOG_INFO("Веб-интерфейс успешно открыт в браузере (%s числа)", type_name);
    printf("[ИНФО] Веб-интерфейс открыт в браузере\n");
    printf("[ИНФО] Тип элементов: %s\n", type_name);
    printf("[ИНФО] Файл: %s\n", html_file);

    return true;
}

void web_interface_cleanup(void)
{
    LOG_DEBUG("Очистка ресурсов веб-интерфейса");
}