#include "mainwindow.h"
#include "ui_mainwindow.h"



#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    uart = new Uart();

    rawChart = new SignalChart(ui->rawFrame, "Raw signal");
    ui->rawFrameLayout->addWidget(rawChart->signalChartView);
    rawDiffChart = new SignalChart(ui->rawFrame, "Raw signal Diff rate");
    ui->rawFrameLayout->addWidget(rawDiffChart->signalChartView);

    bcmChart = new SignalChart(ui->bcmFrame, "Benchmark signal");
    ui->bcmFrameLayout->addWidget(bcmChart->signalChartView);
    bcmDiffChart = new SignalChart(ui->bcmFrame, "Benchmark signal Diff rate");
    ui->bcmFrameLayout->addWidget(bcmDiffChart->signalChartView);

    timer_handler.start(100);
    data_update_timer.start(1);
    connect(&timer_handler, &QTimer::timeout, this, &MainWindow::timer_callback);
    connect(&data_update_timer, &QTimer::timeout, this, &MainWindow::data_update);
}

MainWindow::~MainWindow()
{
    delete uart;
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
        if (!uart->uart_open(portname, baudrate, stopbit, databit)) {
            ui->button_state->setText("Start");
            qDebug() << "Open port failed" << Qt::endl;
        }
    } else {
        ui->button_state->setText("Start");  //stop
        uart->uart_close();
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

void MainWindow::data_update() {
    struct FrameDecoder::FrameData frameData{};
    while (uart->GetOneFrame(frameData) == 0) {
        static auto x = -1;
        static auto raw_base = 0;
        static auto raw_base_temp = 0;
        static auto bcm_base = 0;
        static auto bcm_base_temp = 0;

        qreal raw_y = frameData.frameSignal.raw;
        qreal bcm_y = frameData.frameSignal.benchmark;
        x++;
        if (x < 20) {
            raw_base_temp += raw_y;
            bcm_base_temp += bcm_y;
            return;
        } else if (x == 20) {
            raw_base = raw_base_temp / 20;
            bcm_base = bcm_base_temp / 20;
        }

        if (raw_base == 0) {
            return;
        }
        rawChart->SignalAddPoint(x, raw_y);
        rawDiffChart->SignalAddPoint(x, (raw_y - raw_base) / raw_base);
        bcmChart->SignalAddPoint(x, bcm_y);
        bcmDiffChart->SignalAddPoint(x, (bcm_y - bcm_base) / bcm_base);
    }
}
