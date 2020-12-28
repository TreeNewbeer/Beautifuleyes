//
// Created by genius on 12/24/20.
//

#include "Uart.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

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
    auto json_signal_raw = json_raw.find("signal")->toObject();
    int id = json_signal_raw.find("id")->toInt();
    uint32_t raw_signal = json_signal_raw.find("raw")->toInt();
    uint32_t smooth_signal = json_signal_raw.find("smooth")->toInt();
    uint32_t benchmark_signal = json_signal_raw.find("filter")->toInt();
    auto frame_body = new struct FrameData;
    frame_body->deviceType = Channel;
    frame_body->frameSignal.id = id;
    frame_body->frameSignal.raw = raw_signal;
    frame_body->frameSignal.smooth = smooth_signal;
    frame_body->frameSignal.benchmark = benchmark_signal;
    frameMutex.lock();
    frameBuffer.push_front(frame_body);
    frameMutex.unlock();
    return 0;
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
    connect(&serial_port, &QSerialPort::readyRead, this, &Uart::uart_read_ready);
    moveToThread(&uart_thread);
    uart_thread.start();
}

void Uart::uart_read_ready() {
    uart_data_buffer.append(serial_port.readAll());
    auto start_index = uart_data_buffer.indexOf("$*", 0);
    auto end_index = uart_data_buffer.indexOf("*$", 0);
    if (start_index != -1 && end_index != -1) {
        if (AddToFrameBuffer(uart_data_buffer) == 0) {

        }
        uart_data_buffer.clear();
    }
}

bool Uart::uart_open(const QString &port_name, const int &baud_rate, const int &data_bit, const int &stop_bit) {
    serial_port.setPortName(port_name);
    serial_port.setBaudRate(baud_rate);
    serial_port.setDataBits((QSerialPort::DataBits)data_bit);
    serial_port.setStopBits((QSerialPort::StopBits)stop_bit);
    qDebug() << port_name << baud_rate  << Qt::endl;
    return serial_port.open(QIODevice::ReadOnly);
}

void Uart::uart_close() {
    serial_port.close();
}
