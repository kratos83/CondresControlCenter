#ifndef TABLEWIDGETHEADER_H
#define TABLEWIDGETHEADER_H

#include <QtWidgets>
#include <QPainter>

class TableWidgetHeader : public QHeaderView
{
    Q_OBJECT
public:
    TableWidgetHeader(Qt::Orientation orientation, QWidget *parent = nullptr);
protected:
  void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
  void mousePressEvent(QMouseEvent *event);

private:
  bool vero;
signals:
  void checkStausChange(bool check);
};

#endif // TABLEWIDGETHEADER_H
