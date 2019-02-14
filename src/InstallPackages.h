#ifndef INSTALLPACKAGES_H
#define INSTALLPACKAGES_H

#include <QDialog>
#include <QtWidgets>
#include <QtCore>
#include <QLoggingCategory>
#include "tablewidgetheader.h"
#include "Apply.h"

Q_DECLARE_LOGGING_CATEGORY(InstallPackagesDebug)

namespace Ui {
class InstallPackages;
}

class InstallPackages : public QDialog
{
    Q_OBJECT

public:
    explicit InstallPackages(QWidget *parent = nullptr);
    ~InstallPackages();
    void lista();

public slots:
    QString getPeso(QString byteReceived);
    void TableClicked(const QModelIndex &index);
    void TableClickedItem(QTableWidgetItem *item);
    void ApplyImpo();
    void removeImpo();
    void searchPackages(QString text);
    void on_pul_list_clicked();

private:
    Ui::InstallPackages *ui;
    TableWidgetHeader *m_table;
    QString m_peso;
    Apply *m_apply;
    QStringList m_string;
};

#endif // INSTALLPACKAGES_H
