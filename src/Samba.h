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

#ifndef SAMBA_H
#define SAMBA_H

#include <QDialog>
#include <QtWidgets>
#include <QtCore>
#include <QtSql/QtSql>
#include <QtPrintSupport/QtPrintSupport>
#include "settingsmanager.h"
#include "users/UsersPage.h"

class SambaConfiguration;
class SambaPrinter;
class SambaUser;

#define SAMBA_FILE "/etc/samba/smb.conf"
#define SAMBA_TEMP "/tmp/smb.conf"
#define SAMBA_SPOOL "/var/spool/samba"
#define SAMBA_CUPS "cups"

Q_DECLARE_LOGGING_CATEGORY(CondresSambaConfig)

namespace Ui
{class Samba;
 class SambaConfiguration;
 class SambaPrinter;
 class SambaUser;}

/**
 * @todo write docs
 */
class Samba : public QDialog
{
    Q_OBJECT

public:
    explicit Samba(QWidget *parent=nullptr);
    ~Samba();
    void load(QString nameFile);
    
private slots:
    void readShareFileDirectory();
    void loadFile();
    void startSamba();
    void restartSamba();
    void stopSamba();
    void StartStopRestProcess(int exitCode, QProcess::ExitStatus);
    void StartStopRest();
    void statusSamba();
    void readChecBox(bool ok);
    void saveFile();
    void saveFile(QString nameFile);
    void openConfigDir();
    void addCongigDir();
    void delSambaShare();
    //Samba printer
    void readSharePrinter();
    void addSambaPrinter();
    void configSambaPrinter();
    void delSambaPrinter();
    //Samba user
    void readUser();
    void addUser();
    void configUser();
    void delUser();
    void openUser();
private:
    Ui::Samba *ui;
    QProcess *proc;
    QStandardItemModel *model,*modelPrinter, *modelUser;
    bool m_modify, m_edit;
    SettingsManager *manager;
    UsersPage *m_pageUsers;
};

//Samba configuration shared directory 
class SambaConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit SambaConfiguration(QStringList listConfigDir, QString create_modify, QString id, QWidget *parent = nullptr);
    ~SambaConfiguration();
    
private slots:
    void saveFile();
    void openDirectory();
private:
    Ui::SambaConfiguration *m_ui;
    QStringList m_list;
    QString m_createModify, m_id;
signals:
    void saveConfig(QString);
    void saveList();
};

//Samba configuration shared printer
class SambaPrinter : public QDialog
{
    Q_OBJECT

public:
    explicit SambaPrinter(QStringList listConfigDir, QString create_modify, QString id, QWidget *parent = nullptr);
    ~SambaPrinter();
    
private slots:
    void viewPrinter();
    void saveFile();
    void openDirectory();
private:
    Ui::SambaPrinter *_ui;
    QStringList m_list;
    QString m_createModify, m_id;
signals:
    void saveConfig(QString);
    void saveList();
};

//Samba user
class SambaUser : public QDialog
{
    Q_OBJECT

public:
    explicit SambaUser(QString id, QString users, QString edit, QWidget *parent = nullptr);
    ~SambaUser();

private slots:
    void readProcess();
    void saveConfig();
private:
    Ui::SambaUser *_ui_;
    QProcess process,*process_user;
    QString m_id, m_user, m_edit;
signals:
    void saveList();
};

#endif // SAMBA_H
