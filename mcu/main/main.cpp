/* Touch Sensor waterproof - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <iostream>
#include <array>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
#include "TouchSense.h"
#include "esp_log.h"

#define TAG "TouchSense"

void button_callback(ts_btn_handle_t out_handle, ts_btn_state_t out_state, void *arg)
{
    if (out_state == TS_BTN_STATE_PRESS) {
        ESP_LOGI(TAG, "button[%p] press", out_handle);
    } else if (out_state == TS_BTN_STATE_RELEASE) {
        ESP_LOGI(TAG, "button[%p] release", out_handle);
    } else if (out_state == TS_BTN_STATE_LONGPRESS) {
        ESP_LOGI(TAG, "button[%p] long press", out_handle);
    }
}

extern "C" void app_main(void)
{
    cJSON *root =  cJSON_CreateObject();
    cJSON *root_signal =  cJSON_CreateObject();
    cJSON *root_signal_channel1 =  cJSON_CreateObject();
    cJSON_AddItemToObject(root, "type", cJSON_CreateString("channel"));
    cJSON_AddItemToObject(root, "signal", root_signal);
    cJSON_AddItemToObject(root_signal, "channel1", root_signal_channel1);

    auto raw_num = cJSON_CreateNumber(6000);

    cJSON_AddNumberToObject(root_signal_channel1, "id", 1);
    cJSON_AddItemToObject(root_signal_channel1, "raw", raw_num);
    cJSON_AddNumberToObject(root_signal_channel1, "smooth", 2000);
    cJSON_AddNumberToObject(root_signal_channel1, "filter", 2000);

    auto touchsense = new TouchSense(nullptr);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(20));
        uint32_t sig_val = touchsense->GetChannelValue(TOUCH_PAD_NUM1);
        raw_num->valuedouble = sig_val;
        char *char_buf;
        char *cjson_str = cJSON_PrintUnformatted(root);
        asprintf(&char_buf, "$*%s*$", cjson_str);
        ets_printf("%s\n", char_buf);
        free(cjson_str);
        free(char_buf);
    }
}