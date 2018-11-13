#ifndef OUTPUTWINDOW_H
#define OUTPUTWINDOW_H

#include <QWidget>
#include <QCloseEvent>

namespace Ui {
class OutputWindow;
}

class OutputWindow : public QWidget
{
    Q_OBJECT

public:
    explicit OutputWindow(QWidget *parent = nullptr);
    ~OutputWindow();

public slots:
    void slotOutputChanged(QString outputStr);

signals:
    void signalWindowClosed();
    void signalExportButtonClicked();

private:
    void closeEvent(QCloseEvent *event);
    Ui::OutputWindow *ui;
};

#endif // OUTPUTWINDOW_H
