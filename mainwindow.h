#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

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
    void onParseButtonClicked();
    void slotCloseWidget(QString host, int port, int timeout, bool flag);
    void slotReceiveFrame(QByteArray recv);
    void slotTimeOut(int times);
    void slotSendStoped();

private:
    void setLabel();
    void sendFrame();
    JSONPRS parseIntoJson(QString& str);

private:
    Ui::MainWindow *ui;
    SettingWidget*          m_SettingWidget;
    Messenger*              m_Messenger;
    QThread*                m_ZmqThread;
};

#endif // MAINWINDOW_H
