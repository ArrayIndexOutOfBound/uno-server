#ifndef MYSERVER_H
#define MYSERVER_H
#include <QTcpServer>
#include <QObject>
#include <QDebug>
#include <mythread.h>
#include "unocard.h"


class MyServer : public QTcpServer
{
    Q_OBJECT

public:
    MyServer(QObject *parent=nullptr);

    unocard Deck[108];
    unocard Defausse[108];
    unocard LastPlayed;
    QVector<unocard> Joueur1;
    QVector<unocard> Joueur2;
    int whereDeck=107; // Track le deck
    int whereDefausse=0; // Track la defausse
    void Shuffle();
    bool whichturn; // false pour J1, true pour J2  
    QVector<MyThread*> clients;

    //Start the server
        void StartServer();
        void sendDataToAll(const QByteArray, MyThread *fromClient);
        void sendDataToClient(int idCarte, int id, QString action);
        void sendDataToAllFromLine(QByteArray);


      signals:
        void emitDataReceivedFromMyServer(const QByteArray, MyThread *fromClient);
        void emitSendMessageFromMyServer(QByteArray);

      public slots:
        void slotDataReceivedFromThread(QByteArray, MyThread *fromClient);
        void slotRemoveClient(MyThread* thisThread);
        void slotSendDataToClient(int idCarte, int id, QString action);
        void slotSendDataToAllFromLine(QByteArray);

      protected:
         void incomingConnection(qintptr);   //This is where we deal with incoming connections

      private:
       qintptr socketDescriptor;

};

#endif // MYSERVER_H
