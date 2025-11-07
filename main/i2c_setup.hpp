#pragma once
#include "driver/i2c_types.h"
#ifndef DEV_SETUP_HPP
#define DEV_SETUP_HPP
#include "driver/i2c_master.h"

#define I2C_0_SDA_PIN 21
#define I2C_0_SCL_PIN 22

#define I2C_1_SDA_PIN
#define I2C_1_SCL_PIN

#define TWO_BUS

class I2C {
  private:
    i2c_master_bus_handle_t i2c_bus_0{};
    i2c_master_bus_handle_t i2c_bus_1{};

    auto i2c_config_init() -> void {
        //Configure I2C bus
        i2c_master_bus_config_t i2c_bus_cfg = {};
        i2c_bus_cfg.i2c_port = I2C_NUM_0;
        i2c_bus_cfg.sda_io_num = gpio_num_t(I2C_0_SDA_PIN);
        i2c_bus_cfg.scl_io_num = gpio_num_t(I2C_0_SCL_PIN);
        i2c_bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
        i2c_bus_cfg.glitch_ignore_cnt = 7;
        i2c_bus_cfg.flags.enable_internal_pullup = true;
        i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus_0);

#ifdef TWO_BUS
        i2c_master_bus_config_t i2c_bus_1_cfg = {};
        i2c_bus_1_cfg.i2c_port = I2C_NUM_1;
        i2c_bus_1_cfg.sda_io_num = gpio_num_t(I2C_1_SDA_PIN);
        i2c_bus_1_cfg.scl_io_num = gpio_num_t(I2C_1_SCL_PIN);
        i2c_bus_1_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
        i2c_bus_1_cfg.glitch_ignore_cnt = 7;
        i2c_bus_1_cfg.flags.enable_internal_pullup = true;
        i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus_1);
#endif
    }

  public:
    I2C() {
        i2c_config_init();
    };
    auto get_bus_0_hdl() -> i2c_master_bus_handle_t {
        return i2c_bus_0;
    }
    auto get_bus_1_hdl() -> i2c_master_bus_handle_t {
        return i2c_bus_1;
    }
};

#endif
