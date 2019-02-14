#include "InstallPackages.h"
#include "ui_InstallPackages.h"
#include "backend/backend.h"

Q_LOGGING_CATEGORY(InstallPackagesDebug,"ControlCenter")

InstallPackages::InstallPackages(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallPackages)
{
    ui->setupUi(this);
    setWindowTitle("Install packages");
    connect(ui->chiudi,&QPushButton::clicked,this,&InstallPackages::close);
    connect(ui->applica,&QPushButton::clicked,this,&InstallPackages::ApplyImpo);
    connect(ui->remove,&QPushButton::clicked,this,&InstallPackages::removeImpo);
    connect(ui->cerca_line,&QLineEdit::textChanged,this,&InstallPackages::searchPackages);
    lista();
}

void InstallPackages::lista()
{
    m_table = new TableWidgetHeader(Qt::Horizontal);
    m_table->setSectionsClickable(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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
        QTableWidgetItem *item = new QTableWidgetItem(" ");
        if(list.at(0) == "i"){
            item->data(Qt::CheckStateRole);
            item->setCheckState(Qt::Checked);
        }
        else if(list.at(0) == "n"){
            item->data(Qt::CheckStateRole);
            item->setCheckState(Qt::Unchecked);
        }
        QTableWidgetItem *name = new QTableWidgetItem(QString(list.at(2)));
        QTableWidgetItem *versione = new QTableWidgetItem(QString(list.at(3)));
        QTableWidgetItem *repo = new QTableWidgetItem(QString(list.at(1)));
        QTableWidgetItem *Peso = new QTableWidgetItem(getPeso(list.at(5)));
        QTableWidgetItem *pesoCount = new QTableWidgetItem(list.at(5));
        name->setFlags(Qt::ItemIsSelectable);
        versione->setFlags(Qt::ItemIsSelectable);
        repo->setFlags(Qt::ItemIsSelectable);
        Peso->setFlags(Qt::ItemIsSelectable);
        pesoCount->setFlags(Qt::ItemIsSelectable);
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row,0,item);
        ui->tableWidget->setItem(row,1,name);
        ui->tableWidget->setItem(row,2,versione);
        ui->tableWidget->setItem(row,3,repo);
        ui->tableWidget->setItem(row,4,Peso);
        ui->tableWidget->setItem(row,5,pesoCount);
        ui->tableWidget->setColumnHidden(5,true);
    }
    connect(ui->tableWidget,&QTableWidget::clicked,this,&InstallPackages::TableClicked);
    connect(ui->tableWidget,&QTableWidget::itemChanged,this,&InstallPackages::TableClickedItem);
}

void InstallPackages::searchPackages(QString text)
{
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

        ui->label_name->setText(name);
        ui->label_version->setText(version);
        ui->label_repo->setText(repo);
        ui->label_size->setText(m_size);
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
        if(m_index.data(Qt::CheckStateRole) == Qt::Checked)
          ui->textEdit->append(ui->tableWidget->model()->index(item->row(),1,QModelIndex()).data(Qt::DisplayRole).toString());
        else if(m_index.data(Qt::CheckStateRole) == Qt::Unchecked)
            ui->textEdit->append(ui->tableWidget->model()->index(item->row(),1,QModelIndex()).data(Qt::DisplayRole).toString());
    }
}

void InstallPackages::ApplyImpo()
{
    QStringList list;
    list << ui->textEdit->toPlainText();
    m_apply = new Apply(list,"install",this);
    m_apply->exec();
}

void InstallPackages::removeImpo()
{
    QStringList list;
    list << ui->textEdit->toPlainText();
    m_apply = new Apply(list,"remove",this);
    m_apply->exec();
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
}
