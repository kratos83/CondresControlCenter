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
#include "esci.h"
#include "ui_esci.h"

esci::esci(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::esci)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QIcon icon(":/images/application-exit.png");
    ui->label->setPixmap(icon.pixmap(QSize(64,64)));
}

esci::~esci()
{
    delete ui;
}

void esci::on_pushButton_2_clicked()
{
    qApp->quit();
}

void esci::on_pushButton_clicked()
{
    close();
}
