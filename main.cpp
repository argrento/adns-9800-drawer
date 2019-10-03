#include "MouseDrawer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MouseDrawer w;
    w.show();

    return a.exec();
}
