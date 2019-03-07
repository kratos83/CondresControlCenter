#ifndef CONSTANT_H
#define CONSTANT_H

#include <QtCore>

#define VERSION "0.2.0"
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
#endif // CONSTANT_H
