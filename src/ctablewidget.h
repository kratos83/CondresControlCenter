#ifndef CTABLEWIDGET_H
#define CTABLEWIDGET_H

#include <QtWidgets>
#include <QtCore>

/**
 * @todo write docs
 */
class CTableWidget : public QTableWidget
{
    Q_OBJECT
    
public:
    explicit CTableWidget(QWidget *parent = nullptr);
    ~CTableWidget();
    
private slots:
    void mousePressEvent(QMouseEvent *even);
    void Popup(const QPoint& pt);
    
signals:
    void addInstallPkg(QString,QString);
};

#endif // CTABLEWIDGET_H
