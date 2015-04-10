#include "iqtarprocess.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>

IqTarProcess::IqTarProcess(QObject *parent) : QObject(parent)
{
    connect(&m_tar, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &IqTarProcess::removeUncompressedFile);
}

void IqTarProcess::setProcessEnvironment(const QProcessEnvironment &environment)
{
    m_tar.setProcessEnvironment(environment);
}

void IqTarProcess::start(const QString &filePath)
{
    QFileInfo file (filePath);
    if (!file.exists()) {
        qWarning () << "Not found file " << filePath;
        return;
    }


    m_filePath = filePath;

    QString tarProgram;
    tarProgram.append(binPath());
    tarProgram.append(" -czf ");
    tarProgram.append(file.fileName() + ".tar.gz ");
    tarProgram.append(file.fileName());

    m_tar.setWorkingDirectory(file.path());
    m_tar.start(tarProgram);

    qDebug() << "Start commpress file " << filePath << " with command " << tarProgram << " in " << file.path() << " dirrectory.";

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

void IqTarProcess::removeUncompressedFile(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        QFile file (m_filePath);
        if(!file.remove())
            qWarning() << "Error on remove file " << m_filePath;

        qDebug() << "End commpress file " << m_filePath;
    } else {
        qWarning() << "Error on compress file " << m_filePath;
    }

    emit finished(exitCode, exitStatus);
}


