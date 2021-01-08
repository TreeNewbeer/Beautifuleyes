//
// Created by genius on 1/7/21.
//

#include "SignalChart.h"

SignalChart::SignalChart(QWidget *parent, const QString& chartTitle, int chartPointSize) {
    pointsSize = chartPointSize;
    signalChart = new QChart();
    signalLabel = new QLabel();
    signalLabel->hide();
    signalChart->legend()->setVisible(true);
    signalChart->setTheme(QChart::ChartThemeBlueIcy);
    signalChart->setTitle(chartTitle);
    signalChart->setAnimationOptions(QChart::NoAnimation);
    signalChartView = new QChartView(signalChart, parent);
    signalChartView->setRenderHint(QPainter::Antialiasing);
}

SignalChart::~SignalChart() {
    for (const auto& signalLine : signalLines) {
        if (signalLine == nullptr) {
            continue;
        }
        delete signalLine;
    }
    delete signalLabel;
    delete signalChart;
    delete signalChartView;
}

void SignalChart::AddPoint(int channel, qreal x, qreal y) {
    auto signalLine = signalLines[channel - 1];
    if (signalLine->points().size() >= pointsSize) {
        signalLine->remove(0);
    }
    signalLine->append(x, y);
}

void SignalChart::ShowPointInfo(const QPointF &pointPos, bool pointState) {
    if (pointState && QGuiApplication::keyboardModifiers() == Qt::ControlModifier) {
        signalLabel->setText(QString::asprintf("%f", pointPos.y()));
        signalLabel->move(QCursor::pos().x(), QCursor::pos().y());
        signalLabel->show();
    } else {
        signalLabel->hide();
    }
}

int SignalChart::AddLine(int channel) {
    if (channel < 1 || channel > 14) {
        qDebug() << "channel input error";
        return -1;
    }
    signalLines[channel - 1] = new QLineSeries();
    auto signalLine = signalLines[channel - 1];
    signalChart->addSeries(signalLine);
    signalChart->createDefaultAxes();
    connect(signalLine, &QScatterSeries::hovered, this, &SignalChart::ShowPointInfo);
    return 0;
}

void SignalChart::UpdateAxis() {
    qreal yMax = 0;
    qreal yMin = 0;
    qreal xMax = 0;
    qreal xMin = 0;
    for (const auto& signalLine : signalLines) {
        if (signalLine == nullptr) {
            continue;
        }
        for (const auto& point : signalLine->points()) {
            if (xMax == 0 && xMin == 0) {
                xMin = point.x();
                xMax = point.x();
                yMin = point.y();
                yMax = point.y();
            }
            xMin = qMin(xMin, point.x());
            xMax = qMax(xMax, point.x());
            yMin = qMin(yMin, point.y());
            yMax = qMax(yMax, point.y());
        }
    }
    signalChart->axisX()->setRange(xMin - qAbs(xMax - xMin) / 10, xMax + qAbs(xMax - xMin) / 10);
    if (yMax == yMin) {
        signalChart->axisY()->setRange(yMin - qAbs(yMin) / 10, yMax + qAbs(yMax) / 10);
    } else {
        signalChart->axisY()->setRange(yMin - qAbs(yMax - yMin) / 10, yMax + qAbs(yMax - yMin) / 10);
    }
}
