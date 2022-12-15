#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
using namespace Eigen;

#include "leastsquare.h"
#include "fitchart.h"
#include "ui_leastsquare.h"
#include <QDebug>
#include <QString>
#include <QMessageBox>

/*
    最小二乘法
    https://blog.csdn.net/weixin_44344462/article/details/88850409

    A W = B
    AT A W = AT B
    (AT A)^(-1) AT A W = (AT A)^(-1) AT B
    W = (AT A)^(-1) AT B
*/
// 设置是N阶拟合
QVector<double> method(int N, QVector<double> x, QVector<double> y)
{
    // 防御检查
    if (x.size() != y.size())
    {
        qDebug() << "format error!";
        return QVector<double>();
    }

    // 创建A矩阵
    MatrixXd A(x.size(), N + 1);
    for (int i = 0; i < x.size(); ++i) // 遍历所有点
    {
        for (int n = N, dex = 0; n >= 1; --n, ++dex) // 遍历N到1阶
        {
            A(i, dex) = pow(x.at(i), n);
        }

        A(i, N) = 1; //
    }

    // 创建B矩阵
    MatrixXd B(y.size(), 1);
    for (int i = 0; i < y.size(); ++i)
    {
        B(i, 0) = y.at(i);
    }

    // 创建矩阵W
    MatrixXd W;
    W = (A.transpose() * A).inverse() * A.transpose() * B;

    // 打印W结果
    qDebug() << "Factor:";
    QVector<double> ans;
    for (int i = 0; i <= N; i++)
    {
        double temp = W(i, 0);
        if (abs(temp) >= 1e-10)
            ans << temp;
        else
            ans << 0;
        qDebug() << temp;
    }
    return ans;
}

void generateData(int left, int right, double step, QVector<double> &factor, QVector<double> &x, QVector<double> &y)
{
    if (left > right)
    {
        int temp = right;
        right = left;
        left = temp;
    }
    x.clear();
    y.clear();
    int order = factor.length();
    double temp;
    for (double i = left; i <= right; i += step)
    {
        x << i;
        temp = factor.at(order - 1);
        for (int j = 1; j < order; j++)
        {
            temp += factor.at(order - 1 - j) * pow(i, j);
        }
        y << temp;
        qDebug() << "generate:" << i << temp;
    }
}

/*
    qt中获取容器Vector中的最大值和最小值：
    https://blog.csdn.net/Littlehero_121/article/details/100565527
*/
double max(QVector<double> &data)
{
    auto max = std::max_element(std::begin(data), std::end(data));
    return *max;
}

double min(QVector<double> &data)
{
    auto min = std::min_element(std::begin(data), std::end(data));
    return *min;
}

LeastSquare::LeastSquare(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::LeastSquare)
{
    ui->setupUi(this);

    samplePointSum = ui->twAverage->rowCount();
    order = ui->spbxOrder->value();

    ui->twAverage->horizontalHeader()->setVisible(true);
    ui->twAverage->verticalHeader()->setVisible(true);
    ui->twAverage->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 自适应缩放
    ui->twAverage->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);     // 不可调整
    ui->twFactor->horizontalHeader()->setVisible(true);

    ui->twFactor->verticalHeader()->setVisible(true);
    ui->twFactor->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 自适应缩放
    ui->twFactor->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);     // 不可调整

    QStringList HorizontalHeader;
    HorizontalHeader << "X:标准平均";
    HorizontalHeader << "Y:待定平均";
    ui->twAverage->setHorizontalHeaderLabels(HorizontalHeader); // 设置表头

    HorizontalHeader.clear();
    HorizontalHeader << "N-1 阶系数";
    ui->twFactor->setHorizontalHeaderLabels(HorizontalHeader); // 设置表头

    for (int i = 0; i < samplePointSum; i++) // 需要初始化表格Item
    {
        for (int j = 0; j < 2; j++)
        {
            QTableWidgetItem *temp = new QTableWidgetItem();
            ui->twAverage->setItem(i, j, temp);
            ui->twAverage->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }

    connect(this, &LeastSquare::collectDataXYChanged, ui->chartFit, &FitChart::updateCollectPlot);
    connect(this, &LeastSquare::fitDataChanged, ui->chartFit, &FitChart::updateFitPlot);
}

LeastSquare::~LeastSquare()
{
    delete ui;
}

void LeastSquare::on_spbxSamplePointSum_valueChanged(int arg1)
{
    ui->twAverage->setRowCount(arg1);
    if (arg1 > samplePointSum)
    {
        for (int j = 0; j < 2; j++) // 需要初始化表格Item
        {
            QTableWidgetItem *temp = new QTableWidgetItem();
            int row = arg1 - 1;
            ui->twAverage->setItem(row, j, temp);
            ui->twAverage->item(row, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
    samplePointSum = arg1;
    qDebug() << "samplePointSum:" << samplePointSum;
}

void LeastSquare::on_spbxOrder_valueChanged(int arg1)
{
    order = arg1;
    ui->twFactor->setRowCount(arg1 + 1);
    qDebug() << "order:" << order;
    emit;
}

void LeastSquare::updateTableDataXY(void)
{
    QString qsX, qsY;
    collectDataX.clear(); // 重置x容器
    collectDataY.clear(); // 重置y容器
    for (int i = 0; i < samplePointSum; i++)
    {
        qsX = ui->twAverage->item(i, 0)->text();
        qsY = ui->twAverage->item(i, 1)->text();
        if (qsX.isEmpty() || qsY.isEmpty())
            continue;
        // qDebug() << "counter" << counter;

        collectDataX << qsX.toDouble();
        collectDataY << qsY.toDouble();
        qDebug() << i << ":" << qsX << qsY;
    }
}

void LeastSquare::on_btnFit_clicked()
{
    updateTableDataXY();

    if (collectDataX.length() < 2)
    {
        QMessageBox::critical(this, "错误", "正确格式的数据\n小于两组");
        return;
    }
    factor.clear();
    factor = method(order, collectDataX, collectDataY);
    for (int i = 0; i <= order; i++)
    {
        // 通过setItem来改变条目
        QTableWidgetItem *temp = new QTableWidgetItem(QString::number(factor.at(order - i)));
        ui->twFactor->setItem(i, 0, temp);
    }
    collectDataX_Max = max(collectDataX);
    collectDataX_Min = min(collectDataX);
    double addRange = (collectDataX_Max - collectDataX_Min) / 4.0;
    generateData(collectDataX_Min - addRange, collectDataX_Max + addRange, 0.25, factor, fitDataX, fitDataY);
    emit fitDataChanged(fitDataX, fitDataY);
}

void LeastSquare::on_twAverage_itemDoubleClicked(QTableWidgetItem *item)
{
    // 1、记录旧的单元格内容
    old_text = item->text();
}

/*
 * 正则表达式：
 * https://blog.csdn.net/qq_41622002/article/details/107488528
 */
void LeastSquare::on_twAverage_itemChanged(QTableWidgetItem *item)
{
    // 2、匹配正负整数、正负浮点数
    QString Pattern("(-?[1-9][0-9]+)|(-?[0-9])|(-?[1-9]\\d+\\.\\d+)|(-?[0-9]\\.\\d+)"); // 正则表达式
    QRegExp reg(Pattern);

    // 3.获取修改的新的单元格内容
    QString str = item->text();

    // 完全匹配
    if (reg.exactMatch(str))
    {
        qDebug() << "匹配成功";
        int row = item->row();
        if (ui->twAverage->item(row, 0)->text().isEmpty() ||
            ui->twAverage->item(row, 1)->text().isEmpty())
            return; // 当有一格为空则退出

        updateTableDataXY();
        if (collectDataX.length() > 0)
            emit collectDataXYChanged(collectDataX, collectDataY);
    }
    else
    {
        qDebug() << "匹配失败";
        item->setText(old_text); // 更换之前的内容
    }
}
