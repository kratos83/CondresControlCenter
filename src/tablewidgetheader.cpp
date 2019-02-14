#include "tablewidgetheader.h"
#include <iostream>

using namespace std;

TableWidgetHeader::TableWidgetHeader(Qt::Orientation orientation, QWidget *parent) :
    QHeaderView(orientation,parent),
    vero(false)
{}

void TableWidgetHeader::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    if (logicalIndex == 0)
    {
      QStyleOptionButton option;
      option.rect = QRect(5,6,20,20);
      option.state = QStyle::State_Enabled | QStyle::State_Active;
      if(vero)
         option.state = QStyle::State_On;
      else
         option.state = QStyle::State_Off;
      style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
    }
}

void TableWidgetHeader::mousePressEvent(QMouseEvent *event)
{
    if(vero)
      vero = false;
    else
      vero = true;
    emit checkStausChange(vero);
    update();
    QHeaderView::mousePressEvent(event);
}
