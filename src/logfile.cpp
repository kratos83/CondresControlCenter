#include "logfile.h"
#include "ui_logfile.h"

LogFile::LogFile(QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogFile),
    m_name(name)
{
    ui->setupUi(this);
    QFile file(m_name);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       QTextStream stream(&file);
       while(!stream.atEnd()){
          QString line = stream.readLine();
          ui->textEdit->append(line);
       }
    }
}

LogFile::~LogFile()
{
    delete ui;
}
