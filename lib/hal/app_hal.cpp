#include "app_hal.h"
#include "lvgl.h"

#ifdef LV_USE_SDL
#include <unistd.h>

void hal_setup(void) { lv_sdl_window_create(480, 320); }

void hal_loop(void) {
  uint32_t ms_delay = lv_timer_handler();
  usleep(ms_delay * 1000);
}

#elif ARDUINO_M5Stack_StampS3

#include <Arduino.h>

void hal_setup(void) {}

void hal_loop(void) { delay(5); }

#endif