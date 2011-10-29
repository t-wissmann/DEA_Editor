
#include <QApplication>
#include <DEdit/dedit_mainwindow.h>
#include <multilanguage/translationmanager.h>
#include <io/iconcatcher.h>
#include <QTranslator>
#include <QDir>
#include <QLocale>
#include <QtGlobal>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationVersion("0.1"); 
    QStringList args = app.arguments();
    QString cur;
    for (int i = 0; i < args.size(); ++i)
    {
        cur = args[i];
        if (cur == "-v" || cur == "--version")
        {
            qDebug("DEA Editor %s (Qt %s)",
                       app.applicationVersion().toLocal8Bit().data(),
                       qVersion());
            return 1;
        }
        else if (cur == "-h" || cur == "--help")
        {
            qDebug("usage: %s [OPTIONS] [file.xml]", args[0].toLocal8Bit().data());
            qDebug("   Starts DEA Editor and opens file.xml if passed");
            qDebug("   Valid OPTIONS are:");
            qDebug("     -h --help     Print this help and exit");
            qDebug("     -v --version  Print current version number and exit");

            return 1;
        }
    }


    IconCatcher::init();
    DEdit_MainWindow mw;
    
    mw.show();
    return app.exec();
}

