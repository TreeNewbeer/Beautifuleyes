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

void FrameDecoder::AddFrame(const QByteArray &bytes) {
    auto head_index = bytes.indexOf(head, 0);
    auto tail_index = bytes.indexOf(tail, 0);
    QByteArray frame_bytes;
    for (auto index = head_index; index < tail_index; index++) {
        frame_bytes.append(bytes.at(index + head.size() - 1));
    }
    QJsonObject json_raw = QJsonDocument::fromJson(frame_bytes).object();
    if (!json_raw.isEmpty()) {
        frames_fifo.push_front(json_raw);
    }
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
    int frame_length = end_index - start_index;
    int frame_head_length = qstrlen("*$");
    if (start_index != -1 && end_index != -1) {
        uart_data_buffer.remove(end_index, uart_data_buffer.length() - frame_length);
        uart_data_buffer.remove(start_index, frame_head_length);
        QJsonObject json;
        json = QJsonDocument::fromJson(uart_data_buffer).object();
        if (!json.isEmpty()) {
            json_queue.push_front(json);
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
