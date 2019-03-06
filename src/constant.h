#ifndef CONSTANT_H
#define CONSTANT_H

#include <QtCore>

#define VERSION "0.2.0"
#define ALPHA "ALPHA RELEASE"
#define BETA "BETA RELEASE"
#define RELEASE_CANDIDATE "RC RELEASE"
#define STABLE "STABLE RELEASE"

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
#endif // CONSTANT_H
