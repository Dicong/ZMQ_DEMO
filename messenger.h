#ifndef MESSENGER_H
#define MESSENGER_H

#include <QObject>
#include <QMutex>

enum WhenIsItOver
{
    GO_ON,
    OVER,
};

class Messenger : public QObject
{
    Q_OBJECT
public:
    explicit Messenger(QObject *parent = nullptr);
    ~Messenger();

    QString getZmqHost();
    int getZmqPort();
    int getZmqTimeout();
    void stopSend();

signals:
    void signalReceiveFrame(QByteArray recv);
    void signalTimeOut(int times);
    void signalSendStoped();

public slots:
    void sendRequest2Server(const QString& para1, const QString& para2, const QString& para3);
    void setZmqSocket(QString host, int port, int timeout);

private:
    void initZmqSocket();

private:
    void*                  mContex;
    void*                  mReq;
    QString                mZmqHost;
    int                    mZmqPort;
    QMutex                 mZmqMutex;
    int                    m_RequestTimeOut;
    WhenIsItOver          mOver;
};

#endif // MESSENGER_H
