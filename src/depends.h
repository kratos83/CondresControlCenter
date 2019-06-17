#ifndef DEPENDS_H
#define DEPENDS_H

#include <QDialog>
#include <QtWidgets>

namespace Ui
{
class Depends;
}

/**
 * @todo write docs
 */
class Depends : public QDialog
{
    Q_OBJECT

public:
    explicit Depends(QStringList list, QWidget *parent=nullptr);
    ~Depends();
    
public slots:
    void lista();
    void saveDepends(QTableWidgetItem *item);
    void saveList();
    
private:
    Ui::Depends *ui;
    QStringList m_list;
    
signals:
    void save(QString);
};

#endif // DEPENDS_H
