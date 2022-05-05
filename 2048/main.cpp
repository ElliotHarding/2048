#include "dlg_home.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DLG_Home w;
    w.show();
    return a.exec();
}
