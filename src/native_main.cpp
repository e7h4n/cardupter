/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include "app_hal.h"
#include "fava.h"
#include "gui.h"
#include "lvgl.h"

int main() {
  lv_init();

  hal_setup();

  ui_begin();

  // 用 lv 设置一个定时器，100ms 后执行
  lv_timer_create(
      [](lv_timer_t *timer) {
        const auto &stat = getFavaLedgerStat();
        set_fava_stat(stat);
      },
      1000, nullptr);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  while (true) {
    hal_loop();
  }
#pragma clang diagnostic pop
}
