#pragma once

#include <memory>
#include <vector>
#include <esp_lcd_panel_io.h>

#include "hardware_setup.h"

class Backlight {
    const uint8_t pin;
    uint8_t level = 0;

public:
    explicit Backlight(uint8_t pin) : pin(pin) {
        pinMode(pin, OUTPUT);
    };

    ~Backlight() {
        pinMode(pin, INPUT);
    };

    void set_brightness(uint8_t level) const;
};

struct lcd_message {
    uint8_t command;
    std::vector<uint8_t> parameters;
    size_t length;
};

class Display {
    lv_display_t *display = nullptr;
    std::pair<void *, void *> draw_buffers = {nullptr, nullptr};

    esp_lcd_i80_bus_handle_t i80_bus = nullptr;
    esp_lcd_panel_io_handle_t io_handle = nullptr;
    esp_lcd_panel_handle_t panel_handle = nullptr;

    Backlight backlight = Backlight(PIN_LCD_BL);

    uint16_t width = LCD_VER_RES;
    uint16_t height = LCD_HOR_RES;

    Display();

    void initialize_lvgl();

    void initialize_hardware();

    static Display &get();

    ~Display();

public:
    Display(const Display &) = delete;

    Display(Display &&) = delete;

    Display &operator=(const Display &) = delete;

    Display &operator=(Display &&) = delete;

    static void init();

    uint16_t get_width();

    uint16_t get_height();

    static void set_backlight(uint8_t level);

    static void draw_image(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end, uint16_t *data);

    void send_command(const lcd_message &message);
};