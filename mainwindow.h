#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QStandardItemModel>
#include <QQueue>
#include <QDateTime>

#include "settingwidget.h"
#include "messenger.h"

namespace Ui {
class MainWindow;
}

enum JSONPRS
{
    PRS_SUCCESS,
    PRS_WRGFORMAT,
    PRS_NOFLAG,
};

struct SubRecv
{
    int         topic;
    QDateTime   time;
    int         size;
    QByteArray  data;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void signalSendRequest2Server(const QString& para1, const QString& para2, const QString& para3);

private slots:
    void onSendButtonClicked();
    void onSetting();
    void onParse2JsonButtonClicked();
    void onParseFromConsoleButtonClicked();
    void onClearButtonClicked();
    void slotCloseWidget(QString host, int port, int timeout, bool flag);
    void slotReceiveFrame(QByteArray recv);
    void slotTimeOut(int times);
    void slotSendStoped();

private:
    void setLabel();
    void sendFrame();
    void clearAll();
    JSONPRS parseIntoJson(QString& str);
    bool parseFromConsole(QString str, QStringList& list);

private:
    Ui::MainWindow *ui;
    SettingWidget*          m_SettingWidget;
    Messenger*              m_Messenger;
    QThread*                m_ZmqThread;
    QStandardItemModel*     m_RecvModel;
    QQueue<SubRecv>         m_RecvQueue;
};

#endif // MAINWINDOW_H
