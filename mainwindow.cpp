#include <QDebug>
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QMessageBox>
#include <QJsonObject>
#include <QThread>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingwidget.h"
#include "messenger.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_SettingWidget(NULL),
    m_Messenger(new Messenger),
    m_ZmqThread(new QThread),
    m_RecvModel(new QStandardItemModel)
{
    ui->setupUi(this);
    setLabel();
    ui->outputBox->setVisible(false);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    m_Messenger->moveToThread(m_ZmqThread);
    connect(this, SIGNAL(signalSendRequest2Server(QString,QString,QString)), m_Messenger, SLOT(sendRequest2Server(QString,QString,QString)));
    connect(m_Messenger, SIGNAL(signalReceiveFrame(QByteArray)), this, SLOT(slotReceiveFrame(QByteArray)));
    connect(m_Messenger, SIGNAL(signalTimeOut(int)), this, SLOT(slotTimeOut(int)));
    connect(m_Messenger, SIGNAL(signalSendStoped()), this, SLOT(slotSendStoped()));
    m_ZmqThread->start();
}

MainWindow::~MainWindow()
{
    m_ZmqThread->quit();
    m_ZmqThread->wait();
    delete ui;
}

void MainWindow::setLabel()
{
    QString setting = tr("Zmq Host: %1\n").arg(m_Messenger->getZmqHost());
    setting += tr("Zmq Port: %1\n").arg(m_Messenger->getZmqPort());
    setting += tr("Request Time Out: %1ms").arg(m_Messenger->getZmqTimeout());
    ui->settingLabel->setText(setting);
}

JSONPRS MainWindow::parseIntoJson(QString& str)
{
    QStringList allList = str.split('\n');
    if (str.startsWith('{'))
    {
        str.remove('\\');
        return PRS_SUCCESS;
    }
    QJsonObject obj;
    foreach (QString everyStr, allList)
    {
        QStringList everyList = everyStr.split(' ');
        if (everyList.count() != 2)
            return PRS_WRGFORMAT;
        char flag = everyList.value(1).at(0).toLatin1();
        switch(flag)
        {
        case 's':
            obj.insert(everyList.at(0), everyList.value(1).remove(0, 1));
            break;
        case 'i':
            obj.insert(everyList.at(0), everyList.value(1).remove(0, 1).toInt());
            break;
        case 'l':
            obj.insert(everyList.at(0), everyList.value(1).remove(0, 1).toLongLong());
            break;
        case 'd':
            obj.insert(everyList.at(0), everyList.value(1).remove(0, 1).toDouble());
            break;
        default:
            return PRS_NOFLAG;
        }
    }
    QJsonDocument doc;
    doc.setObject(obj);
    str = QVariant(doc.toJson()).toString();
    return PRS_SUCCESS;
}

bool MainWindow::parseFromConsole(QString str, QStringList& list)
{
    str.remove('\\');
    if (str.startsWith("para: "))
        str.remove(0, 6);
    list = str.split(' ');
    if (list.count() != 3)
        return false;
    for (int i=0; i<3; i++)
    {
        list[i].remove(0, 1);
        list[i].remove(list[i].length()-1, 1);
    }
    return true;
}

void MainWindow::clearAll()
{
    ui->firstEdit->clear();
    ui->secondEdit->clear();
    ui->thirdEdit->clear();
    ui->recvBrowser->clear();
}

void MainWindow::onSendButtonClicked()
{
    if (ui->sendButton->text() == tr("Send"))
    {
        ui->recvBrowser->clear();
        QString msg, result;
        sendFrame();
        ui->sendButton->setText(tr("Cancel"));
    }
    else
    {
        m_Messenger->stopSend();
    }
}

void MainWindow::onSetting()
{
    if (m_SettingWidget == NULL)
        m_SettingWidget = new SettingWidget;
    m_SettingWidget->setWidget(m_Messenger->getZmqHost(), m_Messenger->getZmqPort(), m_Messenger->getZmqTimeout());
    connect(m_SettingWidget, SIGNAL(signalCloseWidget(QString,int,int,bool)), this, SLOT(slotCloseWidget(QString,int,int,bool)));
    m_SettingWidget->show();
}

void MainWindow::slotCloseWidget(QString host, int port, int timeout, bool flag)
{
    if (flag)
    {
        m_Messenger->setZmqSocket(host, port, timeout);
        ui->statusBar->showMessage(tr("Setting has been successful."));
        setLabel();
    }
    else
        ui->statusBar->showMessage(tr("Setting is canceled."));

    if (m_SettingWidget)
    {
        delete m_SettingWidget;
        m_SettingWidget = NULL;
    }
}

void MainWindow::sendFrame()
{
    QString para1 = ui->firstEdit->toPlainText().trimmed();
    QString para2 = ui->secondEdit->toPlainText();
    QString para3 = ui->thirdEdit->toPlainText().trimmed();

    emit signalSendRequest2Server(para1, para2, para3);
}

void MainWindow::slotReceiveFrame(QByteArray recv)
{
    int tabCount = 0;
    QString result, msg;
    result = QVariant(recv).toString();

    //解析Json
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(recv, &err);
    if (err.error != QJsonParseError::NoError)
    {
        qDebug() << "parse json error occured.";
        msg = tr("parse json error occured.");
        goto EndRecv;
    }
    msg = doc.object().value("msg").toString();

    //整理Json输出格式
    result.replace('[', "\n[\n").replace('{', "{\n").replace(',', ",\n").replace('}', "\n}").replace(']', "\n]");
    for (int pos=0; pos<result.length(); pos++)
    {
        if (result.at(pos) == '[' || result.at(pos) == '{')
            tabCount++;
        else if (result.at(pos) == ']' || result.at(pos) == '}')
        {
            tabCount--;
            pos -= 4;
            result.remove(pos, 4);
        }
        if (result.at(pos) == '\n')
            for (int i=0; i<tabCount; i++)
            {
                result.insert(++pos, "    ");
                pos += 3;
            }
    }

EndRecv:
    ui->sendButton->setText(tr("Send"));
    ui->recvBrowser->setText(result);
    ui->statusBar->showMessage(msg);
    if (!ui->outputBox->isVisible())
    {
        ui->outputBox->setVisible(true);
        ui->viewButton->setChecked(true);
    }
}

void MainWindow::slotTimeOut(int times)
{
    ui->statusBar->showMessage(tr("Connecting...It has timed out %1 times.").arg(times));
}

void MainWindow::slotSendStoped()
{
    ui->sendButton->setText(tr("Send"));
    ui->statusBar->showMessage(tr("Cancel success."));
}

void MainWindow::onParse2JsonButtonClicked()
{
    QString str = ui->secondEdit->toPlainText();
    QString info = tr("Already in JSON format.");
    JSONPRS ret = parseIntoJson(str);
    if (ret != PRS_SUCCESS)
    {
        if (ret == PRS_NOFLAG)
            info = tr("Parameter requires format flag.");
        else
            info = tr("Wrong format.");
        QMessageBox::warning(this, tr("Warning"), info, QMessageBox::NoButton);
        return;
    }
    ui->secondEdit->setPlainText(str);
}

void MainWindow::onClearButtonClicked()
{
    clearAll();
}

void MainWindow::onParseFromConsoleButtonClicked()
{
    QStringList list;
    QString str = ui->secondEdit->toPlainText();
    if (parseFromConsole(str, list))
    {
        ui->firstEdit->setPlainText(list[0]);
        ui->secondEdit->setPlainText(list[1]);
        ui->thirdEdit->setPlainText(list[2]);
    }
}
