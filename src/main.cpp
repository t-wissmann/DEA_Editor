
#include <QApplication>
#include <DEdit/dedit_mainwindow.h>
#include <io/iconcatcher.h>


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    IconCatcher::init();
    DEdit_MainWindow mw;
    mw.show();
    return app.exec();
}

