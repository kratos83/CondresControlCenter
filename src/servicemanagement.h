#ifndef SERVICEMANAGEMENT_H
#define SERVICEMANAGEMENT_H

#include <QtWidgets>
#include <QtCore>

Q_DECLARE_LOGGING_CATEGORY(CondresControlCenterService)

namespace Ui
{
class ServiceManagement;
}

/**
 * @todo write docs
 */
class ServiceManagement : public QDialog
{
    Q_OBJECT

public:
    explicit ServiceManagement(QWidget *parent=nullptr);
    ~ServiceManagement();
    void listServices();
    void initTableView();
private slots:
    void clicked(QModelIndex index);
    void startService();
    void restartService();
    void stopService();
    void enableService();
    void disableService();
    void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void searchService(QString text);
protected slots:
    void visibleButton(bool button1, bool button2, bool button3, bool button4, bool button5);
private:
    Ui::ServiceManagement *ui;
    QStandardItemModel *model;
};

#endif // SERVICEMANAGEMENT_H
