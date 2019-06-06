/*
 * Copyright (C) 2019  Angelo Scarnà <angelo.scarna@codelinsoft.it>
 *
 * This part is CondresControlCenter
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

#ifndef NFSSHARE_H
#define NFSSHARE_H

#include <QDialog>
#include <QtWidgets>
#include <QtCore>
#include <QtSql>
#include "settingsmanager.h"

class NfsConfig;

#define NFS_EXPORT "/etc/exports"
#define NFS_TEMP "/tmp/exports"

Q_DECLARE_LOGGING_CATEGORY(CondresControlNfs)

namespace Ui
{   class NfsShare;
    class NfsConfig;}

/**
 * @todo write docs
 */
class NfsShare : public QDialog
{
    Q_OBJECT
    
public:
    explicit NfsShare(QWidget *parent=nullptr);
    ~NfsShare();
    
public slots:
    void viewTerminal(bool vero);
    void readNfsShare(QString nameFile);
    void startNfs();
    void restartNfs();
    void stopNfs();
    void StartStopRestProcess(int exitCode, QProcess::ExitStatus);
    void StartStopRest();
    void statusNfs();
    //Add Nfs directory
    void addNfsConfig();
    void modifyNfsConfig();
    void delNfsConfig();
    //Save file
    void saveFile(QString nameFile,QStringList m_list);
    void saveFile();

private:
    Ui::NfsShare *ui;
    QStandardItemModel *model;
    QProcess *proc;
    SettingsManager *manager;
};

//Nfs Config
class NfsConfig : public QDialog
{
   Q_OBJECT
   
public:
    explicit NfsConfig(QStringList list, QString create_modify, QWidget *parent = nullptr);
    ~NfsConfig();
    
private slots:
    void openDirectory();
    void saveFile();
private:
    Ui::NfsConfig *_ui;
    QStringList m_list;
    QString m_createModify;

signals:
    void saveConfig(QString,QStringList);
    void saveList(QString);
};
#endif // NFSSHARE_H
