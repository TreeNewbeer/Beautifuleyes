#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "cmake-build-debug/qt_uart_cmake_autogen/include/cmake-build-debug/qt_uart_cmake_autogen/include/ui_mainwindow.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    chart = new QChart();
    line_series = new QLineSeries();
    chart->legend()->setVisible(true);
    chart->addSeries(line_series);
    chart->createDefaultAxes();
    chart->setTitle("Raw signal");
    chart->setAnimationOptions(QChart::NoAnimation);
    chart_view = new QChartView(chart, ui->horizontalFrame_2);
    chart_view->setRenderHint(QPainter::Antialiasing);
    ui->bodyLayout->addWidget(chart_view);
    data_update_timer.start(5);
    connect(&data_update_timer, &QTimer::timeout, this, &MainWindow::data_update);

    timer_handler.start(100);
    connect(&timer_handler, &QTimer::timeout, this, &MainWindow::timer_callback);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_state_clicked()
{
    if (ui->button_state->text() == (QString)"Start") {
        ui->button_state->setText("Stop");  //start
        QString portname = ui->combo_port->currentText();
        int baudrate = ui->combo_baud->currentText().toInt();
        QSerialPort::StopBits stopbit = (QSerialPort::StopBits)ui->combo_stopbits->currentText().toInt();
        QSerialPort::DataBits databit = (QSerialPort::DataBits)ui->combo_databits->currentText().toInt();
        if (!uart.uart_open(portname, baudrate, stopbit, databit)) {
            ui->button_state->setText("Start");
            qDebug() << "Open port failed" << Qt::endl;
        }
    } else {
        ui->button_state->setText("Start");  //stop
        uart.uart_close();
    }
}

void MainWindow::timer_callback() {
    auto port_infos = QSerialPortInfo::availablePorts();
    if (ui->combo_port->count() != port_infos.size()) {
        ui->combo_port->clear();
        for (auto port_info : port_infos) {
            ui->combo_port->addItem(port_info.portName());
        }
    }
}

#include <iostream>
#include <sys/time.h>

void MainWindow::data_update() {
    struct FrameDecoder::FrameData frameData{};
    if (uart.GetOneFrame(frameData) != 0) {
        return;
    }
    static auto x = 0;
    qreal y = frameData.frameSignal.raw;

    if (x < 100) {
        line_series->append(x++, y);
    } else {
        line_series->remove(0);
        line_series->append(x++, y);
    }
    QVector<qreal> x_vec;
    QVector<qreal> y_vec;
    for (const auto& point : line_series->points()) {
        x_vec.append(point.x());
        y_vec.append(point.y());
    }
    auto x_min = std::min_element(x_vec.begin(), x_vec.end());
    auto x_max = std::max_element(x_vec.begin(), x_vec.end());
    auto y_min = std::min_element(y_vec.begin(), y_vec.end());
    auto y_max = std::max_element(y_vec.begin(), y_vec.end());
    chart->axisX()->setRange(*x_min - (*x_max - *x_min) / 10, *x_max + (*x_max - *x_min) / 10);
    chart->axisY()->setRange(*y_min - (*y_max - *y_min) / 10, *y_max + (*y_max - *y_min) / 10);
}
