//
// Created by genius on 1/10/21.
//

#include <iostream>
#include "SerialPort.h"

SerialPort::SerialPort(int uartPort, int baudRate, gpio_num_t txPin, gpio_num_t rxPin, int txBufferSize, int rxBufferSize) {
    this->uartPort = uartPort;
    uart_config_t uartConfig = {};
    uartConfig.baud_rate = baudRate;
    uartConfig.data_bits = UART_DATA_8_BITS;
    uartConfig.parity = UART_PARITY_DISABLE;
    uartConfig.stop_bits = UART_STOP_BITS_1;
    uartConfig.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uartConfig.source_clk = UART_SCLK_APB;
    esp_err_t ret;
    BaseType_t os_ret;
    try {
        ret = uart_driver_install(uartPort, rxBufferSize, txBufferSize, 0, nullptr, 0);
        if (ret != ESP_OK)  throw std::runtime_error("uart driver install failed");
        ret = uart_param_config(uartPort, &uartConfig);
        if (ret != ESP_OK)  throw std::runtime_error("uart driver parameter config failed");
        ret = uart_set_pin(uartPort, txPin, rxPin, -1, -1);
        if (ret != ESP_OK)  throw std::runtime_error("uart driver set pin failed");
        receiveMutex = xSemaphoreCreateMutex();
        transmitMutex = xSemaphoreCreateMutex();
        if (receiveMutex == NULL || transmitMutex == NULL)  throw std::runtime_error("Mutex create failed, no memory");
        os_ret = xTaskCreate(uartReceiveTask, "uartReceiveTask", 4 * 1024, this, 10, &receiveTaskHandle);
        if (os_ret != pdPASS)   throw std::runtime_error("uartReceiveTask create failed, no memory");
        os_ret = xTaskCreate(uartTransmitTask, "uartTransmitTask", 4 * 1024, this, 9, &transmitTaskHandle);
        if (os_ret != pdPASS)   throw std::runtime_error("uartTransmitTask create failed, no memory");
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void SerialPort::uartReceiveTask(void *arg)
{
    auto* This = static_cast<SerialPort*>(arg);
    while (true) {
        uint8_t readBuffer[This->uartBlockSize];
        int len = uart_read_bytes(UART_NUM_0, readBuffer, This->uartBlockSize, pdTICKS_TO_MS(1));
        if (len <= 0) {
            continue;
        }
        xSemaphoreTake(This->receiveMutex, portMAX_DELAY);
        std::copy(readBuffer, readBuffer + This->uartBlockSize, std::back_inserter(This->receiveBuffer));
        xSemaphoreGive(This->receiveMutex);
    }
}

void SerialPort::uartTransmitTask(void *arg)
{
    auto* This = static_cast<SerialPort*>(arg);
    while (true) {
        try {
            xSemaphoreTake(This->transmitMutex, portMAX_DELAY);
            while (!This->transmitBuffer.empty()) {
                uint8_t writeBuffer[This->uartBlockSize];
                if (This->transmitBuffer.size() < This->uartBlockSize) {
                    std::copy(This->transmitBuffer.cbegin(), This->transmitBuffer.cbegin() + This->transmitBuffer.size(), writeBuffer);
                    int ret = uart_write_bytes(This->uartPort, writeBuffer, This->transmitBuffer.size());
                    if (ret < 0) throw std::runtime_error("uart send bytes failed");
                    This->transmitBuffer.erase(This->transmitBuffer.begin(), This->transmitBuffer.begin() + This->transmitBuffer.size());
                } else {
                    std::copy(This->transmitBuffer.cbegin(), This->transmitBuffer.cbegin() + This->uartBlockSize, writeBuffer);
                    int ret = uart_write_bytes(This->uartPort, writeBuffer, This->uartBlockSize);
                    if (ret < 0) throw std::runtime_error("uart send bytes failed");
                    This->transmitBuffer.erase(This->transmitBuffer.begin(), This->transmitBuffer.begin() + This->uartBlockSize);
                }
            }
            xSemaphoreGive(This->transmitMutex);
            vTaskDelay(pdTICKS_TO_MS(1));
        } catch (const std::runtime_error& e) {
            std::cout << e.what() << std::endl;
        }
    }
}
