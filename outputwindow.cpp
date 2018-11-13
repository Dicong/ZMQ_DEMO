//#include <QCloseEvent>
#include "outputwindow.h"
#include "ui_outputwindow.h"

OutputWindow::OutputWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputWindow)
{
    ui->setupUi(this);
    connect(ui->exportButton, SIGNAL(clicked()), this, SIGNAL(signalExportButtonClicked()));
}

OutputWindow::~OutputWindow()
{
    delete ui;
}

void OutputWindow::closeEvent(QCloseEvent *event)
{
    emit signalWindowClosed();
}

void OutputWindow::slotOutputChanged(QString outputStr)
{
    ui->outputBrowser->setPlainText(outputStr);
}
