
#include <QApplication>
#include <DEdit/dedit_mainwindow.h>
#include <io/iconcatcher.h>
#include <QTranslator>
#include <QDir>
#include <QLocale>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QTranslator translator;
    
    QString dirpath = app.applicationDirPath() + QDir::separator()
            + ".." + QDir::separator() + "translations" + QDir::separator();
    QString filename = "dea_editor_" + QLocale::languageToString(QLocale::system().language());
    
    translator.load(filename.toLower(), dirpath);
    app.installTranslator(&translator);
    
    IconCatcher::init();
    DEdit_MainWindow mw;
    
    mw.show();
    return app.exec();
}

