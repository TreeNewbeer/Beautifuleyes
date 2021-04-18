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
    rawChart->AddLine(1);
    ui->rawFrameLayout->addWidget(rawChart->signalChartView);
    rawDiffChart = new SignalChart(ui->rawFrame, "Raw signal Diff rate");
    rawDiffChart->AddLine(1);
    ui->rawFrameLayout->addWidget(rawDiffChart->signalChartView);

    bcmChart = new SignalChart(ui->bcmFrame, "Benchmark signal");
    bcmChart->AddLine(1);
    ui->bcmFrameLayout->addWidget(bcmChart->signalChartView);
    bcmDiffChart = new SignalChart(ui->bcmFrame, "Benchmark signal Diff rate");
    bcmDiffChart->AddLine(1);
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

void MainWindow::on_startButton_clicked()
{
    if (ui->startButton->text() == (QString)"Start") {
        ui->startButton->setText("Stop");  //start
        ScanWidgetsInfos();
        if (!uart->UartOpen(widgetsInfos.uartPortName, widgetsInfos.uartBaudRate, widgetsInfos.uartDataBit,
                            widgetsInfos.uartStopBit)) {
            ui->startButton->setText("Start");
            qDebug() << "Open port failed" << Qt::endl;
        } else {
            SetWidgetsDisabled(true);
            FrameEncoder frameEncoder("$*", "*$");
            FrameEncoder::FrameBody frameBody {};
            frameBody.command = FrameEncoder::Start;
            frameBody.deviceType = FrameEncoder::Channel;
            frameBody.payloadTime = 50;
            for (int i = 0; i < widgetsInfos.channelBoxStates.size(); ++i) {
                if(widgetsInfos.channelBoxStates[i]) {
                    frameBody.channels.push_back(i + 1);
                }
            }
            frameEncoder.AddToFrameBuffer(frameBody);
            QString sendStr;
            frameEncoder.GetOneFrame(sendStr);
            qDebug() << sendStr;
            uart->UartSend(sendStr);
        }
    } else {
        ui->startButton->setText("Start");  //stop

        FrameEncoder frameEncoder("$*", "*$");
        FrameEncoder::FrameBody frameBody {};
        frameBody.command = FrameEncoder::Stop;
        frameBody.deviceType = FrameEncoder::Channel;
        frameBody.payloadTime = 50;
        for (int i = 0; i < widgetsInfos.channelBoxStates.size(); ++i) {
            if(widgetsInfos.channelBoxStates[i]) {
                frameBody.channels.push_back(i + 1);
            }
        }
        frameEncoder.AddToFrameBuffer(frameBody);
        QString sendStr;
        frameEncoder.GetOneFrame(sendStr);
        qDebug() << sendStr;
        uart->UartSend(sendStr);
        uart->UartClose();
        SetWidgetsDisabled(false);
    }
}

void MainWindow::timer_callback() {
    auto portInfos = QSerialPortInfo::availablePorts();
    if (ui->portCombo->count() != portInfos.size()) {
        ui->portCombo->clear();
        for (const auto& portInfo : portInfos) {
            ui->portCombo->addItem(portInfo.portName());
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
        rawChart->AddPoint(1, x, raw_y);
        rawDiffChart->AddPoint(1, x, (raw_y - raw_base) / raw_base);
        bcmChart->AddPoint(1, x, bcm_y);
        bcmDiffChart->AddPoint(1, x, (bcm_y - bcm_base) / bcm_base);

        rawChart->UpdateAxis();
        rawDiffChart->UpdateAxis();
        bcmChart->UpdateAxis();
        bcmDiffChart->UpdateAxis();
    }
}

void MainWindow::ScanWidgetsInfos() {
    widgetsInfos.uartPortName = ui->portCombo->currentText();
    widgetsInfos.uartBaudRate = ui->baudCombo->currentText().toInt();
    widgetsInfos.uartDataBit = ui->databitsCombo->currentText().toInt();
    widgetsInfos.uartStopBit = ui->stopbitsCombo->currentText().toInt();

    for (int index = 0; index < ui->channelLayout->count(); index++) {
        auto widget = ui->channelLayout->itemAt(index)->widget();
        auto objectName = widget->objectName();
        if (objectName.contains("channelBox")) {
            auto arrayIndexString = objectName;
            auto arrayIndex = arrayIndexString.replace("channelBox", "").toInt();
            auto channelBox = dynamic_cast<QCheckBox *>(widget);
            widgetsInfos.channelBoxStates[arrayIndex - 1] = channelBox->isChecked();
        }
    }
}

void MainWindow::SetWidgetsDisabled(bool Disabled) {
    for (int index = 0; index < ui->channelLayout->count(); index++) {
        auto widget = ui->channelLayout->itemAt(index)->widget();
        widget->setDisabled(Disabled);
    }

    for (int index = 0; index < ui->uartconfigLayout->count(); index++) {
        auto widget = ui->uartconfigLayout->itemAt(index)->widget();
        widget->setDisabled(Disabled);
    }
}
