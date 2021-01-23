//
// Created by genius on 1/21/21.
//

#include <iostream>
#include <cstring>
#include <algorithm>
#include "FrameDecoder.h"
#include "cJSON.h"

FrameDecoder::FrameDecoder(const std::array<uint8_t, 2> &head, const std::array<uint8_t, 2> &tail)
{
    std::copy(head.cbegin(), head.cend(), this->head.begin());
    std::copy(tail.cbegin(), tail.cend(), this->tail.begin());
    try {
        frameMutex = xSemaphoreCreateMutex();
        if (frameMutex == NULL) throw std::runtime_error("frameMutex create failed, no memory");
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

FrameDecoder::~FrameDecoder()
{
    xSemaphoreTake(frameMutex, portMAX_DELAY);
    for (auto item : frameBuffer) {
        delete item;
    }
    xSemaphoreGive(frameMutex);
    vSemaphoreDelete(frameMutex);
}

int FrameDecoder::AddToFrameBuffer(const std::vector<uint8_t> &bytes)
{
    auto headIter = std::search(bytes.cbegin(), bytes.cend(), head.cbegin(), head.cend());
    auto tailIter = std::search(bytes.cbegin(), bytes.cend(), tail.cbegin(), tail.cend());
    if (headIter == bytes.cend() || tailIter == bytes.cend()) {
        return -1; //Frame's head or tail search failed
    }
    std::string byteString;
    for (auto frameIter = headIter + head.size(); frameIter < tailIter; frameIter++) {
        byteString.push_back(*frameIter);
    }
    cJSON *jsonRoot = NULL;
    cJSON *jsonType = NULL;
    cJSON *jsonChannels = NULL;
    cJSON *jsonTime = NULL;
    cJSON *jsonCommand = NULL;
    try {
        jsonRoot = cJSON_Parse(byteString.c_str());
        if (jsonRoot == NULL) throw std::runtime_error("Root");
        jsonType = cJSON_GetObjectItem(jsonRoot, "type");
        if (jsonType == NULL) throw std::runtime_error("type");
        jsonChannels = cJSON_GetObjectItem(jsonRoot, "channels");
        if (jsonChannels == NULL) throw std::runtime_error("channels");
        jsonTime = cJSON_GetObjectItem(jsonRoot, "time");
        if (jsonTime == NULL) throw std::runtime_error("time");
        jsonCommand = cJSON_GetObjectItem(jsonRoot, "cmd");
        if (jsonCommand == NULL) throw std::runtime_error("cmd");
    } catch (const std::runtime_error& error) {
        std::cout << "Parse error: " << error.what() << std::endl;
        std::cout << byteString << std::endl;
        if (jsonRoot != NULL) cJSON_Delete(jsonRoot);
        if (jsonType != NULL) cJSON_Delete(jsonType);
        if (jsonChannels != NULL) cJSON_Delete(jsonChannels);
        if (jsonTime != NULL) cJSON_Delete(jsonTime);
        return -2; //Parse error
    }
    auto frameBody = new struct FrameBody;
    frameBody->deviceType = static_cast<enum DeviceType>(jsonType->valueint);
    for (int idx = 0; idx < cJSON_GetArraySize(jsonChannels); ++idx) {
        int channel = cJSON_GetArrayItem(jsonChannels, idx)->valueint;
        frameBody->channels.push_back(static_cast<touch_pad_t>(channel));
    }
    frameBody->payloadTime = jsonTime->valueint;
    frameBody->command = static_cast<enum ControlCommand>(jsonCommand->valueint);
    xSemaphoreTake(frameMutex, portMAX_DELAY);
    frameBuffer.insert(frameBuffer.begin(), frameBody);
    xSemaphoreGive(frameMutex);
    cJSON_Delete(jsonRoot);
    cJSON_Delete(jsonType);
    cJSON_Delete(jsonChannels);
    cJSON_Delete(jsonTime);
    cJSON_Delete(jsonCommand);
    return bytes.size();
}

int FrameDecoder::GetOneFrame(FrameDecoder::FrameBody &frameBody)
{
    xSemaphoreTake(frameMutex, portMAX_DELAY);
    if (frameBuffer.empty()) {
        xSemaphoreGive(frameMutex);
        return -1;
    }
    frameBody = *frameBuffer.back();
    delete frameBuffer.back();
    frameBuffer.pop_back();
    xSemaphoreGive(frameMutex);
    return 0;
}
