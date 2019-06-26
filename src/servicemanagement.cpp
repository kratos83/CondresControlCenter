#include "servicemanagement.h"
#include "ui_servicemanagement.h"
#include "itdelegato.h"

Q_LOGGING_CATEGORY(CondresControlCenterService,"ControlCenter")

ServiceManagement::ServiceManagement(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ServiceManagement)
{
    ui->setupUi(this);
    connect(ui->tableView,&QTableView::clicked,this,&ServiceManagement::clicked);
    connect(ui->pushButton,&QPushButton::clicked,this,&ServiceManagement::enableService);
    connect(ui->pushButtonDisable,&QPushButton::clicked,this,&ServiceManagement::disableService);
    connect(ui->pushButtonStop,&QPushButton::clicked,this,&ServiceManagement::stopService);
    connect(ui->pushButtonRestart,&QPushButton::clicked,this,&ServiceManagement::restartService);
    connect(ui->pushButtonStart,&QPushButton::clicked,this,&ServiceManagement::startService);
    connect(ui->lineEditService,&QLineEdit::textEdited,this,&ServiceManagement::searchService);
    listServices();
}

void ServiceManagement::listServices()
{
    model = new QStandardItemModel(0,2,this);
    model->setHeaderData(0,Qt::Horizontal,tr("Unit"));
    model->setHeaderData(1,Qt::Horizontal,tr("STATE"));
    ui->tableView->verticalHeader()->setVisible(false);
    QProcess proc;
    proc.setReadChannel(QProcess::StandardOutput);
    proc.start("systemctl list-unit-files \"*.service\"");
    int count = 0;
    if (proc.waitForStarted(3000))
    {
        if(proc.waitForReadyRead(-1))
        {
            proc.waitForFinished(-1);
            QString m_tex = proc.readAll();
            QStringList list = m_tex.split("\n",QString::SkipEmptyParts);
            foreach(QString txt, list)
            {
                if(txt.isEmpty() || txt.contains("UNIT FILE") || txt.contains("STATE")
                    || txt.contains("listed") || txt.contains("@"))
                    continue;
                    
                QStringList _list = txt.split(" ");
                _list.removeDuplicates();
                
                QStandardItem *unit = new QStandardItem(QString(_list.at(0)));
                unit->setIcon(QIcon(":/images/service.png"));
                model->setItem(count,0,unit);
                
                QStandardItem *active;
                if(QString(_list.at(2)).isEmpty())
                    active = new QStandardItem(QString(_list.at(1)));
                else
                    active = new QStandardItem(QString(_list.at(2)));
                if(QString(_list.at(2)).contains("enabled"))
                    active->setIcon(QIcon(":/images/true.png"));
                else if(QString(_list.at(2)).contains("disabled"))
                    active->setIcon(QIcon(":/images/never.png"));
                else
                    active->setIcon(QIcon(":/images/service.png"));
                model->setItem(count,1,active);
                ++count;
            }
        }
    }
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnWidth(0,500);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setItemDelegateForColumn(2,new coldxdelegato(this));
    
    initTableView();
    QModelIndex index = ui->tableView->model()->index(0,0);
    ui->tableView->selectionModel()->setCurrentIndex(index,QItemSelectionModel::Rows);//setCurrentIndex(index);
    ui->tableView->setFocus();
    clicked(index);
}

void ServiceManagement::restartService()
{
    QString m_restart;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex elemento = model->index(riga,0,QModelIndex());

    m_restart = model->data(elemento).toString();
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("systemctl restart "+m_restart);
    process.closeWriteChannel();
    while(process.state() != QProcess::NotRunning)
    {
        process.waitForReadyRead();
        QString result = process.readAll();
        ui->textEdit->insertHtml("<font color=\"#0055ff\"><b>"+result+"<b></font></br>");
        listServices();
    }
}

void ServiceManagement::startService()
{
    QString m_start;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex elemento = model->index(riga,0,QModelIndex());

    m_start = model->data(elemento).toString();
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("systemctl start "+m_start);
    process.closeWriteChannel();
    while(process.state() != QProcess::NotRunning)
    {
        process.waitForReadyRead();
        QString result = process.readAll();
        ui->textEdit->insertHtml("<font color=\"green\"><b>"+result+"<b></font></br>");
        listServices();
    }
}

void ServiceManagement::stopService()
{
    QString m_stop;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex elemento = model->index(riga,0,QModelIndex());

    m_stop = model->data(elemento).toString();
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("systemctl stop "+m_stop);
    process.closeWriteChannel();
    while(process.state() != QProcess::NotRunning)
    {
        process.waitForReadyRead();
        QString result = process.readAll();
        ui->textEdit->insertHtml("<font color=\"#ff0000\"><b>"+result+"<b></font></br>");
        listServices();
    }
}

void ServiceManagement::enableService()
{
    QString m_enable;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex elemento = model->index(riga,0,QModelIndex());

    m_enable = model->data(elemento).toString();
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("systemctl enable "+m_enable);
    process.closeWriteChannel();
    while(process.state() != QProcess::NotRunning)
    {
        process.waitForReadyRead();
        QString result = process.readAll();
        ui->textEdit->insertHtml("<font color=\"#ff0000\"><b>"+result+"<b></font></br>");
        listServices();
    }
}

void ServiceManagement::disableService()
{
    QString m_disable;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex elemento = model->index(riga,0,QModelIndex());

    m_disable = model->data(elemento).toString();
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("systemctl disable "+m_disable);
    process.closeWriteChannel();
    while(process.state() != QProcess::NotRunning)
    {
        process.waitForReadyRead();
        QString result = process.readAll();
        ui->textEdit->insertHtml("<font color=\"#ff0000\"><b>"+result+"<b></font></br>");
        listServices();
    }
}

void ServiceManagement::searchService(QString text)
{
    for( int i = 0; i < model->rowCount(); ++i )
    {
        bool match = false;
        for( int j = 0; j < model->columnCount(); ++j )
        {
            QStandardItem *item = model->item( i, j );
            if( item->text().contains(text) )
            {
                match = true;
                break;
            }
        }
        ui->tableView->setRowHidden( i, !match );
    }
}

ServiceManagement::~ServiceManagement()
{
    delete ui;
}

void ServiceManagement::clicked(QModelIndex index)
{
    if(index.isValid() && index.column() == 0){
        ui->textEdit->clear();
        QString service = model->data(model->index(index.row(),0),Qt::DisplayRole).toString();
        QProcess proc;
        QString results;
        proc.setReadChannel(QProcess::StandardOutput);
        proc.start("systemctl status "+service);
        if (proc.waitForStarted(3000))
        {
            if(proc.waitForReadyRead(-1))
            {
                QString m_tex = proc.readAll();
                QStringList list = m_tex.split("\n");
                QString html = "<p color=\"black\"><b>";
                for(int i=0; i<list.size();i++)
                {
                    if(QString(list.at(i)).isEmpty())
                        continue;

                    html.append(list.at(i)+"<br>");
                    html += "</p>";
                }
               if(html.contains("active (running)"))
                    ui->textEdit->insertHtml(html.replace("active (running)","<font color=\"green\"><b>active (running)</b></font>"));
               else if(html.contains("active (exited)"))
                    ui->textEdit->insertHtml(html.replace("active (exited)","<font color=\"green\"><b>active (exited)</b></font>"));
                else if(html.contains("inactive (dead)"))
                    ui->textEdit->insertHtml(html.replace("inactive (dead)","<font color=\"red\"><b>inactive (dead)</b></font>"));
                else
                    ui->textEdit->insertHtml(html);
            }
        }
        proc.close();
    }
}

void ServiceManagement::currentRowChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if(current.row() != -1){
         ui->tableView->setCurrentIndex(current);
         clicked(current);
         QString service = model->data(model->index(current.row(),1),Qt::DisplayRole).toString();
         if(service == "enabled")
             visibleButton(false,true,true,true,true);
         else if(service == "disabled")
             visibleButton(true,false,true,true,true);
         else if(service == "static")
             visibleButton(true,true,true,true,true);
     }
     else{
         ui->tableView->setCurrentIndex(previous);
         clicked(previous);
         QString service = model->data(model->index(previous.row(),1),Qt::DisplayRole).toString();
         if(service == "enabled")
             visibleButton(false,true,true,true,true);
         else if(service == "disabled")
             visibleButton(true,false,true,true,true);
         else if(service == "static")
             visibleButton(true,true,true,true,true);
    }
}

void ServiceManagement::initTableView()
{
    QItemSelectionModel *sm = ui->tableView->selectionModel();
    connect(sm, SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)),
            this, SLOT(currentRowChanged(const QModelIndex &,const QModelIndex &)));
}

void ServiceManagement::visibleButton(bool button1, bool button2, bool button3, bool button4, bool button5)
{
    ui->pushButton->setVisible(button1);
    ui->pushButtonDisable->setVisible(button2);
    ui->pushButtonStop->setVisible(button3);
    ui->pushButtonRestart->setVisible(button4);
    ui->pushButtonStart->setVisible(button5);
}
