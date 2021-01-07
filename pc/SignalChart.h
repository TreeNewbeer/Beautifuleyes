//
// Created by genius on 1/7/21.
//

#ifndef QT_UART_CMAKE_SIGNALCHART_H
#define QT_UART_CMAKE_SIGNALCHART_H

#include <QtCharts>

class SignalChart {
public:
    SignalChart(QWidget *parent = nullptr, const QString& chartTitle = "Title", int chartPointSize = 500);
    ~SignalChart();
    void SignalAddPoint(qreal x, qreal y);
    QChartView *signalChartView;

private:
    int pointsSize;
    QLineSeries *signalLine;
    QChart *signalChart;
};


#endif //QT_UART_CMAKE_SIGNALCHART_H
