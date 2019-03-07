#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constant.h"

Q_LOGGING_CATEGORY(ControlCenterMain,"ControlCenter")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    manager(new SettingsManager)
{
    ui->setupUi(this);
    butt_back = new QPushButton(QIcon(":/images/back.png"),tr("Back"),this);
    connect(butt_back,SIGNAL(clicked(bool)),this,SLOT(ritorna_indietro()));
    connect(ui->pushKey,&QPushButton::clicked,this,&MainWindow::openKeyboard);
    connect(ui->hard_conf,&QPushButton::clicked,this,&MainWindow::openHardware);
    ui->tabWidget->tabBar()->setStyle(new Tab_style);
    setWindowTitle(titolo()+tr("Control Center ")+QString(VERSION)+" "+QString(BETA)+" 1");
    ui->stackedWidget->setCurrentIndex(0);
    visButtonStatusBar();
}

QString MainWindow::titolo()
{
    QString os_type = "";
    QString com_lsb = "lsb_release -d ";
    process = new QProcess;
    process->setReadChannel(QProcess::StandardOutput);
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->start(com_lsb);
    process->closeWriteChannel();
    if(process->state()!=QProcess::NotRunning){
        process->waitForReadyRead();
        QByteArray result = process->readAll();
        QStringList list = QString(result).split(":");
        QString text = list.at(1);
        os_type += text;
    }
    return os_type;
}


void MainWindow::visButtonStatusBar()
{
    QLabel *space = new QLabel;
    ui->statusbar->addWidget(space,1);
    ui->statusbar->addWidget(butt_back);
    butt_back->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_butt_repo_clicked()
{
    repo = new RepoEditor(this);
    ui->stackedWidget->insertWidget(1,repo);
    ui->stackedWidget->setCurrentIndex(1);
    butt_back->setVisible(true);
}

void MainWindow::ritorna_indietro()
{
    ui->stackedWidget->setCurrentIndex(0);
    visButtonStatusBar();
}

void MainWindow::on_butt_update_clicked()
{
    showMaximized();
    m_upgrade = new Upgrade("",this);
    connect(m_upgrade,&Upgrade::finishUpdate,this,&MainWindow::resButton);
    m_upgrade->upgradeDB();
    ui->stackedWidget->insertWidget(1,m_upgrade);
    ui->stackedWidget->setCurrentIndex(1);
    butt_back->setVisible(true);
}

void MainWindow::resButton(bool vero)
{
    if(vero)
    {
        manager->setGeneralValue("Update/Upgrade","complete");
    }
}

void MainWindow::on_butt_install_clicked()
{
    showMaximized();
    m_packages = new InstallPackages(this);
    ui->stackedWidget->insertWidget(1,m_packages);
    ui->stackedWidget->setCurrentIndex(1);
    butt_back->setVisible(true);
}

void MainWindow::on_butt_log_clicked()
{
    m_log = new LogFile("/var/log/pacman.log",this);
    ui->stackedWidget->insertWidget(1,m_log);
    ui->stackedWidget->setCurrentIndex(1);
    butt_back->setVisible(true);
}

void MainWindow::on_butt_clean_clicked()
{
    if(!getPacCache().isEmpty()){
        m_cache = new CacheCleaner(this);
        ui->stackedWidget->insertWidget(1,m_cache);
        ui->stackedWidget->setCurrentIndex(1);
        butt_back->setVisible(true);
    }
    else{
        m_apply = new Apply("pacman-contrib","install",this);
        m_apply->exec();
    }

}

void MainWindow::on_actionExit_triggered()
{
    m_esci = new esci(this);
    m_esci->exec();
}

void MainWindow::on_actionInformation_triggered()
{
    m_about = new About(this);
    m_about->exec();
}

void MainWindow::on_actionUpdate_database_triggered()
{
    m_upgrade = new Upgrade("update",this);
    m_upgrade->upgradeDB();
}

void MainWindow::openKeyboard()
{
    page = new KeyboardPage(this);
    page->load();
    ui->stackedWidget->insertWidget(1,page);
    ui->stackedWidget->setCurrentIndex(1);
    butt_back->setVisible(true);
}

void MainWindow::openHardware()
{
    if(!getMHwd().isEmpty()){
        m_pageHardware = new MhwdPage(this);
        m_pageHardware->load();
        ui->stackedWidget->insertWidget(1,m_pageHardware);
        ui->stackedWidget->setCurrentIndex(1);
        butt_back->setVisible(true);
    }
    else{
        m_apply = new Apply(QStringList() << "mhwd" << "mhwd-db","install",this);
        m_apply->exec();
    }
}

void MainWindow::closeEvent(QCloseEvent *event){
    event->ignore();
    m_esci = new esci(this);
    m_esci->exec();
}
