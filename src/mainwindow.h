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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tab_style.h"
#include <QtWidgets>
#include <QtCore>
#include <QLoggingCategory>
#include <QSystemTrayIcon>

//Classi da istanziare
#include "cachecleaner/cachecleaner.h"
#include "repoeditor/repoeditor.h"
#include "upgrade.h"
#include "logfile.h"
#include "about.h"
#include "Apply.h"
#include "InstallPackages.h"
#include "esci.h"
#include "Samba.h"
#include "nfsshare.h"
#include "settingsmanager.h"
#include "users/UsersPage.h"
#include "keyboard/KeyboardPage.h"
#include "mhwd/MhwdPage.h"

Q_DECLARE_LOGGING_CATEGORY(ControlCenterMain)

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    QString titolo();
    void visButtonStatusBar();
    void on_butt_repo_clicked();
    void ritorna_indietro();
    void on_butt_update_clicked();
    void resButton(bool vero);
    void on_butt_install_clicked();
    void on_butt_log_clicked();
    void on_butt_clean_clicked();
    void on_actionExit_triggered();
    void on_actionInformation_triggered();
    void openKeyboard();
    void openHardware();
    void openSamba();
    void openUsers();
    void openNfsShare();
private:
    Ui::MainWindow *ui;
    QProcess *process, *process_locate_paccache;
    QPushButton *butt_back;
    RepoEditor *repo;
    Upgrade *m_upgrade;
    CacheCleaner *m_cache;
    LogFile *m_log;
    About *m_about;
    Apply *m_apply;
    InstallPackages *m_packages;
    esci *m_esci;
    SettingsManager *manager;
    KeyboardPage *page;
    MhwdPage *m_pageHardware;
    Samba *m_samba;
    UsersPage *m_pageUsers;
    NfsShare *m_shareNfs;

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
