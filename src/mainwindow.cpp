#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constant.h"

Q_LOGGING_CATEGORY(ControlCenterMain,"ControlCenter")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    butt_back = new QPushButton(QIcon(":/images/back.png"),tr("Back"),this);
    connect(butt_back,SIGNAL(clicked(bool)),this,SLOT(ritorna_indietro()));
    ui->tabWidget->tabBar()->setStyle(new Tab_style);
    setWindowTitle(titolo()+tr("Control Center ")+QString(VERSION)+" "+QString(ALPHA));
    ui->stackedWidget->setCurrentIndex(0);
    visButtonStatusBar();
    initAction();
    trayIcon();
}

void MainWindow::initAction()
{
    m_install = new QAction(QIcon(":/images/install.png"),"Install packages",this);
    connect(m_install,&QAction::triggered,this,&MainWindow::on_butt_install_clicked);
}

void MainWindow::trayIcon()
{
    trayMenu = new QMenu(this);
    trayMenu->addAction(m_install);
    
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/images/preferences-system.png")), QIcon::Normal, QIcon::Off);

    m_trayIcon = new QSystemTrayIcon(icon,this);
    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();
    
    connect(m_trayIcon, &QSystemTrayIcon::activated,this,&MainWindow::messageClicked);
}

void MainWindow::messageClicked(QSystemTrayIcon::ActivationReason reason){

    switch(reason){
    case QSystemTrayIcon::DoubleClick:
        showNormal();
        break;
    default:
        ;
    }
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

QString MainWindow::getPacCache()
{
    QString os_type = "";
    QString com_lsb = "/usr/bin/paccache";
    process_locate_paccache = new QProcess;
    process_locate_paccache->setReadChannel(QProcess::StandardOutput);
    process_locate_paccache->setProcessChannelMode(QProcess::MergedChannels);
    process_locate_paccache->start(com_lsb);
    process_locate_paccache->closeWriteChannel();
    if(process_locate_paccache->state()!=QProcess::NotRunning){
        process_locate_paccache->waitForReadyRead();
        QByteArray result = process_locate_paccache->readAll();
        os_type += result;
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
    butt_back->setVisible(vero);
}

void MainWindow::on_butt_install_clicked()
{
    showMaximized();
    m_packages = new InstallPackages(this);
    ui->stackedWidget->insertWidget(2,m_packages);
    ui->stackedWidget->setCurrentIndex(2);
    butt_back->setVisible(true);
}

void MainWindow::on_butt_log_clicked()
{
    m_log = new LogFile("/var/log/pacman.log",this);
    ui->stackedWidget->insertWidget(3,m_log);
    ui->stackedWidget->setCurrentIndex(3);
    butt_back->setVisible(true);
}

void MainWindow::on_butt_clean_clicked()
{
    if(!getPacCache().isEmpty()){
        m_cache = new CacheCleaner(this);
        ui->stackedWidget->insertWidget(4,m_cache);
        ui->stackedWidget->setCurrentIndex(4);
        butt_back->setVisible(true);
    }
    else{
        m_apply = new Apply("pacman-contrib","install",this);
        m_apply->exec();
    }

}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
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

void MainWindow::closeEvent(QCloseEvent *event){
    event->ignore();
    hide();
}
