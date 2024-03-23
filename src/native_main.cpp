#include "hal.h"
#include "fava.h"
#include "gui.h"
#include "lvgl.h"

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

    hal_loop();
}
