#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tab_style.h"
#include <QtWidgets>
#include <QtCore>
#include <QLoggingCategory>
#include <QSystemTrayIcon>

//Classi da istanziare
#include "cachecleaner/cachecleaner.h"
#include "repoeditor/repoeditor.h"
#include "upgrade.h"
#include "logfile.h"
#include "about.h"
#include "Apply.h"
#include "InstallPackages.h"

Q_DECLARE_LOGGING_CATEGORY(ControlCenterMain)

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initAction();
    void trayIcon();
public slots:
    QString titolo();
    QString getPacCache();
    void messageClicked(QSystemTrayIcon::ActivationReason reason);
    void visButtonStatusBar();
    void on_butt_repo_clicked();
    void ritorna_indietro();
    void on_butt_update_clicked();
    void resButton(bool vero);
    void on_butt_install_clicked();
    void on_butt_log_clicked();
    void on_butt_clean_clicked();
    void on_actionExit_triggered();
    void on_actionInformation_triggered();
    void on_actionUpdate_database_triggered();

private:
    Ui::MainWindow *ui;
    QProcess *process, *process_locate_paccache;
    QPushButton *butt_back;
    RepoEditor *repo;
    Upgrade *m_upgrade;
    CacheCleaner *m_cache;
    LogFile *m_log;
    About *m_about;
    Apply *m_apply;
    InstallPackages *m_packages;
    QAction * m_install, m_update, m_info;
    QSystemTrayIcon *m_trayIcon;
    QMenu *trayMenu;

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
