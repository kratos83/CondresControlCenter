#include "esci.h"
#include "ui_esci.h"

esci::esci(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::esci)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
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
