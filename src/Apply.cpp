#include "Apply.h"
#include "ui_Apply.h"
#include <iostream>

using namespace std;

Apply::Apply(QStringList listPackages, QString install_remove,
             QWidget *parent) :
    QDialog (parent),
    ui(new Ui::Apply),
    m_process(new QProcess),
    m_process_remove(new QProcess)
{
    ui->setupUi(this);
    setWindowTitle("Install packages");
    m_list = listPackages;
    m_install_remove = install_remove;
    ui->listWidget->addItems(m_list);
    connect(ui->cancel,&QPushButton::clicked,this,&Apply::closeDialog);
    if(m_install_remove == "install"){
        ui->remove->setVisible(false);
        connect(ui->apply_install,&QPushButton::clicked,this,&Apply::installPackages);
    }
    else if(m_install_remove == "remove")
    {
        ui->apply_install->setVisible(false);
        connect(ui->remove,&QPushButton::clicked,this,&Apply::removePackages);
    }
}

Apply::Apply(QString args, QString install_remove, QWidget *parent) :
    QDialog (parent),
    ui(new Ui::Apply),
    m_process(new QProcess),
    m_process_remove(new QProcess)
{
    ui->setupUi(this);
    setWindowTitle("Install packages");
    m_args = args;
    m_install_remove = install_remove;
    ui->listWidget->addItem(m_args);
    connect(ui->cancel,&QPushButton::clicked,this,&Apply::closeDialog);
    if(m_install_remove == "install"){
        ui->remove->setVisible(false);
        connect(ui->apply_install,&QPushButton::clicked,this,&Apply::installPackages);
    }
    else if(m_install_remove == "remove")
    {
        ui->apply_install->setVisible(false);
        connect(ui->remove,&QPushButton::clicked,this,&Apply::removePackages);
    }
}

void Apply::installPackages()
{
    QStringList list,list1;
    m_process->setReadChannel(QProcess::StandardOutput);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_process,&QProcess::readyReadStandardOutput,this,&Apply::ReadyPkg);
    connect(m_process,static_cast<void (QProcess::*)(int)>(&QProcess::finished),this,&Apply::read_packages);

    for(int i=0; i< ui->listWidget->count(); i++)
    {
        list << ui->listWidget->item(i)->text().split("\n");
    }
    list1 << "-S" << "--noconfirm" << list;
    m_process->start("/usr/bin/pacman",list1);
}

void Apply::removePackages()
{
    QStringList list,list1;
    m_process_remove->setReadChannel(QProcess::StandardOutput);
    m_process_remove->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_process_remove,&QProcess::readyReadStandardOutput,this,&Apply::ReadyPkg);
    connect(m_process_remove,static_cast<void (QProcess::*)(int)>(&QProcess::finished),this,&Apply::read_packages);

    for(int i=0; i< ui->listWidget->count(); i++)
    {
        list << ui->listWidget->item(i)->text().split("\n");
    }
    list1 << "-R" << "--noconfirm" << list;
    m_process_remove->start("/usr/bin/pacman",list1);
}

void Apply::read_packages(int exitCode)
{
    if(exitCode > 1)
    {
      //process failed, provide info on errors
      QMessageBox::critical(this,"Control Center", "Error whith the underlying process");
    }
    else if(exitCode == 0){
        if(m_install_remove == "install")
            ui->console->append("<font color=\"white\">Packages installed</font>");
        else if(m_install_remove == "remove")
            ui->console->append("<font color=\"white\">Packages removed</font>");
        ui->listWidget->clear();
    }
}

void Apply::ReadyPkg()
{
    QString results;
    ui->stackedWidget->setCurrentIndex(1);
    if(m_install_remove == "install")
        results = m_process->readAll();
    else if(m_install_remove == "remove")
        results = m_process_remove->readAll();
    QStringList resultsVector = results.split(((QRegExp) "\n"));
    for(int i=0; i<resultsVector.size();i++)
    {
        if(QString(resultsVector.at(i)).isEmpty())
            continue;

        QString str;
        str.append(resultsVector.at(i));
        ui->console->append("<font color=\"white\">"+resultsVector.at(i)+"</font></br>");
    }
}

void Apply::closeDialog()
{
    if(ui->listWidget->count() > 0 || !m_process->waitForFinished() || !m_process_remove->waitForFinished())
        QMessageBox::warning(this,"Condres OS Control Center","Impossible close window before installation");
    else
        close();
}

Apply::~Apply()
{
    delete ui;
    delete m_process;
}
