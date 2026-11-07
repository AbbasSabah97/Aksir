#include <QApplication>
#include "spreadsheetwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SpreadsheetWindow window;
    window.show();
    return app.exec();
}
