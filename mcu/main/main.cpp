#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "TouchElement.h"
#include "FrameEncoder.h"
#include "SerialPort.h"

extern "C" void app_main(void)
{
    auto serialPort = new SerialPort();
    auto frameEncoder = new FrameEncoder();
    auto touchElement = new TouchElement();
    touch_button_config_t buttonConfig = {
        .channel_num = TOUCH_PAD_NUM1,
        .channel_sens = 1000
    };
    auto touchButton = touchElement->CreateButton(buttonConfig);
    touch_element_start();
    while (1) {
        FrameEncoder::FrameData frameData{};
        frameData.deviceType = FrameEncoder::Channel;
        frameData.frameSignal.id = 1;
        frameData.frameSignal.raw = touchButton->GetSignalValue(TE_SIGNAL_RAW);
        frameData.frameSignal.smooth = touchButton->GetSignalValue(TE_SIGNAL_SMOOTH);
        frameData.frameSignal.benchmark = touchButton->GetSignalValue(TE_SIGNAL_BENCHMARK);
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
