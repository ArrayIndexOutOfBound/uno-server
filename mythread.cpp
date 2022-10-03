#include "mythread.h"
#include "mainwindow.h"

MyThread::MyThread(qintptr ID, QObject *parent):
  QThread(parent)
{
    socket = new QTcpSocket(this);
    socket->setSocketOption(QAbstractSocket::LowDelayOption,1);
    this->socketDescriptor = ID;            // Get the socket ID number
    nombreDeThread++ ;
    qDebug()<< "creation du thread "+QString::number(ID)+" ....\n";


}


void MyThread::run()
{
  //thread starts here
  qDebug()<< "lancement d'un thread....\n"<<this->socketDescriptor;



  if ( !socket->setSocketDescriptor(this->socketDescriptor))   //Here we set the socket ID
  {
    emit error (socket->error());         //emit the error signal
    emit emitDataReceivedFromThread("error", this);
    qDebug()<< "thread ID : "+QString::number(this->socketDescriptor)+" error";
    return;
  }
  qDebug()<< "thread: "+QString::number(this->socketDescriptor)+" demarré";
  QString texte = "nombre de client : " + QString::number(nombreDeThread);
  // emit emitDataReceivedFromThread(texte.toLatin1(),this);

  connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()),Qt::DirectConnection  );  //Make a auto connection to the thread
  connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));


  //qDebug()<< "emitDataReceivedFromThread: "+QString::number(nombreDeThread)+" demarré \n";
  //emit emitDataReceivedFromThread("New Thread  demarré....\n", this);
  //client is connected
  //writeData("Bienvenue sur mon serveur .... !\n");


  //IMPORTANT
  //This function will cause the thread to stay alive until we tell it to close
  //Otherwise the run() function will end and the thread will be dropped / destroyed

  exec();

}

unsigned short MyThread::getNombreDeThread()
{
    return nombreDeThread;
}

QString MyThread::getSocketAdresse()
{
    return socket->peerAddress().toString();
}

void MyThread::writeData(QByteArray Data)
{
    QByteArray data = Data;
    qDebug()<< "writeData: " <<QString(data) << " call in thread";
    socket->write(data);
    socket->waitForBytesWritten();
    msleep(100);

}

void MyThread::readyRead() // C'est ici le debut de la reception de données
{
  //QByteArray Data = socket->readAll();    // Get all the information from the connected client
  QByteArray Data = socket->readLine();
  QString texte = QString(Data);
  emit emitDataReceivedFromThread(texte.toLatin1(), this);
  //Send the info back, (echo server)
  //writeData(Data);
}


void MyThread::disconnected()
{
  emit emitRemoveClient(this);
  socket->deleteLater();
  nombreDeThread--;
  QString texte = "nombre de client : " + QString::number(nombreDeThread);
  emit emitDataReceivedFromThread(texte.toLatin1(), this); // Balance a myserver ou mainwindow (a check) le nombre de clients
  //exit(0);
}

