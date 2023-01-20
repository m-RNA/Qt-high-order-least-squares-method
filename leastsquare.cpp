#include "leastsquare.h"
#include "fitchart.h"
#include "ui_leastsquare.h"
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <QThread>

/*
    qt中获取容器Vector中的最大值和最小值：
    https://blog.csdn.net/Littlehero_121/article/details/100565527
*/
DECIMAL_TYPE max(vector<DECIMAL_TYPE> &data)
{
    auto max = std::max_element(std::begin(data), std::end(data));
    return *max;
}

DECIMAL_TYPE min(vector<DECIMAL_TYPE> &data)
{
    auto min = std::min_element(std::begin(data), std::end(data));
    return *min;
}

DECIMAL_TYPE atold(const char *str)
{
    bool negativeFlag = false;
    if (str[0] == '-')
    {
        negativeFlag = true;
        str++;
    }
    else if (str[0] == '+')
    {
        str++;
    }

    unsigned long long allNum = 0;
    unsigned long long count = 1;
    bool dotFlag = false;
    while (*str)
    {
        if (*str == '.')
        {
            dotFlag = true;
            str++;
            continue;
        }
        if (dotFlag == true)
            count *= 10;

        allNum = allNum * 10 + (*str - '0');
        str++;
    }
    return negativeFlag ? -((DECIMAL_TYPE)allNum / count) : ((DECIMAL_TYPE)allNum / count);
}

LeastSquare::LeastSquare(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::LeastSquare)
{
    ui->setupUi(this);

    // 1. 创建任务类对象
    taskGen = new Bll_GenerateData(this);
    taskLeastSquare = new Bll_LeastSquareMethod(this);

    samplePointSum = ui->twAverage->rowCount();
    ui->spbxSamplePointSum->setValue(samplePointSum);
    order = ui->spbxOrder->text().toInt();

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

    // 需要初始化表格Item
    for (int i = 0; i < samplePointSum; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            QTableWidgetItem *temp = new QTableWidgetItem();
            ui->twAverage->setItem(i, j, temp);
            ui->twAverage->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
    connect(ui->twAverage, &QTableWidget::itemChanged, this, &LeastSquare::twAverage_itemChanged);

    connect(this, &LeastSquare::collectDataXYChanged, ui->chartFit, &FitChart::updateCollectPlot);

    // 2. 链接子线程
    connect(this, &LeastSquare::startGenerate, taskGen, &Bll_GenerateData::setGenerateFitData);
    connect(taskGen, &Bll_GenerateData::generateFitDataFinish, ui->chartFit, &FitChart::updateFitPlot);

    connect(this, &LeastSquare::startLeastSquare, taskLeastSquare, &Bll_LeastSquareMethod::setLeastSquareMethod);
    connect(taskLeastSquare, &Bll_LeastSquareMethod::leastSquareMethodFinish, this, &LeastSquare::setFitChartData);
}

LeastSquare::~LeastSquare()
{
    delete ui;
}

void LeastSquare::updateCollectDataXY(void)
{
    DECIMAL_TYPE temp;
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

        temp = atold(qsX.toStdString().c_str());
        collectDataX.push_back(temp);
        printf("LeastSquare::updateCollectDataXY    atold x = %.20LE\n", temp);
        printf("LeastSquare::updateCollectDataXY toDouble x = %.20e\n", qsX.toDouble());
        temp = atold(qsY.toStdString().c_str());
        collectDataY.push_back(temp);
        printf("LeastSquare::updateCollectDataXY    atold y = %.20LE\n", temp);
        printf("LeastSquare::updateCollectDataXY toDouble y = %.20e\n", qsY.toDouble());

        qDebug() << i << ":" << qsX << qsY;
    }
}

// 人为 true 自动 false
void LeastSquare::tryUpdateFitChart(bool man)
{
    updateCollectDataXY();
    if (collectDataX.size() < 2)
    {
        if (man) // 是人为的就要提醒一下
            QMessageBox::critical(this, "错误", "正确格式的数据\n小于两组");
        return;
    }

    // N个点可以确定一个 唯一的 N-1 阶的曲线
    order = ui->spbxOrder->text().toInt();
    if (collectDataX.size() <= order)
        order = collectDataX.size() - 1;

    // 启动子线程
    emit startLeastSquare(order, collectDataX, collectDataY);
    QThreadPool::globalInstance()->start(taskLeastSquare);
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

    tryUpdateFitChart(false);
}

void LeastSquare::on_btnFit_clicked()
{
    tryUpdateFitChart(true);
}

void LeastSquare::setFitChartData(vector<DECIMAL_TYPE> factor)
{
    char buffer[200];
    for (unsigned long long i = 0; i <= order; i++)
    {
        // 通过setItem来改变条目
        snprintf(buffer, sizeof(buffer), "%.8LE", factor.at(order - i));
        // printf("LeastSquare::setFitChartData %s\r\n", buffer);
        QTableWidgetItem *temp = new QTableWidgetItem(buffer); // QString::fromStdString(buffer));
        ui->twFactor->setItem(i, 0, temp);
        ui->twFactor->item(i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    collectDataX_Max = max(collectDataX);
    collectDataX_Min = min(collectDataX);
    DECIMAL_TYPE addRange = (collectDataX_Max - collectDataX_Min) / 4.0f;

    // 启动子线程 生成曲线数据
    emit startGenerate(collectDataX_Min - addRange, collectDataX_Max + addRange, 0.25f, factor); // fitDataX, fitDataY);
    QThreadPool::globalInstance()->start(taskGen);
}

void LeastSquare::on_twAverage_itemSelectionChanged()
{
    // 1、记录旧的单元格内容
    old_text = ui->twAverage->item(ui->twAverage->currentRow(),
                                   ui->twAverage->currentColumn())
                   ->text();
}

/*
 * 正则表达式：
 * https://blog.csdn.net/qq_41622002/article/details/107488528
 */
void LeastSquare::twAverage_itemChanged(QTableWidgetItem *item)
{
    // 匹配正负整数、正负浮点数
    const QString Pattern("(-?[1-9][0-9]+)|(-?[0-9])|(-?[1-9]\\d+\\.\\d+)|(-?[0-9]\\.\\d+)"); // 正则表达式
    QRegExp reg(Pattern);

    // 获取修改的新的单元格内容
    QString str = item->text();
    if (str == "")
    {
        qDebug() << "空字符";
        goto GO_ON;
    }

    // 完全匹配
    if (reg.exactMatch(str))
    {
        qDebug() << "匹配成功";

    GO_ON:
        updateCollectDataXY();

        // std::vector 转换为 QVector
        QVector<double> qv_X = QVector<double>(collectDataX.begin(), collectDataX.end());
        QVector<double> qv_Y = QVector<double>(collectDataY.begin(), collectDataY.end());

        emit collectDataXYChanged(qv_X, qv_Y);
    }
    else
    {
        qDebug() << "匹配失败";
        item->setText(old_text); // 更换之前的内容
    }
}