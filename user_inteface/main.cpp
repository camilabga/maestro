#include "maestro.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Maestro w;
    w.show();

    return a.exec();
}
