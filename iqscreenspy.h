#ifndef IQSCREENSPY_H
#define IQSCREENSPY_H

#include <QObject>
#include <QTimer>

class IQScreenSpy : public QObject
{
    Q_OBJECT
public:
    explicit IQScreenSpy(QObject *parent = 0);
        
public slots:
    void start();

private slots:
    void onTimeout();

private:
    void startNewRecords();
    void clearOld();

    QTimer _timer;
};
#endif // IQSCREENSPY_H
