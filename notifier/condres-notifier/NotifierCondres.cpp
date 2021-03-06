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

NotifierCondres::NotifierCondres(QObject *parent) : 
    QObject(parent),
    m_numberPackages(0),
    m_manager(new SettingsManager)
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
    
    m_controlCenter = new QAction(QIcon(":/images/preferences-system.png"),tr("CondresControlCenter"),this);
    m_controlCenter->setIconVisibleInMenu(true);
    connect(m_controlCenter,&QAction::triggered,this,&NotifierCondres::openControlCenter);
    
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
    connect(m_sync,&QAction::triggered,[this](){ sendDatabase();});
}

void NotifierCondres::createTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon::fromTheme(":/images/db_green.png"));
    m_Menu = new QMenu();
    m_Menu->addAction(m_sync);
    m_Menu->addSeparator();
    m_Menu->addAction(m_controlCenter);
    m_Menu->addSeparator();
    m_Menu->addAction(m_upgrade);
    m_Menu->addSeparator();
    m_Menu->addAction(m_install);
    m_Menu->addSeparator();
    m_Menu->addAction(m_info);
    m_Menu->addSeparator();
    m_Menu->addAction(m_close);
    m_trayIcon->setContextMenu(m_Menu);
    m_trayIcon->show();
    m_client = new PacmanHelperClient("org.condrescontrolcenter.pacmanhelper","/",QDBusConnection::systemBus(), 0);
    connect(m_client,&PacmanHelperClient::syncdbcompleted,this,&NotifierCondres::syncDatabases);
    
    m_timer = new QTimer();
    m_timer->setInterval(1000);
    m_timer->start();
    
    //Settings manager data 
    m_manager->setGeneralValue("Date/hour",QTime::currentTime().hour());
    connect(m_timer,&QTimer::timeout,this,&NotifierCondres::pacmanUpdateTimer);
}

void NotifierCondres::pacmanUpdateTimer()
{
    static bool firstTimePacman = true;
    if(firstTimePacman)
    {
        updateIcon();
        //From now on, we verify if it's time to check for updates every 5 minutes
        m_timer->setInterval(60000 * 5);
    }
    
    if(m_manager->generalValue("Date/hour").toTime().hour() < QTime::currentTime().hour()){
        m_manager->setGeneralValue("Date/hour",QTime::currentTime().hour());
        sendDatabase();
    }
    
    m_timer->stop();
    m_timer->start();
}

void NotifierCondres::sendDatabase()
{
    m_client->syncdb();
    syncDatabases();
}

void NotifierCondres::updateIcon()
{
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

void NotifierCondres::openControlCenter()
{
    QProcess proc;
    proc.startDetached("sh /usr/bin/CondresControlCenterBin");
    proc.close();
}

void NotifierCondres::syncDatabases()
{
    m_trayIcon->setToolTip("Syncronized databases...");
    qCDebug(CondresNotifier) << "Syncronized databases...";
    m_numberPackages = Backend::getUpdateList().length();
    qCDebug(CondresNotifier) << m_numberPackages;
    updateIcon();
}

NotifierCondres::~NotifierCondres()
{
    delete m_trayIcon;
}
