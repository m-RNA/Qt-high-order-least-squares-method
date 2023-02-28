#ifndef LEASTSQUARE_H
#define LEASTSQUARE_H

#include <QWidget>
#include <QTableWidgetItem>
#include "bll_leastssquare.h"

namespace Ui
{
    class LeastSquare;
}

class LeastSquare : public QWidget
{
    Q_OBJECT

public:
    explicit LeastSquare(QWidget *parent = nullptr);
    ~LeastSquare();

public slots:

private slots:
    void on_spbxSamplePointSum_valueChanged(int arg1);

    void on_spbxOrder_valueChanged(int arg1);

    void on_btnFit_clicked();

    void on_twAverage_itemSelectionChanged();

    void twAverage_itemChanged(QTableWidgetItem *item);

    void setFitChartData(vector<DECIMAL_TYPE> factor);

signals:
    void collectDataXYChanged(QVector<double> x, QVector<double> y);
    void fitDataChanged(vector<double> x, vector<double> y);

    void startGenerate(DECIMAL_TYPE t_left, DECIMAL_TYPE t_right, DECIMAL_TYPE t_step, vector<DECIMAL_TYPE> t_factor);
    void startLeastSquare(int t_N, vector<DECIMAL_TYPE> t_x, vector<DECIMAL_TYPE> t_y);

private:
    Ui::LeastSquare *ui;
    unsigned long long order; // 最小二乘法多项式阶数
    int samplePointSum;       // 标定点数
    vector<DECIMAL_TYPE> collectDataX, collectDataY;
    DECIMAL_TYPE collectDataX_Max, collectDataX_Min;
    vector<DECIMAL_TYPE> fitDataX, fitDataY;
    QRegExp rx;
    QString old_text = "";

    // 任务类对象
    Bll_GenerateData *taskGen;
    Bll_LeastSquareMethod *taskLeastSquare;

    void updateCollectDataXY(void);
    void tryUpdateFitChart(bool man);
};

#endif // LEASTSQUARE_H
