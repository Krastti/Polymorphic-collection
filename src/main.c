#include "../tests/unit_tests.h"
#include "logging/logger.h"

int main() {
    logger_init(LOG_DEBUG);
    LOG_DEBUG("Программа запущена");

    logger_set_level(LOG_ERROR);
    run_all_tests();
    return 0;
}
