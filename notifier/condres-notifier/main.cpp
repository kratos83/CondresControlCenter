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

#include "NotifierCondres.h"

#include <QApplication>
#include <QtGui>
#include <QDebug>
#include <QProcess>
#include <QLoggingCategory>
#include "kdsingleapplicationguard/kdsingleapplicationguard.h"
#include "NotifierApp.h"

#include "../src/settingsmanager.h"

Q_DECLARE_LOGGING_CATEGORY(NotifierControlCenterMain)
Q_LOGGING_CATEGORY(NotifierControlCenterMain, "ControlCenter")

//Command
static bool parseArguments(const QStringList &argomento)
{
    QStringListIterator arg_cmd(argomento);
    arg_cmd.next();
    while(arg_cmd.hasNext())
    {
        const QString &a_arg = arg_cmd.next().toLower();
        if(a_arg == "-u" || a_arg == "--update"){
            return true;
        }
        else if(a_arg == "-h" || a_arg == "--help")
        {
            qCDebug(NotifierControlCenterMain) << QObject::tr("USAGE:   ") << QObject::tr("DISPLAYS");
            qCDebug(NotifierControlCenterMain) << QObject::tr("-u or --update       ") << QObject::tr("Update packages:");
            qCDebug(NotifierControlCenterMain) << QObject::tr("VIEW THE INFORMATION OF THE PROGRAM:");
            qCDebug(NotifierControlCenterMain) << QObject::tr("-h or --help     ") << QObject::tr("Software information.");
            return false;
        }
        else{
            qCDebug(NotifierControlCenterMain) << QObject::tr("Command not found: ") << a_arg;
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[])
{  

    NotifierApp app(argc, argv);
  
    if(!parseArguments(app.arguments()))
    {
        return -1;
    }

    const QString arg1 = argv[1];

    const QString arg3 = argv[3];
  
    if (!QFile::exists(DBUS_PACMANHELPER_SERVICE))
    {
        qDebug() << "Aborting notifier as 'pacmanhelper' DBus service could not be found! [" << DBUS_PACMANHELPER_SERVICE << "]";
        return (-3);
    }
    
    app.setQuitOnLastWindowClosed(false);
    // ensure only one tray icon application is runnung
    KDSingleApplicationGuard guard( KDSingleApplicationGuard::AutoKillOtherInstances );

    int returnCode = 0;
    if ( guard.isPrimaryInstance() )
    {
        app.init();
        NotifierCondres m_not( &app );
        returnCode = app.exec();
    }
    else
        qDebug() << "CSM Notifier is already running, shutting down.";
    

    return returnCode;
}
