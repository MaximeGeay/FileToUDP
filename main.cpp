#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icon.png"));
    QCoreApplication::setOrganizationName("Genavir");
    QCoreApplication::setApplicationName("FileToUDP");
    MainWindow w;
    w.show();
    return a.exec();
}
