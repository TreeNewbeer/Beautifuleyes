//
// Created by genius on 12/24/20.
//

#ifndef QT_UART_CMAKE_UART_H
#define QT_UART_CMAKE_UART_H
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QJsonValue>
#include <QTimer>
#include <QThread>
#include <QMutex>

class FrameEncoder {
public:
    enum DeviceType {
        Channel = 0,
        Button = 1,
        Slider = 2,
        Matrix = 3,
    };

    enum ControlCommand {
        Start,
        Stop
    };

    struct FrameBody {
        DeviceType deviceType;
        QVector<QJsonValue> channels;
        QJsonValue payloadTime;
        ControlCommand command;
    };
    explicit FrameEncoder(const QString& frameHead = "$*", const QString& frameTail = "$*");
    ~FrameEncoder();
    int AddToFrameBuffer(const FrameBody& frameBody);
    int GetOneFrame(QString& frameString);

private:
    QString frameHead;
    QString frameTail;
    QMutex frameMutex;
    QVector<QString*> frameBuffer;
};


class FrameDecoder : public QObject {
Q_OBJECT
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

    FrameDecoder(const QString& frameHead, const QString& frameTail);
    ~FrameDecoder();
    int AddToFrameBuffer(const QByteArray& bytes);
    int GetOneFrame(struct FrameData &frame);

protected:
    QMutex frameMutex;
    QByteArray head;
    QByteArray tail;
    QVector<struct FrameData*> frameBuffer;
};

class Uart : public FrameDecoder {
    Q_OBJECT
public:
    explicit Uart();
    ~Uart();
    bool UartOpen(const QString &port_name, const int &baud_rate, const int &data_bit, const int &stop_bit);
    int UartSend(QString &sendStr);
    void UartClose();

private:
    FrameEncoder frameEncoder;
    QThread uartThread;
    QSerialPort serialPort;
    QByteArray uartBuffer;

private slots:
    void UartReadReadySlot();
};


#endif //QT_UART_CMAKE_UART_H
