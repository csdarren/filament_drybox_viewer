#pragma once
#ifndef AHT20_HPP
#define AHT20_HPP

#define AHT20_ADDR_0 0x38
#define AHT20_ADDR_1 0x38
#define AHT20_SCL_SPEED_HZ 100000

#include "driver/i2c_master.h"
#include "esp_lcd_panel_io.h"
#include "types.hpp"

class AHT20 {
  private:
    i2c_master_bus_handle_t i2c_bus = {};
    int i2c_addr{};

    i2c_master_dev_handle_t aht20_io_handle{};
    i2c_device_config_t aht20_cfg = {};
    sensor_data temp_data = {};

    auto add_device_to_i2c_bus() -> void {
        aht20_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
        aht20_cfg.device_address = i2c_addr;
        aht20_cfg.scl_speed_hz = AHT20_SCL_SPEED_HZ;

        i2c_master_bus_add_device(i2c_bus, &aht20_cfg, &aht20_io_handle);
    }

  public:
    AHT20(i2c_master_bus_handle_t i2c_bus_handle, int dev_addr) : i2c_bus(i2c_bus_handle), i2c_addr(dev_addr) {
        add_device_to_i2c_bus();
    }

    auto read_temp_humidity() -> sensor_data {
        uint8_t cmd[] = {0xAC, 0x33, 0x00};
        i2c_master_transmit(aht20_io_handle, cmd, sizeof(cmd), -1);

        vTaskDelay(pdMS_TO_TICKS(80)); // wait for measurement

        uint8_t data[6];
        i2c_master_receive(aht20_io_handle, data, sizeof(data), -1);

        uint32_t raw_h = (data[1] << 12) | (data[2] << 4) | (data[3] >> 4);
        uint32_t raw_t = ((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5];

        temp_data.humidity = ((float)raw_h / 1048576.0F) * 100.0F;
        temp_data.temp = ((float)raw_t / 1048576.0F) * 200.0F - 50.0F;
        return temp_data;
    };
};

#endif
