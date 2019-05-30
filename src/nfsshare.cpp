/*
 * Copyright (C) 2019  Angelo Scarnà <angelo.scarna@codelinsoft.it>
 *
 * This part is CondresControlCenter
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

#include "nfsshare.h"
#include "ui_nfsshare.h"
#include "ui_NfsConfig.h"
#include "itdelegato.h"

Q_LOGGING_CATEGORY(CondresControlNfs, "ControlCenter")

NfsShare::NfsShare(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::NfsShare),
    proc(new QProcess),
    manager(new SettingsManager)
{
    ui->setupUi(this);
    connect(ui->checkBoxTerminal,&CheckBox::clicked,this,&NfsShare::viewTerminal);
    connect(ui->pushButtonStart,&QPushButton::clicked,this,&NfsShare::startNfs);
    connect(ui->pushButtonRestart,&QPushButton::clicked,this,&NfsShare::restartNfs);
    connect(ui->pushButtonStop,&QPushButton::clicked,this,&NfsShare::stopNfs);
    connect(ui->pushButtonStatus,&QPushButton::clicked,this,&NfsShare::statusNfs);
    connect(ui->add_nfs,&QPushButton::clicked,this,&NfsShare::addNfsConfig);
    connect(ui->modify_nfs,&QPushButton::clicked,this,&NfsShare::modifyNfsConfig);
    connect(ui->remove_nfs,&QPushButton::clicked,this,&NfsShare::delNfsConfig);
    connect(ui->pushButtonSave,&QPushButton::clicked,this,static_cast< void(NfsShare::*)()>(&NfsShare::saveFile));
    ui->textEditDebug->setReadOnly(true);
    viewTerminal(false);
    readNfsShare();
}

void NfsShare::viewTerminal(bool vero)
{
    ui->textEditDebug->setVisible(vero);
}

void NfsShare::readNfsShare()
{
    model = new QStandardItemModel(0,10,this);
    model->setHeaderData(0,Qt::Horizontal,tr("ID"));
    model->setHeaderData(1,Qt::Horizontal,tr("Directory"));
    model->setHeaderData(2,Qt::Horizontal,tr("Host access"));
    model->setHeaderData(3,Qt::Horizontal,tr("Read and Write"));
    model->setHeaderData(4,Qt::Horizontal,tr("Sync"));
    model->setHeaderData(5,Qt::Horizontal,tr("Users"));
    model->setHeaderData(6,Qt::Horizontal,tr("AnonUID"));
    model->setHeaderData(7,Qt::Horizontal,tr("AnonGID"));
    model->setHeaderData(8,Qt::Horizontal,tr("Guest"));
    model->setHeaderData(9,Qt::Horizontal,tr("Sub tree"));
    QSqlQuery query;
    query.prepare("select * from nfs_share");
    int count = 0;
    if(query.exec())
        while(query.next())
        {
            model->setItem(count,0,new QStandardItem(query.value("id").toString()));
            QStandardItem *path = new QStandardItem(query.value("directory").toString());
            path->setIcon(QIcon(":/images/directory.png"));
            model->setItem(count,1,path);
            model->setItem(count,2,new QStandardItem(query.value("host_access").toString()));
            model->setItem(count,3,new QStandardItem(query.value("read_write").toString()));
            model->setItem(count,4,new QStandardItem(query.value("sync").toString()));
            model->setItem(count,5,new QStandardItem(query.value("squash").toString()));
            model->setItem(count,6,new QStandardItem(query.value("anonuid").toString()));
            model->setItem(count,7,new QStandardItem(query.value("anongid").toString()));
            model->setItem(count,8,new QStandardItem(query.value("guest").toString()));
            model->setItem(count,9,new QStandardItem(query.value("sub_tree").toString()));
            ++count;
        }
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnWidth(1,200);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setItemDelegateForColumn(8,new coldxdelegato(this));
}

void NfsShare::addNfsConfig()
{
    QSqlQuery query;
    query.prepare("select count(id)+1 from nfs_share");
    if(query.exec()){
        if(query.next())
        {
            NfsConfig *smb = new NfsConfig(QStringList(),"edit",query.value(0).toString(),this);
            connect(smb,&NfsConfig::saveConfig,this,static_cast< void(NfsShare::*)(QString)>(&NfsShare::saveFile));
            connect(smb,&NfsConfig::saveList,this,&NfsShare::readNfsShare);
            smb->exec();
        }
    }
    else qDebug() << query.lastError();
}

void NfsShare::modifyNfsConfig()
{
    if(ui->tableView->selectionModel()->currentIndex().isValid())
    {
        QModelIndex modelIndex = ui->tableView->selectionModel()->currentIndex();
        int riga = modelIndex.row();
        QModelIndex newindex = modelIndex.sibling(riga,0);
        ui->tableView->setCurrentIndex(newindex);
        if(newindex.row() < model->rowCount())
        {
            QStringList list;
            QString m_id = model->data(model->index(newindex.row(),0),Qt::DisplayRole).toString();
            QString m_dir = model->data(model->index(newindex.row(),1),Qt::DisplayRole).toString();
            QString m_host = model->data(model->index(newindex.row(),2),Qt::DisplayRole).toString();
            QString m_rw = model->data(model->index(newindex.row(),3),Qt::DisplayRole).toString();
            QString m_sync = model->data(model->index(newindex.row(),4),Qt::DisplayRole).toString();
            QString m_squash = model->data(model->index(newindex.row(),5),Qt::DisplayRole).toString();
            QString m_anonuid = model->data(model->index(newindex.row(),6),Qt::DisplayRole).toString();
            QString m_anongid = model->data(model->index(newindex.row(),7),Qt::DisplayRole).toString();
            QString m_guest = model->data(model->index(newindex.row(),8),Qt::DisplayRole).toString();
            QString m_tree = model->data(model->index(newindex.row(),9),Qt::DisplayRole).toString();
            list << m_dir << m_host << m_rw << m_sync << m_squash << m_anonuid << m_anongid << m_guest << m_tree;
            NfsConfig *smb = new NfsConfig(list,"modify",m_id,this);
            connect(smb,&NfsConfig::saveConfig,this,static_cast< void(NfsShare::*)(QString)>(&NfsShare::saveFile));
            connect(smb,&NfsConfig::saveList,this,&NfsShare::readNfsShare);
            smb->exec();
        }
    }
    else QMessageBox::warning(this,tr("CondresControlCenter"),tr("Select row to modified"));
}

void NfsShare::delNfsConfig()
{
    QString m_id = ui->tableView->model()->data(ui->tableView->model()->index(ui->tableView->selectionModel()->currentIndex().row(),0),Qt::DisplayRole).toString();
    if (!m_id.isEmpty())
    {
        QSqlQuery query;
        query.prepare("DELETE FROM nfs_share WHERE id='"+m_id+"'");
        if (query.exec())
        {
            qCDebug(CondresControlNfs) << tr("Delete id ")+m_id+tr(" successfull...");
        }
        else
        {
            QMessageBox::warning(this,"SambaPrinter",tr("Cannot delete ")+query.lastError().text());
        }
    }
    readNfsShare();
}

void NfsShare::saveFile(QString nameFile)
{
    QFile file(nameFile);
    if(file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream stream(&file);
        for(int x=0; x < ui->tableView->model()->rowCount(); x++){
            if(x != -1)
            {
                QString m_dir = ui->tableView->model()->data(ui->tableView->model()->index(x,1),Qt::DisplayRole).toString();
                QString m_host = ui->tableView->model()->data(ui->tableView->model()->index(x,2),Qt::DisplayRole).toString();
                QString m_rw = ui->tableView->model()->data(ui->tableView->model()->index(x,3),Qt::DisplayRole).toString();
                QString m_sync = ui->tableView->model()->data(ui->tableView->model()->index(x,4),Qt::DisplayRole).toString();
                QString m_squash = ui->tableView->model()->data(ui->tableView->model()->index(x,5),Qt::DisplayRole).toString();
                QString m_anonuid = ui->tableView->model()->data(ui->tableView->model()->index(x,6),Qt::DisplayRole).toString();
                QString m_anongid = ui->tableView->model()->data(ui->tableView->model()->index(x,7),Qt::DisplayRole).toString();
                QString m_guest = ui->tableView->model()->data(ui->tableView->model()->index(x,8),Qt::DisplayRole).toString();
                QString m_tree = ui->tableView->model()->data(ui->tableView->model()->index(x,9),Qt::DisplayRole).toString();
                QString exports = m_dir;
                exports += " "+m_host;
                exports += "("+m_rw+",";
                exports += m_sync+",";
                exports += m_squash+",";
                if(m_anonuid != "")
                    exports += m_anonuid+",";
                if(m_anongid != "")
                    exports += m_anongid+",";
                if(m_guest != "")
                    exports += m_guest+",";
                exports += m_tree+")";
                stream << exports;
            }
        }
    }
    file.close();
}

void NfsShare::saveFile()
{
    saveFile(NFS_TEMP);
    QProcess::startDetached("cp -rv /etc/exports /etc/exports.bak");
    QProcess::startDetached("cp -rv /tmp/exports /etc/exports");
    QProcess::startDetached("exportfs -r");
    QProcess::startDetached("rm -rf "+QString(NFS_TEMP));
}

void NfsShare::startNfs()
{
    proc->setReadChannel(QProcess::StandardOutput);
    proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(proc,&QProcess::readyReadStandardOutput,this,&NfsShare::StartStopRest);
    connect(proc,static_cast<void (QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),this,&NfsShare::StartStopRestProcess);
    proc->start("/usr/bin/systemctl start nfs-server");
}

void NfsShare::restartNfs()
{
    proc->setReadChannel(QProcess::StandardOutput);
    proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(proc,&QProcess::readyReadStandardOutput,this,&NfsShare::StartStopRest);
    connect(proc,static_cast<void (QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),this,&NfsShare::StartStopRestProcess);
    proc->start("/usr/bin/systemctl restart nfs-server");
}

void NfsShare::stopNfs()
{
    proc->setReadChannel(QProcess::StandardOutput);
    proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(proc,&QProcess::readyReadStandardOutput,this,&NfsShare::StartStopRest);
    connect(proc,static_cast<void (QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),this,&NfsShare::StartStopRestProcess);
    proc->start("/usr/bin/systemctl stop nfs-server");
}

void NfsShare::StartStopRest()
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

void NfsShare::statusNfs()
{
    ui->textEditDebug->clear();
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("systemctl status nfs-server");
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

void NfsShare::StartStopRestProcess(int exitCode, QProcess::ExitStatus)
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

NfsShare::~NfsShare()
{
    delete ui;
}

//Nfs config
NfsConfig::NfsConfig(QStringList list, QString create_modify, 
                     QString id, QWidget* parent) :
                     QDialog(parent),
                     _ui(new Ui::NfsConfig),
                     m_list(list),
                     m_createModify(create_modify),
                     m_id(id)
{
    _ui->setupUi(this);
    if(m_createModify == tr("modify"))
    {
        setWindowTitle(tr("Modify Nfs configuration"));
        _ui->lineEditDir->setText(QString(m_list.at(0)));
        _ui->lineEditAccess->setText(QString(m_list.at(1)));
        _ui->comboBoxRW->setCurrentText(QString(m_list.at(2)));
        _ui->lineEditSync->setText(QString(m_list.at(3)));
        _ui->comboBoxSquash->setCurrentText(QString(m_list.at(4)));
        _ui->lineEditAnonUID->setText(QString(m_list.at(5)));
        _ui->lineEditAnonGID->setText(QString(m_list.at(6)));
        _ui->lineEditGuest->setText(QString(m_list.at(7)));
        _ui->comboBoxSub->setCurrentText(QString(m_list.at(8)));
    }
    else if(m_createModify == tr("edit"))
    {setWindowTitle(tr("Add Nfs configuration"));}
    connect(_ui->pushButtonClose,&QPushButton::clicked,this,&NfsConfig::close);
    connect(_ui->pushButtonDir,&QPushButton::clicked,this,&NfsConfig::openDirectory);
    connect(_ui->pushButtonSave,&QPushButton::clicked,this,&NfsConfig::saveFile);
}

void NfsConfig::openDirectory()
{
    QString fileName = QFileDialog::getExistingDirectory(this, QObject::tr("Open directory"),
                                                            QDir::currentPath());
    
    _ui->lineEditDir->setText(fileName);
}

void NfsConfig::saveFile()
{
    if(_ui->lineEditAccess->text() == "" || _ui->lineEditSync->text() == ""){
        _ui->lineEditAccess->setStyleSheet("QLineEdit{background-color:#FF6347}");
        _ui->lineEditSync->setStyleSheet("QLineEdit{background-color:#FF6347}");
        QMessageBox::warning(this,tr("CondresControlCenter"),"This cell is required.");
    }
    else{
        if(m_createModify == tr("modify"))
        {
            QSqlQuery query;
            query.prepare("UPDATE nfs_share SET directory='"+_ui->lineEditDir->text()+"',host_access='"+_ui->lineEditAccess->text()+"',"
                "read_write='"+_ui->comboBoxRW->currentText()+"',sync='"+_ui->lineEditSync->text()+"',squash='"+_ui->comboBoxSquash->currentText()+"',"
                "anonuid='"+_ui->lineEditAnonUID->text()+"',anongid='"+_ui->lineEditAnonGID->text()+"',guest='"+_ui->lineEditGuest->text()+"', sub_tree='"+_ui->comboBoxSub->currentText()+"' where id='"+m_id+"'");
            if(query.exec())
                qCDebug(CondresControlNfs) << "Update nfs_share successfull...";
            else
                qCDebug(CondresControlNfs) << "Cannot insert nfs_share...";
        }
        else if(m_createModify == tr("edit"))
        {
            QSqlQuery query;
            query.prepare("INSERT INTO nfs_share VALUES('"+m_id+"','"+_ui->lineEditDir->text()+"','"+_ui->lineEditAccess->text()+"',"
                "'"+_ui->comboBoxRW->currentText()+"','"+_ui->lineEditSync->text()+"','"+_ui->comboBoxSquash->currentText()+"',"
                "'"+_ui->lineEditAnonUID->text()+"','"+_ui->lineEditAnonGID->text()+"','"+_ui->lineEditGuest->text()+"','"+_ui->comboBoxSub->currentText()+"')");
            if(query.exec())
                qCDebug(CondresControlNfs) << "Insert nfs_share successfull...";
            else
                qCDebug(CondresControlNfs) << "Cannot insert nfs_share...";
        }
        emit saveConfig(NFS_EXPORT);
        emit saveList();
        close();
    }
}

NfsConfig::~NfsConfig()
{
    delete _ui;
}
