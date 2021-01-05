//
// Created by genius on 1/6/21.
//

#ifndef MCU_FIRMWARE_FRAMEENCODER_H
#define MCU_FIRMWARE_FRAMEENCODER_H

#include <string>
#include <vector>
#include <queue>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class FrameEncoder {
public:
    enum DeviceType {
        Channel = 0,
        Button = 1,
        Slider = 2,
        Matrix = 3,
    };

    struct FrameSignal {
        int id;
        uint32_t raw;
        uint32_t smooth;
        uint32_t benchmark;
    };

    struct FrameData {
        DeviceType deviceType;
        struct FrameSignal frameSignal;
    };

    explicit FrameEncoder(const std::string& head = "$*", const std::string& tail = "*$");
    ~FrameEncoder();
    int AddToFrameBuffer(const FrameData& body);
    void SendOneFrame();

protected:
    SemaphoreHandle_t frameMutex;
    std::string frameHead;
    std::string frameTail;
    std::queue<std::string*> frameBuffer;
};


#endif //MCU_FIRMWARE_FRAMEENCODER_H
