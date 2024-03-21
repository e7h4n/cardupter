#include <string>
#include "gui.h"
#include "lvgl.h"
#include "fava.h"
#include "number.h"

static lv_point_t line_points[] = {{-320, 0},
                                   {320,  0}};

static void setup_fava_ui(lv_obj_t *tv);

static lv_obj_t *setup_tileview(void);

static lv_obj_t *lv_tileview;
static lv_obj_t *remains_label;

void ui_switch_page(void) {
    static uint8_t n;
    n++;
    lv_obj_set_tile_id(lv_tileview, 0, n % UI_PAGE_COUNT, LV_ANIM_ON);
}

void ui_begin() {
    lv_tileview = setup_tileview();

    lv_obj_t *page1 = lv_tileview_add_tile(lv_tileview, 0, 0, LV_DIR_VER);
    setup_fava_ui(page1);
}

void set_fava_stat(const FavaLedgerStat &stat) {
    lv_label_set_text_fmt(remains_label, LV_SYMBOL_DRIVE " %s",
                          thousandSeparator(stat.cashBalance + stat.creditCardBalance, 2).c_str());
}

static void setup_fava_ui(lv_obj_t *cont) {
    remains_label = lv_label_create(cont);
    lv_label_set_text(remains_label, "Reimains");
    lv_obj_set_style_text_font(remains_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_align(remains_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(remains_label, LV_ALIGN_CENTER, 0, 0);
}

static lv_obj_t *setup_tileview() {
    lv_obj_t *lv_tileview = lv_tileview_create(lv_scr_act());
    lv_obj_align(lv_tileview, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_size(lv_tileview, LV_PCT(100), LV_PCT(100));
    lv_obj_remove_style(lv_tileview, 0, LV_PART_SCROLLBAR);

    return lv_tileview;
}