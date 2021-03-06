#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Uart.h"
#include "SignalChart.h"

#include <QMainWindow>
#include <QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    struct WidgetsInfos {
        QString uartPortName;
        int uartBaudRate;
        int uartStopBit;
        int uartDataBit;
        std::array<bool, 14> channelBoxStates;
    };
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void open_uart_signal(const QString& port, const int& baud_rate);

private slots:
    void on_startButton_clicked();
    void timer_callback();
    void data_update();

private:
    void ScanWidgetsInfos();
    void SetWidgetsDisabled(bool Disabled);

    Ui::MainWindow *ui;
    Uart *uart;
    QTimer timer_handler;
    QTimer data_update_timer;
    SignalChart *rawChart;
    SignalChart *rawDiffChart;
    SignalChart *bcmChart;
    SignalChart *bcmDiffChart;

    struct WidgetsInfos widgetsInfos {};
};
#endif // MAINWINDOW_H
