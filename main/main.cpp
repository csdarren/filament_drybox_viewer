#include "display/lv_display.h"
#include "driver/i2c_types.h"
#include "esp_log.h"
#include "i2c_setup.hpp"
#include "aht20.hpp"
#include "misc/lv_area.h"
#include "ssd1306_lvgl.hpp"
#include "types.hpp"

#include "esp_lvgl_port.h"
#include "esp_lcd_panel_ssd1306.h"
#include "esp_lcd_panel_io.h"

I2C i2c;

using sensor_task_params_t = struct {
    i2c_master_bus_handle_t i2c_bus_hdl;
    int i2c_dev_addr;
    QueueHandle_t queue;
};

void sensor_task(void *param) {
    auto *ctx = static_cast<sensor_task_params_t *>(param);
    sensor_data data{};
    AHT20 sensor(ctx->i2c_bus_hdl, ctx->i2c_dev_addr);

    while (true) {
        data = sensor.read_temp_humidity();
        xQueueOverwrite(ctx->queue, &data);

        ESP_LOGI("QUEUE SEND", "Sending data over queue to lvgl task");
        vTaskDelay(pdMS_TO_TICKS(1000)); // x ms delay keeps scheduler happy
    }
}

extern "C" void app_main() {
    QueueHandle_t queues[4];
    for (auto &queue : queues) {
        queue = xQueueCreate(1, sizeof(sensor_data));
    }
    static sensor_task_params_t t_param1 = {i2c.get_bus_0_hdl(), AHT20_ADDR_0, queues[0]};
    xTaskCreate(sensor_task, "sensor1", 4096, &t_param1, 5, nullptr);

    static sensor_task_params_t t_param2 = {i2c.get_bus_0_hdl(), AHT20_ADDR_1, queues[1]};
    xTaskCreate(sensor_task, "sensor2", 4096, &t_param2, 5, nullptr);

    static sensor_task_params_t t_param3 = {i2c.get_bus_1_hdl(), AHT20_ADDR_0, queues[2]};
    xTaskCreate(sensor_task, "sensor3", 4096, &t_param3, 5, nullptr);

    static sensor_task_params_t t_param4 = {i2c.get_bus_1_hdl(), AHT20_ADDR_1, queues[3]};
    xTaskCreate(sensor_task, "sensor4", 4096, &t_param4, 5, nullptr);

    SSD1306_LVGL displays[4] = {
        {i2c.get_bus_0_hdl(), SSD1306_ADDR_0},
        {i2c.get_bus_0_hdl(), SSD1306_ADDR_1},
        {i2c.get_bus_1_hdl(), SSD1306_ADDR_0},
        {i2c.get_bus_1_hdl(), SSD1306_ADDR_1}};

    lv_obj_t *tempText[4];
    lv_obj_t *humidityText[4];
    lv_obj_t *scrs[4];
    for (int i = 0; i < (sizeof(scrs) / sizeof(scrs[0])); i++) {
        scrs[i] = displays[i].get_lvgl_disp();
        lvgl_port_lock(0);

        tempText[i] = lv_label_create(scrs[i]);
        humidityText[i] = lv_label_create(scrs[i]);
        lv_obj_center(tempText[i]);
        lv_obj_align(humidityText[i], LV_ALIGN_CENTER, 0, 15);
        lvgl_port_unlock();
    }

    while (true) {
        sensor_data temp_data{};
        for (int i = 0; i < 4; i++) {
            if (xQueueReceive(queues[i], &temp_data, 0) == pdTRUE) {
                ESP_LOGI("QUEUE RECEIVE", "Receiving data from queue in lvgl task TEMP: [%.2f], HUMIDITY: [%.2f]", temp_data.temp, temp_data.humidity);
            }
            char temp_bf[20];
            char humidity_bf[20];

            sprintf(temp_bf, "TEMP: %.2f", temp_data.temp);
            sprintf(humidity_bf, "HUMIDITY: %.2f", temp_data.humidity);

            lvgl_port_lock(0);
            lv_label_set_text(tempText[i], temp_bf);
            lv_label_set_text(humidityText[i], humidity_bf);
            lvgl_port_unlock();

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}
