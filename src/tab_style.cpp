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

#include "tab_style.h"

Tab_style::Tab_style()
{

}

QSize Tab_style::sizeFromContents(ContentsType type, const QStyleOption *option,
                                  const QSize &size, const QWidget *widget) const
{
    QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
           if(type == QStyle::CT_TabBarTab)
               s.transpose();
           return s;
}

void Tab_style::drawControl(ControlElement element, const QStyleOption *option,
                            QPainter *painter, const QWidget *widget) const
{
    if(element == CE_TabBarTabLabel)
    {
        if(const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
        {
              QStyleOptionTab opt(*tab);
              opt.shape = QTabBar::RoundedNorth;
              QProxyStyle::drawControl(element, &opt, painter, widget);
              return;
        }
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}
