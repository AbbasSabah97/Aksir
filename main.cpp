#include <QApplication>
#include <QStyleFactory>
#include "spreadsheetwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // إجبار التطبيق على استخدام نمط Fusion لضمان توحيد المظهر على جميع الأجهزة
    app.setStyle(QStyleFactory::create("Fusion"));
    
    SpreadsheetWindow window;
    window.show();
    return app.exec();
}
