#include "gui.h"
#include "fava.h"
#include "lvgl.h"
#include "number.h"

static void setup_fava_ui(lv_obj_t *cont);

static lv_obj_t *setup_tile_view();

static lv_obj_t *lv_tile_view;
static lv_obj_t *remains_label;

void ui_begin() {
  lv_tile_view = setup_tile_view();

  auto page = lv_tileview_add_tile(lv_tile_view, 0, 0, LV_DIR_VER);
  setup_fava_ui(page);
}

void set_fava_stat(const FavaLedgerStat &stat) {
  lv_label_set_text_fmt(
      remains_label, LV_SYMBOL_DRIVE " %s",
      thousandSeparator(stat.cashBalance + stat.creditCardBalance, 2).c_str());
}

static void setup_fava_ui(lv_obj_t *cont) {
  remains_label = lv_label_create(cont);
  lv_label_set_text(remains_label, "Remains");
  lv_obj_set_style_text_font(remains_label, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_align(remains_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(remains_label, LV_ALIGN_CENTER, 0, 0);

  lv_obj_add_event_cb(
      remains_label, [](lv_event_t *e) { printf("got clicked\n"); },
      LV_EVENT_PRESSED, nullptr);
}

static lv_obj_t *setup_tile_view() {
  auto tv = lv_tileview_create(lv_scr_act());
  lv_obj_align(tv, LV_ALIGN_TOP_RIGHT, 0, 0);
  lv_obj_set_size(tv, LV_PCT(100), LV_PCT(100));
  lv_obj_remove_style(tv, 0, LV_PART_SCROLLBAR);

  return tv;
}