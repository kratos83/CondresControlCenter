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
#ifndef APPLY_H
#define APPLY_H

#include <QDialog>
#include <QtCore>
#include <QtWidgets>
#include <QtNetwork>

Q_DECLARE_LOGGING_CATEGORY(CondresApplyPackages)

namespace Ui{
    class Apply;
}

class Apply : public QDialog
{
public:
    //Install list packages
    Apply(QStringList listPackages, QString install_remove,
          QWidget *parent = nullptr);
    //Instlla single packages
    Apply(QString args, QString install_remove,
          QWidget *parent = nullptr);
    ~Apply();

public slots:
    void installPackages();
    void removePackages();
    void closeDialog();
    void read_packages(int exitCode);
    void ReadyPkg();
    void localPackages();
    void stopJobs();
private:
    Ui::Apply *ui;
    QString _arg, m_args, m_install_remove;
    QStringList m_list;
    QProcess *m_process,*m_process_remove,*m_process_local;
};

#endif // APPLY_H
