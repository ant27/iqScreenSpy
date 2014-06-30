#ifndef IQFFMPEGPROCESS_H
#define IQFFMPEGPROCESS_H

#include <QObject>
#include <QProcess>
#include <QTimer>

class IQFFmpegProcess : public QObject
{
    Q_OBJECT
public:
    explicit IQFFmpegProcess(QObject *parent = 0);

    void setProcessEnvironment(const QProcessEnvironment &environment);

    void setFFmpegBinString(const QString &ffmpegBinStr);

    void setOutputDir(const QString &dir);

signals:
    void finished(const int code);
    
public slots:
    void start(const QString &hostName, const int height, const int width, const qint64 duration);

private slots:
    void stop();
    
private:
    static const int _finishedInterval = 30000;
    static const int _lapping = 30000;

    QProcess _ffmpeg;
    QString _ffmpegBinString;
    QString _outputDir;
    QTimer _finishedTimer;

    QString _host;
    QString _fileName;
};

#endif // IQFFMPEGPROCESS_H
