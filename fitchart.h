#ifndef FITCHART_H
#define FITCHART_H

#include <QWidget>
#include "qcustomplot.h"

namespace Ui
{
    class FitChart;
}

class FitChart : public QWidget
{
    Q_OBJECT

public:
    explicit FitChart(QWidget *parent = nullptr);
    ~FitChart();

public slots:
    void updateCollectPlot(QVector<double> x, QVector<double> y); // 更新散点图
    void updateFitPlot(QVector<double> x, QVector<double> y);     // 更新折线图

    void addVLine(double x);
    void addHLine(double y);

    void clear();

private slots:
    void axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part);
    void axisXYDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();

    void moveLegend();
    void findGraph();
    void hideCollectPlot();
    void showCollectPlot();
    void hideFitPlot();
    void showFitPlot();

    void contextMenuRequest(QPoint pos);
    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);

private:
    Ui::FitChart *ui;
    unsigned int x_default = 0;
    double xRange = 80;
};

#endif // CUSTOMCHART_H
