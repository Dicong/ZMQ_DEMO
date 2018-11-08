#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>

namespace Ui {
class SettingWidget;
}

class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWidget(QWidget *parent = nullptr);
    ~SettingWidget();
    void setWidget(QString host, int port, int timeout);

signals:
    void signalCloseWidget(QString host, int port, int timeout, bool flag);

private slots:
    void onSetAccepted();
    void onSetRejected();

private:
    Ui::SettingWidget *ui;
};

#endif // SETTINGWIDGET_H
