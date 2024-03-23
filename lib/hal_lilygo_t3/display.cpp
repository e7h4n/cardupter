#include <Arduino.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h> // Create LCD panel for model ST7789
#include <lvgl.h>
#include <array>

#include "display.h"
#include "hardware_setup.h"

const std::array<lcd_message, 14> INIT_COMMANDS = {
        lcd_message{0x11, {0}, 0 | 0x80},
        lcd_message{0x3A, {0X05}, 1},
        lcd_message{0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
        lcd_message{0xB7, {0X75}, 1},
        lcd_message{0xBB, {0X28}, 1},
        lcd_message{0xC0, {0X2C}, 1},
        lcd_message{0xC2, {0X01}, 1},
        lcd_message{0xC3, {0X1F}, 1},
        lcd_message{0xC6, {0X13}, 1},
        lcd_message{0xD0, {0XA7}, 1},
        lcd_message{0xD0, {0XA4, 0XA1}, 2},
        lcd_message{0xD6, {0XA1}, 1},
        lcd_message{0xE0, {
                0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32
        }, 14},
        lcd_message{0xE1, {
                0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37
        }, 14}
};

Display::Display() {
    initialize_lvgl();

    pinMode(PIN_LCD_POWER, OUTPUT);
    digitalWrite(PIN_LCD_POWER, HIGH);

    pinMode(PIN_LCD_RD, OUTPUT);
    digitalWrite(PIN_LCD_RD, HIGH);

    initialize_hardware();

    backlight.set_brightness(50);
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
}

void Display::initialize_lvgl() {
    if (!lv_is_initialized()) {
        lv_init();
    }

    const esp_timer_create_args_t lvgl_tick_timer_args = {
            .callback = [](void *arg) {
                lv_tick_inc(LVGL_TICK_PERIOD_MS);
            },
            .name = "lvgl_tick"};
    esp_timer_handle_t lvgl_tick_timer = nullptr;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    display = lv_display_create(height, width);

    const auto buffer_size = width * 10;

    draw_buffers.first = heap_caps_malloc(buffer_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!draw_buffers.first) {
        LV_LOG_ERROR("display draw buffer malloc failed");
        return;
    }

    draw_buffers.second = heap_caps_malloc(buffer_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!draw_buffers.second) {
        LV_LOG_ERROR("display buffer malloc failed");
        lv_free(draw_buffers.first);
        return;
    }

    lv_display_set_buffers(display, draw_buffers.first, draw_buffers.second, buffer_size,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    const auto flush_callback = [](lv_display_t *disp, const lv_area_t *area, uint8_t *pix_map) {
        lv_draw_sw_rgb565_swap(pix_map, (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1));
        Display::draw_image(area->x1, area->x2, area->y1, area->y2, reinterpret_cast<uint16_t *>(pix_map));
    };

    lv_display_set_flush_cb(display, flush_callback);
}

void Display::initialize_hardware() {
    const esp_lcd_i80_bus_config_t bus_config{
            .dc_gpio_num = PIN_LCD_DC,
            .wr_gpio_num = PIN_LCD_WR,
            .clk_src = LCD_CLK_SRC_PLL160M,
            .data_gpio_nums = {
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
            .max_transfer_bytes = LVGL_LCD_BUF_SIZE, .psram_trans_align = sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    const auto pixel_transfer_callback = [](esp_lcd_panel_io_handle_t _panel_io,
                                            esp_lcd_panel_io_event_data_t *_event_data, void *user_ctx) {
        auto disp = static_cast<lv_display_t *>(user_ctx);
        lv_display_flush_ready(disp);
        return false;
    };

    const esp_lcd_panel_io_i80_config_t io_config = {
            .cs_gpio_num = PIN_LCD_CS,
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .trans_queue_depth = 20,
            .on_color_trans_done = pixel_transfer_callback,
            .user_ctx = display,
            .lcd_cmd_bits = 8,
            .lcd_param_bits = 8,
            .dc_levels = {
                    .dc_idle_level = 0,
                    .dc_cmd_level = 0,
                    .dc_dummy_level = 0,
                    .dc_data_level = 1,
            },
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    const esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = PIN_LCD_RES,
            .color_space = ESP_LCD_COLOR_SPACE_RGB,
            .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 0, 35));

    for (const auto &message: INIT_COMMANDS) {
        ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, message.command, message.parameters.data(),
                                                  message.length & 0x7f)); // No idea why I have this and here
        if (message.length & 0x80) { // No idea about this and either
            delay(120); // No idea why I gotta sleep here
        }
    }
}

Display::~Display() {
    ESP_ERROR_CHECK(esp_lcd_panel_del(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_io_del(io_handle));
    ESP_ERROR_CHECK(esp_lcd_del_i80_bus(i80_bus));

    pinMode(PIN_LCD_RD, INPUT);
    pinMode(PIN_LCD_POWER, INPUT);
}

uint16_t Display::get_width() {
    return width;
}

uint16_t Display::get_height() {
    return height;
}

void Display::set_backlight(uint8_t level) {
    get().backlight.set_brightness(level);
}

void Display::draw_image(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end, uint16_t *data) {
    auto &display = Display::get();
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(display.panel_handle, x_start, y_start, x_end + 1, y_end + 1, data));
}

void Display::send_command(const lcd_message &message) {
    esp_lcd_panel_io_tx_param(io_handle, message.command, message.parameters.data(), message.length);
}

Display &Display::get() {
    static Display instance;
    return instance;
}

void Display::init() {
    Display::get();
}

void Backlight::set_brightness(uint8_t lvl) const {
    analogWrite(pin, lvl);
}