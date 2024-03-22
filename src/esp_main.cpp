
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "gui.h"
#include <Arduino.h>
#include <driver/spi_common.h>
#include <drivers/display/st7789/lv_st7789.h>
#include <lvgl.h>

#define LCD_MOSI_PIN 35
#define LCD_MISO_PIN -1
#define LCD_SCLK_PIN 36
#define LCD_DC_PIN 34
#define LCD_CS_PIN 37
#define LCD_RST_PIN 33
#define LCD_BL_PIN 38

/* 向LCD发送短命令。此函数将等待事务完成。 */
void esp_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                      const uint8_t *param, size_t param_size) {}

/* 向LCD发送大量像素数据。如果需要，此函数必须进行字节交换。此函数可以在后台进行传输。
 */
void esp_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                        uint8_t *param, size_t param_size) {}

esp_lcd_panel_handle_t setup_lcd() {
  gpio_config_t back_light_gpio_config = {
      .pin_bit_mask = 1ULL << LCD_BL_PIN,
      .mode = GPIO_MODE_OUTPUT,
  };
  ESP_ERROR_CHECK(gpio_config(&back_light_gpio_config));

  spi_bus_config_t bus_cfg = {
      .mosi_io_num = LCD_MOSI_PIN,
      .miso_io_num = LCD_MISO_PIN,
      .sclk_io_num = LCD_SCLK_PIN,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 135 * 80 * sizeof(uint16_t),
  };

  ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

  esp_lcd_panel_io_handle_t io_handle = nullptr;
  esp_lcd_panel_io_spi_config_t io_config = {
      .cs_gpio_num = LCD_CS_PIN,
      .dc_gpio_num = LCD_DC_PIN,
      .spi_mode = 0,
      .pclk_hz = (20 * 1000 * 1000),
      .trans_queue_depth = 10,
      .lcd_cmd_bits = 8,
      .lcd_param_bits = 8,
  };
  // Attach the LCD to the SPI bus
  ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST,
                                           &io_config, &io_handle));

  esp_lcd_panel_handle_t panel_handle = nullptr;
  esp_lcd_panel_dev_config_t panel_config = {
      .reset_gpio_num = LCD_RST_PIN,
      .color_space = ESP_LCD_COLOR_SPACE_RGB,
      .bits_per_pixel = 16,
  };
  // Create LCD panel handle for ST7789, with the SPI IO device handle
  ESP_ERROR_CHECK(
      esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

  return panel_handle;
}

void setup() {
  lv_init();

  lv_st7789_create(240, 135, LV_LCD_FLAG_NONE, esp_lcd_send_cmd,
                   esp_lcd_send_color);

  ui_begin();

  set_fava_stat(FavaLedgerStat{
      .creditCardBalance = 100.0, .cashBalance = 1000.0, .xirr = 0.23});
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(1);
}