/*
 * Copyright (C) 2019  Angelo Scarna' <angelo.scarna@codelinsoft.it>
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

#include "NotifierCondres.h"
#include <iostream>

Q_LOGGING_CATEGORY(CondresNotifier, "ControlCenter")

NotifierCondres::NotifierCondres(QWidget *parent) : 
    QMainWindow(parent),
    m_numberPackages(0),
    processUpdate(new QProcess)
{
  m_pacmanDatabase =
            new QFileSystemWatcher(QStringList() << DATABASE, this);

  connect(m_pacmanDatabase,&QFileSystemWatcher::directoryChanged, this, &NotifierCondres::updateIcon);

  createAction();
  createTrayIcon();
}

void NotifierCondres::createAction()
{
    m_close = new QAction(QIcon(":/images/application-exit.png"),"Exit",this);
    m_close->setIconVisibleInMenu(true);
    connect(m_close,&QAction::triggered,qApp,&QApplication::quit);
    
    m_info = new QAction(QIcon(":/images/svn_status.png"),"Info",this);
    m_info->setIconVisibleInMenu(true);
    connect(m_info,&QAction::triggered,this,&NotifierCondres::viewInfo);
    
    m_install = new QAction(QIcon(":/images/install.png"),"Install packages",this);
    m_install->setIconVisibleInMenu(true);
    connect(m_install,&QAction::triggered,this,&NotifierCondres::viewInstallPackages);
    
    m_upgrade = new QAction(QIcon(":/images/ark.png"),"Upgrade packages",this);
    m_upgrade->setIconVisibleInMenu(true);
    connect(m_upgrade,&QAction::triggered,this,&NotifierCondres::viewUpdate);
    
    m_sync = new QAction(QIcon(":/images/ark.png"),"Syncronized databases",this);
    m_sync->setIconVisibleInMenu(true);
    connect(m_sync,&QAction::triggered,this,&NotifierCondres::syncDatabases);
}

void NotifierCondres::createTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon::fromTheme(":/images/db_green.png"));
    m_Menu = new QMenu(this);
    m_Menu->addAction(m_sync);
    m_Menu->addSeparator();
    m_Menu->addAction(m_upgrade);
    m_Menu->addSeparator();
    m_Menu->addAction(m_install);
    m_Menu->addSeparator();
    m_Menu->addAction(m_info);
    m_Menu->addSeparator();
    m_Menu->addAction(m_close);
    m_trayIcon->setContextMenu(m_Menu);
    
    m_client = new CchClient("org.condrescontrolcenter.pacmanhelper","/",QDBusConnection::systemBus(), 0);
    connect(m_client,&CchClient::syncdbcompleted,this,&NotifierCondres::syncDatabases);
    
    QTimer time;
    m_trayIcon->show();
}

void NotifierCondres::updateIcon()
{
    qCDebug(CondresNotifier) << m_numberPackages;
    if(m_numberPackages > 0){
        m_trayIcon->setIcon(QIcon(":/images/db_red.png"));
        m_trayIcon->setToolTip("Are avaible "+QString::number(m_numberPackages)+" updates...");
    }
    else if(m_numberPackages == 0){
        m_trayIcon->setIcon(QIcon(":/images/db_green.png"));
        m_trayIcon->setToolTip("System update complete.");
    }
}

void NotifierCondres::viewInfo()
{
    m_about = new About;
    m_about->exec();
}

void NotifierCondres::viewInstallPackages()
{
    QProcess proc;
    proc.startDetached("sh /usr/bin/CondresControlCenterInstallBin");
    proc.close();
}

void NotifierCondres::viewUpdate()
{
    QProcess proc;
    proc.startDetached("sh /usr/bin/CondresControlCenterUpdateBin");
    proc.close();
}

void NotifierCondres::syncDatabases()
{
    m_numberPackages = 0;
    QProcess proc;
    proc.startDetached("gksu \"/usr/bin/pacman -Sy\"");
    processUpdate->setReadChannel(QProcess::StandardOutput);
    processUpdate->setProcessChannelMode(QProcess::MergedChannels);
    connect(processUpdate,&QProcess::readyReadStandardOutput,this,&NotifierCondres::showProgressInDebug);
    connect(processUpdate,static_cast<void (QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),this,&NotifierCondres::updatePackagesProcess);
    processUpdate->start("pacman -Sup --print-format \"%n\"");
    proc.close();
}

void NotifierCondres::updatePackagesProcess(int exitCode, QProcess::ExitStatus)
{
    if(exitCode > 1)
    {
        qCDebug(CondresNotifier) << processUpdate->errorString();
    }
    else if(exitCode == 0){
        processUpdate->close();
    }
}

void NotifierCondres::showProgressInDebug()
{
    QString m_list = processUpdate->readAll();
    QStringList lines = m_list.split(QRegExp("\\n"), QString::SkipEmptyParts);

    //process package list
    for(int i = 0; i < lines.length(); i++)
    {
        QString line = lines.at(i);

        if(line.isEmpty())
            m_numberPackages = i;
        else{
            QString str;
            str.append(line);
            m_numberPackages = i+1;
        }
    }
    updateIcon();
}

NotifierCondres::~NotifierCondres()
{
    delete m_trayIcon;
}
