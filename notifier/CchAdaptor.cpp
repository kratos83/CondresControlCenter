/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c CchAdaptor -a CchAdaptor.h:CchAdaptor.cpp polkit/org.condrescontrolcenter.pacmanhelper.xml
 *
 * qdbusxml2cpp is Copyright (C) 2019 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "CchAdaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class CchAdaptor
 */

CchAdaptor::CchAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

CchAdaptor::~CchAdaptor()
{
    // destructor
}

void CchAdaptor::syncdb()
{
    // handle method call org.condrescontrolcenter.pacmanhelper.syncdb
    QMetaObject::invokeMethod(parent(), "syncdb");
}

