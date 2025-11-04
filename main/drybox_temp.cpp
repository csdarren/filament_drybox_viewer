
#include <cstdio>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "aht20.h"

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
    AHT20_t aht1;
}

extern "C" void app_main() {
    xTaskCreate(&i2c_task, "I2C", 1024 * 6, nullptr, 2, nullptr);
}
