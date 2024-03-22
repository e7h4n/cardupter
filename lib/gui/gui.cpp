#include "gui.h"
#include "fava.h"
#include "lvgl.h"
#include "number.h"

static void setup_fava_ui(lv_obj_t *cont);

static lv_obj_t *setup_tile_view();

static lv_obj_t *lv_tile_view;
static lv_obj_t *remains_label;
static lv_obj_t *xirr_label;

void ui_begin() {
  lv_tile_view = setup_tile_view();

  auto page = lv_tileview_add_tile(lv_tile_view, 0, 0, LV_DIR_VER);
  setup_fava_ui(page);
}

void set_fava_stat(const FavaLedgerStat &stat) {
  lv_label_set_text_fmt(
      remains_label, LV_SYMBOL_HOME " %s",
      thousandSeparator(stat.cashBalance + stat.creditCardBalance, 2).c_str());

  lv_label_set_text_fmt(xirr_label, LV_SYMBOL_CHARGE " %.2f%%",
                        stat.xirr * 100);
}

static void setup_fava_ui(lv_obj_t *cont) {
  static int32_t column_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1),
                                 LV_GRID_TEMPLATE_LAST};
  static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
                              LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

  lv_obj_set_layout(cont, LV_LAYOUT_GRID);
  lv_obj_set_grid_dsc_array(cont, column_dsc, row_dsc);

  xirr_label = lv_label_create(cont);
  lv_obj_set_grid_cell(xirr_label, LV_GRID_ALIGN_CENTER, 0, 2,
                       LV_GRID_ALIGN_CENTER, 0, 1);
  lv_label_set_text(xirr_label, "XIRR");
  lv_obj_set_style_margin_top(xirr_label, 10, 0);
  lv_obj_set_style_text_align(xirr_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_font(xirr_label, &lv_font_montserrat_24, 0);

  remains_label = lv_label_create(cont);
  lv_obj_set_grid_cell(remains_label, LV_GRID_ALIGN_CENTER, 0, 2,
                       LV_GRID_ALIGN_CENTER, 1, 3);
  lv_label_set_text(remains_label, "Remains");
  lv_obj_set_style_text_align(remains_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_font(remains_label, &lv_font_montserrat_40, 0);

  lv_obj_add_event_cb(
      cont, [](lv_event_t *e) { printf("got clicked\n"); }, LV_EVENT_CLICKED,
      nullptr);
}

static lv_obj_t *setup_tile_view() {
  auto tv = lv_tileview_create(lv_scr_act());
  lv_obj_align(tv, LV_ALIGN_TOP_RIGHT, 0, 0);
  lv_obj_set_size(tv, LV_PCT(100), LV_PCT(100));
  lv_obj_remove_style(tv, 0, LV_PART_SCROLLBAR);

  return tv;
}