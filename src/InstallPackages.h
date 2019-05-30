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
#ifndef INSTALLPACKAGES_H
#define INSTALLPACKAGES_H

#include <QDialog>
#include <QtWidgets>
#include <QtCore>
#include <QLoggingCategory>
#include "tablewidgetheader.h"
#include "Apply.h"

Q_DECLARE_LOGGING_CATEGORY(InstallPackagesDebug)

namespace Ui {
class InstallPackages;
}

class InstallPackages : public QDialog
{
    Q_OBJECT

public:
    explicit InstallPackages(QWidget *parent = nullptr);
    ~InstallPackages();
    void lista();
    void setMenuInstall();
public slots:
    QString getPeso(QString byteReceived);
    void TableClicked(const QModelIndex &index);
    void TableClickedItem(QTableWidgetItem *item);
    void ApplyImpo();
    void removeImpo();
    void searchPackages(QString text);
    void on_pul_list_clicked();
    void viewGroupsPackages(bool vero);
    void viewGroupsRepo(bool vero);
    void viewPackagesGroup();
    void viewRepo();
    void installLocalPackages();
    void clickListItem(QListWidgetItem *item);
    void clickListRepo(QListWidgetItem *item);
    void openTabWidget(bool ok);

private:
    Ui::InstallPackages *ui;
    TableWidgetHeader *m_table;
    QString m_peso;
    Apply *m_apply;
    QStringList m_string;
    QMenu *menu, *menuFile, *menuGroup;
    QAction *install_local_packages, *boxGroup, *m_repo;
    QProcess *m_process_group,*m_process_list;
    QTableWidget *widgetLista,*widgetListaGroup;
    QTableWidgetItem *name,*m_item, *versione,*repo, *Peso, *pesoCount, *desc;
};

#endif // INSTALLPACKAGES_H
