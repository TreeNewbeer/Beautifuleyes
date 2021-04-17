#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "TouchElement.h"
#include "FrameEncoder.h"
#include "FrameDecoder.h"
#include "SerialPort.h"

static QueueHandle_t serialRecvQueue = nullptr;

static void serial_receive_task(void *arg)
{
    auto *serialPort = (SerialPort *)arg;
    auto frameDecoder = new FrameDecoder({'$', '*'}, {'*', '$'});
    while (true) {
        std::vector<uint8_t> receiveBuffer;
        int receiveBytes = serialPort->ReceiveMessage(receiveBuffer);
        if (receiveBytes > 0) {
            frameDecoder->AddToFrameBuffer(receiveBuffer);
        }
        FrameDecoder::FrameBody frameBody;
        int frameNum = frameDecoder->GetOneFrame(frameBody);
        if (frameNum != -1) {
            xQueueSend(serialRecvQueue, &frameBody, portMAX_DELAY);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static void control_task(void *arg)
{
    auto *serialPort = (SerialPort *)arg;
    auto frameEncoder = new FrameEncoder();
    auto touchElement = new TouchElement();
    FrameDecoder::FrameBody frameBody;
    TouchButton* touchButton = nullptr;
    while (true) {
        BaseType_t ret = xQueueReceive(serialRecvQueue, &frameBody, pdMS_TO_TICKS(50));
        if (ret == pdTRUE) {
            if (frameBody.command == FrameDecoder::Start) {
                touch_button_config_t buttonConfig = {
                    .channel_num = frameBody.channels[0],
                    .channel_sens = 1000
                };
                touchButton = touchElement->CreateButton(buttonConfig);
                touch_element_start();
                vTaskDelay(pdMS_TO_TICKS(100));
            } else {
                touch_element_stop();
                delete touchButton;
                touchButton = nullptr;
            }
        }

        if (frameBody.command == FrameDecoder::Start && touchButton != nullptr) {
            FrameEncoder::FrameData frameData{};
            frameData.deviceType = FrameEncoder::Channel;
            frameData.frameSignal.id =touchButton->GetChannelNum();
            frameData.frameSignal.raw = touchButton->GetSignalValue(TE_SIGNAL_RAW);
            frameData.frameSignal.smooth = touchButton->GetSignalValue(TE_SIGNAL_SMOOTH);
            frameData.frameSignal.benchmark = touchButton->GetSignalValue(TE_SIGNAL_BENCHMARK);
            frameEncoder->AddToFrameBuffer(frameData);
            std::string sendString;
            int frame_ret = frameEncoder->GetOneFrame(sendString);
            if (frame_ret >= 0) {
                sendString += "\r\n";
                serialPort->TransmitMessage(sendString);
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

extern "C" void app_main(void)
{
    auto serialPort = new SerialPort();
    serialRecvQueue = xQueueCreate(10, sizeof(FrameDecoder::FrameBody));
    assert(serialRecvQueue);
    assert(serialPort);
    xTaskCreate(serial_receive_task, "serial_receive", 4 * 1024, serialPort, 10, nullptr);
    xTaskCreate(control_task, "control", 4 * 1024, serialPort, 9, nullptr);
}
