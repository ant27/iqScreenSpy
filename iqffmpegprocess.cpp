#include "iqffmpegprocess.h"
#include <QDateTime>
#include <QDebug>
#include <QHostInfo>

IqFfmpegProcess::IqFfmpegProcess(QObject *parent) :
    QObject(parent)
{
    connect(&m_ffmpeg, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &IqFfmpegProcess::finished);
    connect(&m_finishedTimer, &QTimer::timeout, this, &IqFfmpegProcess::stop);
}

void IqFfmpegProcess::setProcessEnvironment(const QProcessEnvironment &environment)
{
    m_ffmpeg.setProcessEnvironment(environment);
}

void IqFfmpegProcess::setOutputDir(const QString &dir)
{
    if (m_outputDir != dir)
        m_outputDir = dir;
}

void IqFfmpegProcess::start(const qint64 duration)
{
    QDateTime curDT = QDateTime::currentDateTime();
    QDateTime endDT = curDT.addMSecs(duration);
    QString startDT = curDT.toString("dd.MM.yyyy_hh.mm");
    QString endDTStr = endDT.toString("dd.MM.yyyy_hh.mm");
    QString outputFile = QString("%0/%1_%2-%3%4")
            .arg(m_outputDir)
            .arg(QHostInfo::localHostName())
            .arg(startDT)
            .arg(endDTStr)
            .arg(outputFileExtension());

    QString ffmpegProgram;
    ffmpegProgram.append(binPath());
    ffmpegProgram.append(" -y ");
    ffmpegProgram.append(" -s ");
    ffmpegProgram.append(screenGeometry());
    ffmpegProgram.append(" -f x11grab ");
    ffmpegProgram.append(" -i :0.0 ");
    ffmpegProgram.append(" -r ");
    ffmpegProgram.append(QString::number(fps()));
    ffmpegProgram.append(" -threads ");
    ffmpegProgram.append(QString::number(threads()));
    ffmpegProgram.append(" -vcodec ");
    ffmpegProgram.append(vcodeParam());
    ffmpegProgram.append(" ");
    ffmpegProgram.append(outputFile);

    qDebug() << "Start record screen to file " << outputFile << ". With command " << ffmpegProgram;
    m_fileName = outputFile;
    m_ffmpeg.start(ffmpegProgram);

    if(!m_ffmpeg.waitForStarted()) {
        m_ffmpeg.kill();
        return;
    }

    m_finishedTimer.start(duration + m_lapping);
}

void IqFfmpegProcess::stop()
{
    m_ffmpeg.write("q");
    if (!m_ffmpeg.waitForFinished(m_finishedInterval))
        m_ffmpeg.kill();
    qDebug() << "End record screen. File saved to " << m_fileName;
}
QString IqFfmpegProcess::screenGeometry() const
{
    return m_screenGeometry;
}

void IqFfmpegProcess::setScreenGeometry(const QString &screenGeometry)
{
    m_screenGeometry = screenGeometry;
}

QString IqFfmpegProcess::binPath() const
{
    return m_binPath;
}

void IqFfmpegProcess::setBinPath(const QString &binPath)
{
    m_binPath = binPath;
}


int IqFfmpegProcess::threads() const
{
    return m_threads;
}

void IqFfmpegProcess::setThreads(int threads)
{
    if (m_threads != threads)
        m_threads = threads;
}

QString IqFfmpegProcess::vcodeParam() const
{
    return m_vcodeParam;
}

void IqFfmpegProcess::setVcodeParam(const QString &vcodeParam)
{
    if (m_vcodeParam != vcodeParam)
        m_vcodeParam = vcodeParam;
}

int IqFfmpegProcess::fps() const
{
    return m_fps;
}

void IqFfmpegProcess::setFps(int fps)
{
    if (m_fps != fps)
        m_fps = fps;
}

QString IqFfmpegProcess::outputFileExtension() const
{
    return m_outputFileExtension;
}

void IqFfmpegProcess::setOutputFileExtension(const QString &outputFileExtension)
{
    if (m_outputFileExtension != outputFileExtension)
        m_outputFileExtension = outputFileExtension;
}

QString IqFfmpegProcess::outputDir() const
{
    return m_outputDir;
}

