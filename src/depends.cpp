#include "depends.h"
#include "ui_depends.h"

Depends::Depends(QStringList list, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Depends),
    m_list(list)
{
    ui->setupUi(this);
    connect(ui->closeDialog,&QPushButton::clicked,this,&Depends::close);
    connect(ui->pushButtonAdd,&QPushButton::clicked,this,&Depends::saveList);
    lista();
}

void Depends::lista()
{
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setColumnWidth(0,30);
    ui->tableWidget->setColumnWidth(1,200);
    foreach(QString m_deps, m_list)
    {
        QStringList _list = m_deps.split(" ");
        QTableWidgetItem * check = new QTableWidgetItem("");
        check->setCheckState(Qt::Unchecked);
        QTableWidgetItem * item = new QTableWidgetItem(m_deps);
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row,0,check);
        ui->tableWidget->setItem(row,1,item);
    }
    connect(ui->tableWidget,&QTableWidget::itemClicked,this,&Depends::saveDepends);
}

void Depends::saveDepends(QTableWidgetItem *item)
{
    if(item->column() == 0)
    {
        if(!item)
           return;
        QStringList m_list;
        QModelIndex m_index;
        m_index = ui->tableWidget->model()->index(item->row(),0,QModelIndex());
        QString name = ui->tableWidget->model()->index(item->row(),1,QModelIndex()).data(Qt::DisplayRole).toString();
        if(m_index.data(Qt::CheckStateRole) == Qt::Checked)
        {
            ui->textEdit->append(name);
        }
    }
}

void Depends::saveList()
{
    emit save(ui->textEdit->toPlainText());
    close();
}

Depends::~Depends()
{
    delete ui;
}
