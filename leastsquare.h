#ifndef LEASTSQUARE_H
#define LEASTSQUARE_H

#include <QWidget>
#include <QTableWidgetItem>

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

    void on_twAverage_itemDoubleClicked(QTableWidgetItem *item);

    void on_twAverage_itemChanged(QTableWidgetItem *item);

signals:
    void tableDataXYChanged(QVector<double> x, QVector<double> y);
    void fitDataChanged(QVector<double> x, QVector<double> y);

private:
    Ui::LeastSquare *ui;
    int order;          // 最小二乘法多项式阶数
    int samplePointSum; // 标定点数
    QVector<double> tableDataX, tableDataY;
    QVector<double> fitDataX, fitDataY;
    QVector<double> factor;
    QRegExp rx;
    QString old_text;

    void updateTableDataXY(void);
};

#endif // LEASTSQUARE_H
