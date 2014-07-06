#include "aptdb.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Aptdb w;
    w.show();

    return a.exec();
}
