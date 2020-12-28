//
// Created by genius on 12/24/20.
//

#ifndef QT_UART_CMAKE_UART_H
#define QT_UART_CMAKE_UART_H
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QThread>


class FrameDecoder : public QObject {
Q_OBJECT
public:
    FrameDecoder(const QString& frameHead, const QString& frameTail);
    ~FrameDecoder();

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

    };

public slots:
    void AddFrame(const QByteArray& bytes);

protected:
    QByteArray head;
    QByteArray tail;

private:
    QVector<QJsonObject> frames_fifo;
};


class Uart : public FrameDecoder {
    Q_OBJECT
public:
    explicit Uart();
    bool uart_open(const QString &port_name, const int &baud_rate, const int &data_bit, const int &stop_bit);
    void uart_close();
    QVector<QJsonObject> json_queue;

private:
    QThread uart_thread;
    QSerialPort serial_port;
    QByteArray uart_data_buffer;

private slots:
    void uart_read_ready();
};


#endif //QT_UART_CMAKE_UART_H
