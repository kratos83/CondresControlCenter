#ifndef APPLY_H
#define APPLY_H

#include <QDialog>
#include <QtCore>
#include <QtWidgets>
#include <QtNetwork>

namespace Ui{
    class Apply;
}

class Apply : public QDialog
{
public:
    //Install list packages
    Apply(QStringList listPackages, QString install_remove,
          QWidget *parent = nullptr);
    //Instlla single packages
    Apply(QString args, QString install_remove,
          QWidget *parent = nullptr);
    ~Apply();

public slots:
    void installPackages();
    void removePackages();
    void closeDialog();
    void read_packages(int exitCode);
    void ReadyPkg();
    void localPackages();
private:
    Ui::Apply *ui;
    QString _arg, m_args, m_install_remove;
    QStringList m_list;
    QProcess *m_process,*m_process_remove,*m_process_local;
};

#endif // APPLY_H
