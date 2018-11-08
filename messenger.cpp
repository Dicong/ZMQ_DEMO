#include <QDebug>
#include <QElapsedTimer>

#include "messenger.h"
#include "zmq.hpp"
#include "zmq_utils.h"

Messenger::Messenger(QObject *parent)
    : QObject(parent)
    , mContex(NULL)
    , mReq(NULL)
    , mZmqHost("192.168.11.218")
    , mZmqPort(5858)
    , m_RequestTimeOut(5000)
{
    mContex = zmq_ctx_new();
    initZmqSocket();
}

Messenger::~Messenger()
{
    try
    {
        zmq_close(mReq);
        zmq_ctx_destroy(mContex);
    }
    catch (zmq::error_t e)
    {
        qWarning() << e.what();
    }
}

QString Messenger::getZmqHost()
{
    return mZmqHost;
}

int Messenger::getZmqPort()
{
    return mZmqPort;
}

int Messenger::getZmqTimeout()
{
    return m_RequestTimeOut;
}

void Messenger::initZmqSocket()
{
    mReq = zmq_socket(mContex, ZMQ_REQ);
    zmq_setsockopt(mReq, ZMQ_RCVTIMEO, &m_RequestTimeOut, sizeof(m_RequestTimeOut));
    QString url = QString("tcp://%1:%2").arg(mZmqHost).arg(mZmqPort);
    qDebug() << "zmq dbdataservice url:" << url;

    zmq_connect(mReq, url.toUtf8().data());
}

void Messenger::setZmqSocket(QString host, int port, int timeout)
{
    QString url = QString("tcp://%1:%2").arg(mZmqHost).arg(mZmqPort);
    zmq_disconnect(mReq, url.toUtf8().data());

    zmq_setsockopt(mReq, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    url = QString("tcp://%1:%2").arg(host).arg(port);
    zmq_connect(mReq, url.toUtf8().data());

    mZmqHost = host;
    mZmqPort = port;
    m_RequestTimeOut = timeout;
}

void Messenger::sendRequest2Server(const QString& para1, const QString& para2, const QString& para3)
{
    mOver = GO_ON;

    QSharedPointer<QByteArray> recvData(new QByteArray);

    //加锁, 避免多个请求相互干扰
    QMutexLocker locker(&mZmqMutex);
    qDebug() << "para:" << para1 << para2 << para3;

    QByteArray t_para1 = para1.toUtf8();
    QByteArray t_para2 = para2.toUtf8();
    QByteArray t_para3 = para3.toUtf8();

    //接收reply
    zmq_msg_t reply;
    zmq_msg_init(&reply);

    int expiredTimes = 0;
    int ret = -1;
    do
    {
        if (mOver == OVER)
        {
            emit signalSendStoped();
            return;
        }
        QElapsedTimer timer;
        timer.start();

        zmq_send(mReq, t_para1.data(), t_para1.length(), ZMQ_SNDMORE);
        zmq_send(mReq, t_para2.data(), t_para2.length(), ZMQ_SNDMORE);
        zmq_send(mReq, t_para3.data(), t_para3.length(), 0);

        ret = zmq_recvmsg(mReq, &reply, 0);
        qDebug() << t_para1 << "请求时间 " << timer.elapsed();
        emit signalTimeOut(expiredTimes);
        if (expiredTimes > 0)
        {
            qDebug() << t_para1 << "超时连接 " << expiredTimes << "次";
//
        }
        expiredTimes++;
    }
    while (ret == -1);

    int size = zmq_msg_size(&reply);
    recvData->append((char*)zmq_msg_data(&reply), size);
    zmq_msg_close(&reply);
    qDebug() << QString::fromUtf8(*recvData);

    emit signalReceiveFrame(*recvData.data());
}

void Messenger::stopSend()
{
    mOver = OVER;
}
