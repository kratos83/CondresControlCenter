/*
 * Copyright (C) 2019  angelo <angelo.scarna@codelinsoft.it>
 * 
 * This file is part of Condres Control Center.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include "Apply.h"
#include "ui_Apply.h"
#include <iostream>
#include "constant.h"

using namespace std;

Apply::Apply(QStringList listPackages, QString install_remove,
             QWidget *parent) :
    QDialog (parent),
    ui(new Ui::Apply),
    m_process(new QProcess),
    m_process_remove(new QProcess),
    m_process_local(new QProcess)
{
    ui->setupUi(this);
    setWindowTitle("Install packages");
    m_list = listPackages;
    m_install_remove = install_remove;
    ui->listWidget->addItems(m_list);
    connect(ui->cancel,&QPushButton::clicked,this,&Apply::closeDialog);
    connect(ui->pushButtonJobs, &QPushButton::clicked,this,&Apply::stopJobs);
    if(m_install_remove == "install"){
        ui->remove->setVisible(false);
        ui->local->setVisible(false);
        connect(ui->apply_install,&QPushButton::clicked,this,&Apply::installPackages);
    }
    else if(m_install_remove == "remove")
    {
        ui->apply_install->setVisible(false);
        ui->local->setVisible(false);
        connect(ui->remove,&QPushButton::clicked,this,&Apply::removePackages);
    }
    else if(m_install_remove == "local")
    {
        ui->remove->setVisible(false);
        ui->apply_install->setVisible(false);
        connect(ui->local,&QPushButton::clicked,this,&Apply::localPackages);
    }
}

Apply::Apply(QString args, QString install_remove, QWidget *parent) :
    QDialog (parent),
    ui(new Ui::Apply),
    m_process(new QProcess),
    m_process_remove(new QProcess),
    m_process_local(new QProcess)
{
    ui->setupUi(this);
    setWindowTitle("Install packages");
    m_args = args;
    m_install_remove = install_remove;
    ui->listWidget->addItem(m_args);
    connect(ui->cancel,&QPushButton::clicked,this,&Apply::closeDialog);
    if(m_install_remove == "install"){
        ui->remove->setVisible(false);
        ui->local->setVisible(false);
        connect(ui->apply_install,&QPushButton::clicked,this,&Apply::installPackages);
    }
    else if(m_install_remove == "remove")
    {
        ui->apply_install->setVisible(false);
        ui->local->setVisible(false);
        connect(ui->remove,&QPushButton::clicked,this,&Apply::removePackages);
    }
    else if(m_install_remove == "local")
    {
        ui->remove->setVisible(false);
        ui->apply_install->setVisible(false);
        connect(ui->local,&QPushButton::clicked,this,&Apply::localPackages);
    }
}

void Apply::installPackages()
{
    QStringList list,list1;
    m_process->setReadChannel(QProcess::StandardOutput);
    m_process->setProcessChannelMode(QProcess::ForwardedOutputChannel);
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

void Apply::localPackages()
{
    QStringList list,list1;
    m_process_local->setReadChannel(QProcess::StandardOutput);
    m_process_local->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_process_local,&QProcess::readyReadStandardOutput,this,&Apply::ReadyPkg);
    connect(m_process_local,static_cast<void (QProcess::*)(int)>(&QProcess::finished),this,&Apply::read_packages);

    for(int i=0; i< ui->listWidget->count(); i++)
    {
        list << ui->listWidget->item(i)->text().split("\n");
    }
    list1 << "-U" << "--noconfirm" << list;
    m_process_local->start("/usr/bin/pacman",list1);
}

void Apply::read_packages(int exitCode)
{
    if(exitCode > 1)
    {
      //process failed, provide info on errors
      QMessageBox::critical(this,"Control Center", "Error whith the underlying process");
    }
    else if(exitCode == 0){
        if(m_install_remove == "install" || m_install_remove == "local")
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
        results = m_process->readAllStandardOutput();
    else if(m_install_remove == "remove")
        results = m_process_remove->readAll();
    else if(m_install_remove == "local")
        results = m_process_local->readAll();
    QStringList resultsVector = results.split(((QRegExp) "\n"));
    for(int i=0; i<resultsVector.size();i++)
    {
        if(QString(resultsVector.at(i)).isEmpty())
            continue;

        QString str;
        str.append(resultsVector.at(i));
        std::cout << str.toStdString() << std::endl;
        ui->console->append("<font color=\"white\">"+resultsVector.at(i)+"</font></br>");
    }
}

void Apply::closeDialog()
{
    if(ui->listWidget->count() > 0){
        QMessageBox *box= new QMessageBox(this);
        box->setWindowTitle("Condres OS Control Center");
        box->setText("Condres OS Control Center");
        box->setInformativeText("Impossible close window before installation. You want to really close?");
        box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        box->setDefaultButton(QMessageBox::Ok);
        box->setIcon(QMessageBox::Information);
        int ret = box->exec();
        switch(ret){
            case QMessageBox::Ok:
                //Open backup
                box->close();
                close();
                break;
            case QMessageBox::Cancel:
                box->close();
                break;
        }
    }
    else
        close();
}

void Apply::stopJobs()
{
  QProcess pacman;
  QString command = "\"killall pacman; rm " + QString(DATABASE_VAR) + "\"";
  pacman.start(command);
  pacman.waitForFinished();
}

Apply::~Apply()
{
    delete ui;
    delete m_process;
}
