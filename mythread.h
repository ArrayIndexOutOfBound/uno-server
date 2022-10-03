#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>
#include <QTcpSocket>
#include <QDebug>

static unsigned short nombreDeThread(0) ;

class MyThread : public QThread
{
    Q_OBJECT

public:
  explicit MyThread(qintptr ID, QObject *parent = nullptr);        //We added an ID to the constructor (For the socket ID number)

  //Just because it's a thread we will ad a run function. This will run when the socket connects.
  //Basically we're overiding the QThread run function
  void run() override;
  static unsigned short getNombreDeThread();
  QString getSocketAdresse();
  void writeData(QByteArray);
  QTcpSocket *socket;

signals:
  void error(QTcpSocket::SocketError socketerror);    //If something goes wrong we'll emit that
  void emitDataReceivedFromThread(QByteArray Data, MyThread* fromClient);
  void emitRemoveClient(MyThread*);

public slots:
  void readyRead();
  void disconnected();

private:
   qintptr socketDescriptor;
};

#endif // MYTHREAD_H
