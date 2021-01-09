#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "TouchSense.h"
#include "FrameEncoder.h"
#include "SerialPort.h"

extern "C" void app_main(void)
{
    auto serialPort = new SerialPort();
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
        std::string sendString;
        int ret = frameEncoder->GetOneFrame(sendString);
        if (ret >= 0) {
            sendString += "\r\n";
            serialPort->TransmitMessage(sendString);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

}