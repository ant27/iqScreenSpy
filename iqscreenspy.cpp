#define RECORD_DURATION_MINUTES 10

#define SETTING_GENERAL_OUTPUT_DIR "outputDir"

#define SETTING_GROUP_FFMPEG "FFmpegParams"
#define SETTING_FFMPEG_FFMPEG_BIN_PATH "ffmpegBinPath"
#define SETTING_FFMPEG_FFMPEG_EXTRA_LD_LIBRARYPATH "extraLdLibraryPath"
#define SETTING_FFMPEG_MAXIMUM_FPS "maximumFps"
#define SETTING_FFMPEG_MAXIMUM_THREADS "maximumThreads"
#define SETTING_FFMPEG_VCODEC_PARAMS "vcodecParams"
#define SETTING_FFMPEG_OUTPUT_FILE_EXTENSION "outputFileExtension"

#define SETTING_GROUP_ROTATE "RotateParams"
#define SETTING_ROTATE_STORAGE_TIME "storageTime"

#define SETTING_GROUP_ARCHIVE "ArchiveParams"
#define SETTING_ARCHIVE_COMMPRESS_ENABLED "commpressEnabled"
#define SETTING_ARCHIVE_UNCOMPRESSED_STORAGE_TIME "uncompressedStorageTime"
#define SETTING_ARCHIVE_TAR_BIN_PATH "tarBinPath"

#define CUSTOM_SETTINGS_FILE "iqScreenSpyCustom"
#define SETTING_SCREEN "screen"

#include "iqscreenspy.h"
#include <QSettings>
#include <QDateTime>
#include <QStringList>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QDesktopWidget>
#include <QApplication>
#include "iqffmpegprocess.h"
#include "iqtarprocess.h"

IqScreenSpy::IqScreenSpy(QObject *parent) :
    QObject(parent),
    m_archiveInProcess(false)
{
    m_timer.setInterval(1000);
    m_timer.setSingleShot(false);

    initializeSettings();

    connect(&m_timer, &QTimer::timeout, this, &IqScreenSpy::onTimeout);
}

void IqScreenSpy::start()
{
    m_screenGeometry = QString("%0x%1")
            .arg(QApplication::desktop()->screenGeometry().width())
            .arg(QApplication::desktop()->screenGeometry().height());

    startNewRecords();
    m_timer.start();
}

void IqScreenSpy::onTimeout()
{
    QTime currtentTime = QTime::currentTime();
    int recordDurationInMinutes = RECORD_DURATION_MINUTES;
    if (currtentTime.second() != 0 || currtentTime.minute() % recordDurationInMinutes != 0)
        return;

    startNewRecords();
}

void IqScreenSpy::initializeSettings()
{
    QSettings settings;
    if (!settings.contains(SETTING_GENERAL_OUTPUT_DIR))
        settings.setValue(SETTING_GENERAL_OUTPUT_DIR, "~");

    settings.beginGroup(SETTING_GROUP_FFMPEG);
    if (!settings.contains(SETTING_FFMPEG_FFMPEG_BIN_PATH))
        settings.setValue(SETTING_FFMPEG_FFMPEG_BIN_PATH, "/opt/iq/bin/ffmpeg");
    if (!settings.contains(SETTING_FFMPEG_FFMPEG_EXTRA_LD_LIBRARYPATH))
        settings.setValue(SETTING_FFMPEG_FFMPEG_EXTRA_LD_LIBRARYPATH, "/opt/csw/X11/lib");
    if (!settings.contains(SETTING_FFMPEG_MAXIMUM_FPS))
        settings.setValue(SETTING_FFMPEG_MAXIMUM_FPS, 4);
    if (!settings.contains(SETTING_FFMPEG_MAXIMUM_THREADS))
        settings.setValue(SETTING_FFMPEG_MAXIMUM_THREADS, 2);
    if (!settings.contains(SETTING_FFMPEG_VCODEC_PARAMS))
        settings.setValue(SETTING_FFMPEG_VCODEC_PARAMS, "libx264 -crf 18 -tune stillimage");
    if (!settings.contains(SETTING_FFMPEG_OUTPUT_FILE_EXTENSION))
        settings.setValue(SETTING_FFMPEG_OUTPUT_FILE_EXTENSION, ".avi");
    settings.endGroup();

    settings.beginGroup(SETTING_GROUP_ROTATE);
    if (!settings.contains(SETTING_ROTATE_STORAGE_TIME))
        settings.setValue(SETTING_ROTATE_STORAGE_TIME, (qint64)2592000000);
    settings.endGroup();

    settings.beginGroup(SETTING_GROUP_ARCHIVE);
    if (!settings.contains(SETTING_ARCHIVE_UNCOMPRESSED_STORAGE_TIME))
        settings.setValue(SETTING_ARCHIVE_UNCOMPRESSED_STORAGE_TIME, (qint64)86400000);
    if (!settings.contains(SETTING_ARCHIVE_COMMPRESS_ENABLED))
        settings.setValue(SETTING_ARCHIVE_COMMPRESS_ENABLED, true);
    if (!settings.contains(SETTING_ARCHIVE_TAR_BIN_PATH))
        settings.setValue(SETTING_ARCHIVE_TAR_BIN_PATH, "/opt/csw/bin/gtar");
    settings.endGroup();
}

void IqScreenSpy::startNewRecords()
{
    QSettings settings;
    QString outputDir = settings.value(SETTING_GENERAL_OUTPUT_DIR).toString();

    settings.beginGroup(SETTING_GROUP_FFMPEG);
    QString binPath = settings.value(SETTING_FFMPEG_FFMPEG_BIN_PATH).toString();
    QString extraLibraryPath = settings.value(SETTING_FFMPEG_FFMPEG_EXTRA_LD_LIBRARYPATH).toString();
    int ffmpegFps = settings.value(SETTING_FFMPEG_MAXIMUM_FPS, -1).toInt();
    int ffmpegThreads = settings.value(SETTING_FFMPEG_MAXIMUM_THREADS, -1).toInt();
    QString ffmpegVcodec = settings.value(SETTING_FFMPEG_VCODEC_PARAMS).toString();
    QString outputFileExtension = settings.value(SETTING_FFMPEG_OUTPUT_FILE_EXTENSION).toString();
    settings.endGroup();

    int recordDurationInMinutes = RECORD_DURATION_MINUTES;

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QDateTime nextDateTime = currentDateTime;
    if (currentDateTime.time().minute() >= 50) {
        nextDateTime = nextDateTime.addSecs(3600);
        nextDateTime.setTime(QTime(nextDateTime.time().hour(), 0, 0));
    }
    else {
        int nextMinute = currentDateTime.time().minute() + recordDurationInMinutes - currentDateTime.time().minute()%recordDurationInMinutes;
        nextDateTime.setTime(QTime(nextDateTime.time().hour(), nextMinute, 0));
    }
    qint64 duration = currentDateTime.msecsTo(nextDateTime);

    QProcessEnvironment ffmpegEnvironment = QProcessEnvironment::systemEnvironment();
    if (!extraLibraryPath.isEmpty()) {
        ffmpegEnvironment.insert("LD_LIBRARY_PATH", ffmpegEnvironment.value("LD_LIBRARY_PATH") + ":" + extraLibraryPath);
    }

    QSettings customSettings (QApplication::organizationName(), CUSTOM_SETTINGS_FILE);
    int screen = customSettings.value(SETTING_SCREEN, 0);

    IqFfmpegProcess *ffmpeg = new IqFfmpegProcess(this);
    ffmpeg->setOutputDir(outputDir);
    ffmpeg->setOutputFileExtension(outputFileExtension);
    ffmpeg->setBinPath(binPath);
    ffmpeg->setProcessEnvironment(ffmpegEnvironment);
    ffmpeg->setFps(ffmpegFps);
    ffmpeg->setThreads(ffmpegThreads);
    ffmpeg->setVcodeParam(ffmpegVcodec);
    ffmpeg->setScreenGeometry(m_screenGeometry);
    ffmpeg->setScreen(screen);
    ffmpeg->start(duration);

    connect(ffmpeg, &IqFfmpegProcess::finished, ffmpeg, &QObject::deleteLater);

    rotate();
    archive();
}

void IqScreenSpy::rotate()
{
    QSettings settings;
    settings.beginGroup(SETTING_GROUP_ROTATE);
    qint64 storageTime = settings.value(SETTING_ROTATE_STORAGE_TIME).toLongLong();
    settings.endGroup();

    foreach (const QString &filePath, findOldFile(storageTime)) {
        if (QFile::remove(filePath)) {
            qDebug() << "Remove " << filePath;
        } else {
            qWarning() << "Error on remove " << filePath;
        }
    }
}

void IqScreenSpy::archive()
{
    QSettings settings;
    settings.beginGroup(SETTING_GROUP_ARCHIVE);
    bool useCompress = settings.value(SETTING_ARCHIVE_COMMPRESS_ENABLED).toBool();
    qint64 uncompressStorageTime = settings.value(SETTING_ARCHIVE_UNCOMPRESSED_STORAGE_TIME).toLongLong();
    settings.endGroup();

    if (!useCompress)
        return;

    if (m_archiveInProcess)
        return;
    m_archiveInProcess = true;
    m_filestToArchive = findOldFile(uncompressStorageTime);
    archiveNext();
}

void IqScreenSpy::archiveNext()
{
    if (m_filestToArchive.isEmpty()) {
        m_archiveInProcess = false;
        return;
    }

    QString nextFile = m_filestToArchive.first();
    m_filestToArchive.removeFirst();

    if (nextFile.right(7) == ".tar.gz") {
        archiveNext();
        return;
    }

    QSettings settings;
    settings.beginGroup(SETTING_GROUP_ARCHIVE);
    QString tarBinPath = settings.value(SETTING_ARCHIVE_TAR_BIN_PATH).toString();
    settings.endGroup();

    IqTarProcess *tarProcess = new IqTarProcess(this);
    tarProcess->setBinPath(tarBinPath);
    connect(tarProcess, &IqTarProcess::finished, this, &IqScreenSpy::archiveNext);
    tarProcess->start(nextFile);
}

QStringList IqScreenSpy::findOldFile(qint64 time) const
{
    QStringList result;

    QSettings settings;
    QString outputDirStr = settings.value(SETTING_GENERAL_OUTPUT_DIR).toString();

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QDir outputDir (outputDirStr);
    QFileInfoList outputDirEntry = outputDir.entryInfoList();
    QRegExp rx ("(.*)_(\\d\\d\\.\\d\\d\\.\\d\\d\\d\\d)_(\\d\\d\\.\\d\\d)-(\\d\\d\\.\\d\\d\\.\\d\\d\\d\\d)_(\\d\\d\\.\\d\\d)[\\.\\w]+");
    foreach (const QFileInfo &file, outputDirEntry) {
        if (rx.indexIn(file.fileName()) == -1)
            continue;
        QStringList filePrs = rx.capturedTexts();
        if (filePrs.size() != 6)
            continue;
        QDateTime fileDateTime = QDateTime::fromString(filePrs[2] + " " + filePrs[3], "dd.MM.yyyy hh.mm");

        if (fileDateTime.msecsTo(currentDateTime) > time) {
            result << file.filePath();
        }
    }

    return result;
}
