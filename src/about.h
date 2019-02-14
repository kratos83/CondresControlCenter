#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QtCore>
#include <QtWidgets>

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    ~About();

public slots:
    void viewPacman();
    void viewControlCenter();

private slots:
    void on_chiudi_clicked();

private:
    Ui::About *ui;
    QProcess *process;
};

#endif // ABOUT_H
