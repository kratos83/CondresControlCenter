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

#ifndef NOTIFIERCONDRES_H
#define NOTIFIERCONDRES_H

#include <QtWidgets>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QFileSystemWatcher>
#include <QLoggingCategory>
#include <KNotifications/KStatusNotifierItem>
#include "../pacmanhelper/pacmanhelperclient.h"
#include "../../src/about.h"
#include "../../src/settingsmanager.h"
#include "../../src/backend/backend.h"

//Database pacman
const QString DATABASE = "/var/lib/pacman";
const QString DBUS_PACMANHELPER_SERVICE = "/usr/share/dbus-1/system-services/org.condrescontrolcenter.pacmanhelper.service";

Q_DECLARE_LOGGING_CATEGORY(CondresNotifier)

class NotifierCondres : public QObject
{
    Q_OBJECT
    
public:
    explicit NotifierCondres(QObject *parent=0);
    virtual ~NotifierCondres();
    
public slots:
    void createAction();
    void createTrayIcon();
    void updateIcon();
    void viewInfo();
    void viewUpdate();
    void viewInstallPackages();
    void openControlCenter();
    void syncDatabases();
    void pacmanUpdateTimer();
    void sendDatabase();
    
private:
    KStatusNotifierItem *m_trayIcon;
    QMenu *m_Menu;
    QFileSystemWatcher *m_pacmanDatabase;
    QAction *m_info, *m_upgrade, *m_install, *m_close, *m_sync, *m_controlCenter;
    PacmanHelperClient *m_client;
    About *m_about;
    int m_numberPackages;
    QTimer *m_timer, *m_readFile;
    SettingsManager *m_manager;
};

#endif // NOTIFIERCONDRES_H
