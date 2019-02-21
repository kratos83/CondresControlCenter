#ifndef UPGRADE_H
#define UPGRADE_H

#include <QDialog>
#include <QtCore>
#include <QtWidgets>
#include <QtNetwork>
#include "tablewidgetheader.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(ControlCenterUpgrade)

namespace Ui {
class Upgrade;
}

class Upgrade : public QDialog
{
    Q_OBJECT

public:
    explicit Upgrade(QString database, QWidget *parent = nullptr);
    ~Upgrade();
    bool NoConnection();
public slots:
    //View list
    void getListUpdate();
    void lista();
    //Update database
    void UpdateDb(int exitCode, QProcess::ExitStatus);
    void upgradeDB();
    void UpdateDBReady();
    QString getPeso(QString byteReceived);
    void itemClicked(QTableWidgetItem *item);
    void setList(QStringList item);
    QStringList getList();
    void removeDBPacman();
    //Update packages
    void updatePackages();
    void updatePackagesProcess(int exitCode, QProcess::ExitStatus);
    void showProgressInqDebug();

private slots:
    void on_dettagli_clicked();
    void on_back_clicked();

private:
    Ui::Upgrade *ui;
    QProcess processUpdate,*processDB,*processUpgrade,*remove_db;
    QStringList m_update, m_upgrade, m_addItem;
    bool m_updateDB;
    QString m_peso, m_database;
    TableWidgetHeader *m_table;
signals:
    void finishUpdate(bool);
};

#endif // UPGRADE_H
