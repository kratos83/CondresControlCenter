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

#include "CchHelper.h"
#include "CchAdaptor.h"
#include <QCoreApplication>
#include <QtDBus/QDBusConnection>
#include <QProcess>

Q_LOGGING_CATEGORY(DBusControlCenter,"ControlCenter")

CchHelper::CchHelper(QObject* parent) :
  QObject(parent)
{
    (void) new CchAdaptor(this);
    if (!QDBusConnection::systemBus().registerService("org.condrescontrolcenter.pacmanhelper")) {
      qCDebug(DBusControlCenter) << "Another helper is already running!";
      QCoreApplication::instance()->quit();
    }

    if (!QDBusConnection::systemBus().registerObject("/", this)) {
        qCDebug(DBusControlCenter) << "Unable to register service interface to dbus!";
        QCoreApplication::instance()->quit();
    }
}

void CchHelper::syncdb()
{
  QProcess pacman;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  pacman.setProcessEnvironment(env);

  QStringList params;
  params << "-Sy";
  QString command = "/usr/bin/pacman";
  pacman.execute(command, params);
  
  emit syncdbcompleted();
}
