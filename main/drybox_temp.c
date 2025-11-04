#include "driver/i2c.h"
#include "esp_lcd_panel_ssd1306.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"

#define I2C_HOST I2C_NUM_0
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_SPEED_HZ 400000
#define SSD1306_ADDR 0x3C

static const char *TAG = "SSD1306_EXAMPLE";

void app_main() {
    // 1. Configure I2C bus
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_SPEED_HZ,
    };
    i2c_param_config(I2C_HOST, &i2c_conf);
    i2c_driver_install(I2C_HOST, I2C_MODE_MASTER, 0, 0, 0);

    // 2. Create LCD I/O handle
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = SSD1306_ADDR,
        .control_phase_bytes = 1, // typical for SSD1306
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_bit_offset = 6, // typical offset in control byte
    };
    esp_lcd_panel_io_handle_t io_handle = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_HOST, &io_config, &io_handle));

    // 3. Create SSD1306 panel handle
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1, // monochrome
    };
    esp_lcd_panel_handle_t panel_handle = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

    // 4. Initialize and turn on display
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    // 5. Draw something
    uint8_t buffer[128 * 64 / 8] = {0}; // full framebuffer
    for (int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = (i & 0x10) ? 0xFF : 0x00; // pattern
    }
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 128, 64, buffer));

    ESP_LOGI(TAG, "Display updated!");
}
