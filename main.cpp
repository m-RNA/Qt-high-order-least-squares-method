#include "leastsquare.h"
#include <QApplication>
#include <QThread>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QVector<double>>("QVector<double>"); // 添加不支持的类型
    qDebug() << "主线程ID：" << QThread::currentThread();

    LeastSquare w;
    w.show();
    return a.exec();
}
