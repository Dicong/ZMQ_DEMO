#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    bool b = false;
    b = translator.load("ZMQ_DEMO_zh_CN.qm");
    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
