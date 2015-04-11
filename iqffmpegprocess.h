#ifndef IQFFMPEGPROCESS_H
#define IQFFMPEGPROCESS_H

#include <QObject>
#include <QProcess>
#include <QTimer>

class IqFfmpegProcess : public QObject
{
    Q_OBJECT
public:
    explicit IqFfmpegProcess(QObject *parent = 0);

    void setProcessEnvironment(const QProcessEnvironment &environment);

    void start(const qint64 duration);

signals:
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

public:
    QString outputDir() const;
    void setOutputDir(const QString &dir);

    QString outputFileExtension() const;
    void setOutputFileExtension(const QString &outputFileExtension);

    int fps() const;
    void setFps(int fps);

    QString vcodeParam() const;
    void setVcodeParam(const QString &vcodeParam);

    int threads() const;
    void setThreads(int threads);

    QString binPath() const;
    void setBinPath(const QString &binPath);

    QString screenGeometry() const;
    void setScreenGeometry(const QString &screenGeometry);

private:
    void stop();
    
private:
    static const int m_finishedInterval = 30000;
    static const int m_lapping = 30000;

    QString m_binPath;
    QString m_outputDir;
    QString m_vcodeParam;
    QString m_outputFileExtension;
    QString m_screenGeometry;
    int m_fps;
    int m_threads;

    QProcess m_ffmpeg;
    QTimer m_finishedTimer;

    QString m_fileName;
};

#endif // IQFFMPEGPROCESS_H
