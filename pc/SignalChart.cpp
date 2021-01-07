//
// Created by genius on 1/7/21.
//

#include "SignalChart.h"

SignalChart::SignalChart(QWidget *parent, const QString& chartTitle, int chartPointSize) {
    pointsSize = chartPointSize;
    signalChart = new QChart();
    signalLine = new QLineSeries();
    signalChart->legend()->setVisible(true);
    signalChart->setTheme(QChart::ChartThemeDark);
    signalChart->addSeries(signalLine);
    signalChart->createDefaultAxes();
    signalChart->setTitle(chartTitle);
    signalChart->setAnimationOptions(QChart::NoAnimation);
    signalChartView = new QChartView(signalChart, parent);
    signalChartView->setRenderHint(QPainter::Antialiasing);
}

SignalChart::~SignalChart() {
    delete signalLine;
    delete signalChart;
    delete signalChartView;
}

void SignalChart::SignalAddPoint(qreal x, qreal y) {
    if (signalLine->points().size() >= pointsSize) {
        signalLine->remove(0);
    }
    signalLine->append(x, y);
    QVector<qreal> x_vec;
    QVector<qreal> y_vec;
    for (const auto& point : signalLine->points()) {
        x_vec.append(point.x());
        y_vec.append(point.y());
    }
    auto x_min = *std::min_element(x_vec.begin(), x_vec.end());
    auto x_max = *std::max_element(x_vec.begin(), x_vec.end());
    auto y_min = *std::min_element(y_vec.begin(), y_vec.end());
    auto y_max = *std::max_element(y_vec.begin(), y_vec.end());
    signalChart->axisX()->setRange(x_min - (x_max - x_min) / 10, x_max + (x_max - x_min) / 10);
    if (y_min != y_max) {
        signalChart->axisY()->setRange(y_min - (y_max - y_min) / 10, y_max + (y_max - y_min) / 10);
    } else {
        signalChart->axisY()->setRange(y_min - y_min / 10, y_min + y_min / 10);
    }
}
