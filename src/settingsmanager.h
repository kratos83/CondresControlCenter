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
#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QMap>
#include <QSettings>
#include <QtGui>
#include <QDir>
#include <QVariant>

class SettingsManager: public QObject {
    
Q_OBJECT

public:
	SettingsManager(QObject *parent=0);
    QVariant generalValue(QString const &key, QVariant const &defaultValue = QVariant()) const;
    void setGeneralValue( QString const &key, QVariant const &value);
    void removeValue(QString const &key);
	QDir *profilesDir() const;

	void createPluginSettings( const QString& );
	void deletePluginSettings( const QString& );
	bool existsPluginSettings( const QString& ) const;
	void setPluginValue( const QString&, const QString&, const QVariant& );
	QVariant pluginValue( const QString&, const QString&, const QVariant& ) const;

	QStringList profilesList( const QString& ) const;
	
	QDir *settingsDir, *profDir, *logDir, *logDirBack;
private:
	QMap<QString,QSettings*> pluginSettings;
	QSettings *general;
};

extern SettingsManager *settingsManager;

#endif
