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

#include "upgrade.h"
#include "ui_upgrade.h"
#include <iostream>
#include "backend/backend.h"
#include "constant.h"

Q_LOGGING_CATEGORY(ControlCenterUpgrade, "ControlCenter")

Upgrade::Upgrade(QString database,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Upgrade),
    processDB(new QProcess),
    processUpgrade(new QProcess),
    remove_db(new QProcess),
    m_updateDB(false),
    m_database(database),
    manager(new SettingsManager)
{
    ui->setupUi(this);
    setWindowTitle("Update system");
    QMovie *mov = new QMovie(":/images/loading.gif");
    mov->setScaledSize(QSize(300,300));
    ui->label_movie->setMovie(mov);
    mov->start();
    connect(ui->applica,&QPushButton::clicked,this,&Upgrade::updatePackages);
    connect(ui->tableWidget,&QTableWidget::itemChanged,this,&Upgrade::itemClicked);
    connect(ui->pushButtonStop,&QPushButton::clicked,this,&Upgrade::stopJobs);
    lista();
    getListUpdate();
}

void Upgrade::lista()
{
    m_table = new TableWidgetHeader(Qt::Horizontal);
    m_table->setSectionsClickable(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeader(m_table);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setColumnHidden(5,true);
    ui->tableWidget->setColumnWidth(0,30);
    ui->tableWidget->setColumnWidth(1,250);
    ui->tableWidget->setColumnWidth(2,150);
    ui->tableWidget->setColumnWidth(3,150);
    ui->tableWidget->setColumnWidth(4,150);
    ui->progressBar->setVisible(true);
}

bool Upgrade::NoConnection()
{
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool result = false;
    for (int i = 0; i < ifaces.count(); i++)
    {
       QNetworkInterface iface = ifaces.at(i);
       if (iface.flags().testFlag(QNetworkInterface::IsUp)
           && !iface.flags().testFlag(QNetworkInterface::IsLoopBack))
       {
           for (int j = 0; j < iface.addressEntries().count(); j++)
           {
              // got an interface which is up, and has an ip address
              if (result == false)
                 result = true;
           }
       }
    }
    return result;
}

void Upgrade::getListUpdate()
{
    if (!NoConnection())
        QMessageBox::warning(this,"Condres OS Control Center","Unable to estabilish internet connection");
    lista();
    removeDBPacman();
    ui->tableWidget->setRowCount(0);
    for ( int i = 0; i < ui->tableWidget->rowCount(); i++ )
    {
        ui->tableWidget->removeRow(i);
    }
    ui->console->append("<font color=\"white\">Update view list</font>");
    ui->up_progress->setText("Update view list");
    QStringList list = Backend::getUpdateList();
    if(list.isEmpty()){
            ui->stackedWidget->setCurrentIndex(3);
            ui->applica->setVisible(false);
            ui->dettagli->setVisible(false);
            ui->pushButtonStop->setVisible(false);
            ui->progressBar->setVisible(false);
            emit finishUpdate(true);
    }
    else{
        foreach(QString txt, list){    
            if(!txt.isEmpty())
            {
                ui->stackedWidget->setCurrentIndex(1);
                ui->download->setVisible(true);
                ui->up_progress->setVisible(true);
                QStringList resultsVector = txt.split(" ");
                ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr(" ") << tr("Name") <<
                                                            tr("Version") << tr("Repository") << tr("Size") << tr("Peso"));
                QTableWidgetItem *item = new QTableWidgetItem(QString(resultsVector.at(0)));
                item->data(Qt::CheckStateRole);
                item->setCheckState(Qt::Unchecked);
                QTableWidgetItem *name = new QTableWidgetItem(QString(resultsVector.at(2)));
                QTableWidgetItem *versione = new QTableWidgetItem(QString(resultsVector.at(3)));
                QTableWidgetItem *repo = new QTableWidgetItem(QString(resultsVector.at(1)));
                QTableWidgetItem *Peso = new QTableWidgetItem(getPeso(resultsVector.at(4)));
                QTableWidgetItem *pesoCount = new QTableWidgetItem(resultsVector.at(4));
                int row = ui->tableWidget->rowCount();
                ui->tableWidget->insertRow(row);
                ui->tableWidget->setItem(row,0,item);
                ui->tableWidget->setItem(row,1,name);
                ui->tableWidget->setItem(row,2,versione);
                ui->tableWidget->setItem(row,3,repo);
                ui->tableWidget->setItem(row,4,Peso);
                ui->tableWidget->setItem(row,5,pesoCount);
                ui->tableWidget->setColumnHidden(5,true);
                connect(m_table,&TableWidgetHeader::checkStausChange, [item](bool check){
                        if(check == true)
                            item->setCheckState(Qt::Checked);
                        else
                            item->setCheckState(Qt::Unchecked);
                });
                double peso = 0;
                for(int x=0; x < ui->tableWidget->rowCount(); ++x){
                    peso += ui->tableWidget->item(x,5)->text().toDouble();
                }
                ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
                ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
                ui->download->clear();
                ui->download->setText("<center><font size=\"4\"><b>Total size download: "+getPeso(QString::number(peso))+"</b></font></center>");
            }
        }
    }
}

Upgrade::~Upgrade()
{
    delete ui;
}

void Upgrade::on_dettagli_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void Upgrade::on_back_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Upgrade::updatePackages()
{
    ui->progressBar->setMinimum(0);
    QStringList list1;
    if (!NoConnection())
       QMessageBox::warning(this,"Condres OS Control Center","Unable to estabilish internet connection");

    removeDBPacman();

    ui->console->append("<font color=\"white\">Update packages</font>");
    ui->up_progress->setText("Update packages");
    connect(processUpgrade,&QProcess::readyReadStandardOutput,this,&Upgrade::showProgressInqDebug);
    connect(processUpgrade,static_cast<void (QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),this,&Upgrade::updatePackagesProcess);
    list1 << "-S" << "--noconfirm" << getList();
    if(getList().isEmpty()){
        ui->console->append("<font color=\"white\">Select a packages</font>");
        QMessageBox::warning(this,"Condres OS Control Center","Select a packages");
    }
    else{
        processUpgrade->start("/usr/bin/pacman",list1);
    }
}

void Upgrade::updatePackagesProcess(int exitCode, QProcess::ExitStatus)
{
    if(exitCode > 1)
    {
        ui->console->append("<font color=\"white\">Cannot start</font>");
        ui->console->append("<font color=\"white\">"+QString(processUpgrade->error())+"</font>");
        ui->console->append(processUpgrade->errorString());
    }
    else if(exitCode == 0){
        ui->console->append("<font color=\"white\">Update complete</font>");
        ui->up_progress->setText("<font color=\"white\">Update complete</font>");
        lista();
        getListUpdate();
        emit finishUpdate(true);
    }

}

void Upgrade::showProgressInqDebug()
{
    QString m_list = processUpgrade->readAll();
    QStringList lines = m_list.split(QRegExp("\\n"), QString::SkipEmptyParts);
    //process package list
    for(int i = 0; i < lines.length(); i++)
    {
       QString line = lines.at(i);
       
       if(line.isEmpty())
            continue;
        
       ui->progressBar->setMaximum(i);
       ui->progressBar->setValue(i);
       QString str;
       str.append(line);
       ui->up_progress->setText(str);
       ui->console->append("<font color=\"white\">"+line+"</font>");
    }
}


void Upgrade::itemClicked(QTableWidgetItem *item)
{
    if(item->column() == 0){

        if(!item)
           return;

        QStringList m_list;
        QModelIndex index;
        for(int i=0; i < ui->tableWidget->rowCount() && i < item->row()+1; i++ ){
            index = ui->tableWidget->model()->index(i,0,QModelIndex());
            if(index.data(Qt::CheckStateRole) == Qt::Checked)
                m_list << ui->tableWidget->model()->index(i,1,QModelIndex()).data(Qt::DisplayRole).toString();
        }
        setList(m_list);
    }
}

void Upgrade::setList(QStringList item)
{
    item.removeAll("");
    m_addItem = item;
}

QStringList Upgrade::getList()
{
    return m_addItem;
}

void Upgrade::removeDBPacman()
{
    remove_db->start("rm " +QString(DATABASE_VAR));
    ui->console->setText("<font color=\"white\">Remove database pacman </font>");
}

void Upgrade::stopJobs()
{
    QProcess pacman;
    QString command = "killall pacman";
    pacman.startDetached(command);
    removeDBPacman();
}
