#include <QPushButton>

#include "settingwidget.h"
#include "ui_settingwidget.h"

SettingWidget::SettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidget)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
}

SettingWidget::~SettingWidget()
{
    delete ui;
}

void SettingWidget::setWidget(QString host, int port, int timeout)
{
    ui->hostEdit->setText(host);
    ui->portEdit->setText(QString::number(port));
    ui->timeoutEdit->setText(QString::number(timeout));
}

void SettingWidget::onSetAccepted()
{
    QString host = ui->hostEdit->text().trimmed();
    int port = ui->portEdit->text().trimmed().toInt();
    int timeout = ui->timeoutEdit->text().trimmed().toInt();
    emit signalCloseWidget(host, port, timeout, true);
}

void SettingWidget::onSetRejected()
{
    emit signalCloseWidget("", 0, 0, false);
}
