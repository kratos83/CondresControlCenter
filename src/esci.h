#ifndef ESCI_H
#define ESCI_H

#include <QDialog>

namespace Ui {
    class esci;
}

class esci : public QDialog
{
    Q_OBJECT

public:
    explicit esci(QWidget *parent = 0);
    ~esci();
    QString versione;

private:
    Ui::esci *ui;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // ESCI_H
