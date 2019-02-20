#include "mainwindow.h"
#include <QApplication>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <QMessageBox>
#include "constant.h"
#include <QtCore>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(CondresControlCenterMain)
Q_LOGGING_CATEGORY(CondresControlCenterMain, "ControlCenter")

//Enable root
static bool enableRoot()
{
    if(getuid() != 0)
        return 1;
    else
        return 0;
}

//Command
static bool parseArguments(const QStringList &argomento)
{
    QStringListIterator arg_cmd(argomento);
    arg_cmd.next();
    while(arg_cmd.hasNext())
    {
        const QString &a_arg = arg_cmd.next().toLower();
        if(a_arg == "-i" || a_arg == "--install"){
            return true;
        }
        else if(a_arg == "-u" || a_arg == "--update"){
            return true;
        }
        else if(a_arg == "-v" || a_arg == "--version"){
            qCDebug(CondresControlCenterMain) << QString::fromUtf8(QObject::tr("This version software is: ").toStdString().c_str()) << QString(VERSION);
            return false;
        }
        else if(a_arg == "-h" || a_arg == "--help")
        {
            qCDebug(CondresControlCenterMain) << QObject::tr("USAGE:\t ") << QObject::tr("DISPLAYS\n")
                     << QObject::tr("-i or --install \t") << QObject::tr("Install packages\n")
                     << QObject::tr("-u or --update \t\t") << QObject::tr("Update packages:\n")
                     << QObject::tr("VIEW THE INSTALLED SOFTWARE VERSION:\n\n")
                     << QObject::tr("-v or --version \t") << QObject::tr("Software version\n\n\n")
                     << QObject::tr("VIEW THE INFORMATION OF THE PROGRAM:\n\n")
                     << QObject::tr("-h or --help \t\t") << QObject::tr("Software information.");
            return false;
        }
        else{
            qCDebug(CondresControlCenterMain) << QObject::tr("Command not found: ") << a_arg;
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(enableRoot() == 0){
        
        if(!parseArguments(a.arguments()))
        {
            return -1;
        }

        const QString arg1 = argv[1];

        const QString arg3 = argv[3];
        QScreen *sc = QGuiApplication::primaryScreen();
        MainWindow *w = new MainWindow();
        if((arg1.toLower() == "-i" || arg1.toLower() == "--install"))
        {
            w->setGeometry(sc->geometry());
            w->on_butt_install_clicked();
            w->showMaximized();
        }
        if((arg1.toLower() == "-u" || arg1.toLower() == "--update"))
        {
            w->setGeometry(sc->geometry());
            w->on_butt_update_clicked();
            w->showMaximized();
        }
        else{
            w->setGeometry(sc->geometry());
            w->showMaximized();
        }
    }
    else{
        QMessageBox::warning(nullptr,"Condres OS Control Center",QObject::tr("You cannot perform this operation unless you are root"));
        exit(0);
    }
    return a.exec();
}
