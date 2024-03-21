
#include <LovyanGFX.h>
#include <lvgl.h>
#include "gui.h"

#define LCD_MOSI_PIN 35
#define LCD_MISO_PIN -1
#define LCD_SCLK_PIN 36
#define LCD_DC_PIN   34
#define LCD_CS_PIN   37
#define LCD_RST_PIN  33
#define LCD_BUSY_PIN -1
#define LCD_BL_PIN   38

class LGFX_Cardputer : public lgfx::LGFX_Device {
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;

public:
    LGFX_Cardputer(void) {
        {
            auto cfg = _bus_instance.config();

            cfg.pin_mosi = LCD_MOSI_PIN;
            cfg.pin_miso = LCD_MISO_PIN;
            cfg.pin_sclk = LCD_SCLK_PIN;
            cfg.pin_dc = LCD_DC_PIN;
            cfg.freq_write = 40000000;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();

            cfg.invert = true;
            cfg.pin_cs = LCD_CS_PIN;
            cfg.pin_rst = LCD_RST_PIN;
            cfg.pin_busy = LCD_BUSY_PIN;
            cfg.panel_width = 135;
            cfg.panel_height = 240;
            cfg.offset_x = 52;
            cfg.offset_y = 40;

            _panel_instance.config(cfg);
        }
        {
            auto cfg = _light_instance.config();

            cfg.pin_bl = LCD_BL_PIN;
            cfg.invert = false;
            cfg.freq = 200;
            cfg.pwm_channel = 7;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        setPanel(&_panel_instance);
    }
};

LGFX_Device *_display;
static lv_disp_draw_buf_t draw_buf;

static void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    _display->startWrite();
    _display->setAddrWindow(area->x1, area->y1, w, h);
    _display->writePixels((uint16_t *) &color_p->full, w * h, true);
    _display->endWrite();

    lv_disp_flush_ready(disp);
}

void setup() {
    _display = new LGFX_Cardputer;
    _display->init();
    _display->setRotation(1);

    lv_init();
    static lv_color_t buf[240 * 10];
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, 240 * 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 135;
    disp_drv.flush_cb = display_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    ui_begin();
    ui_switch_page();
    set_fava_stat(FavaLedgerStat{.creditCardBalance=100.0, .cashBalance=1000.0, .xirr=0.23});
}

void loop() {
    lv_timer_handler(); /* let the GUI do its work */
    delay(1);
}