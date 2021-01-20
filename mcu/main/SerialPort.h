//
// Created by genius on 1/10/21.
//

#ifndef MCU_FIRMWARE_UART_H
#define MCU_FIRMWARE_SERIALPORT_H

#include <iostream>
#include <array>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/uart.h"
#include "driver/gpio.h"

class SerialPort{
public:
    explicit SerialPort(int uartPort = 0, int baudRate = 115200, gpio_num_t txPin = GPIO_NUM_43, gpio_num_t rxPin = GPIO_NUM_44,
                        int txBufferSize = 1024, int rxBufferSize = 1024);
    ~SerialPort();

    template<typename Container>
    void TransmitMessage(const Container& container) {
        xSemaphoreTake(transmitMutex, portMAX_DELAY);
        std::copy(container.cbegin(), container.cend(), std::back_inserter(transmitBuffer));
        xSemaphoreGive(transmitMutex);
    }

    template<typename Container>
    int ReceiveMessage(Container& container) {
        xSemaphoreTake(receiveMutex, portMAX_DELAY);
        if (receiveBuffer.empty()) {
            xSemaphoreGive(receiveMutex);
            return -1;
        }
        std::copy(receiveBuffer.cbegin(), receiveBuffer.cend(), std::back_inserter(container));
        int copySize = receiveBuffer.size();
        receiveBuffer.clear();
        xSemaphoreGive(receiveMutex);
        return copySize;
    }

private:
    std::vector<uint8_t> receiveBuffer;
    std::vector<uint8_t> transmitBuffer;
    TaskHandle_t receiveTaskHandle;
    TaskHandle_t transmitTaskHandle;
    SemaphoreHandle_t receiveMutex;
    SemaphoreHandle_t transmitMutex;
    int uartPort;
    static void uartReceiveTask(void *arg);
    static void uartTransmitTask(void *arg);
    static const auto uartBlockSize = 100;
};


#endif //MCU_FIRMWARE_UART_H
