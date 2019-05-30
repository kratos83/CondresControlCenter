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
#ifndef LOGFILE_H
#define LOGFILE_H

#include <QDialog>
#include <QtCore>
#include <QtWidgets>

namespace Ui {
class LogFile;
}

class LogFile : public QDialog
{
    Q_OBJECT

public:
    explicit LogFile(QString name, QWidget *parent = nullptr);
    ~LogFile();

private:
    Ui::LogFile *ui;
    QString m_name;
};

#endif // LOGFILE_H
