#include "gobangui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GobangUI w;
    w.show();

    return a.exec();
}
