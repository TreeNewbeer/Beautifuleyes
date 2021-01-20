//
// Created by genius on 1/21/21.
//

#ifndef MCU_FIRMWARE_FRAMEDECODER_H
#define MCU_FIRMWARE_FRAMEDECODER_H

#include <vector>
#include <array>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "touch_element/touch_element.h"

class FrameDecoder {
public:
    enum DeviceType {
        Channel,
        Button,
        Slider,
        Matrix,
    };

    enum ControlCommand {
        Start,
        Stop
    };

    struct FrameBody {
        enum DeviceType deviceType;
        std::vector<touch_pad_t> channels;
        int payloadTime;
        enum ControlCommand command;
    };

    FrameDecoder(const std::array<uint8_t, 2>& head, const std::array<uint8_t, 2>& tail);
    ~FrameDecoder();

    int AddToFrameBuffer(const std::vector<uint8_t>& bytes);
    int GetOneFrame(struct FrameBody& frameBody);

private:
    SemaphoreHandle_t frameMutex = nullptr;
    std::array<uint8_t, 2> head = {};
    std::array<uint8_t, 2> tail = {};
    std::vector<struct FrameBody*> frameBuffer;
};


#endif //MCU_FIRMWARE_FRAMEDECODER_H
