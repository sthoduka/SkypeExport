#include "model/skypeparser.h"
#include <iostream>
#include "resources/ui/skype_export_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SkypeExportWindow w;
    w.show();

    return a.exec();
}

