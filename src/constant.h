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

#ifndef CONSTANT_H
#define CONSTANT_H

#include <QtCore>
#include <QtWidgets>
#include <QtSql/QtSql>

#define VERSION "0.2.2"
#define ALPHA "ALPHA RELEASE"
#define BETA "BETA RELEASE"
#define RELEASE_CANDIDATE "RC RELEASE"
#define STABLE "STABLE RELEASE"

static QString getPacCache()
{
    QString os_type = "";
    QString com_lsb = "/usr/bin/paccache";
    QProcess process_locate_paccache;
    process_locate_paccache.setReadChannel(QProcess::StandardOutput);
    process_locate_paccache.setProcessChannelMode(QProcess::MergedChannels);
    process_locate_paccache.start(com_lsb);
    process_locate_paccache.closeWriteChannel();
    if(process_locate_paccache.state()!=QProcess::NotRunning){
        process_locate_paccache.waitForReadyRead();
        QByteArray result = process_locate_paccache.readAll();
        os_type += result;
    }
    process_locate_paccache.close();
    return os_type;
}

static QString getMHwd()
{
    QString os_type = "";
    QString com_lsb = "/usr/bin/mhwd";
    QProcess process_locate_paccache;
    process_locate_paccache.setReadChannel(QProcess::StandardOutput);
    process_locate_paccache.setProcessChannelMode(QProcess::MergedChannels);
    process_locate_paccache.start(com_lsb);
    process_locate_paccache.closeWriteChannel();
    if(process_locate_paccache.state()!=QProcess::NotRunning){
        process_locate_paccache.waitForReadyRead();
        QByteArray result = process_locate_paccache.readAll();
        os_type += result;
    }
    process_locate_paccache.close();
    return os_type;
}

static QString getSamba()
{
    QString os_type = "";
    QString com_lsb = "/usr/bin/samba-tool";
    QProcess process_locate_paccache;
    process_locate_paccache.setReadChannel(QProcess::StandardOutput);
    process_locate_paccache.setProcessChannelMode(QProcess::MergedChannels);
    process_locate_paccache.start(com_lsb);
    process_locate_paccache.closeWriteChannel();
    if(process_locate_paccache.state()!=QProcess::NotRunning){
        process_locate_paccache.waitForReadyRead();
        QByteArray result = process_locate_paccache.readAll();
        os_type += result;
    }
    process_locate_paccache.close();
    return os_type;
}

static QString getNfs()
{
    QString os_type = "";
    QString com_lsb = "/usr/bin/nfsconf";
    QProcess process_locate_paccache;
    process_locate_paccache.setReadChannel(QProcess::StandardOutput);
    process_locate_paccache.setProcessChannelMode(QProcess::MergedChannels);
    process_locate_paccache.start(com_lsb);
    process_locate_paccache.closeWriteChannel();
    if(process_locate_paccache.state()!=QProcess::NotRunning){
        process_locate_paccache.waitForReadyRead();
        QByteArray result = process_locate_paccache.readAll();
        os_type += result;
    }
    process_locate_paccache.close();
    return os_type;
}

static QString getProcess(QString m_proc, QString name)
{
    QProcess proc;
    QString results;
    proc.setReadChannel(QProcess::StandardOutput);
    proc.start(m_proc+" "+name);
    if (proc.waitForStarted(3000))
    {
        if(proc.waitForReadyRead(-1))
        {
            proc.waitForFinished(-1);
            
            results = QString(proc.readAll()).remove(name+" ");
        }
    }
    if(results != "")
        return results;
    else
        return QString("No information to download.");
}

static QString getProcessName(QString m_proc)
{
    QProcess proc;
    QString results;
    proc.setReadChannel(QProcess::StandardOutput);
    proc.start(m_proc);
    if (proc.waitForStarted(3000))
    {
        if(proc.waitForReadyRead(-1))
        {
            proc.waitForFinished(-1);
            QString m_tex = proc.readAllStandardOutput();
            QStringList list = m_tex.split(":");
            results = list.at(0);
        }
    }
    if(results != "")
        return results;
    else
        return QString("No information to download.");
}

static QString searchItem(QString nameFile, QString name)
{
    QString text;
    QFile file(nameFile);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        QMessageBox::warning(nullptr,"Condres Control Center", "Can't open file <b>smb.conf</b>");
    else{
        while(!file.atEnd())
        {
            int pos = file.pos();
            QString line = file.readLine();
            if(line.contains(name) && pos)
            {
                text = line;
            }
        }
    }
    return text;
}

static QStringList searchItemName(QString nameFile, QString name)
{
    QStringList text;
    QFile file(nameFile);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        QMessageBox::warning(nullptr,"Condres Control Center", "Can't open file <b>smb.conf</b>");
    else{
        while(!file.atEnd())
        {
            int pos = file.pos();
            QString line = file.readLine();
            if(line.contains(name) && pos)
            {
                text << line;
            }
        }
    }
    return text;
}

/*static QString returnQuery(QString name)
{
    
}*/
#endif // CONSTANT_H
