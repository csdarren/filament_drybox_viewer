#pragma once
#ifndef SSD1306_HPP

#include "esp_lcd_panel_vendor.h"
#include "esp_lvgl_port.h"
#define SSD1306_HPP

#define SSD1306_H_RES 128
#define SSD1306_V_RES 64
#define SSD1306_SCL_SPEED_HZ 400000

#define SSD1306_ADDR_0 0x3C
#define SSD1306_ADDR_1 0x3D
#define SSD1306_H_RES 128
#define SSD1306_V_RES 64

class SSD1306_LVGL {
  private:
    i2c_master_bus_handle_t i2c_bus = {};
    int i2c_addr{};

    esp_lcd_panel_io_handle_t io_handle = nullptr;
    esp_lcd_panel_handle_t panel_handle = nullptr;

    esp_lcd_panel_io_i2c_config_t io_cfg = {};
    lvgl_port_display_cfg_t disp_cfg = {};
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lv_disp_t *lvgl_display = {};

    static auto notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) -> bool {
        auto *disp = (lv_disp_t *)user_ctx;
        lvgl_port_flush_ready(disp);
        return false;
    }
    auto display_io_cfg() -> void {
        io_cfg.dev_addr = i2c_addr;
        io_cfg.control_phase_bytes = 1; // typical for SSD1306
        io_cfg.lcd_cmd_bits = 8;
        io_cfg.lcd_param_bits = 8;
        io_cfg.dc_bit_offset = 6; // typical offset in control byte
        io_cfg.scl_speed_hz = SSD1306_SCL_SPEED_HZ;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_cfg, &io_handle));
    }
    auto display_panel_cfg() -> void {
        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.bits_per_pixel = 1; // monochrome
        ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    }
    auto display_prepare() -> void {
        ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
        ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    }
    auto display_lvgl_cfg() -> void {
        lvgl_port_init(&lvgl_cfg);

        disp_cfg.io_handle = io_handle;
        disp_cfg.panel_handle = panel_handle;
        disp_cfg.buffer_size = SSD1306_H_RES * SSD1306_V_RES;
        disp_cfg.double_buffer = true;
        disp_cfg.hres = SSD1306_H_RES;
        disp_cfg.vres = SSD1306_V_RES;
        disp_cfg.monochrome = true;
        disp_cfg.rotation =
            {
                .swap_xy = false,
                .mirror_x = true,
                .mirror_y = true,
            };
        lvgl_display = lvgl_port_add_disp(&disp_cfg);
    }
    auto input_output_setup() -> void {
        const esp_lcd_panel_io_callbacks_t cbs = {
            .on_color_trans_done = notify_lvgl_flush_ready,
        };
        esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, lvgl_display);
    }

  public:
    SSD1306_LVGL(i2c_master_bus_handle_t i2c_bus_handle, int dev_addr) : i2c_bus(i2c_bus_handle), i2c_addr(dev_addr) {
        display_io_cfg();
        display_panel_cfg();
        display_prepare();
        display_lvgl_cfg();
        input_output_setup();
    }
    auto get_lvgl_disp() -> lv_obj_t * {
        return lv_disp_get_scr_act(lvgl_display);
    }
};

#endif
