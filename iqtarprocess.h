#ifndef IQTARPROCESS_H
#define IQTARPROCESS_H

#include <QObject>
#include <QProcess>

class IqTarProcess : public QObject
{
    Q_OBJECT
public:
    explicit IqTarProcess(QObject *parent = 0);

    void setProcessEnvironment(const QProcessEnvironment &environment);

    void start(const QString &filePath);

signals:
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

public:
    QString binPath() const;
    void setBinPath(const QString &binPath);

private:
    void removeUncompressedFile(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess m_tar;
    QString m_binPath;
    QString m_filePath;
};

#endif // IQTARPROCESS_H
