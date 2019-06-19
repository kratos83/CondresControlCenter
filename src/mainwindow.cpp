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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constant.h"

Q_LOGGING_CATEGORY(ControlCenterMain,"ControlCenter")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    manager(new SettingsManager)
{
    ui->setupUi(this);
    butt_back = new QPushButton(QIcon(":/images/back.png"),tr("Back"),this);
    connect(butt_back,SIGNAL(clicked(bool)),this,SLOT(ritorna_indietro()));
    connect(ui->pushKey,&QPushButton::clicked,this,&MainWindow::openKeyboard);
    connect(ui->hard_conf,&QPushButton::clicked,this,&MainWindow::openHardware);
    connect(ui->share,&QPushButton::clicked,this,&MainWindow::openSamba);
    connect(ui->users,&QPushButton::clicked,this,&MainWindow::openUsers);
    connect(ui->pushButtonNFS,&QPushButton::clicked,this,&MainWindow::openNfsShare);
    connect(ui->pushButtonLanguage,&QPushButton::clicked,this,&MainWindow::openLocalLanguages);
    connect(ui->pushButtonTime,&QPushButton::clicked,this,&MainWindow::openTimeDate);
    ui->tabWidget->tabBar()->setStyle(new Tab_style);
    setWindowTitle(titolo()+tr("Control Center  ")+QString(VERSION)+"  "+QString(RELEASE_CANDIDATE)+" 2");
    ui->stackedWidget->setCurrentIndex(0);
    visButtonStatusBar();
}

QString MainWindow::titolo()
{
    QString os_type = "";
    QString com_lsb = "lsb_release -d ";
    process = new QProcess;
    process->setReadChannel(QProcess::StandardOutput);
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->start(com_lsb);
    process->closeWriteChannel();
    if(process->state()!=QProcess::NotRunning){
        process->waitForReadyRead();
        QByteArray result = process->readAll();
        QStringList list = QString(result).split(":");
        QString text = list.at(1);
        os_type += text;
    }
    return os_type;
}


void MainWindow::visButtonStatusBar()
{
    QLabel *space = new QLabel;
    ui->statusbar->addWidget(space,1);
    ui->statusbar->addWidget(butt_back);
    butt_back->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_butt_repo_clicked()
{
    repo = new RepoEditor(this);
    ui->stackedWidget->insertWidget(1,repo);
    ui->stackedWidget->setCurrentIndex(1);
    butt_back->setVisible(true);
}

void MainWindow::ritorna_indietro()
{
    ui->stackedWidget->setCurrentIndex(0);
    visButtonStatusBar();
}

void MainWindow::on_butt_update_clicked()
{
    showMaximized();
    m_upgrade = new Upgrade("",this);
    ui->stackedWidget->insertWidget(1,m_upgrade);
    ui->stackedWidget->setCurrentIndex(1);
    resButton(true);
}

void MainWindow::resButton(bool vero)
{
    butt_back->setVisible(vero);
}

void MainWindow::on_butt_install_clicked()
{
    showMaximized();
    m_packages = new InstallPackages(this);
    ui->stackedWidget->insertWidget(1,m_packages);
    ui->stackedWidget->setCurrentIndex(1);
    butt_back->setVisible(true);
}

void MainWindow::on_butt_log_clicked()
{
    m_log = new LogFile("/var/log/pacman.log",this);
    ui->stackedWidget->insertWidget(1,m_log);
    ui->stackedWidget->setCurrentIndex(1);
    butt_back->setVisible(true);
}

void MainWindow::on_butt_clean_clicked()
{
    if(!getPacCache().isEmpty()){
        m_cache = new CacheCleaner(this);
        ui->stackedWidget->insertWidget(1,m_cache);
        ui->stackedWidget->setCurrentIndex(1);
        butt_back->setVisible(true);
    }
    else{
        m_apply = new Apply("pacman-contrib","install",this);
        m_apply->exec();
    }

}

void MainWindow::on_actionExit_triggered()
{
    m_esci = new esci(this);
    m_esci->exec();
}

void MainWindow::on_actionInformation_triggered()
{
    m_about = new About(this);
    m_about->exec();
}

void MainWindow::openKeyboard()
{
    page = new KeyboardPage(this);
    page->load();
    ui->stackedWidget->insertWidget(1,page);
    ui->stackedWidget->setCurrentIndex(1);
    butt_back->setVisible(true);
}

void MainWindow::openHardware()
{
    if(!getMHwd().isEmpty()){
        m_pageHardware = new MhwdPage(this);
        m_pageHardware->load();
        ui->stackedWidget->insertWidget(1,m_pageHardware);
        ui->stackedWidget->setCurrentIndex(1);
        butt_back->setVisible(true);
    }
    else{
        m_apply = new Apply(QStringList() << "mhwd" << "mhwd-db","install",this);
        m_apply->exec();
    }
}

void MainWindow::openSamba()
{
    if(!getSamba().isEmpty()){
        m_samba = new Samba(this);
        ui->stackedWidget->insertWidget(1,m_samba);
        ui->stackedWidget->setCurrentIndex(1);
        butt_back->setVisible(true);
    }
    else{
        m_apply = new Apply("samba","install",this);
        m_apply->exec();
    }
}

void MainWindow::openUsers()
{
    m_pageUsers = new UsersPage(this);
    m_pageUsers->load();
    ui->stackedWidget->insertWidget(1,m_pageUsers);
    ui->stackedWidget->setCurrentIndex(1);
    butt_back->setVisible(true);
}

void MainWindow::openNfsShare()
{
    if(!getNfs().isEmpty()){
        m_shareNfs = new NfsShare(this);
        ui->stackedWidget->insertWidget(1,m_shareNfs);
        ui->stackedWidget->setCurrentIndex(1);
        butt_back->setVisible(true);
    }
    else{
        m_apply = new Apply("nfs-utils","install",this);
        m_apply->exec();
    }
}

void MainWindow::openLocalLanguages()
{
    m_localLanguage = new LocalePage(this);
    m_localLanguage->load();
    ui->stackedWidget->insertWidget(1,m_localLanguage);
    ui->stackedWidget->setCurrentIndex(1);
    butt_back->setVisible(true);
}

void MainWindow::openTimeDate()
{
    m_timePage = new TimeDatePage(this);
    m_timePage->load();
    ui->stackedWidget->insertWidget(1,m_timePage);
    ui->stackedWidget->setCurrentIndex(1);
    butt_back->setVisible(true);
}

void MainWindow::closeEvent(QCloseEvent *event){
    event->ignore();
    m_esci = new esci(this);
    m_esci->exec();
}
