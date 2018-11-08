#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "settingwidget.h"
#include "messenger.h"

namespace Ui {
class MainWindow;
}

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
    void slotCloseWidget(QString host, int port, int timeout, bool flag);
    void slotReceiveFrame(QByteArray recv);
    void slotTimeOut(int times);
    void slotSendStoped();

private:
    void setLabel();
    void sendFrame();

private:
    Ui::MainWindow *ui;
    SettingWidget*          m_SettingWidget;
    Messenger*              m_Messenger;
    QThread*                m_ZmqThread;
};

#endif // MAINWINDOW_H
