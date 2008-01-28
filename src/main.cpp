
#include <QApplication>
#include <DEdit/dedit_mainwindow.h>
#include <multilanguage/translationmanager.h>
#include <io/iconcatcher.h>
#include <QTranslator>
#include <QDir>
#include <QLocale>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    
    /*
    */
    
    
    IconCatcher::init();
    DEdit_MainWindow mw;
    
    mw.show();
    return app.exec();
}

