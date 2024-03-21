#include "app_hal.h"

#ifdef USE_SDL

#include <SDL2/SDL.h>
#include "sdl/sdl.h"


/**
 * A task to measure the elapsed time for LittlevGL
 * @param data unused
 * @return never return
 */
_Noreturn static int tick_thread(void *data) {
    (void) data;

    while (1) {
        SDL_Delay(5);
        lv_tick_inc(5);
    }
}


void hal_setup(void) {
    /* Add a display
     * Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/

    static lv_disp_draw_buf_t disp_buf;
    static lv_color_t buf[SDL_HOR_RES * 10];                          /*Declare a buffer for 10 lines*/
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, SDL_HOR_RES * 10);    /*Initialize the display buffer*/

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = sdl_display_flush;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.hor_res = SDL_HOR_RES;
    disp_drv.ver_res = SDL_VER_RES;
    lv_disp_drv_register(&disp_drv);

    /* Add the mouse as input device
     * Use the 'mouse' driver which reads the PC's mouse*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);            /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = sdl_mouse_read;       /*This function will be called periodically (by the library) to get the mouse position and state*/
    lv_indev_drv_register(&indev_drv);

    sdl_init();

    /* Tick init.
     * You have to call 'lv_tick_inc()' in periodically to inform LittelvGL about how much time were elapsed
     * Create an SDL thread to do this*/
    SDL_CreateThread(tick_thread, "tick", NULL);
}

_Noreturn void hal_loop(void) {
    while (1) {
        SDL_Delay(5);
        lv_task_handler();
    }
}

#elif ARDUINO_M5Stack_StampS3

#include <Arduino.h>

void hal_setup(void) {
}

void hal_loop(void) {
    delay(5);
}

#endif