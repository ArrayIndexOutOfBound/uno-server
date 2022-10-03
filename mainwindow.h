#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "myserver.h"
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void slotDataReceivedFromMyServer(const QByteArray Data, MyThread * source);

signals:
  void emitSendDataToClient(int idCarte, int id, QString action);
  void emitSendDataToAllFromLine(QByteArray);

private slots:
    void on_pushButton_clicked();

private:

    Ui::MainWindow *ui;
    MyServer *server;

};
#endif // MAINWINDOW_H
