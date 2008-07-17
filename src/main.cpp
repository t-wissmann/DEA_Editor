
#include <QApplication>
#include <DEdit/dedit_mainwindow.h>



int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    DEdit_MainWindow mw;
    mw.show();
    return app.exec();
}

