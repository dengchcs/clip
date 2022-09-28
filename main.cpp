// #define GUI
#ifdef GUI
#include "mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
       QApplication a(argc, argv);
       MainWindow w;
       w.show();
       return a.exec();
}
#else
#include "algo_test.h"
QTEST_MAIN(AlgoTest)
#endif
