//
// Created by genius on 12/24/20.
//

#include "Uart.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>

FrameEncoder::FrameEncoder(const QString& frameHead, const QString& frameTail)
{
    this->frameHead = frameHead;
    this->frameTail = frameTail;
}

FrameEncoder::~FrameEncoder()
{
    qDebug() << "FrameEncoder delete";
}

int FrameEncoder::AddToFrameBuffer(const FrameEncoder::FrameBody &frameBody)
{
    QJsonObject jsonRoot;
    jsonRoot["type"] = frameBody.deviceType;
    QJsonArray channelArray;
    std::copy(frameBody.channels.cbegin(), frameBody.channels.cend(), std::back_inserter(channelArray));
    jsonRoot["channels"] = channelArray;
    jsonRoot["time"] = frameBody.payloadTime;
    jsonRoot["cmd"] = frameBody.command;
    QJsonDocument jsonDocument(jsonRoot);
    auto jsonString = new QString(frameHead + jsonDocument.toJson(QJsonDocument::Compact) + frameTail);
    frameMutex.lock();
    frameBuffer.push_front(jsonString);
    frameMutex.unlock();
    return 0;
}

//$*{"type":1,"channels":[1,2,3,4],"time":50,"cmd":0}*$
int FrameEncoder::GetOneFrame(QString &frameString)
{
    frameMutex.lock();
    if (frameBuffer.isEmpty()) {
        frameMutex.unlock();
        return -1;
    }
    auto framePtr = frameBuffer.back();
    std::copy(framePtr->cbegin(), framePtr->cend(), std::back_inserter(frameString));
    frameBuffer.pop_back();
    frameMutex.unlock();
    return 0;
}

FrameDecoder::FrameDecoder(const QString& frameHead, const QString& frameTail) {
    qDebug() << "FrameDecoder create";
    for (const auto& head_str : frameHead) {
        this->head.append(head_str.toLatin1());
    }
    for (const auto& tail_str : frameTail) {
        this->tail.append(tail_str.toLatin1());
    }
}

FrameDecoder::~FrameDecoder() {
    qDebug() << "FrameDecoder delete";
}

int FrameDecoder::AddToFrameBuffer(const QByteArray &bytes) {
    auto head_index = bytes.indexOf(head, 0);
    auto tail_index = bytes.indexOf(tail, 0);
    if (head_index == -1 || tail_index == -1) {
        return -1;
    }
    QByteArray frame_bytes;
    for (auto index = head_index + head.size(); index < tail_index; index++) {
        frame_bytes.append(bytes.at(index));
    }
    QJsonObject json_raw = QJsonDocument::fromJson(frame_bytes).object();
    if (json_raw.isEmpty()) {
        return -2;
    }
    auto deviceType = json_raw.find("type")->toInt();
    auto json_signal_raw = json_raw.find("signal")->toObject();
    if (json_signal_raw.isEmpty()) {
        return -2;
    }
    int id = json_signal_raw.find("id")->toInt();
    uint32_t raw_signal = json_signal_raw.find("raw")->toInt();
    uint32_t smooth_signal = json_signal_raw.find("smo")->toInt();
    uint32_t benchmark_signal = json_signal_raw.find("bcm")->toInt();
    auto frame_body = new struct FrameData;
    frame_body->deviceType = static_cast<DeviceType>(deviceType);
    frame_body->frameSignal.id = id;
    frame_body->frameSignal.raw = raw_signal;
    frame_body->frameSignal.smooth = smooth_signal;
    frame_body->frameSignal.benchmark = benchmark_signal;
    frameMutex.lock();
    frameBuffer.push_front(frame_body);
    frameMutex.unlock();
    return (tail_index - (head_index + head.size()));
}

int FrameDecoder::GetOneFrame(struct FrameData& frame) {
    frameMutex.lock();
    if (frameBuffer.isEmpty()) {
        frameMutex.unlock();
        return -1;
    }
    auto current_frame =  frameBuffer.last();
    frame.deviceType = current_frame->deviceType;
    frame.frameSignal.raw = current_frame->frameSignal.raw;
    frame.frameSignal.smooth = current_frame->frameSignal.smooth;
    frame.frameSignal.benchmark = current_frame->frameSignal.benchmark;
    frameBuffer.pop_back();
    delete current_frame;
    frameMutex.unlock();
    return 0;
}


Uart::Uart() : FrameDecoder("$*", "*$") {
    moveToThread(&uartThread);
    uartThread.start();
}

Uart::~Uart() {

}

void Uart::UartReadReadySlot() {
    int bytes = serialPort.bytesAvailable();
    if (bytes <= 0) {
        return;
    }
    uartBuffer.append(serialPort.read(bytes));
    uartBuffer.replace("\r\n", "");
    while (uartBuffer.contains(head) && uartBuffer.contains(tail)) {
        int headIndex = uartBuffer.indexOf(head);
        int tailIndex = uartBuffer.indexOf(tail);
        if (headIndex >= tailIndex) {
            uartBuffer.remove(0, headIndex);
        }
        int process_length = AddToFrameBuffer(uartBuffer);
        if (process_length >= 0) {
            uartBuffer.remove(0, process_length + head.size() + tail.size());
        } else if (process_length == -2) {
            uartBuffer.clear();
        }
    }
}

bool Uart::UartOpen(const QString &port_name, const int &baud_rate, const int &data_bit, const int &stop_bit) {
    connect(&serialPort, &QSerialPort::readyRead, this, &Uart::UartReadReadySlot, Qt::DirectConnection);
    serialPort.setPortName(port_name);
    serialPort.setBaudRate(baud_rate);
    serialPort.setDataBits((QSerialPort::DataBits)data_bit);
    serialPort.setStopBits((QSerialPort::StopBits)stop_bit);
    qDebug() << port_name << baud_rate  << Qt::endl;
    bool ret = serialPort.open(QIODevice::ReadWrite);
    return ret;
}

void Uart::UartClose() {
    serialPort.waitForBytesWritten(-1);
    disconnect(&serialPort, &QSerialPort::readyRead, this, &Uart::UartReadReadySlot);
    serialPort.close();
    uartBuffer.clear();
    frameMutex.lock();
    frameBuffer.clear();
    frameMutex.unlock();
}

int Uart::UartSend(QString &sendStr)
{
    int ret = serialPort.write(sendStr.toLocal8Bit().data());
    return ret;
}
