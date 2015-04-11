#ifndef IQSCREENSPY_H
#define IQSCREENSPY_H

#include <QObject>
#include <QTimer>
#include <QStringList>

class IqScreenSpy : public QObject
{
    Q_OBJECT
public:
    explicit IqScreenSpy(QObject *parent = 0);
        
public slots:
    void start();

private:
    void onTimeout();
    void initializeSettings();
    void startNewRecords();
    void rotate();
    void archive();
    void archiveNext();
    QStringList findOldFile(qint64 time) const;

private:
    QTimer m_timer;
    bool m_archiveInProcess;
    QStringList m_filestToArchive;
    QString m_screenGeometry;
};
#endif // IQSCREENSPY_H
