/*
 * Copyright (C) 2019  Angelo Scarna' <angelo.scarna@codelinsoft.it>
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

#ifndef CCHHELPER_H
#define CCHHELPER_H

#include <QtCore>
#include <QtDBus/QDBusContext>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(DBusControlCenter)

class CchHelper : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.condrescontrolcenter.pacmanhelper")
public:
    explicit CchHelper(QObject *parent = 0);
  
public slots:
    void syncdb();

signals:
    void syncdbcompleted();
};

#endif // CCHHELPER_H
