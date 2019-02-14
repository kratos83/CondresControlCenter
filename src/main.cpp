#include "mainwindow.h"
#include <QApplication>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <QMessageBox>
#include "constant.h"
#include <QtCore>

//Enable root
static bool enableRoot()
{
    if(getuid() != 0)
        return 1;
    else
        return 0;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(enableRoot() == 0){
        QScreen *sc = QGuiApplication::primaryScreen();
        MainWindow *w = new MainWindow();
        w->setGeometry(sc->geometry());
        w->showMaximized();
    }
    else{
        QMessageBox::warning(nullptr,"Condres OS Control Center",QObject::tr("You cannot perform this operation unless you are root"));
        exit(0);
    }
    return a.exec();
}
