//
// Created by genius on 1/6/21.
//

#include <iostream>
#include "cJSON.h"
#include "FrameEncoder.h"

FrameEncoder::FrameEncoder(const std::string &head, const std::string &tail) {
    frameHead = head;
    frameTail = tail;
    frameMutex = xSemaphoreCreateMutex();
}

FrameEncoder::~FrameEncoder() {
    vSemaphoreDelete(frameMutex);
}

int FrameEncoder::AddToFrameBuffer(const FrameEncoder::FrameData &body) {
    auto jsonRoot = cJSON_CreateObject();
    auto jsonRootSignal = cJSON_CreateObject();

    cJSON_AddItemToObject(jsonRoot, "type", cJSON_CreateNumber(body.deviceType));
    cJSON_AddItemToObject(jsonRoot, "signal", jsonRootSignal);
    cJSON_AddItemToObject(jsonRootSignal, "id", cJSON_CreateNumber(body.frameSignal.id));
    cJSON_AddItemToObject(jsonRootSignal, "raw", cJSON_CreateNumber(body.frameSignal.raw));
    cJSON_AddItemToObject(jsonRootSignal, "smo", cJSON_CreateNumber(body.frameSignal.smooth));
    cJSON_AddItemToObject(jsonRootSignal, "bcm", cJSON_CreateNumber(body.frameSignal.benchmark));

    char *jsonChar = cJSON_PrintUnformatted(jsonRoot);
    auto jsonString = new std::string(jsonChar);
    *jsonString = frameHead + *jsonString + frameTail;
    xSemaphoreTake(frameMutex, portMAX_DELAY);
    frameBuffer.push(jsonString);
    xSemaphoreGive(frameMutex);

    cJSON_free(jsonChar);
    cJSON_Delete(jsonRoot);
    return jsonString->size();
}

int FrameEncoder::GetOneFrame(std::string &frameString) {
    xSemaphoreTake(frameMutex, portMAX_DELAY);
    auto jsonString = frameBuffer.front();
    if (!frameBuffer.empty()) {
        frameBuffer.pop();
    } else {
        xSemaphoreGive(frameMutex);
        return -1;
    }
    xSemaphoreGive(frameMutex);
    std::copy(jsonString->cbegin(), jsonString->cend(), std::back_inserter(frameString));
    delete jsonString;
    return 0;
}

