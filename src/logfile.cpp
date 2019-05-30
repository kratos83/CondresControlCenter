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
#include "logfile.h"
#include "ui_logfile.h"

LogFile::LogFile(QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogFile),
    m_name(name)
{
    ui->setupUi(this);
    QFile file(m_name);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       QTextStream stream(&file);
       while(!stream.atEnd()){
          QString line = stream.readLine();
          ui->textEdit->append(line);
       }
    }
}

LogFile::~LogFile()
{
    delete ui;
}
