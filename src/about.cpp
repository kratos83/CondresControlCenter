#include "about.h"
#include "ui_about.h"
#include "constant.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    setWindowTitle("About");
    viewControlCenter();
    viewPacman();
}

void About::viewControlCenter()
{
    QString com_lsb = "pacman -V ";
    process = new QProcess;
    process->setReadChannel(QProcess::StandardOutput);
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->start(com_lsb);
    process->closeWriteChannel();
    if(process->state()!=QProcess::NotRunning){
        process->waitForReadyRead();
        QByteArray result = process->readAll();
        QStringList pac = QString(result).split("\n");
        ui->label_pacman->setText("<p><b>"+QString(pac.at(1)).remove(" .--.")+"</b><br>"+QString(pac.at(2)).remove("/ _.-' .-.  .-.  .-.")+"<br>"
                                  +QString(pac.at(3)).remove("\\  '-. '-'  '-'  '-'   ")+"<br></p><p>"
                                 "This program may be freely redistributed under<br>the terms of the GNU General Public License.</p>");
    }
}

void About::viewPacman()
{
    ui->label_centro->setText("<p><b>CondresOS Control Center</b><br>"
                              "Site web: <a href=\"https://condresos.codelinsoft.it\">https://condresos.codelinsoft.it/</a><br>"
                              "Version: <b>"+QString(VERSION)+" "+QString(BETA)+" 1</b><br>"
                              "(C) 2019 Angelo Scarnà - Team Codelinsoft</p>");
}

About::~About()
{
    delete ui;
}

void About::on_chiudi_clicked()
{
    close();
}
