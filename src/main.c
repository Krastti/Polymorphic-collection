#include "../tests/unit_tests.h"
#include "logging/logger.h"

#ifdef _WIN32
  #include <windows.h>
#endif

int main() {
    #ifdef _WIN32
      SetConsoleOutputCP(CP_UTF8);
    #endif

    logger_set_level(LOG_DEBUG);
    LOG_DEBUG("Программа запущена");
    logger_set_level(LOG_ERROR);
    run_all_tests();
    return 0;
}
