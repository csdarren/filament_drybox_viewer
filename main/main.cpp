#include "display/lv_display.h"
#include "esp_log.h"
#include "i2c_setup.hpp"
#include "aht20.hpp"
#include "misc/lv_area.h"
#include "ssd1306_lvgl.hpp"
#include "types.hpp"

#include "esp_lvgl_port.h"
#include "esp_lcd_panel_ssd1306.h"
#include "esp_lcd_panel_io.h"
#define SSD1306_H_RES 128
#define SSD1306_V_RES 64

static QueueHandle_t sensor_queue;

I2C i2c;

void sensor_task(void * /*task_param*/) {
    AHT20 aht20(i2c.get_bus_hdl());
    while (true) {
        sensor_data data = aht20.read_temp_humidity();

        ESP_LOGI("QUEUE SEND", "Sending data over queue to lvgl task");
        xQueueOverwrite(sensor_queue, &data);
        vTaskDelay(pdMS_TO_TICKS(1000)); // x ms delay keeps scheduler happy
    }
}

extern "C" void app_main() {
    sensor_queue = xQueueCreate(1, sizeof(sensor_data));
    xTaskCreate(sensor_task, "SENSOR TASK", 4096, nullptr, 5, nullptr);
    sensor_data temp_data = {};
    SSD1306_LVGL display(i2c.get_bus_hdl());
    lv_obj_t *tempText = {};
    lv_obj_t *humidityText = {};
    lv_obj_t *scr = lv_screen_active();

    lvgl_port_lock(0);
    tempText = lv_label_create(scr);
    humidityText = lv_label_create(scr);
    lv_obj_center(tempText);
    lv_obj_align(humidityText, LV_ALIGN_CENTER, 0, 15);
    lvgl_port_unlock();

    while (true) {
        if (xQueueReceive(sensor_queue, &temp_data, 0) == pdTRUE) {
            ESP_LOGI("QUEUE RECEIVE", "Receiving data from queue in lvgl task TEMP: [%.2f], HUMIDITY: [%.2f]", temp_data.temp, temp_data.humidity);
        }
        char temp_bf[20];
        char humidity_bf[20];

        sprintf(temp_bf, "TEMP: %.2f", temp_data.temp);
        sprintf(humidity_bf, "HUMIDITY: %.2f", temp_data.humidity);

        lvgl_port_lock(0);
        lv_label_set_text(tempText, temp_bf);
        lv_label_set_text(humidityText, humidity_bf);
        lvgl_port_unlock();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
