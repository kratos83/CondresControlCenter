/*
 * Copyright (C) 2019  angelo <angelo.scarna@codelinsoft.it>
 * 
 * This file is part of Condres Control Center.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "tablewidgetheader.h"
#include <iostream>

using namespace std;

TableWidgetHeader::TableWidgetHeader(Qt::Orientation orientation, QWidget *parent) :
    QHeaderView(orientation,parent),
    vero(false)
{
}

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
