//
// Created by genius on 1/7/21.
//

#ifndef QT_UART_CMAKE_SIGNALCHART_H
#define QT_UART_CMAKE_SIGNALCHART_H

#include <array>
#include <QtCharts>

class SignalChart : public QObject {
    Q_OBJECT
public:
    SignalChart(QWidget *parent = nullptr, const QString& chartTitle = "Title", int chartPointSize = 500);
    ~SignalChart();
    int AddLine(int channel);
    void AddPoint(int channel, qreal x, qreal y);
    void UpdateAxis();
    QChartView *signalChartView;

private:
    int pointsSize;
    QChart *signalChart;
    QLabel *signalLabel;
    std::array<QLineSeries*, 14> signalLines{nullptr};

private slots:
    void ShowPointInfo(const QPointF& pointPos, bool pointState);
};


#endif //QT_UART_CMAKE_SIGNALCHART_H
