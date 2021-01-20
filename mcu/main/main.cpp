#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "TouchElement.h"
#include "FrameEncoder.h"
#include "FrameDecoder.h"
#include "SerialPort.h"

extern "C" void app_main(void)
{
    auto serialPort = new SerialPort();
    auto frameDecoder = new FrameDecoder({'$', '*'}, {'*', '$'});
    auto frameEncoder = new FrameEncoder();
    auto touchElement = new TouchElement();

    TouchButton* touchButton = nullptr;
    FrameDecoder::FrameBody frameBody;
    while (true) {
        std::vector<uint8_t> receiveBuffer;
        int receiveBytes = serialPort->ReceiveMessage(receiveBuffer);
        if (receiveBytes > 0) {
            frameDecoder->AddToFrameBuffer(receiveBuffer);
        }
        int frameNum = frameDecoder->GetOneFrame(frameBody);
        if (frameNum != -1) {
            if (frameBody.command == FrameDecoder::Start) {
                touch_button_config_t buttonConfig = {
                        .channel_num = frameBody.channels[0],
                        .channel_sens = 1000
                };
                touchButton = touchElement->CreateButton(buttonConfig);
                touch_element_start();
            } else {
                touch_element_stop();
                delete touchButton;
                touchButton = nullptr;
            }
        }
        if (touchButton) {
            FrameEncoder::FrameData frameData{};
            frameData.deviceType = static_cast<FrameEncoder::DeviceType>(frameBody.deviceType);
            frameData.frameSignal.id = frameBody.channels[0];
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
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
