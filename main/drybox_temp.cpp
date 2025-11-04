
#include <cstdio>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "aht20.h"
#include "ssd1306.h"

#include "font8x8_basic.h"

/*
 You have to set this config value with menuconfig
 CONFIG_INTERFACE

 for i2c
 CONFIG_MODEL
 CONFIG_SDA_GPIO
 CONFIG_SCL_GPIO
 CONFIG_RESET_GPIO

 for SPI
 CONFIG_CS_GPIO
 CONFIG_DC_GPIO
 CONFIG_RESET_GPIO
*/

#define tag "SSD1306"

#if CONFIG_I2C_PORT_0
#define I2C_NUM I2C_NUM_0
#elif CONFIG_I2C_PORT_1
#define I2C_NUM I2C_NUM_1
#endif

auto aht20_i2c_init(i2c_master_bus_handle_t i2c_bus_handle) -> aht20_dev_handle_t {
    aht20_i2c_config_t i2c_conf = {};
    i2c_conf.bus_inst = i2c_bus_handle;  // I2C bus instance
    i2c_conf.i2c_addr = AHT20_ADDRRES_0; // Device address
    aht20_dev_handle_t aht20_handle = {};
    ESP_ERROR_CHECK(aht20_new_sensor(&i2c_conf, &aht20_handle));
    return aht20_handle;
};

using AHT20_t = struct {
    i2c_master_bus_handle_t _i2c_bus_handle;
};

void i2c_task(void *pvParameter) {
    // Register the device with i2c address = 0c3C
    SSD1306_t dev1;
    AHT20_t aht1;
    i2c_master_init(&dev1, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

    // Show Device 1
    ssd1306_init(&dev1, 128, 64);
    ssd1306_clear_screen(&dev1, false);
    ssd1306_contrast(&dev1, 0xff);

    aht20_dev_handle_t aht20_handle = aht20_i2c_init(dev1._i2c_bus_handle); // piggybacking off SSD1306 i2c handle

    uint32_t temperature_raw = 0;
    uint32_t humidity_raw = 0;
    float temperature = 0.0;
    float humidity = 0.0;

    aht20_read_temperature_humidity(aht20_handle, &temperature_raw, &temperature, &humidity_raw, &humidity);

    ssd1306_display_text_x3(&dev1, 0, "", 5, false);

    ESP_LOGI("TEMPS", "%-20s: %2.2f %%", "humidity is", humidity);
    ESP_LOGI("TEMPS", "%-20s: %2.2f degC", "temperature is", temperature);

    // // Show Device 2
    // ssd1306_init(&dev2, 128, 64);
    // ssd1306_clear_screen(&dev2, false);
    // ssd1306_contrast(&dev2, 0xff);
    // ssd1306_display_text_x3(&dev2, 0, "World", 5, false);

    vTaskDelete(nullptr);
}

extern "C" void app_main() {
    ESP_LOGI(tag, "INTERFACE is i2c");
    ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d", CONFIG_SDA_GPIO);
    ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d", CONFIG_SCL_GPIO);
    ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d", CONFIG_RESET_GPIO);

    xTaskCreate(&i2c_task, "I2C", 1024 * 6, nullptr, 2, nullptr);
}
