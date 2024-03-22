
#include "gui.h"
#include <Arduino.h>
#include <drivers/display/st7789/lv_st7789.h>
#include <lvgl.h>

#define LCD_MOSI_PIN 35
#define LCD_MISO_PIN -1
#define LCD_SCLK_PIN 36
#define LCD_DC_PIN 34
#define LCD_CS_PIN 37
#define LCD_RST_PIN 33
#define LCD_BUSY_PIN -1
#define LCD_BL_PIN 38

/* 向LCD发送短命令。此函数将等待事务完成。 */
void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                     const uint8_t *param, size_t param_size) {}

/* 向LCD发送大量像素数据。如果需要，此函数必须进行字节交换。此函数可以在后台进行传输。
 */
void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                       uint8_t *param, size_t param_size) {}

void setup() {
  lv_init();

  lv_st7789_create(240, 135, LV_LCD_FLAG_NONE, my_lcd_send_cmd,
                   my_lcd_send_color);

  ui_begin();
  ui_switch_page();
  set_fava_stat(FavaLedgerStat{
      .creditCardBalance = 100.0, .cashBalance = 1000.0, .xirr = 0.23});
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(1);
}