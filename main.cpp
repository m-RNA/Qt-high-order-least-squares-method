#include "leastsquare.h"
#include <QApplication>
#include <QThread>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "主线程ID：" << QThread::currentThread();
    qRegisterMetaType<QVector<double>>("QVector<double>"); // 添加不支持的类型
    qRegisterMetaType<vector<double>>("vector<double>"); // 添加不支持的类型
    qRegisterMetaType<QVector<long double>>("QVector<long double>"); // 添加不支持的类型
    qRegisterMetaType<vector<long double>>("vector<long double>");   // 添加不支持的类型

    LeastSquare w;
    w.show();
    return a.exec();
}
