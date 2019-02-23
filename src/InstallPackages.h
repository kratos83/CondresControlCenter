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
    void setMenuInstall();
public slots:
    QString getPeso(QString byteReceived);
    void TableClicked(const QModelIndex &index);
    void TableClickedItem(QTableWidgetItem *item);
    void ApplyImpo();
    void removeImpo();
    void searchPackages(QString text);
    void on_pul_list_clicked();
    void viewGroupsPackages(bool vero);
    void viewGroupsRepo(bool vero);
    void viewPackagesGroup();
    void viewRepo();
    void installLocalPackages();
    void clickListItem(QListWidgetItem *item);
    void clickListRepo(QListWidgetItem *item);

private:
    Ui::InstallPackages *ui;
    TableWidgetHeader *m_table;
    QString m_peso;
    Apply *m_apply;
    QStringList m_string;
    QMenu *menu, *menuFile, *menuGroup;
    QAction *install_local_packages, *boxGroup, *m_repo;
    QProcess *m_process_group,*m_process_list;
    QTableWidget *widgetLista,*widgetListaGroup;
    QTableWidgetItem *name,*m_item, *versione,*repo, *Peso, *pesoCount, *desc;
};

#endif // INSTALLPACKAGES_H
