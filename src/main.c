#include "../tests/unit_tests.h"
#include "logging/logger.h"
#include <stdio.h>

int main() {
    logger_init(LOG_DEBUG);
    LOG_DEBUG("Программа запущена");

    printf("Включить тесты");
    int value;
    scanf("%d",&value);
    if (value == 1) {
      logger_set_level(LOG_ERROR);
      run_all_tests();
    }
    return 0;
}
