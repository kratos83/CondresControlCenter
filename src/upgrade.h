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
#ifndef UPGRADE_H
#define UPGRADE_H

#include <QDialog>
#include <QtCore>
#include <QtWidgets>
#include <QtNetwork>
#include "tablewidgetheader.h"
#include <QLoggingCategory>
#include "settingsmanager.h"
#include "constant.h"
Q_DECLARE_LOGGING_CATEGORY(ControlCenterUpgrade)

namespace Ui {
class Upgrade;
}

class Upgrade : public QDialog
{
    Q_OBJECT

public:
    explicit Upgrade(QString database, QWidget *parent = nullptr);
    ~Upgrade();
    bool NoConnection();

public slots:
    //View list
    void getListUpdate();
    void lista();
    void itemClicked(QTableWidgetItem *item);
    void setList(QStringList item);
    QStringList getList();
    void removeDBPacman();
    //Update packages
    void updatePackages();
    void updatePackagesProcess(int exitCode, QProcess::ExitStatus);
    void showProgressInqDebug();
    void stopJobs();
    
private slots:
    void on_dettagli_clicked();
    void on_back_clicked();

private:
    Ui::Upgrade *ui;
    QProcess processUpdate,*processDB,*processUpgrade,*remove_db;
    QStringList m_update, m_upgrade, m_addItem;
    bool m_updateDB;
    QString m_peso, m_database;
    TableWidgetHeader *m_table;
    SettingsManager *manager;
signals:
    void finishUpdate(bool);
};

#endif // UPGRADE_H
