#include "iqtarprocess.h"
#include <QDebug>

IqTarProcess::IqTarProcess(QObject *parent) : QObject(parent)
{
    connect(&m_tar, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &IqTarProcess::finished);
}

void IqTarProcess::setProcessEnvironment(const QProcessEnvironment &environment)
{
    m_tar.setProcessEnvironment(environment);
}

void IqTarProcess::start(const QString &filePath)
{
    qDebug() << "Start commpress file " << filePath;

    QString tarProgram;
    tarProgram.append(binPath());
    tarProgram.append(" -czf ");
    tarProgram.append(filePath + ".tar.gz ");
    tarProgram.append(filePath);

    m_tar.start(tarProgram);

    if(!m_tar.waitForStarted()) {
        m_tar.kill();
        return;
    }
}

QString IqTarProcess::binPath() const
{
    return m_binPath;
}

void IqTarProcess::setBinPath(const QString &binPath)
{
    m_binPath = binPath;
}


