#include "upgrade.h"
#include "ui_upgrade.h"
#include <iostream>

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
    lista();
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

void Upgrade::upgradeDB()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->download->setVisible(false);
    ui->up_progress->setVisible(false);
    if (!NoConnection())
        QMessageBox::warning(this,"Condres OS Control Center","Unable to estabilish internet connection");

    ui->console->append("<font color=\"white\">Update database started</font>");
    ui->up_progress->setText("Update database started");
    processDB->setReadChannel(QProcess::StandardOutput);
    processDB->setProcessChannelMode(QProcess::MergedChannels);
    connect(processDB,&QProcess::readyReadStandardOutput,this,&Upgrade::UpdateDBReady);
    connect(processDB,static_cast<void (QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),this,&Upgrade::UpdateDb);
    processDB->start("/usr/bin/pacman -Sy");
}

void Upgrade::UpdateDb(int exitCode, QProcess::ExitStatus)
{
    if(exitCode > 1)
    {
      //process failed, provide info on errors
      QMessageBox::critical(this,"Control Center", "Error whith the underlying process");
    }
    else if(exitCode == 0){
        if(m_database == "update")
            QMessageBox::information(this,"Condres OS Control Center","Syncronized database successfull.");
        ui->console->append("<font color=\"white\">Update complete</font>");
        getListUpdate();
    }
}

void Upgrade::UpdateDBReady()
{
    QString results = processDB->readAll();
    QStringList resultsVector = results.split(((QRegExp) "\n"));
    for(int i=0; i<resultsVector.size();i++)
    {
        if(QString(resultsVector.at(i)).isEmpty())
            continue;

        QString str;
        str.append(resultsVector.at(i));
        ui->up_progress->setText(str);
        ui->console->append("<font color=\"white\">"+resultsVector.at(i)+"</font></br>");
    }
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
    processUpdate.setReadChannel(QProcess::StandardOutput);
    processUpdate.start("pacman -Sup --print-format \"%n %v %r %s\"");
    if (processUpdate.waitForStarted(3000))
    {
        ui->console->append("<font color=\"white\">Update view list</font>");
        ui->up_progress->setText("Update view list");
        if(processUpdate.waitForReadyRead(-1))
        {
            manager->setGeneralValue("Update/upgrade","update");
            ui->stackedWidget->setCurrentIndex(1);
            ui->download->setVisible(true);
            ui->up_progress->setVisible(true);
            processUpdate.waitForFinished(-1);
            QString results = processUpdate.readAllStandardOutput();
            QStringList resultsVector = results.split(((QRegExp) "\n"),QString::SkipEmptyParts);
            for(int i = 0; i < resultsVector.length(); ++i){

                QString line = resultsVector.at(i);
                m_update = line.split(" ");
                ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr(" ") << tr("Name") <<
                                                           tr("Version") << tr("Repository") << tr("Size") << tr("Peso"));
                QTableWidgetItem *item = new QTableWidgetItem(" ");
                item->data(Qt::CheckStateRole);
                item->setCheckState(Qt::Unchecked);
                QTableWidgetItem *name = new QTableWidgetItem(QString(m_update.at(0)));
                QTableWidgetItem *versione = new QTableWidgetItem(QString(m_update.at(1)));
                QTableWidgetItem *repo = new QTableWidgetItem(QString(m_update.at(2)));
                QTableWidgetItem *Peso = new QTableWidgetItem(getPeso(m_update.at(3)));
                QTableWidgetItem *pesoCount = new QTableWidgetItem(m_update.at(3));
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

                }
                double peso = 0;
                for(int x=0; x < ui->tableWidget->rowCount(); ++x){
                    peso += ui->tableWidget->item(x,5)->text().toDouble();
                }
                ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
                ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
                ui->download->clear();
                ui->download->setText("<center><font size=\"4\"><b>Total size download: "+getPeso(QString::number(peso))+"</b></font></center>");
            }
            else{
                ui->stackedWidget->setCurrentIndex(3);
                ui->applica->setVisible(false);
                ui->dettagli->setVisible(false);
                manager->setGeneralValue("Update/upgrade","complete");
                emit finishUpdate(true);
            }
        }
}

QString Upgrade::getPeso(QString byteReceived)
{
    //Calcolo la dimensione del file da scaricare
     double peso = byteReceived.toDouble();

    QString dt;
    if (peso < 1024) {
        dt = "bytes";
    } else if (peso < 1024*1024) {
        peso /= 1024;
        dt = "kB";
    } else {
        peso /= 1024*1024;
        dt = "MB";
    }

    QString text = QString::fromLatin1("%1 %2").arg(peso,3,'f',1).arg(dt);
    m_peso = text;

    return m_peso;
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
    QStringList list1;
    if (!NoConnection())
       QMessageBox::warning(this,"Condres OS Control Center","Unable to estabilish internet connection");

    removeDBPacman();

    ui->console->append("<font color=\"white\">Update packages</font>");
    ui->up_progress->setText("Update packages");
    processUpgrade->setReadChannel(QProcess::StandardOutput);
    processUpgrade->setProcessChannelMode(QProcess::MergedChannels);
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
    remove_db->start("rm -rf /var/lib/pacman/db.lck");
    ui->console->setText("<font color=\"white\">Remove database pacman </font>");
}
