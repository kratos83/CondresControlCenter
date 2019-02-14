#ifndef LOGFILE_H
#define LOGFILE_H

#include <QDialog>
#include <QtCore>
#include <QtWidgets>

namespace Ui {
class LogFile;
}

class LogFile : public QDialog
{
    Q_OBJECT

public:
    explicit LogFile(QString name, QWidget *parent = nullptr);
    ~LogFile();

private:
    Ui::LogFile *ui;
    QString m_name;
};

#endif // LOGFILE_H
