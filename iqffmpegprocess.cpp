#include "iqffmpegprocess.h"
#include <QDateTime>
#include <QDebug>

IQFFmpegProcess::IQFFmpegProcess(QObject *parent) :
    QObject(parent)
{
    connect(&_finishedTimer, SIGNAL(timeout()), this, SLOT(stop()));
}

void IQFFmpegProcess::setProcessEnvironment(const QProcessEnvironment &environment)
{
    _ffmpeg.setProcessEnvironment(environment);
}

void IQFFmpegProcess::setFFmpegBinString(const QString &ffmpegBinStr)
{
    if (_ffmpegBinString == ffmpegBinStr)
        return;
    _ffmpegBinString = ffmpegBinStr;
}

void IQFFmpegProcess::setOutputDir(const QString &dir)
{
    if (_outputDir == dir)
        return;
    _outputDir = dir;
}

void IQFFmpegProcess::start(const QString &hostName, const int height, const int width, const qint64 duration)
{
    QString ffmpegProgram = _ffmpegBinString;
    ffmpegProgram.replace("%RESOLUTION%", QString("%0x%1").arg(height).arg(width));
    ffmpegProgram.replace("%HOSTNAME%", hostName);
    QDateTime curDT = QDateTime::currentDateTime();
    QDateTime endDT = curDT.addMSecs(duration);
    QString startDT = curDT.toString("dd.MM.yyyy_hh.mm");
    QString endDTStr = endDT.toString("dd.MM.yyyy_hh.mm");
    QString outputFile = QString("%0/%1_%2-%3.mkv")
            .arg(_outputDir)
            .arg(hostName)
            .arg(startDT)
            .arg(endDTStr);
    ffmpegProgram.replace("%OUPTUPFILE%", outputFile);

    qDebug() << "Start spy to host " << hostName << "to file " << outputFile;
    _host = hostName;
    _fileName = outputFile;
    _ffmpeg.start(ffmpegProgram);
    _finishedTimer.start(duration + _lapping);
}

void IQFFmpegProcess::stop()
{
    if(!_ffmpeg.waitForStarted())
    {
        _ffmpeg.kill();
        emit finished(1);
        return;
    }
    _ffmpeg.write("q");
    if (!_ffmpeg.waitForFinished(_finishedInterval))
        _ffmpeg.kill();
    qDebug() << "End spy to host " << _host << ". File saved to " << _fileName;
    emit finished(0);
}
