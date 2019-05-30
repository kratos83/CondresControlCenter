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

#include "Samba.h"
#include "ui_Samba.h"
#include "ui_samba_conf_dir.h"
#include "ui_SambaPrinter.h"
#include "ui_sambauser.h"
#include "constant.h"
#include "itdelegato.h"

Q_LOGGING_CATEGORY(CondresSambaConfig,"ControlCenter")

Samba::Samba(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Samba),
    proc(new QProcess),
    m_modify(false),
    m_edit(false),
    manager(new SettingsManager)
{
    ui->setupUi(this);
    connect(ui->loadConfigFile,&QPushButton::clicked,this,&Samba::loadFile);
    connect(ui->pushButton,&QPushButton::clicked,this,&Samba::startSamba);
    connect(ui->pushButton_2,&QPushButton::clicked,this,&Samba::restartSamba);
    connect(ui->pushButton_3,&QPushButton::clicked,this,&Samba::stopSamba);
    connect(ui->pushButton_4,&QPushButton::clicked,this,&Samba::statusSamba);
    connect(ui->checkBoxTerminal,&CheckBox::clicked,this,&Samba::readChecBox);
    connect(ui->modify_file,&QPushButton::clicked,this,&Samba::openConfigDir);
    connect(ui->add_share,&QPushButton::clicked,this,&Samba::addCongigDir);
    connect(ui->remove_file,&QPushButton::clicked,this,&Samba::delSambaShare);
    connect(ui->pushButtonApply,&QPushButton::clicked,this,static_cast< void(Samba::*)()>(&Samba::saveFile));
    //Samba printer
    connect(ui->add_share_printer,&QPushButton::clicked,this,&Samba::addSambaPrinter);
    connect(ui->modify_printer,&QPushButton::clicked,this,&Samba::configSambaPrinter);
    connect(ui->remove_printer,&QPushButton::clicked,this,&Samba::delSambaPrinter);
    //SambaUser
    connect(ui->add_user,&QPushButton::clicked,this,&Samba::addUser);
    connect(ui->modify_user,&QPushButton::clicked,this,&Samba::configUser);
    connect(ui->remove_user,&QPushButton::clicked,this,&Samba::delUser);
    connect(ui->open_user,&QPushButton::clicked,this,&Samba::openUser);
    load(SAMBA_FILE);
    readShareFileDirectory();
    readSharePrinter();
    readUser();
    readChecBox(false);
}

void Samba::load(QString nameFile)
{
    Q_UNUSED(nameFile)
    
    QSqlQuery query;
    query.prepare("select * from samba");
    query.exec();
    while(query.next()){
        ui->lineEditSambaConfig->setText(SAMBA_FILE);
        ui->lineEditWorkgroup->setText(query.value("workgroup").toString());
        ui->lineEditServer->setText(query.value("server_string").toString());
        ui->lineEditSecurity->setText(query.value("security").toString());
        ui->lineEditNetbios->setText(query.value("netbios").toString());
        ui->lineEditHost->setText(query.value("host_allow").toString());
        ui->lineEditGuest->setText(query.value("guest_account").toString());
        ui->lineEdit->setText(query.value("log_file").toString());
        ui->spinBoxLogSize->setValue(query.value("max_log_size").toInt());
        ui->comboBoxLoadPrinters->setCurrentText(query.value("load_printers").toString());
        ui->lineEditPrinting->setText(query.value("printing").toString());
    }   
}

void Samba::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, QObject::tr("Open config file samba"),
                                                            "*.conf", QObject::tr("Config file(*.conf)"));
    
    if(!fileName.contains("smb.conf"))
        QMessageBox::warning(this,"CondresControlCenter","Can't open file <b>"+QFileInfo(fileName).fileName()+"</b>. Find smb.conf in /etc/samba");
    else
        load(fileName);
}

void Samba::restartSamba()
{  
    proc->setReadChannel(QProcess::StandardOutput);
    proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(proc,&QProcess::readyReadStandardOutput,this,&Samba::StartStopRest);
    connect(proc,static_cast<void (QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),this,&Samba::StartStopRestProcess);
    proc->start("/usr/bin/systemctl restart smb");
}

void Samba::startSamba()
{
    proc->setReadChannel(QProcess::StandardOutput);
    proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(proc,&QProcess::readyReadStandardOutput,this,&Samba::StartStopRest);
    connect(proc,static_cast<void (QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),this,&Samba::StartStopRestProcess);
    proc->start("/usr/bin/systemctl start smb");
}

void Samba::stopSamba()
{
    proc->setReadChannel(QProcess::StandardOutput);
    proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(proc,&QProcess::readyReadStandardOutput,this,&Samba::StartStopRest);
    connect(proc,static_cast<void (QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),this,&Samba::StartStopRestProcess);
    proc->start("/usr/bin/systemctl stop smb");
}

void Samba::statusSamba()
{
    ui->textEditDebug->clear();
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("systemctl status smb");
    process.closeWriteChannel();
    while(process.state() != QProcess::NotRunning)
    {
        process.waitForReadyRead();
        QString result = process.readAll();
        QStringList resultsVector = result.split(((QRegExp) "\n"));
        for(int i=0; i<resultsVector.size();i++)
        {
            if(QString(resultsVector.at(i)).isEmpty())
                continue;

            QString str;
            str.append(resultsVector.at(i));
            ui->textEditDebug->append("<font color=\"white\">"+resultsVector.at(i)+"</font></br>");
        }
    }
}

void Samba::StartStopRest()
{
    QString results = proc->readAll();
    QStringList resultsVector = results.split(((QRegExp) "\n"));
    for(int i=0; i<resultsVector.size();i++)
    {
        if(QString(resultsVector.at(i)).isEmpty())
            continue;

        QString str;
        str.append(resultsVector.at(i));
        ui->textEditDebug->append("<font color=\"white\">"+resultsVector.at(i)+"</font></br>");
    }
}

void Samba::StartStopRestProcess(int exitCode, QProcess::ExitStatus)
{
    if(exitCode > 1)
    {
      //process failed, provide info on errors
      ui->textEditDebug->clear();
      ui->textEditDebug->append("<font color=\"white\">Error process systemctl</font></br>");
    }
    else if(exitCode == 0){
        ui->textEditDebug->clear();
        ui->textEditDebug->append("<font color=\"white\">Process finished</font></br>");
    }
}

void Samba::readChecBox(bool ok)
{
    ui->textEditDebug->setVisible(ok);
}

void Samba::readShareFileDirectory()
{
    model = new QStandardItemModel(0,10,this);
    model->setHeaderData(0,Qt::Horizontal,tr("Comment"));
    model->setHeaderData(1,Qt::Horizontal,tr("Shared"));
    model->setHeaderData(2,Qt::Horizontal,tr("Browseable"));
    model->setHeaderData(3,Qt::Horizontal,tr("Public"));
    model->setHeaderData(4,Qt::Horizontal,tr("Writable"));
    model->setHeaderData(5,Qt::Horizontal,tr("Printable"));
    model->setHeaderData(6,Qt::Horizontal,tr("Guest"));
    model->setHeaderData(7,Qt::Horizontal,tr("Create mask"));
    model->setHeaderData(8,Qt::Horizontal,tr("Mask directory"));
    model->setHeaderData(9,Qt::Horizontal,tr("Valid users"));
    model->setHeaderData(10,Qt::Horizontal,tr("ID"));
    QSqlQuery query;
    query.prepare("select * from samba_share");
    int count = 0;
    if(query.exec())
        while(query.next())
        {
            QStandardItem *path = new QStandardItem(query.value("path").toString());
            path->setIcon(QIcon(":/images/directory.png"));
            model->setItem(count,0,new QStandardItem(query.value("comment").toString()));
            model->setItem(count,1,path);
            model->setItem(count,2,new QStandardItem(query.value("browseable").toString()));
            model->setItem(count,3,new QStandardItem(query.value("public").toString()));
            model->setItem(count,4,new QStandardItem(query.value("writable").toString()));
            model->setItem(count,5,new QStandardItem(query.value("printable").toString()));
            model->setItem(count,6,new QStandardItem(query.value("guest_ok").toString()));
            model->setItem(count,7,new QStandardItem(query.value("create_mask").toString()));
            model->setItem(count,8,new QStandardItem(query.value("directory_mask").toString()));
            model->setItem(count,9,new QStandardItem(query.value("valid_users").toString()));
            model->setItem(count,10,new QStandardItem(query.value("id").toString()));
            ++count;
        }
    ui->tableViewFile->setModel(model);
    ui->tableViewFile->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewFile->setColumnWidth(1,200);
    ui->tableViewFile->setColumnWidth(7,150);
    ui->tableViewFile->setColumnHidden(10,true);
    ui->tableViewFile->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewFile->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewFile->setItemDelegateForColumn(10,new coldxdelegato(this));
}

void Samba::readSharePrinter()
{
    modelPrinter = new QStandardItemModel(0,10,this);
    modelPrinter->setHeaderData(0,Qt::Horizontal,tr("Printer"));
    modelPrinter->setHeaderData(1,Qt::Horizontal,tr("Comment"));
    modelPrinter->setHeaderData(2,Qt::Horizontal,tr("Directory"));
    modelPrinter->setHeaderData(3,Qt::Horizontal,tr("Writable"));
    modelPrinter->setHeaderData(4,Qt::Horizontal,tr("Browseable"));
    modelPrinter->setHeaderData(5,Qt::Horizontal,tr("Printable"));
    modelPrinter->setHeaderData(6,Qt::Horizontal,tr("ID"));
    modelPrinter->setHeaderData(7,Qt::Horizontal,tr("Guest"));
    modelPrinter->setHeaderData(8,Qt::Horizontal,tr("Printing"));
    modelPrinter->setHeaderData(9,Qt::Horizontal,tr("Valid users"));
    QSqlQuery query;
    query.prepare("select * from samba_print");
    int count = 0;
    if(query.exec())
        while(query.next())
        {
            QStandardItem *printer = new QStandardItem(query.value("printer").toString());
            printer->setIcon(QIcon(":/images/printer.png"));
            modelPrinter->setItem(count,0,printer);
            modelPrinter->setItem(count,1,new QStandardItem(query.value("comment").toString()));
            QStandardItem *path = new QStandardItem(query.value("directory").toString());
            path->setIcon(QIcon(":/images/directory.png"));
            modelPrinter->setItem(count,2,path);
            modelPrinter->setItem(count,3,new QStandardItem(query.value("writable").toString()));
            modelPrinter->setItem(count,4,new QStandardItem(query.value("browseable").toString()));
            modelPrinter->setItem(count,5,new QStandardItem(query.value("printable").toString()));
            modelPrinter->setItem(count,6,new QStandardItem(query.value("id").toString()));
            modelPrinter->setItem(count,7,new QStandardItem(query.value("guest").toString()));
            modelPrinter->setItem(count,8,new QStandardItem(query.value("printing").toString()));
            modelPrinter->setItem(count,9,new QStandardItem(query.value("valid_users").toString()));
            ++count;
        }
    ui->tableView->setModel(modelPrinter);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnWidth(0,200);
    ui->tableView->setColumnWidth(1,150);
    ui->tableView->setColumnHidden(6,true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setItemDelegateForColumn(9,new coldxdelegato(this));
}

void Samba::readUser()
{
    modelUser = new QStandardItemModel(0,2,this);
    modelUser->setHeaderData(0,Qt::Horizontal,tr("User"));
    modelUser->setHeaderData(1,Qt::Horizontal,tr("ID"));
    QSqlQuery query;
    query.prepare("select * from samba_user");
    int count = 0;
    if(query.exec())
        while(query.next())
        {
            QStandardItem *printer = new QStandardItem(query.value("user").toString());
            printer->setIcon(QIcon(":/images/users.png"));
            modelUser->setItem(count,0,printer);
            modelUser->setItem(count,1,new QStandardItem(query.value("id").toString()));
            ++count;
        }
    ui->tableView_2->setModel(modelUser);
    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_2->setColumnWidth(0,200);
    ui->tableView_2->setColumnHidden(1,true);
    ui->tableView_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_2->setItemDelegateForColumn(1,new coldxdelegato(this));
}

void Samba::openConfigDir()
{
    if(ui->tableViewFile->selectionModel()->currentIndex().isValid())
    {
        QModelIndex modelIndex = ui->tableViewFile->selectionModel()->currentIndex();
        int riga = modelIndex.row();
        QModelIndex newindex = modelIndex.sibling(riga,0);
        ui->tableViewFile->setCurrentIndex(newindex);
        if(newindex.row() < model->rowCount())
        {
            QStringList list;
            QString m_com = model->data(model->index(newindex.row(),0),Qt::DisplayRole).toString();
            QString m_dir = model->data(model->index(newindex.row(),1),Qt::DisplayRole).toString();
            QString m_pub = model->data(model->index(newindex.row(),3),Qt::DisplayRole).toString();
            QString m_write = model->data(model->index(newindex.row(),4),Qt::DisplayRole).toString();
            QString m_print = model->data(model->index(newindex.row(),5),Qt::DisplayRole).toString();
            QString m_guest = model->data(model->index(newindex.row(),6),Qt::DisplayRole).toString();
            QString m_cmask = model->data(model->index(newindex.row(),7),Qt::DisplayRole).toString();
            QString m_dirmask = model->data(model->index(newindex.row(),8),Qt::DisplayRole).toString();
            QString m_validUsers = model->data(model->index(newindex.row(),9),Qt::DisplayRole).toString();
            QString m_brow = model->data(model->index(newindex.row(),2),Qt::DisplayRole).toString();
            QString m_id = model->data(model->index(newindex.row(),10),Qt::DisplayRole).toString();
            list << m_com << m_dir << m_brow << m_pub << m_write << m_print << m_guest << m_cmask << m_dirmask << m_validUsers;
            SambaConfiguration *smb = new SambaConfiguration(list,"modify",m_id,this);
            connect(smb,&SambaConfiguration::saveConfig,this,static_cast< void(Samba::*)(QString)>(&Samba::saveFile));
            connect(smb,&SambaConfiguration::saveList,this,&Samba::readShareFileDirectory);
            smb->exec();
        }
    }
    else QMessageBox::warning(this,tr("CondresControlCenter"),tr("Select row to modified"));
}

void Samba::configSambaPrinter()
{
    if(ui->tableView->selectionModel()->currentIndex().isValid())
    {
        QModelIndex modelIndex = ui->tableView->selectionModel()->currentIndex();
        int riga = modelIndex.row();
        QModelIndex newindex = modelIndex.sibling(riga,0);
        ui->tableView->setCurrentIndex(newindex);
        if(newindex.row() < modelPrinter->rowCount())
        {
            QStringList list;
            QString m_printer = modelPrinter->data(modelPrinter->index(newindex.row(),0),Qt::DisplayRole).toString();
            QString m_com = modelPrinter->data(modelPrinter->index(newindex.row(),1),Qt::DisplayRole).toString();
            QString m_dir = modelPrinter->data(modelPrinter->index(newindex.row(),2),Qt::DisplayRole).toString();
            QString m_write = modelPrinter->data(modelPrinter->index(newindex.row(),3),Qt::DisplayRole).toString();
            QString m_brow = modelPrinter->data(modelPrinter->index(newindex.row(),4),Qt::DisplayRole).toString();
            QString m_print = modelPrinter->data(modelPrinter->index(newindex.row(),5),Qt::DisplayRole).toString();
            QString m_guest = modelPrinter->data(modelPrinter->index(newindex.row(),7),Qt::DisplayRole).toString();
            QString m_printing = modelPrinter->data(modelPrinter->index(newindex.row(),8),Qt::DisplayRole).toString();
            QString m_validUsers = modelPrinter->data(modelPrinter->index(newindex.row(),9),Qt::DisplayRole).toString();
            QString m_id = modelPrinter->data(modelPrinter->index(newindex.row(),6),Qt::DisplayRole).toString();
            list << m_printer << m_com << m_dir << m_write << m_brow << m_print << m_guest << m_printing << m_validUsers;
            SambaPrinter *smb = new SambaPrinter(list,"modify",m_id,this);
            connect(smb,&SambaPrinter::saveConfig,this,static_cast< void(Samba::*)(QString)>(&Samba::saveFile));
            connect(smb,&SambaPrinter::saveList,this,&Samba::readSharePrinter);
            smb->exec();
        }
    }
    else QMessageBox::warning(this,tr("CondresControlCenter"),tr("Select row to modified"));
}

void Samba::configUser()
{
    if(ui->tableView_2->selectionModel()->currentIndex().isValid())
    {
        QModelIndex modelIndex = ui->tableView_2->selectionModel()->currentIndex();
        int riga = modelIndex.row();
        QModelIndex newindex = modelIndex.sibling(riga,0);
        ui->tableView_2->setCurrentIndex(newindex);
        if(newindex.row() < modelUser->rowCount())
        {
            QString m_user = modelUser->data(modelUser->index(newindex.row(),0),Qt::DisplayRole).toString();
            QString m_id = modelUser->data(modelUser->index(newindex.row(),6),Qt::DisplayRole).toString();
            SambaUser *smb = new SambaUser(m_id,m_user,"modify",this);
            connect(smb,&SambaUser::saveList,this,&Samba::readUser);
            smb->exec();
        }
    }
    else QMessageBox::warning(this,tr("CondresControlCenter"),tr("Select row to modified"));
}

void Samba::addCongigDir()
{
    QSqlQuery query;
    query.prepare("select count(id)+1 from samba_share");
    query.exec();
    if(query.next()){
        SambaConfiguration *smb = new SambaConfiguration(QStringList(),"edit",query.value(0).toString(),this);
        connect(smb,&SambaConfiguration::saveConfig,this,static_cast< void(Samba::*)(QString)>(&Samba::saveFile));
        connect(smb,&SambaConfiguration::saveList,this,&Samba::readShareFileDirectory);
        smb->exec();
    }
}

void Samba::addSambaPrinter()
{
    QSqlQuery query;
    query.prepare("select count(id)+1 from samba_print");
    query.exec();
    if(query.next()){
        SambaPrinter *smb = new SambaPrinter(QStringList(),"edit",query.value(0).toString(),this);
        connect(smb,&SambaPrinter::saveConfig,this,static_cast< void(Samba::*)(QString)>(&Samba::saveFile));
        connect(smb,&SambaPrinter::saveList,this,&Samba::readSharePrinter);
        smb->exec();
    }
}

void Samba::addUser()
{
    QSqlQuery query;
    query.prepare("select count(id)+1 from samba_user");
    query.exec();
    if(query.next()){
        SambaUser *smb = new SambaUser(query.value(0).toString(),QString(),"edit",this);
        connect(smb,&SambaUser::saveList,this,&Samba::readUser);
        smb->exec();
    }
}

void Samba::saveFile(QString nameFile)
{
    QFile file(nameFile);
    if(file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream stream(&file);
        //Global
        stream << "[global]" << "\n";
        stream << QString("\tworkgroup =")+QString(ui->lineEditWorkgroup->text()) << "\n";
        stream << QString("\tserver string =")+QString(ui->lineEditServer->text()) << "\n";
        if(ui->lineEditNetbios->text() == "")
            stream << QString("\t;netbios name =") << "\n";
        else
            stream << QString("\tnetbios name =")+QString(ui->lineEditNetbios->text()) << "\n";
        stream << QString("\tsecurity = ")+QString(ui->lineEditSecurity->text()) << "\n";
        if(ui->lineEditHost->text() == "")
            stream << QString("\t;hosts allow =") << "\n";
        else
            stream << QString("\thosts allow =")+QString(ui->lineEditHost->text()) << "\n";
        stream << QString("\tguest account =")+QString(ui->lineEditGuest->text()) << "\n";
        stream << QString("\tlog file =")+QString(ui->lineEdit->text()) << "\n";
        stream << QString("\tmax log size =")+QString::number(ui->spinBoxLogSize->value()) << "\n";
        stream << QString("\tload printers =")+QString(ui->comboBoxLoadPrinters->currentText()) << "\n";
        stream << QString("\tprinting =")+QString(ui->lineEditPrinting->text()) << "\n";
        stream << "\n\n\n\n";
        stream << "#============================ Share Definitions ==============================" << "\n";
        for(int i=0; i < ui->tableViewFile->model()->rowCount(); i++){
            if(i != -1)
            {
                QString m_com = ui->tableViewFile->model()->data(ui->tableViewFile->model()->index(i,0),Qt::DisplayRole).toString();
                QString m_dir = ui->tableViewFile->model()->data(ui->tableViewFile->model()->index(i,1),Qt::DisplayRole).toString();
                QString m_brow = ui->tableViewFile->model()->data(ui->tableViewFile->model()->index(i,2),Qt::DisplayRole).toString();
                QString m_pub = ui->tableViewFile->model()->data(ui->tableViewFile->model()->index(i,3),Qt::DisplayRole).toString();
                QString m_write = ui->tableViewFile->model()->data(ui->tableViewFile->model()->index(i,4),Qt::DisplayRole).toString();
                QString m_print = ui->tableViewFile->model()->data(ui->tableViewFile->model()->index(i,5),Qt::DisplayRole).toString();
                QString m_guest = ui->tableViewFile->model()->data(ui->tableViewFile->model()->index(i,6),Qt::DisplayRole).toString();
                QString m_cmask = ui->tableViewFile->model()->data(ui->tableViewFile->model()->index(i,7),Qt::DisplayRole).toString();
                QString m_dirmask = ui->tableViewFile->model()->data(ui->tableViewFile->model()->index(i,8),Qt::DisplayRole).toString();
                QString m_validUsers = ui->tableViewFile->model()->data(ui->tableViewFile->model()->index(i,9),Qt::DisplayRole).toString();
                QStringList dir = QString(m_dir).split("/");
                QVector<QString> str;
                for(int i=0; i < dir.size(); i++)
                    str << dir.at(i);
                stream << QString("["+str.back()).remove("\n")+QString("]") << "\n";
                stream << QString("\tcomment =")+QString(m_com.remove("\n")).remove(" ") << "\n";
                stream << QString("\tpath =")+QString(m_dir.remove("\n")).remove(" ") << "\n";
                stream << QString("\tbrowseable =")+QString(m_brow.remove("\n")).remove(" ") << "\n";
                stream << QString("\tpublic =")+QString(m_pub) << "\n";
                stream << QString("\twritable =")+QString(m_write) << "\n";
                stream << QString("\tprintable =")+QString(m_print) << "\n";
                stream << QString("\tguest ok =")+QString(m_guest) << "\n";
                stream << QString("\tcreate mask =")+QString(m_cmask) << "\n";
                stream << QString("\tdirectory mask =")+QString(m_dirmask) << "\n";
                if(m_validUsers == "")
                    stream << QString("\t;valid users =")+QString("") << "\n";
                else
                    stream << QString("\tvalid users =")+QString(m_validUsers) << "\n";
            }
        }
        for(int x=0; x < ui->tableView->model()->rowCount(); x++){
            if(x != -1)
            {
                QString m_printer = ui->tableView->model()->data(ui->tableView->model()->index(x,0),Qt::DisplayRole).toString();
                QString m_com = ui->tableView->model()->data(ui->tableView->model()->index(x,1),Qt::DisplayRole).toString();
                QString m_dir = ui->tableView->model()->data(ui->tableView->model()->index(x,2),Qt::DisplayRole).toString();
                QString m_write = ui->tableView->model()->data(ui->tableView->model()->index(x,3),Qt::DisplayRole).toString();
                QString m_brow = ui->tableView->model()->data(ui->tableView->model()->index(x,4),Qt::DisplayRole).toString();
                QString m_print = ui->tableView->model()->data(ui->tableView->model()->index(x,5),Qt::DisplayRole).toString();
                QString m_guest = ui->tableView->model()->data(ui->tableView->model()->index(x,7),Qt::DisplayRole).toString();
                QString m_printing = ui->tableView->model()->data(ui->tableView->model()->index(x,8),Qt::DisplayRole).toString();
                QString m_validUsers = ui->tableView->model()->data(ui->tableView->model()->index(x,9),Qt::DisplayRole).toString();
                //Write printer
                stream << QString("["+m_printer).remove("\n")+QString("]") << "\n";
                stream << QString("\tcomment =")+QString(m_com.remove("\n")).remove(" ") << "\n";
                stream << QString("\tpath =")+QString(m_dir.remove("\n")).remove(" ") << "\n";
                stream << QString("\tbrowseable =")+QString(m_brow.remove("\n")).remove(" ") << "\n";
                stream << QString("\twritable =")+QString(m_write) << "\n";
                stream << QString("\tprintable =")+QString(m_print) << "\n";
                stream << QString("\tguest ok =")+QString(m_guest) << "\n";
                stream << QString("\tprinting =")+QString(m_printing) << "\n";
                if(m_validUsers == "")
                    stream << QString("\t;valid users =")+QString("") << "\n";
                else
                    stream << QString("\tvalid users =")+QString(m_validUsers) << "\n";
            }
        }
    }
    file.close();
}

void Samba::saveFile()
{
    QSqlQuery query;
    query.prepare("UPDATE samba SET workgroup='"+ui->lineEditWorkgroup->text()+"', "
                  "server_string='"+ui->lineEditServer->text()+"', security='"+ui->lineEditSecurity->text()+"',"
                  "load_printers='"+ui->comboBoxLoadPrinters->currentText()+"', guest_account='"+ui->lineEditGuest->text()+"', " "log_file='"+ui->lineEdit->text()+"', max_log_size='"+QString::number(ui->spinBoxLogSize->value())+"',"
                  "netbios='"+ui->lineEditNetbios->text()+"', host_allow='"+ui->lineEditHost->text()+"',"
                  "printing='"+ui->lineEditPrinting->text()+"' where id=1");
    if(query.exec())
        qCDebug(CondresSambaConfig) << "update samba config successfull...";
    else
        qCDebug(CondresSambaConfig) << "Cannot update samba config...\n" << query.lastError();
    
    saveFile(SAMBA_TEMP);
    QProcess::startDetached("cp -rv /etc/samba/smb.conf /etc/samba/smb.conf.bak");
    QProcess::startDetached("cp -rv /tmp/smb.conf /etc/samba/smb.conf");
    load(SAMBA_FILE);
    QProcess::startDetached("rm -rf "+QString(SAMBA_TEMP));
}

void Samba::delSambaShare()
{
    QString m_id = ui->tableViewFile->model()->data(ui->tableViewFile->model()->index(ui->tableViewFile->selectionModel()->currentIndex().row(),10),Qt::DisplayRole).toString();
    if (!m_id.isEmpty())
    {
        QSqlQuery query;
        query.prepare("DELETE FROM samba_share WHERE id='"+m_id+"'");
        if (query.exec())
        {
            qCDebug(CondresSambaConfig) << tr("Delete id ")+m_id+tr(" successfull...");
        }
        else
        {
            QMessageBox::warning(this,"SambaConfiguration",tr("Cannot delete ")+query.lastError().text());
        }
    }
    readShareFileDirectory();
}

void Samba::delSambaPrinter()
{
    QString m_id = ui->tableView->model()->data(ui->tableView->model()->index(ui->tableView->selectionModel()->currentIndex().row(),6),Qt::DisplayRole).toString();
    if (!m_id.isEmpty())
    {
        QSqlQuery query;
        query.prepare("DELETE FROM samba_print WHERE id='"+m_id+"'");
        if (query.exec())
        {
            qCDebug(CondresSambaConfig) << tr("Delete id ")+m_id+tr(" successfull...");
        }
        else
        {
            QMessageBox::warning(this,"SambaPrinter",tr("Cannot delete ")+query.lastError().text());
        }
    }
    readSharePrinter();
}

void Samba::delUser()
{
    QString m_id = ui->tableView_2->model()->data(ui->tableView_2->model()->index(ui->tableView_2->selectionModel()->currentIndex().row(),1),Qt::DisplayRole).toString();
    if (!m_id.isEmpty())
    {
        QSqlQuery qctrl;
        qctrl.prepare("SELECT * FROM samba_user WHERE id='"+m_id+"'");
        qctrl.exec();
        if (qctrl.next())
        {
            QProcess::startDetached(QString("smbpasswd -x ")+qctrl.value("user").toString());
        }

        QSqlQuery query;
        query.prepare("DELETE FROM samba_user WHERE id='"+m_id+"'");
        if (query.exec())
        {
            qCDebug(CondresSambaConfig) << tr("Delete id ")+m_id+tr(" successfull...");
        }
        else
        {
            QMessageBox::warning(this,"SambaPrinter",tr("Cannot delete ")+query.lastError().text());
        }
    }
    readUser();
}

void Samba::openUser()
{
    m_pageUsers = new UsersPage(this);
    m_pageUsers->load();
    m_pageUsers->show();
}


Samba::~Samba()
{
    delete ui;
    delete proc;
}

//Samba config directory
SambaConfiguration::SambaConfiguration(QStringList listConfigDir, 
                                       QString create_modify, QString id, QWidget* parent) :
                                       QDialog(parent),
                                       m_ui(new Ui::SambaConfiguration)
{
    m_ui->setupUi(this);
    m_list = listConfigDir;
    m_createModify = create_modify;
    m_id = id;
    m_ui->lineEditDir->setReadOnly(true);
    if(m_createModify == tr("modify"))
    {
        setWindowTitle(tr("Edit Samba configuration"));
        m_ui->lineEditComment->setText(QString(m_list.at(0)));
        m_ui->lineEditDir->setText(QString(m_list.at(1)));
        m_ui->comboBoxPublic->setCurrentText(QString(m_list.at(3)));
        m_ui->comboBoxWritable->setCurrentText(QString(m_list.at(4)));
        m_ui->comboBoxPrintable->setCurrentText(QString(m_list.at(5)));
        m_ui->comboBoxGuest->setCurrentText(QString(m_list.at(6)));
        m_ui->comboBoxBrow->setCurrentText(QString(m_list.at(2)));
        m_ui->lineEditCreateMask->setText(QString(m_list.at(7)));
        m_ui->lineEditDirectoryMask->setText(QString(m_list.at(8)));
        m_ui->lineEditValidUser->setText(QString(m_list.at(9)));
    }
    else if(m_createModify == tr("edit")){ 
        setWindowTitle(tr("Add Samba configuration"));
    }
    connect(m_ui->close,&QPushButton::clicked,this,&SambaConfiguration::close);
    connect(m_ui->save,&QPushButton::clicked,this,&SambaConfiguration::saveFile);
    connect(m_ui->lineEditValidUser,&QLineEdit::editingFinished,[this](){
        if(!m_ui->lineEditValidUser->text().isEmpty())
            m_ui->lineEditValidUser->setText(m_ui->lineEditValidUser->text()+" ");
    });
    connect(m_ui->buttonOpenDir,&QPushButton::clicked,this,&SambaConfiguration::openDirectory);
}

void SambaConfiguration::saveFile()
{
    if(m_createModify == tr("modify"))
    {
        QSqlQuery query; query.prepare("UPDATE samba_share SET comment='"+m_ui->lineEditComment->text()+"',"
            "path='"+m_ui->lineEditDir->text()+"', browseable='"+m_ui->comboBoxBrow->currentText()+"',"
            "public='"+m_ui->comboBoxPublic->currentText()+"',writable='"+m_ui->comboBoxWritable->currentText()+"',"
            "printable='"+m_ui->comboBoxPrintable->currentText()+"',"
            "guest_ok='"+m_ui->comboBoxGuest->currentText()+"',create_mask='"+m_ui->lineEditCreateMask->text()+"',"
            "directory_mask='"+m_ui->lineEditDirectoryMask->text()+"', valid_users='"+m_ui->lineEditValidUser->text()+"' "
            "where id='"+m_id+"'");
        if(query.exec())
            qCDebug(CondresSambaConfig) << "Update successfull...";
        else
            qCDebug(CondresSambaConfig) << "Cannot update..." << query.lastError().text();;
    }
    else if(m_createModify == tr("edit"))
    {
        QSqlQuery query; query.prepare("INSERT INTO samba_share VALUES ('"+m_ui->lineEditComment->text()+"',"
            "'"+m_ui->lineEditDir->text()+"','"+m_ui->comboBoxBrow->currentText()+"',"
            "'"+m_ui->comboBoxPublic->currentText()+"','"+m_ui->comboBoxWritable->currentText()+"',"
            "'"+m_ui->comboBoxPrintable->currentText()+"','"+m_ui->comboBoxGuest->currentText()+"','"+m_ui->lineEditCreateMask->text()+"',"
            "'"+m_ui->lineEditDirectoryMask->text()+"','"+m_ui->lineEditValidUser->text()+"','"+m_id+"')");
        if(query.exec())
            qCDebug(CondresSambaConfig) << "Insert successfull...";
        else
            qCDebug(CondresSambaConfig) << "Cannot insert..." << query.lastError().text();
    }
    
    emit saveConfig(SAMBA_TEMP);
    emit saveList();
    close();
}

void SambaConfiguration::openDirectory()
{
    QString fileName = QFileDialog::getExistingDirectory(this, QObject::tr("Open directory"),
                                                            QDir::currentPath());
    
    m_ui->lineEditDir->setText(fileName);
    QStringList dir = QString(fileName).split("/");
    QVector<QString> str;
    for(int i=0; i < dir.size(); i++)
        str << dir.at(i);
    m_ui->lineEditComment->setText(str.back());
}

SambaConfiguration::~SambaConfiguration()
{
    delete m_ui;
}

//Samba config printer
SambaPrinter::SambaPrinter(QStringList listConfigDir, QString create_modify, 
                           QString id, QWidget* parent) :
                           QDialog(parent),
                           _ui(new Ui::SambaPrinter),
                           m_list(listConfigDir),
                           m_createModify(create_modify),
                           m_id(id)
{
    _ui->setupUi(this);
    if(m_createModify == tr("modify"))
    {
        setWindowTitle(tr("Edit Samba Printer"));
        _ui->comboBoxPrinter->setCurrentText(QString(m_list.at(0)));
        _ui->lineEditComment->setText(QString(m_list.at(1)));
        _ui->lineEditDirectory->setText(QString(m_list.at(2)));
        _ui->comboBoxWritable->setCurrentText(QString(m_list.at(3)));
        _ui->comboBoxBrow->setCurrentText(QString(m_list.at(4)));
        _ui->comboBoxPrintable->setCurrentText(QString(m_list.at(5)));
        _ui->comboBoxGuest->setCurrentText(QString(m_list.at(6)));
        _ui->lineEditPrinting->setText(QString(m_list.at(7)));
        _ui->lineEditValidUsers->setText(QString(m_list.at(8)));
    }
    else if(m_createModify == tr("edit")){ 
        setWindowTitle(tr("Add Samba Printer"));
        _ui->lineEditDirectory->setText(SAMBA_SPOOL);
        _ui->lineEditPrinting->setText(SAMBA_CUPS);
    }
    connect(_ui->pushButtonDirectory,&QPushButton::clicked,this,&SambaPrinter::openDirectory);
    connect(_ui->comboBoxPrinter,static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),[this](){
        _ui->lineEditComment->setText(_ui->comboBoxPrinter->currentText());
    });
    connect(_ui->pushButtonClose,&QPushButton::clicked,this,&SambaPrinter::close);
    connect(_ui->pushButtonSave,&QPushButton::clicked,this,&SambaPrinter::saveFile);
    viewPrinter();
}

void SambaPrinter::openDirectory()
{
    QString fileName = QFileDialog::getExistingDirectory(this, QObject::tr("Open directory"),
                                                            QDir::currentPath());
    
    _ui->lineEditDirectory->setText(fileName);
    QStringList dir = QString(fileName).split("/");
    QVector<QString> str;
    for(int i=0; i < dir.size(); i++)
        str << dir.at(i);
}

void SambaPrinter::viewPrinter()
{
    QStringList list_stampanti;
    list_stampanti << QPrinterInfo::availablePrinterNames();
    _ui->comboBoxPrinter->addItems(list_stampanti);
}

void SambaPrinter::saveFile()
{
    if(m_createModify == tr("modify"))
    {
        QSqlQuery query; query.prepare("UPDATE samba_print SET comment='"+_ui->lineEditComment->text()+"',"
            "directory='"+_ui->lineEditDirectory->text()+"', browseable='"+_ui->comboBoxBrow->currentText()+"',"
            "printer='"+_ui->comboBoxPrinter->currentText()+"',writable='"+_ui->comboBoxWritable->currentText()+"',"
            "printable='"+_ui->comboBoxPrintable->currentText()+"',"
            "guest='"+_ui->comboBoxGuest->currentText()+"',printing='"+_ui->lineEditPrinting->text()+"',"
            "valid_users='"+_ui->lineEditValidUsers->text()+"' "
            "where id='"+m_id+"'");
        if(query.exec())
            qCDebug(CondresSambaConfig) << "Update successfull...";
        else
            qCDebug(CondresSambaConfig) << "Cannot update..." << query.lastError().text();;
    }
    else if(m_createModify == tr("edit"))
    {
        QSqlQuery query; query.prepare("INSERT INTO samba_print VALUES ('"+_ui->comboBoxPrinter->currentText()+"',"
            "'"+_ui->lineEditComment->text()+"','"+_ui->lineEditDirectory->text()+"',"
            "'"+_ui->comboBoxWritable->currentText()+"','"+_ui->comboBoxBrow->currentText()+"',"
            "'"+_ui->comboBoxPrintable->currentText()+"','"+m_id+"','"+_ui->comboBoxGuest->currentText()+"',"
            "'"+_ui->lineEditPrinting->text()+"','"+_ui->lineEditValidUsers->text()+"')");
        if(query.exec())
            qCDebug(CondresSambaConfig) << "Insert successfull...";
        else
            qCDebug(CondresSambaConfig) << "Cannot insert..." << query.lastError().text();
    }
    emit saveConfig(SAMBA_TEMP);
    emit saveList();
    close();
}

SambaPrinter::~SambaPrinter()
{
    delete _ui;
}

//Samba user 
SambaUser::SambaUser(QString id, QString users, QString edit, QWidget *parent) :
    QDialog(parent),
    _ui_(new Ui::SambaUser),
    m_id(id),
    m_user(users),
    m_edit(edit)
{
    _ui_->setupUi(this);
    if(m_edit == tr("modify"))
    {
        setWindowTitle(tr("Modify Samba Users"));
        _ui_->lineEditUser->setText(m_user);
    }
    else if(m_edit == tr("edit")) setWindowTitle(tr("Add Samba Users"));
    connect(_ui_->pushButton,&QPushButton::clicked,this,&SambaUser::readProcess);
    connect(_ui_->pushButton_2,&QPushButton::clicked,this,&SambaUser::close);
    _ui_->lineEditPassowrd->setEchoMode(QLineEdit::Password);
    _ui_->lineEditConfirmPassword->setEchoMode(QLineEdit::Password);
}

void SambaUser::readProcess()
{
    QTextStream qout(stdout);
    QByteArray result;
    QProcess cmd2;
    if(m_edit == tr("modify"))
    {
        cmd2.setReadChannel(QProcess::StandardOutput);
        cmd2.setProcessChannelMode(QProcess::MergedChannels);
        cmd2.start("smbpasswd -a "+_ui_->lineEditUser->text()+" -s");
        if (!cmd2.waitForStarted()){
            qout << tr("Error: Could not start!") << endl;
            return;
        }
        cmd2.write(_ui_->lineEditPassowrd->text().toLocal8Bit()+"\n");
        cmd2.write(_ui_->lineEditConfirmPassword->text().toLocal8Bit()+"\n");
        cmd2.closeWriteChannel();   //done Writing

        while(cmd2.state()!=QProcess::NotRunning){
            cmd2.waitForReadyRead();
            result = cmd2.readAll();
            qout << result;
        }
    }
    else if(m_edit == tr("edit"))
    {
        cmd2.setReadChannel(QProcess::StandardOutput);
        cmd2.setProcessChannelMode(QProcess::MergedChannels);
        cmd2.start("smbpasswd -a "+_ui_->lineEditUser->text()+" -s");
        if (!cmd2.waitForStarted()){
            qout << tr("Error: Could not start!") << endl;
            return;
        }
        cmd2.write(_ui_->lineEditPassowrd->text().toLocal8Bit()+"\n");
        cmd2.write(_ui_->lineEditConfirmPassword->text().toLocal8Bit()+"\n");
        cmd2.closeWriteChannel();   //done Writing

        while(cmd2.state()!=QProcess::NotRunning){
            cmd2.waitForReadyRead();
            result = cmd2.readAll();
            qout << result;
        }
    }
    saveConfig();
}

void SambaUser::saveConfig()
{
    if(m_edit == tr("modify"))
    {
        QSqlQuery query; query.prepare("UPDATE samba_user SET user='"+_ui_->lineEditUser->text()+"' "
            "where id='"+m_id+"'");
        if(query.exec())
            qCDebug(CondresSambaConfig) << "Update successfull...";
        else
            qCDebug(CondresSambaConfig) << "Cannot update..." << query.lastError().text();;
    }
    else if(m_edit == tr("edit"))
    {
        QSqlQuery query; query.prepare("INSERT INTO samba_user VALUES ('"+_ui_->lineEditUser->text()+"',"
            "'"+m_id+"')");
        if(query.exec())
            qCDebug(CondresSambaConfig) << "Insert successfull...";
        else
            qCDebug(CondresSambaConfig) << "Cannot insert..." << query.lastError().text();
    }
    emit saveList();
    close();
}

SambaUser::~SambaUser()
{
    delete _ui_;
}
