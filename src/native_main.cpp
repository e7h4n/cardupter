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
#include <SDL2/SDL.h>

void on_fava_timer() {
  const auto &stat = getFavaLedgerStat();
  set_fava_stat(stat);
}

int main() {
  lv_init();

  hal_setup();

  ui_begin();

  on_fava_timer();
  lv_timer_create([](lv_timer_t *timer) { on_fava_timer(); }, 1000 * 60,
                  nullptr);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  while (true) {
    lv_timer_handler();
    SDL_Delay(10);
  }
#pragma clang diagnostic pop
}
