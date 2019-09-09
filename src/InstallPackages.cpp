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

#include "InstallPackages.h"
#include "ui_InstallPackages.h"
#include "backend/backend.h"
#include "constant.h"
#include "ctablewidget.h"
#include <QVector>

#include <alpm.h>
#include <alpm_list.h>
#include <alpm_octopi_utils.h>
    
Q_LOGGING_CATEGORY(InstallPackagesDebug,"ControlCenter")

InstallPackages::InstallPackages(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallPackages),
    m_process_group(new QProcess)
{
    ui->setupUi(this);
    setWindowTitle("Install packages");
    connect(ui->applica,&QPushButton::clicked,this,&InstallPackages::ApplyImpo);
    connect(ui->remove,&QPushButton::clicked,this,&InstallPackages::removeImpo);
    connect(ui->cerca_line,&QLineEdit::textChanged,this,&InstallPackages::searchPackages);
    connect(ui->viewInfo,&QToolButton::clicked,this,&InstallPackages::openTabWidget);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->viewInfo->setToolTip("Open information");
    lista();
    setMenuInstall();
    viewPackagesGroup();
    viewRepo();
    openTabWidget(false);
}

void InstallPackages::openTabWidget(bool ok)
{
    ui->tabWidget->setVisible(ok);
}

void InstallPackages::setMenuInstall()
{    
    //Menu file
    menuFile = new QMenu(this);
    menuFile->addSeparator();
    install_local_packages = new QAction(QIcon(":/images/install.png"),"Install local file",this);
    menuFile->addAction(install_local_packages);
    ui->menuFile->setMenu(menuFile);
    menuGroup = new QMenu(this);
    menuGroup->addSeparator();
    boxGroup = new QAction(QIcon(":/images/packages.png"),"View group packages",this);
    boxGroup->setCheckable(true);
    viewGroupsPackages(false);
    m_repo = new QAction(QIcon(":/images/ark.png"),"View repository");
    m_repo->setCheckable(true);
    viewGroupsRepo(false);
    menuGroup->addAction(boxGroup);
    menuGroup->addAction(m_repo);
    ui->menuGroup->setMenu(menuGroup);
    
    connect(install_local_packages,&QAction::triggered,this,&InstallPackages::installLocalPackages);
    connect(boxGroup,&QAction::triggered,this,&InstallPackages::viewGroupsPackages);
    connect(m_repo,&QAction::triggered,this,&InstallPackages::viewGroupsRepo);
}

void InstallPackages::installLocalPackages()
{
    QString fileName = QFileDialog::getOpenFileName(this, QObject::tr("Open installer"),
                                                            "*.tar.xz", QObject::tr("Installer(*.tar.xz)"));
    
    if(!fileName.indexOf(".tar.xz"))
        QMessageBox::warning(this,"CondresControlCenter","Support file install is tar.xz");
    else if(!fileName.isEmpty()){
        m_apply = new Apply(fileName,"local",this);
        m_apply->exec();
    }
}

void InstallPackages::viewGroupsPackages(bool vero)
{
    ui->groupBoxPackages->setVisible(vero);
    ui->groupBoxRepo->setVisible(false);
    if(boxGroup->isChecked())
        m_repo->setChecked(false);
}

void InstallPackages::viewGroupsRepo(bool vero)
{
    ui->groupBoxRepo->setVisible(vero);
    ui->groupBoxPackages->setVisible(false);
    if(m_repo->isChecked())
        boxGroup->setChecked(false);
}

void InstallPackages::viewRepo()
{
    viewGroupsPackages(false);
    QStringList m_list = Backend::getPackageList();
    QStringList repository;
    foreach(QString txt, m_list){
        QStringList list = txt.split(" ");
        repository << "All list" <<list.at(1);
        repository.removeDuplicates();
        repository.sort();
    }
    ui->listWidgetRepo->addItems(repository);
    connect(ui->listWidgetRepo,&QListWidget::itemClicked,this,&InstallPackages::clickListRepo);
}

void InstallPackages::clickListRepo(QListWidgetItem* item)
{
    ui->tableWidget->setRowCount(0);
    for ( int i = 0; i < ui->tableWidget->rowCount(); i++ )
    {
        ui->tableWidget->removeRow(i);
    }
    
    if(item->text() != "")
    {
        if(item->text() == "All list")
            lista();
        else{
            QStringList m_listGroup = Backend::getRepoList(item->text().toStdString().c_str());
            foreach(QString m_text, m_listGroup)
            {
                QStringList list = m_text.split(" ");
                m_item = new QTableWidgetItem(" ");
                if(list.at(0) == "i"){
                    m_item->data(Qt::CheckStateRole);
                    m_item->setData(Qt::BackgroundRole,QColor(197,255,201));
                    m_item->setCheckState(Qt::Checked);
                }
                else if(list.at(0) == "n"){
                    m_item->data(Qt::CheckStateRole);
                    m_item->setData(Qt::BackgroundRole,QColor(255,113,103));
                    m_item->setCheckState(Qt::Unchecked);
                }
                else if(list.at(0) == "n"){
                    m_item->data(Qt::CheckStateRole);
                    m_item->setData(Qt::BackgroundRole,QColor(174,220,255));
                    m_item->setCheckState(Qt::Checked);
                }
                name = new QTableWidgetItem(QString(list.at(2)));
                versione = new QTableWidgetItem(QString(list.at(3)));
                repo = new QTableWidgetItem(QString(list.at(1)));
                Peso = new QTableWidgetItem(getPeso(list.at(5)));
                pesoCount = new QTableWidgetItem(list.at(5));
                int row = ui->tableWidget->rowCount();
                ui->tableWidget->insertRow(row);
                ui->tableWidget->setItem(row,0,m_item);
                ui->tableWidget->setItem(row,1,name);
                ui->tableWidget->setItem(row,2,versione);
                ui->tableWidget->setItem(row,3,repo);
                ui->tableWidget->setItem(row,4,Peso);
                ui->tableWidget->setItem(row,5,pesoCount);
                QStringList m_desc = m_text.split("\t");
                desc = new QTableWidgetItem(QString(m_desc.at(1)));
                desc->setFlags(Qt::ItemIsSelectable);
                ui->tableWidget->setItem(row,6,desc);
                ui->tableWidget->setColumnHidden(6,true);
            }
        }
    }
    connect(ui->tableWidget,&CTableWidget::addInstallPkg,this,&InstallPackages::installGroup);
}

void InstallPackages::viewPackagesGroup()
{
    viewGroupsRepo(false);
    ui->listWidgetPackages->clear();
    m_process_group->setReadChannel(QProcess::StandardOutput);
    m_process_group->start("pacman -Sg");
    if (m_process_group->waitForStarted(3000))
    {
        if(m_process_group->waitForReadyRead(-1))
        {
            m_process_group->waitForFinished(-1);
            QString results = m_process_group->readAllStandardOutput();
            QStringList m_list = results.split("\n");
            m_list.sort();
            ui->listWidgetPackages->addItems(m_list);
            if(ui->listWidgetPackages->item(0)->text() == "")
                ui->listWidgetPackages->item(0)->setText("Group all");
        }
    }
    connect(ui->listWidgetPackages,&QListWidget::itemClicked,this,&InstallPackages::clickListItem);
}

void InstallPackages::clickListItem(QListWidgetItem* item)
{
    ui->tableWidget->setRowCount(0);
    for ( int i = 0; i < ui->tableWidget->rowCount(); i++ )
    {
        ui->tableWidget->removeRow(i);
    }
    
    if(item->text() != "")
    {
        if(item->text() == "Group all")
            lista();
        else{
            QStringList m_listGroup = Backend::getGroupList(item->text().toStdString().c_str());
            foreach(QString m_text, m_listGroup)
            {
                QStringList list = m_text.split(" ");
                m_item = new QTableWidgetItem(" ");
                if(list.at(0) == "i"){
                    m_item->data(Qt::CheckStateRole);
                    m_item->setData(Qt::BackgroundRole,QColor(197,255,201));
                    m_item->setCheckState(Qt::Checked);
                }
                else if(list.at(0) == "n"){
                    m_item->data(Qt::CheckStateRole);
                    m_item->setData(Qt::BackgroundRole,QColor(255,113,103));
                    m_item->setCheckState(Qt::Unchecked);
                }
                else if(list.at(0) == "n"){
                    m_item->data(Qt::CheckStateRole);
                    m_item->setData(Qt::BackgroundRole,QColor(174,220,255));
                    m_item->setCheckState(Qt::Checked);
                }
                name = new QTableWidgetItem(QString(list.at(2)));
                versione = new QTableWidgetItem(QString(list.at(3)));
                repo = new QTableWidgetItem(QString(list.at(1)));
                Peso = new QTableWidgetItem(getPeso(list.at(5)));
                pesoCount = new QTableWidgetItem(list.at(5));
                int row = ui->tableWidget->rowCount();
                ui->tableWidget->insertRow(row);
                ui->tableWidget->setItem(row,0,m_item);
                ui->tableWidget->setItem(row,1,name);
                ui->tableWidget->setItem(row,2,versione);
                ui->tableWidget->setItem(row,3,repo);
                ui->tableWidget->setItem(row,4,Peso);
                ui->tableWidget->setItem(row,5,pesoCount);
                QStringList m_desc = m_text.split("\t");
                desc = new QTableWidgetItem(QString(m_desc.at(1)));
                desc->setFlags(Qt::ItemIsSelectable);
                ui->tableWidget->setItem(row,6,desc);
                ui->tableWidget->setColumnHidden(6,true);
            }
        }
    }
    connect(ui->tableWidget,&CTableWidget::addInstallPkg,this,&InstallPackages::installGroup);
}

void InstallPackages::lista()
{
    m_table = new TableWidgetHeader(Qt::Horizontal);
    m_table->setSectionsClickable(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setHorizontalHeader(m_table);
    ui->tableWidget->setColumnHidden(5,true);
    ui->tableWidget->setColumnWidth(0,30);
    ui->tableWidget->setColumnWidth(1,400);
    ui->tableWidget->setColumnWidth(2,150);
    ui->tableWidget->setColumnWidth(3,150);
    ui->tableWidget->setColumnWidth(4,100);
    ui->tableWidget->verticalHeader()->setVisible(false);
    QStringList m_list = Backend::getPackageList();
    foreach(QString txt, m_list){
        QStringList list = txt.split(" ");
        m_item = new QTableWidgetItem(" ");
        if(list.at(0) == "i"){
            m_item->data(Qt::CheckStateRole);
            m_item->setData(Qt::BackgroundRole,QColor(197,255,201));
            m_item->setCheckState(Qt::Checked);
        }
        else if(list.at(0) == "n"){
            m_item->data(Qt::CheckStateRole);
            m_item->setData(Qt::BackgroundRole,QColor(255,113,103));
            m_item->setCheckState(Qt::Unchecked);
        }
        else if(list.at(0) == "n"){
            m_item->data(Qt::CheckStateRole);
            m_item->setData(Qt::BackgroundRole,QColor(174,220,255));
            m_item->setCheckState(Qt::Checked);
        }
        name = new QTableWidgetItem(QString(list.at(2)));
        versione = new QTableWidgetItem(QString(list.at(3)));
        repo = new QTableWidgetItem(QString(list.at(1)));
        Peso = new QTableWidgetItem(getPeso(list.at(5)));
        pesoCount = new QTableWidgetItem(list.at(5));
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row,0,m_item);
        ui->tableWidget->setItem(row,1,name);
        ui->tableWidget->setItem(row,2,versione);
        ui->tableWidget->setItem(row,3,repo);
        ui->tableWidget->setItem(row,4,Peso);
        ui->tableWidget->setItem(row,5,pesoCount);
        QStringList m_desc = txt.split("\t");
        desc = new QTableWidgetItem(QString(m_desc.at(1)));
        desc->setFlags(Qt::ItemIsSelectable);
        ui->tableWidget->setItem(row,6,desc);
        ui->tableWidget->setColumnHidden(6,true);
    }
    connect(ui->tableWidget,&QTableWidget::clicked,this,&InstallPackages::TableClicked);
    connect(ui->tableWidget,&QTableWidget::itemChanged,this,&InstallPackages::TableClickedItem);
}

void InstallPackages::searchPackages(QString text)
{
    qCDebug(InstallPackagesDebug) << text;
    for( int i = 0; i < ui->tableWidget->rowCount(); ++i )
    {
        bool match = false;
        for( int j = 0; j < ui->tableWidget->columnCount(); ++j )
        {
            QTableWidgetItem *item = ui->tableWidget->item( i, j );
            if( item->text().contains(text) )
            {
                match = true;
                break;
            }
        }
        ui->tableWidget->setRowHidden( i, !match );
    }
}

void InstallPackages::TableClicked(const QModelIndex &index)
{
    if(index.isValid())
    {
        QString name = ui->tableWidget->selectionModel()->model()->data(ui->tableWidget->selectionModel()->model()->index(index.row(),1),Qt::DisplayRole).toString();
        QString version = ui->tableWidget->selectionModel()->model()->data(ui->tableWidget->selectionModel()->model()->index(index.row(),2),Qt::DisplayRole).toString();
        QString repo = ui->tableWidget->selectionModel()->model()->data(ui->tableWidget->selectionModel()->model()->index(index.row(),3),Qt::DisplayRole).toString();
        QString m_size = ui->tableWidget->selectionModel()->model()->data(ui->tableWidget->selectionModel()->model()->index(index.row(),4),Qt::DisplayRole).toString();
        QString m_desc = ui->tableWidget->selectionModel()->model()->data(ui->tableWidget->selectionModel()->model()->index(index.row(),6),Qt::DisplayRole).toString();
        QStringList m_list = Backend::getDepsPackages(name.toStdString().c_str());
        QString html;
        html += "<p align=justify>Description: <b>"+m_desc+"</b></p></br>"
                 "<p align=justify>Software: <b>"+name+"</b></p></br>";
        if(!m_list.isEmpty())
            html += "<p align=justify>Depends: <b>"+QString(m_list.join(" "))+"</b></p></br>";
        else
            html += "<p align=justify>Depends: <b>none</b></p></br>";
        html += "<p align=justify>Version: <b>"+version+"</b></p></br>"
                "<p align=justify>Repository: <b>"+repo+"</b></p></br>"
                "<p align=justify>Size: <b>"+m_size+"</b></p></br>";
        ui->info->setHtml(html);
        ui->listFile->clear();
        QString results = getProcess("/usr/bin/pacman -Ql",name);
        QStringList resultsVector = results.split(((QRegExp) "\n"),QString::SkipEmptyParts);
        for(int i = 0; i < resultsVector.length(); ++i){
                QString line = resultsVector.at(i);
                ui->listFile->append(line);
        }
    }
}

void InstallPackages::TableClickedItem(QTableWidgetItem *item)
{
    if(item->column() == 0){

        if(!item)
           return;

        QStringList m_list;
        QModelIndex m_index;
        m_index = ui->tableWidget->model()->index(item->row(),0,QModelIndex());
        QString name = ui->tableWidget->model()->index(item->row(),1,QModelIndex()).data(Qt::DisplayRole).toString();
        if(m_index.data(Qt::CheckStateRole) == Qt::Checked)
        {
            
                ui->textEditRemove->clear();
                QStringList list;
                list << name;
                foreach(QString m_name, list)
                    ui->textEdit->append(m_name);
        }
        else if(m_index.data(Qt::CheckStateRole) == Qt::Unchecked){
                ui->textEdit->clear();
                QStringList list;
                list << name;
                foreach(QString m_name, list)
                    ui->textEditRemove->append(m_name);
        }
    }
}

/*void InstallPackages::readPackages(QString pkg)
{
    if(!pkg.isEmpty())
    {
        QProcess proc;
        QString results = getProcessQuery("/usr/bin/pacman -Qqk",pkg);        
        QStringList resultsVector = results.split(((QRegExp) "\n"),QString::SkipEmptyParts);
        foreach(QString deps, resultsVector)
        {
            QStringList m_deps = deps.split("'");
            setDepends(QStringList() << m_deps.at(1));
        }
        m_depends = new Depends(getDepends(),this);
        connect(m_depends,&Depends::save,[this](const QVariant &results){
            ui->textEdit->append(results.toString());
        });
        m_depends->exec();
    }
}

void InstallPackages::setDepends(QStringList deps)
{
    _deps.append(deps);
    _deps.removeDuplicates();
}

QStringList InstallPackages::getDepends()
{
    return _deps;
}*/

void InstallPackages::ApplyImpo()
{
    if(ui->textEdit->toPlainText().length() == 0)
        QMessageBox::warning(this,"CondresControlCenter","No packages install selected");
    else{
        QStringList list;
        list << ui->textEdit->toPlainText();
        m_apply = new Apply(list,"install",this);
        m_apply->exec();
    }
}

void InstallPackages::removeImpo()
{
    if(ui->textEditRemove->toPlainText().length() == 0)
        QMessageBox::warning(this,"CondresControlCenter","No packages remove selected");
    else{
        QStringList list;
        list << ui->textEditRemove->toPlainText();
        m_apply = new Apply(list,"remove",this);
        m_apply->exec();
    }
}

InstallPackages::~InstallPackages()
{
    delete ui;
}

void InstallPackages::on_pul_list_clicked()
{
    ui->textEdit->clear();
    ui->textEditRemove->clear();
}

void InstallPackages::installGroup(QString pkg, QString in_rem)
{
    if(in_rem == "install"){
        ui->textEditRemove->clear();
        ui->textEdit->append(pkg);
    }
    else if(in_rem == "remove")
    {
        ui->textEdit->clear();
        ui->textEditRemove->append(pkg);
    }
}
