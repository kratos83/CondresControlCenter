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

NotifierCondres::NotifierCondres(QWidget *parent) : 
    QMainWindow(parent)
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
}

void NotifierCondres::createTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon::fromTheme(":/images/db_green.png"));
    m_Menu = new QMenu(this);
    m_Menu->addAction(m_upgrade);
    m_Menu->addSeparator();
    m_Menu->addAction(m_install);
    m_Menu->addSeparator();
    m_Menu->addAction(m_info);
    m_Menu->addSeparator();
    m_Menu->addAction(m_close);
    m_trayIcon->setContextMenu(m_Menu);
    
    m_client = new CchClient("org.condrescontrolcenter.pacmanhelper","/",QDBusConnection::systemBus(), 0);
    
    m_trayIcon->show();
}

void NotifierCondres::updateIcon()
{
    //Not complete
    m_trayIcon->setIcon(QIcon(":/images/db_green.png"));
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

NotifierCondres::~NotifierCondres()
{
    delete m_trayIcon;
}
