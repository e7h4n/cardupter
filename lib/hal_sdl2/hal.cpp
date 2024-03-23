#include <SDL2/SDL.h>
#include "hal.h"
#include "lvgl.h"


void hal_setup(void) {
    lv_group_set_default(lv_group_create());
    auto display = lv_sdl_window_create(240, 135);

    auto mouse = lv_sdl_mouse_create();
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_display(mouse, display);
    lv_display_set_default(display);

    LV_IMAGE_DECLARE(mouse_cursor_icon);
    auto cursor_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(mouse, cursor_obj);

    auto mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_display(mousewheel, display);

    auto keyboard = lv_sdl_keyboard_create();
    lv_indev_set_display(keyboard, display);
    lv_indev_set_group(keyboard, lv_group_get_default());
}

[[noreturn]] void hal_loop(void) {
    lv_tick_inc(3);
    while (true) {
        lv_timer_handler();
        SDL_Delay(10);
    }
}

