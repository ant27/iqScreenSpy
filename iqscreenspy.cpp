#define SETTING_PARAM_OUTPUT_DIR "outputDir"
#define SETTING_PARAM_HOSTS "hosts"
#define SETTING_PARAM_FFMPEG_BIN_STRING "ffmpegBinString"
#define SETTING_PARAM_FFMPEG_LIBRARY_PATH "ffmpegLibraryPath"
#define SETTING_PARAM_STORAGE_TIME "storageTime"
#define SETTING_PARAM_RECORD_DURATION "recordDuration"

#include "iqscreenspy.h"
#include <QSettings>
#include <QDateTime>
#include <QStringList>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include "iqffmpegprocess.h"

IQScreenSpy::IQScreenSpy(QObject *parent) :
    QObject(parent)
{
    _timer.setInterval(1000);
    _timer.setSingleShot(false);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

void IQScreenSpy::start()
{
    startNewRecords();
    _timer.start();
}

void IQScreenSpy::onTimeout()
{
    QTime currtentTime = QTime::currentTime();
    QSettings settings;
    if (!settings.contains(SETTING_PARAM_RECORD_DURATION))
        settings.setValue(SETTING_PARAM_RECORD_DURATION, 600000);
    qint64 recordDuration = settings.value(SETTING_PARAM_RECORD_DURATION);
    int recordDurationInMinutes = recordDuration / 60000;
    if (currtentTime.second() != 0 || currtentTime.minute() % recordDurationInMinutes != 0)
        return;

    startNewRecords();
}

void IQScreenSpy::startNewRecords()
{
    if (!settings.contains(SETTING_PARAM_RECORD_DURATION))
        settings.setValue(SETTING_PARAM_RECORD_DURATION, 600000);
    qint64 recordDuration = settings.value(SETTING_PARAM_RECORD_DURATION);
    int recordDurationInMinutes = recordDuration / 60000;

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QDateTime nextDateTime = currentDateTime;
    if (currentDateTime.time().minute() >= 50)
    {
        nextDateTime = nextDateTime.addSecs(3600);
        nextDateTime.setTime(QTime(nextDateTime.time().hour(), 0, 0));
    }
    else
    {
        int nextMinute = currentDateTime.time().minute() + recordDurationInMinutes - currentDateTime.time().minute()%recordDurationInMinutes;
        nextDateTime.setTime(QTime(nextDateTime.time().hour(), nextMinute, 0));
    }
    qint64 duration = currentDateTime.msecsTo(nextDateTime);

    if (!settings.contains(SETTING_PARAM_OUTPUT_DIR))
        settings.setValue(SETTING_PARAM_OUTPUT_DIR, "~");
    QString outputDir = settings.value(SETTING_PARAM_OUTPUT_DIR).toString();
    if (!settings.contains(SETTING_PARAM_FFMPEG_BIN_STRING))
        settings.setValue(SETTING_PARAM_FFMPEG_BIN_STRING, "ffmpeg -r 10 -s %RESOLUTION% -f x11grab -i %HOSTNAME%:0.0 -s %RESOLUTION% -vcodec libx264 -y %OUPTUPFILE%");
    QString ffmpegBinStr = settings.value(SETTING_PARAM_FFMPEG_BIN_STRING).toString();
    if (!settings.contains(SETTING_PARAM_HOSTS))
        settings.setValue(SETTING_PARAM_HOSTS, "localhost:1366x768");
    QString hosts = settings.value(SETTING_PARAM_HOSTS).toString();
    QStringList hostsList = hosts.split(" ");

    if (!settings.contains(SETTING_PARAM_FFMPEG_LIBRARY_PATH))
        settings.setValue(SETTING_PARAM_FFMPEG_LIBRARY_PATH, "");
    QString ffmpegLibraryPath = settings.value(SETTING_PARAM_FFMPEG_LIBRARY_PATH).toString();
    QProcessEnvironment ffmpegEnvironment = QProcessEnvironment::systemEnvironment();
    if (!ffmpegLibraryPath.isEmpty())
    {
        ffmpegEnvironment.insert("LD_LIBRARY_PATH", ffmpegEnvironment.value("LD_LIBRARY_PATH") + ":" + ffmpegLibraryPath);
    }

    foreach (QString hostStr, hostsList)
    {
        QStringList pars1 = hostStr.split(":");
        if (pars1.size() != 2)
            continue;
        QString host = pars1[0];
        QStringList pars2 = pars1[1].split("x");
        if (pars2.size() != 2)
            continue;
        int height = pars2[0].toInt();
        int width = pars2[1].toInt();

        IQFFmpegProcess *ffmpeg = new IQFFmpegProcess(this);
        connect(ffmpeg, SIGNAL(finished(int)), ffmpeg, SLOT(deleteLater()));
        ffmpeg->setFFmpegBinString(ffmpegBinStr);
        ffmpeg->setProcessEnvironment(ffmpegEnvironment);
        ffmpeg->setOutputDir(outputDir);
        ffmpeg->start(host, height, width, duration);
    }

    clearOld();
}

void IQScreenSpy::clearOld()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QSettings settings;
    QString outputDirStr = settings.value(SETTING_PARAM_OUTPUT_DIR).toString();
    if (!settings.contains(SETTING_PARAM_STORAGE_TIME))
        settings.setValue(SETTING_PARAM_STORAGE_TIME, (qint64)3600000);
    qint64 storageTime = settings.value(SETTING_PARAM_STORAGE_TIME).toLongLong();
    QDir outputDir (outputDirStr);
    QFileInfoList outputDirEntry = outputDir.entryInfoList(QStringList() << "*.mkv" << "*.mkv.gz");
    QRegExp rx ("(.*)_(\\d\\d\\.\\d\\d\\.\\d\\d\\d\\d)_(\\d\\d\\.\\d\\d)-(\\d\\d\\.\\d\\d\\.\\d\\d\\d\\d)_(\\d\\d\\.\\d\\d)");
    foreach (QFileInfo file, outputDirEntry)
    {
        if (rx.indexIn(file.fileName()) == -1)
            continue;
        QStringList filePrs = rx.capturedTexts();
        if (filePrs.size() != 6)
            continue;
        QDateTime fileDateTime = QDateTime::fromString(filePrs[2] + " " + filePrs[3], "dd.MM.yyyy hh.mm");

        if (fileDateTime.msecsTo(currentDateTime) > storageTime)
        {
            QFile::remove(file.filePath());
            qDebug() << "Remove " << file.filePath();
        }
    }
}
