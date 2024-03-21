static const char *TAG = "main";

#include "OneButton.h"
#include "lvgl.h"

#include "Arduino.h"
#include "WiFi.h"
#include "Wire.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "gui.h"
#include "pin_config.h"
#include "time.h"
#include "fava.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <esp_wifi.h>
#include "number.h"

#define CUSTOM_TIMEZONE "CST-8"

esp_lcd_panel_io_handle_t io_handle = NULL;
static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_color_t *lv_disp_buf;
static bool is_initialized_lvgl = false;
OneButton button1(PIN_BUTTON_1, true);
OneButton button2(PIN_BUTTON_2, true);

typedef struct {
    uint8_t cmd;
    uint8_t data[14];
    uint8_t len;
} lcd_cmd_t;

static const lcd_cmd_t LCD_ST7789V_INIT_COMMANDS[] = {
        {0x11, {0},                                                                                  0 | 0x80},
        {0x3A, {0X05},                                                                               1},
        {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33},                                                       5},
        {0xB7, {0X75},                                                                               1},
        {0xBB, {0X28},                                                                               1},
        {0xC0, {0X2C},                                                                               1},
        {0xC2, {0X01},                                                                               1},
        {0xC3, {0X1F},                                                                               1},
        {0xC6, {0X13},                                                                               1},
        {0xD0, {0XA7},                                                                               1},
        {0xD0, {0XA4, 0XA1},                                                                         2},
        {0xD6, {0XA1},                                                                               1},
        {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
        {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
};

void setup_wifi(void);

void timeavailable(struct timeval *t);

void printLocalTime();

void setTimezone();

static bool
notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    if (is_initialized_lvgl) {
        lv_disp_drv_t *disp_driver = (lv_disp_drv_t *) user_ctx;
        lv_disp_flush_ready(disp_driver);
    }
    return false;
}

static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

esp_lcd_panel_handle_t setup_lcd_panel() {
    pinMode(PIN_LCD_RD, OUTPUT);
    digitalWrite(PIN_LCD_RD, HIGH);
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
            .dc_gpio_num = PIN_LCD_DC,
            .wr_gpio_num = PIN_LCD_WR,
            .clk_src = LCD_CLK_SRC_PLL160M,
            .data_gpio_nums =
                    {
                            PIN_LCD_D0,
                            PIN_LCD_D1,
                            PIN_LCD_D2,
                            PIN_LCD_D3,
                            PIN_LCD_D4,
                            PIN_LCD_D5,
                            PIN_LCD_D6,
                            PIN_LCD_D7,
                    },
            .bus_width = 8,
            .max_transfer_bytes = LVGL_LCD_BUF_SIZE * sizeof(uint16_t),
            .psram_trans_align = 0,
            .sram_trans_align = 0};
    esp_lcd_new_i80_bus(&bus_config, &i80_bus);

    esp_lcd_panel_io_i80_config_t io_config = {
            .cs_gpio_num = PIN_LCD_CS,
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .trans_queue_depth = 20,
            .on_color_trans_done = notify_lvgl_flush_ready,
            .user_ctx = &disp_drv,
            .lcd_cmd_bits = 8,
            .lcd_param_bits = 8,
            .dc_levels =
                    {
                            .dc_idle_level = 0,
                            .dc_cmd_level = 0,
                            .dc_dummy_level = 0,
                            .dc_data_level = 1,
                    },
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = PIN_LCD_RES,
            .color_space = ESP_LCD_COLOR_SPACE_RGB,
            .bits_per_pixel = 16,
            .vendor_config = NULL};
    esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);

    esp_lcd_panel_invert_color(panel_handle, true);
    esp_lcd_panel_swap_xy(panel_handle, true);
    esp_lcd_panel_mirror(panel_handle, false, true);

    esp_lcd_panel_set_gap(panel_handle, 0, 35);

    for (uint8_t i = 0; i < (sizeof(LCD_ST7789V_INIT_COMMANDS) / sizeof(lcd_cmd_t)); i++) {
        esp_lcd_panel_io_tx_param(io_handle, LCD_ST7789V_INIT_COMMANDS[i].cmd, LCD_ST7789V_INIT_COMMANDS[i].data,
                                  LCD_ST7789V_INIT_COMMANDS[i].len & 0x7f);
        if (LCD_ST7789V_INIT_COMMANDS[i].len & 0x80)
            delay(120);
    }

    /* Lighten the screen with gradient */
    ledcSetup(0, 10000, 8);
    ledcAttachPin(PIN_LCD_BL, 0);
    for (uint8_t i = 0; i < 0xFF; i++) {
        ledcWrite(0, i);
        delay(2);
    }

    return panel_handle;
}

void setup_lv(const esp_lcd_panel_handle_t &panel_handle) {
    lv_init();
    lv_disp_buf = (lv_color_t *) heap_caps_malloc(LVGL_LCD_BUF_SIZE * sizeof(lv_color_t),
                                                  MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    lv_disp_draw_buf_init(&disp_buf, lv_disp_buf, NULL, LVGL_LCD_BUF_SIZE);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    lv_disp_drv_register(&disp_drv);

    is_initialized_lvgl = true;
}

void setup_button() {

    button1.attachClick([]() {
        pinMode(PIN_POWER_ON, OUTPUT);
        pinMode(PIN_LCD_BL, OUTPUT);
        digitalWrite(PIN_POWER_ON, LOW);
        digitalWrite(PIN_LCD_BL, LOW);
        esp_sleep_enable_ext0_wakeup((gpio_num_t) PIN_BUTTON_2, 0); // 1 = High, 0 = Low
        esp_deep_sleep_start();
    });

    button2.attachClick([]() { ui_switch_page(); });
}

void prepare_setup() {
    Serial.begin(115200);
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);
}

void setup() {
    prepare_setup();

    const esp_lcd_panel_handle_t &panel_handle = setup_lcd_panel();
    setup_lv(panel_handle);
    setup_wifi();
    setup_button();
    ui_begin();
}

void loop() {
    lv_timer_handler();
    button1.tick();
    button2.tick();
    delay(3);
    static uint32_t last_tick{0};
    if (last_tick == 0 || millis() - last_tick > 60 * 60 * 1000) {
        last_tick = millis();

        FavaLedgerStat stat = getFavaLedgerStat();
        ESP_LOGI(TAG, "Credit Card Balance: %f", stat.creditCardBalance);
        ESP_LOGI(TAG, "Cash Balance: %f", stat.cashBalance);
        ESP_LOGI(TAG, "XIRR: %f", stat.xirr);

        lv_msg_send(MSG_NEW_FAVA_STAT, &stat);
    }
}

void setup_wifi(void) {
    ESP_LOGI(TAG, "Connecting to WiFi (%s) ...", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        ESP_LOGV(TAG, "Wifi Connecting...");
        LV_DELAY(1000);
    }

    setTimezone();
}

void printLocalTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        ESP_LOGE(TAG, "No time available (yet)");
        return;
    }
}

void timeavailable(struct timeval *t) {
    ESP_LOGV(TAG, "Got time adjustment from NTP!");
    printLocalTime();
    WiFi.disconnect();
}

void setTimezone() {
    String timezone = CUSTOM_TIMEZONE;

    ESP_LOGW(TAG, "Timezone %s", timezone);
    setenv("TZ", timezone.c_str(), 1);
    tzset();
}