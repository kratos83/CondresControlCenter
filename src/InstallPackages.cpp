#include "InstallPackages.h"
#include "ui_InstallPackages.h"
#include "backend/backend.h"

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
    connect(ui->cerca_line,&QLineEdit::textEdited,this,&InstallPackages::searchPackages);
    lista();
    setMenuInstall();
    viewPackagesGroup();
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
    boxGroup = new QAction(QIcon(":/images/packages.png"),"View groups packages",this);
    boxGroup->setCheckable(true);
    viewGroupsPackages(false);
    menuGroup->addAction(boxGroup);
    ui->menuGroup->setMenu(menuGroup);
    
    connect(install_local_packages,&QAction::triggered,this,&InstallPackages::installLocalPackages);
    connect(boxGroup,&QAction::triggered,this,&InstallPackages::viewGroupsPackages);
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
}

void InstallPackages::viewPackagesGroup()
{
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
                    m_item->setCheckState(Qt::Checked);
                }
                else if(list.at(0) == "n"){
                    m_item->data(Qt::CheckStateRole);
                    m_item->setCheckState(Qt::Unchecked);
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
}

void InstallPackages::lista()
{
    m_table = new TableWidgetHeader(Qt::Horizontal);
    m_table->setSectionsClickable(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
            m_item->setCheckState(Qt::Checked);
        }
        else if(list.at(0) == "n"){
            m_item->data(Qt::CheckStateRole);
            m_item->setCheckState(Qt::Unchecked);
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
        QString html = "<p align=justify>Description: <b>"+m_desc+"</b></p></br>"
                       "<p align=justify>Software: <b>"+name+"</b></p></br>"
                       "<p align=justify>Version: <b>"+version+"</b></p></br>"
                       "<p align=justify>Repository: <b>"+repo+"</b></p></br>"
                       "<p align=justify>Size: <b>"+m_size+"</b></p></br>";
        ui->info->setHtml(html);
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
                ui->textEdit->append(name);
        }
        else if(m_index.data(Qt::CheckStateRole) == Qt::Unchecked){
                ui->textEdit->clear();
                ui->textEditRemove->append(name);
        }
    }
}

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

QString InstallPackages::getPeso(QString byteReceived)
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

InstallPackages::~InstallPackages()
{
    delete ui;
}

void InstallPackages::on_pul_list_clicked()
{
    ui->textEdit->clear();
    ui->textEditRemove->clear();
}
