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
#include "ui_AddSamba.h"
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
    //SambaUser
    connect(ui->add_user,&QPushButton::clicked,this,&Samba::addUser);
    connect(ui->modify_user,&QPushButton::clicked,this,&Samba::configUser);
    connect(ui->remove_user,&QPushButton::clicked,this,&Samba::delUser);
    connect(ui->open_user,&QPushButton::clicked,this,&Samba::openUser);
    load(SAMBA_FILE);
    readUser();
    readChecBox(false);
    manager->setGeneralValue("Shared/directory","");
    manager->setGeneralValue("Shared/printer","");
}

void Samba::load(QString nameFile)
{
    QFile file(nameFile);
    file.open(QIODevice::ReadOnly);
    if(file.exists()){
        while(!file.atEnd()){
        QString line = file.readLine();
        if(";" == line.left(1))
            continue;
        else{
            ui->lineEditSambaConfig->setText(SAMBA_FILE);
            if(!searchItem(nameFile,"workgroup").isEmpty()){
            QStringList list = searchItem(nameFile,"workgroup").split("=");
            ui->lineEditWorkgroup->setText(QString(list[1]).remove("\n"));}
            else ui->lineEditWorkgroup->clear();
            if(!searchItem(nameFile,"server string").isEmpty()){
            QStringList list = searchItem(nameFile,"server string").split("=");
            ui->lineEditServer->setText(QString(list[1]).remove("\n"));}
            else ui->lineEditServer->clear();
            if(!searchItem(nameFile,"netbios name").isEmpty()){
            QStringList list = searchItem(nameFile,"netbios name").split("=");
            ui->lineEditNetbios->setText(QString(list[1]).remove("\n"));}
            else ui->lineEditNetbios->clear();
            if(!searchItem(nameFile,"security").isEmpty()){
            QStringList list = searchItem(nameFile,"security").split("=");
            ui->lineEditSecurity->setText(QString(list[1]).remove("\n"));}
            else ui->lineEditSecurity->clear();
            if(!searchItem(nameFile,"hosts allow").isEmpty()){
            QStringList list = searchItem(nameFile,"hosts allow").split("=");
            ui->lineEditHost->setText(QString(list[1]).remove("\n"));}
            else ui->lineEditHost->clear();
            if(!searchItem(nameFile,"guest account").isEmpty()){
            QStringList list = searchItem(nameFile,"guest account").split("=");
            ui->lineEditGuest->setText(QString(list[1]).remove("\n"));}
            else ui->lineEditGuest->clear();
            if(!searchItem(nameFile,"log file").isEmpty()){
            QStringList list = searchItem(nameFile,"log file").split("=");
            ui->lineEdit->setText(QString(list[1]).remove("\n"));}
            else ui->lineEdit->clear();
            if(!searchItem(nameFile,"max log size").isEmpty()){
            QStringList list = searchItem(nameFile,"max log size").split("=");
            int number = list[1].toInt();
            ui->spinBoxLogSize->setValue(number);}
            else ui->spinBoxLogSize->setValue(0);
            if(!searchItem(nameFile,"load printers").isEmpty()){
            QStringList list = searchItem(nameFile,"load printers").split("=");
            ui->comboBoxLoadPrinters->setCurrentText(QString(list[1]).remove("\n"));}
            else ui->comboBoxLoadPrinters->setCurrentIndex(0);
            if(!searchItem(nameFile,"printing").isEmpty()){
            QStringList list = searchItem(nameFile,"printing").split("=");
            ui->lineEditPrinting->setText(QString(list[1]).remove("\n"));}
            else ui->lineEditPrinting->clear();       
            readShareFileDirectory(nameFile);
        }
      }
    }
    else
        QMessageBox::warning(this,"CondresControlCenter",tr("Can't open file <b>")+QFileInfo(nameFile).fileName()+tr("</b>. Find smb.conf in /etc/samba"));
    file.close();
}

void Samba::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, QObject::tr("Open config file samba"),
                                                            "*.conf", QObject::tr("Config file(*.conf)"));
    
    if(!fileName.contains("smb.conf"))
        QMessageBox::warning(this,"CondresControlCenter",tr("Can't open file <b>")+QFileInfo(fileName).fileName()+tr("</b>. Find smb.conf in /etc/samba"));
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

void Samba::readShareFileDirectory(QString nameFile)
{
    model = new QStandardItemModel(0,11,this);
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
    model->setHeaderData(10,Qt::Horizontal,tr("Printing"));
    int count = 0, com=0, br=0, pc=0, wr=0,pn=0,gt=0,cr=0,mr=0,vl=0,prt=0;
        if(!searchItemName(nameFile,QString("comment")).isEmpty()){
            foreach(QString pr, searchItemName(nameFile,QString("comment"))){
                QString pt = pr.remove("\t").remove("\n").remove(" ").remove("comment=");
                QStandardItem *path = new QStandardItem(pt);
                model->setItem(com,0,path);
                ++com;
            }
        }
        if(!searchItemName(nameFile,QString("path")).isEmpty()){
            foreach(QString pr, searchItemName(nameFile,QString("path"))){
                QString pt = pr.remove("\t").remove("\n").remove(" ").remove("path=");
                QStandardItem *path = new QStandardItem(pt);
                if(pt == SAMBA_SPOOL)
                    path->setIcon(QIcon(":/images/printer.png"));
                else
                    path->setIcon(QIcon(":/images/directory.png"));
                model->setItem(count,1,path);
                ++count;
            }
        }
        if(!searchItemName(nameFile,QString("browseable")).isEmpty()){
            foreach(QString pr, searchItemName(nameFile,QString("browseable"))){
                QString pt = pr.remove("\t").remove("\n").remove(" ").remove("browseable=");
                QStandardItem *path = new QStandardItem(pt);
                model->setItem(br,2,path);
                ++br;
            }
        }
        if(!searchItemName(nameFile,QString("public")).isEmpty()){
            foreach(QString pr, searchItemName(nameFile,QString("public"))){
                QString pt = pr.remove("\t").remove("\n").remove(" ").remove("public=");
                QStandardItem *path = new QStandardItem(pt);
                model->setItem(pc,3,path);
                ++pc;
            }
        }
        if(!searchItemName(nameFile,QString("writable")).isEmpty()){
            foreach(QString pr, searchItemName(nameFile,QString("writable"))){
                QString pt = pr.remove("\t").remove("\n").remove(" ").remove("writable=");
                QStandardItem *path = new QStandardItem(pt);
                model->setItem(wr,4,path);
                ++wr;
            }
        }
        if(!searchItemName(nameFile,QString("printable")).isEmpty()){
            foreach(QString pr, searchItemName(nameFile,QString("printable"))){
                QString pt = pr.remove("\t").remove("\n").remove(" ").remove("printable=");
                QStandardItem *path = new QStandardItem(pt);
                model->setItem(pn,5,path);
                ++pn;
            }
        }
        if(!searchItemName(nameFile,QString("guest ok")).isEmpty()){
            foreach(QString pr, searchItemName(nameFile,QString("guest ok"))){
                QString pt = pr.remove("\t").remove("\n").remove("guest ok =");
                QStandardItem *path = new QStandardItem(pt);
                model->setItem(gt,6,path);
                ++gt;
            }
        }
        if(!searchItemName(nameFile,QString("create mask")).isEmpty()){
            foreach(QString pr, searchItemName(nameFile,QString("create mask"))){
                QString pt = pr.remove("\t").remove("\n").remove("create mask =");
                QStandardItem *path = new QStandardItem(pt);
                model->setItem(cr,7,path);
                ++cr;
            }
        }
        if(!searchItemName(nameFile,QString("directory mask")).isEmpty()){
            foreach(QString pr, searchItemName(nameFile,QString("directory mask"))){
                QString pt = pr.remove("\t").remove("\n").remove("directory mask =");
                QStandardItem *path = new QStandardItem(pt);
                model->setItem(mr,8,path);
                ++mr;
            }
        }
        if(!searchItemName(nameFile,QString("valid users")).isEmpty()){
            foreach(QString pr, searchItemName(nameFile,QString("valid users"))){
                QString pt = pr.remove("\t").remove("\n").remove("valid users =");
                QStandardItem *path = new QStandardItem(pt);
                model->setItem(vl,9,path);
                ++vl;
            }
        }
        if(!searchItemName(nameFile,QString("printing")).isEmpty()){
            foreach(QString pr, searchItemName(nameFile,QString("printing"))){
                QString pt = pr.remove("\t").remove("\n").remove("printing =");
                QStandardItem *path = new QStandardItem(pt);
                model->setItem(prt,10,path);
                ++prt;
            }
        }
    ui->tableViewFile->setModel(model);
    ui->tableViewFile->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewFile->setColumnWidth(1,200);
    ui->tableViewFile->setColumnWidth(7,150);
    ui->tableViewFile->setColumnHidden(10,true);
    ui->tableViewFile->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewFile->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewFile->setItemDelegateForColumn(11,new coldxdelegato(this));
}

void Samba::readUser()
{
    modelUser = new QStandardItemModel(0,1,this);
    modelUser->setHeaderData(0,Qt::Horizontal,tr("User"));
    int count = 0;
    QProcess proc;
    QString results;
    proc.setReadChannel(QProcess::StandardOutput);
    proc.start("pdbedit -L");
    if (proc.waitForStarted(3000))
    {
        if(proc.waitForReadyRead(-1))
        {
            proc.waitForFinished(-1);
            QString m_tex = proc.readAllStandardOutput();
            QStringList list = m_tex.split(":");
            results = list.at(0);
            QStandardItem *user = new QStandardItem(results);
            user->setIcon(QIcon(":/images/users.png"));
            modelUser->setItem(count,0,user);
            ++count;
        }
    }
    ui->tableView_2->setModel(modelUser);
    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_2->setColumnWidth(0,200);
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
            QString m_printing = model->data(model->index(newindex.row(),10),Qt::DisplayRole).toString();
            list << m_com << m_dir << m_brow << m_pub << m_write << m_print << m_guest << m_cmask << m_dirmask << m_validUsers << m_printing;
            if(m_dir == SAMBA_SPOOL)
            {
                manager->setGeneralValue("Shared/printer","printer");
                SambaPrinter *smb = new SambaPrinter(list,"modify",this);
                connect(smb,&SambaPrinter::saveConfig,this,static_cast< void(Samba::*)(QString,QStringList)>(&Samba::saveFile));
                connect(smb,&SambaPrinter::saveList,this,&Samba::readShareFileDirectory);
                smb->exec();
            }
            else{
                manager->setGeneralValue("Shared/directory","directory");
                SambaConfiguration *smb = new SambaConfiguration(list,"modify",this);
                connect(smb,&SambaConfiguration::saveConfig,this,static_cast< void(Samba::*)(QString,QStringList)>(&Samba::saveFile));
                connect(smb,&SambaConfiguration::saveList,this,&Samba::readShareFileDirectory);
                smb->exec();
            }
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
            SambaUser *smb = new SambaUser(m_user,"modify",this);
            connect(smb,&SambaUser::saveList,this,&Samba::readUser);
            smb->exec();
        }
    }
    else QMessageBox::warning(this,tr("CondresControlCenter"),tr("Select row to modified"));
}

void Samba::addCongigDir()
{
    AddSamba *smb = new AddSamba(this);
    connect(smb,&AddSamba::shareSamba,this,&Samba::addShareAll);
    smb->exec();
}

void Samba::addShareAll(QString m_share)
{
    if(m_share == "printer")
    {
        manager->setGeneralValue("Shared/printer","printer");
        SambaPrinter *smb = new SambaPrinter(QStringList(),"edit",this);
        connect(smb,&SambaPrinter::saveConfig,this,static_cast< void(Samba::*)(QString,QStringList)>(&Samba::saveFile));
        connect(smb,&SambaPrinter::saveList,this,&Samba::readShareFileDirectory);
        smb->exec();
    }
    else if(m_share == "directory"){
        manager->setGeneralValue("Shared/directory","directory");
        SambaConfiguration *smb = new SambaConfiguration(QStringList(),"edit",this);
        connect(smb,&SambaConfiguration::saveConfig,this,static_cast< void(Samba::*)(QString,QStringList)>(&Samba::saveFile));
        connect(smb,&SambaConfiguration::saveList,this,&Samba::readShareFileDirectory);
        smb->exec();
    }
}

void Samba::addUser()
{
    SambaUser *smb = new SambaUser(QString(),"edit",this);
    connect(smb,&SambaUser::saveList,this,&Samba::readUser);
    smb->exec();
}

void Samba::saveFile(QString nameFile, QStringList m_list)
{
    QFile file(nameFile);
    if(file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream stream(&file);
        //Global
        stream << "[global]" << "\n";
        stream << QString("\tworkgroup =")+QString(ui->lineEditWorkgroup->text()) << "\n";
        stream << QString("\tserver string =")+QString(ui->lineEditServer->text()) << "\n";
        if(ui->lineEditNetbios->text().remove("\n") == "")
            stream << QString("\t;netbios name =") << "\n";
        else
            stream << QString("\tnetbios name =")+QString(ui->lineEditNetbios->text()) << "\n";
        stream << QString("\tsecurity = ")+QString(ui->lineEditSecurity->text()) << "\n";
        if(ui->lineEditHost->text().remove("\n") == "")
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
                QString m_printing = ui->tableViewFile->model()->data(ui->tableViewFile->model()->index(i,10),Qt::DisplayRole).toString();
                    if(m_dir == SAMBA_SPOOL){
                        stream << QString("[")+QString(m_com).remove("\n")+QString("]") << "\n";
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
                    else if(m_dir != SAMBA_SPOOL){
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
        }
        if(manager->generalValue("Shared/printer",QVariant()).toString() == "printer"){
            stream << QString("[")+QString(m_list.at(0)).remove("\n")+QString("]") << "\n";
            stream << QString("\tcomment =")+QString(m_list.at(1)).remove("\n").remove(" ") << "\n";
            stream << QString("\tpath =")+QString(m_list.at(2)).remove("\n").remove(" ") << "\n";
            stream << QString("\tbrowseable =")+QString(m_list.at(3)).remove("\n").remove(" ") << "\n";
            stream << QString("\twritable =")+QString(m_list.at(4)) << "\n";
            stream << QString("\tprintable =")+QString(m_list.at(5)) << "\n";
            stream << QString("\tguest ok =")+QString(m_list.at(6)) << "\n";
            stream << QString("\tprinting =")+QString(m_list.at(7)) << "\n";
            if(m_list.at(8) == "")
                stream << QString("\t;valid users =")+QString("") << "\n";
            else
                stream << QString("\tvalid users =")+QString(m_list.at(8)) << "\n";
        }
        else if(manager->generalValue("Shared/directory",QVariant()).toString() == "directory"){
            QStringList dir = QString(m_list.at(0)).split("/");
            QVector<QString> str;
            for(int i=0; i < dir.size(); i++)
                str << dir.at(i);
            stream << QString("["+str.back()).remove("\n")+QString("]") << "\n";
            stream << QString("\tcomment =")+QString(m_list.at(1)).remove("\n").remove(" ") << "\n";
            stream << QString("\tpath =")+QString(m_list.at(0)).remove("\n").remove(" ") << "\n";
            stream << QString("\tbrowseable =")+QString(m_list.at(2)).remove("\n").remove(" ") << "\n";
            stream << QString("\tpublic =")+QString(m_list.at(3)) << "\n";
            stream << QString("\twritable =")+QString(m_list.at(4)) << "\n";
            stream << QString("\tprintable =")+QString(m_list.at(5)) << "\n";
            stream << QString("\tguest ok =")+QString(m_list.at(6)) << "\n";
            stream << QString("\tcreate mask =")+QString(m_list.at(7)) << "\n";
            stream << QString("\tdirectory mask =")+QString(m_list.at(8)) << "\n";
            if(m_list.at(9) == "")
                stream << QString("\t;valid users =")+QString("") << "\n";
            else
                stream << QString("\tvalid users =")+QString(m_list.at(9)) << "\n";
        }
    }
    file.close();
    if(manager->generalValue("Shared/directory",QVariant()).toString() == "directory" || 
        manager->generalValue("Shared/printer",QVariant()).toString() == "printer") 
        load(SAMBA_TEMP);
}

void Samba::saveFile()
{    
    if(ui->lineEditWorkgroup->text().isEmpty() || ui->lineEdit->text().isEmpty() || ui->lineEditGuest->text().isEmpty()
        || ui->lineEditSecurity->text().isEmpty() || ui->lineEditPrinting->text().isEmpty() || ui->lineEditServer->text().isEmpty())
    {
        ui->lineEditWorkgroup->setStyleSheet("QLineEdit{background-color:red}");
        ui->lineEdit->setStyleSheet("QLineEdit{background-color:red}");
        ui->lineEditGuest->setStyleSheet("QLineEdit{background-color:red}");
        ui->lineEditSecurity->setStyleSheet("QLineEdit{background-color:red}");
        ui->lineEditPrinting->setStyleSheet("QLineEdit{background-color:red}");
        ui->lineEditServer->setStyleSheet("QLineEdit{background-color:red}");
        QMessageBox::warning(this,tr("CondresControlCenter"),tr("This strings is required"));
    }
    else{
        manager->setGeneralValue("Shared/directory","");
        manager->setGeneralValue("Shared/printer","");
        saveFile(SAMBA_TEMP,QStringList());
        QProcess::startDetached("cp -rv /etc/samba/smb.conf /etc/samba/smb.conf.bak");
        QProcess::startDetached("cp -rv /tmp/smb.conf /etc/samba/smb.conf");
        load(SAMBA_FILE);
        QProcess::startDetached("rm -rf "+QString(SAMBA_TEMP));
    }
}

void Samba::delSambaShare()
{
    manager->setGeneralValue("Shared/directory","");
    manager->setGeneralValue("Shared/printer","");
    QModelIndex currentIndex = ui->tableViewFile->selectionModel()->currentIndex();
    model->removeRow(currentIndex.row());
}

void Samba::delUser()
{
    QString m_user = ui->tableView_2->model()->data(ui->tableView_2->model()->index(ui->tableView_2->selectionModel()->currentIndex().row(),0),Qt::DisplayRole).toString();
    QProcess::startDetached(QString("smbpasswd -x ")+m_user);
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
                                       QString create_modify, QWidget* parent) :
                                       QDialog(parent),
                                       m_ui(new Ui::SambaConfiguration)
{
    m_ui->setupUi(this);
    m_list = listConfigDir;
    m_createModify = create_modify;
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
    if(m_ui->lineEditCreateMask->text().isEmpty() ||  m_ui->lineEditDirectoryMask->text().isEmpty())
    {
        m_ui->lineEditCreateMask->setStyleSheet("QLineEdit{background-color:red}");
        m_ui->lineEditDirectoryMask->setStyleSheet("QLineEdit{background-color:red}");
        QMessageBox::warning(this,tr("CondresControlCenter"),tr("This strings is required"));
    }
    else{
        QStringList list;
        list << m_ui->lineEditDir->text() << m_ui->lineEditComment->text() << m_ui->comboBoxBrow->currentText() <<
                m_ui->comboBoxPublic->currentText() << m_ui->comboBoxWritable->currentText() << m_ui->comboBoxPrintable->currentText() <<
                m_ui->comboBoxGuest->currentText() << m_ui->lineEditCreateMask->text() << m_ui->lineEditDirectoryMask->text() <<
                m_ui->lineEditValidUser->text();
        
        emit saveConfig(SAMBA_TEMP,list);
        emit saveList(SAMBA_TEMP);
        close();
    }
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
                           QWidget* parent) :
                           QDialog(parent),
                           _ui(new Ui::SambaPrinter),
                           m_list(listConfigDir),
                           m_createModify(create_modify)
{
    _ui->setupUi(this);
    if(m_createModify == tr("modify"))
    {
        setWindowTitle(tr("Edit Samba Printer"));
        _ui->comboBoxPrinter->setCurrentText(QString(m_list.at(0)));
        _ui->lineEditComment->setText(QString(m_list.at(0)));
        _ui->lineEditDirectory->setText(QString(m_list.at(1)));
        _ui->comboBoxWritable->setCurrentText(QString(m_list.at(4)));
        _ui->comboBoxBrow->setCurrentText(QString(m_list.at(2)));
        _ui->comboBoxPrintable->setCurrentText(QString(m_list.at(5)));
        _ui->comboBoxGuest->setCurrentText(QString(m_list.at(6)));
        _ui->lineEditPrinting->setText(QString(m_list.at(10)));
        _ui->lineEditValidUsers->setText(QString(m_list.at(9)));
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
    QStringList list;
    list << _ui->comboBoxPrinter->currentText() << _ui->lineEditComment->text() << _ui->lineEditDirectory->text() <<
            _ui->comboBoxBrow->currentText() << _ui->comboBoxWritable->currentText() << _ui->comboBoxPrintable->currentText() <<
            _ui->comboBoxGuest->currentText() << _ui->lineEditPrinting->text() << _ui->lineEditValidUsers->text();
    emit saveConfig(SAMBA_TEMP,list);
    emit saveList(SAMBA_TEMP);
    close();
}

SambaPrinter::~SambaPrinter()
{
    delete _ui;
}

//Samba user 
SambaUser::SambaUser(QString users, QString edit, QWidget *parent) :
    QDialog(parent),
    _ui_(new Ui::SambaUser),
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
    emit saveList();
    close();
}

SambaUser::~SambaUser()
{
    delete _ui_;
}

//addSamba
AddSamba::AddSamba(QWidget* parent) :
    QDialog(parent),
    _ui_(new Ui::AddSamba)
{
    _ui_->setupUi(this);
    setWindowTitle("Select option");
    connect(_ui_->toolButtonDir,&QToolButton::clicked,this,&AddSamba::addShareDirectory);
    connect(_ui_->toolButtonPrinter,&QToolButton::clicked,this,&AddSamba::addPrinterDirectory);
    connect(_ui_->toolButtonClose,&QToolButton::clicked,this,&AddSamba::close);
}

void AddSamba::addPrinterDirectory()
{
    close();
    emit shareSamba(QString("printer"));
}

void AddSamba::addShareDirectory()
{
    close();
    emit shareSamba(QString("directory"));
}

AddSamba::~AddSamba()
{
    delete _ui_;
}
