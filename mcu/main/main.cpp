/* Touch Sensor waterproof - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "TouchSense.h"
#include "FrameEncoder.h"

extern "C" void app_main(void)
{
    auto frameEncoder = new FrameEncoder();
    auto touchSense = new TouchSense();
    while (1) {
        FrameEncoder::FrameData frameData{};
        frameData.deviceType = FrameEncoder::Channel;
        frameData.frameSignal.id = 1;
        frameData.frameSignal.raw = touchSense->GetChannelValue(TOUCH_PAD_NUM1, TS_SIGNAL_RAW);
        frameData.frameSignal.smooth = touchSense->GetChannelValue(TOUCH_PAD_NUM1, TS_SIGNAL_SMOOTH);
        frameData.frameSignal.benchmark = touchSense->GetChannelValue(TOUCH_PAD_NUM1, TS_SIGNAL_BENCHMARK);
        frameEncoder->AddToFrameBuffer(frameData);
        frameEncoder->SendOneFrame();
        vTaskDelay(pdMS_TO_TICKS(10));
    }

}