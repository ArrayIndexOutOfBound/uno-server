#include "myserver.h"

MyServer::MyServer(QObject *parent):
  QTcpServer(parent)
{

}

void MyServer::StartServer()
{
    if (!listen(QHostAddress::Any,2222))
    {
        /*QMessageBox::critical(this, tr("Server"),
                           tr("Unable to start the server: %1.").arg(this->errorString()));*/
        qDebug()<< tr("Unable to start the server: %1.").arg(this->errorString());
    }
    else
    {
        //Server is now listening..
        qDebug()<< "MyServeur: Serveur demarré";
    }
    srand(std::time(nullptr)); // Roll un nombre entre 0 et 1 grace au temps interne
    whichturn=rand()%2;
    if (whichturn==false) qDebug() << "Le J1 va commencer";
    else qDebug() << "Le J2 va commencer";
}

void MyServer::sendDataToAll(const QByteArray data, MyThread* fromClient)
{
    qDebug() << "Nombre de clients: " << clients.size();
    qDebug() << "MyServer : SendToAll : " <<  data.toStdString().c_str() << " from  " << fromClient->getSocketAdresse();
    for(int i=0;i<clients.size();i++)
    {
        if(clients[i]->getSocketAdresse() != fromClient->getSocketAdresse())
        {
        qDebug() <<"to :" << clients[i]->getSocketAdresse();
        clients[i]->writeData(data);
        }
    }
}

void MyServer::sendDataToAllFromLine(QByteArray data)
{
    qDebug() << "Send Data To all : " << data.toStdString().c_str();
    for(int i=0;i<clients.size();i++)
    {
        qDebug() <<"Envoie à :" << clients[i]->getSocketAdresse();
        clients[i]->writeData(data);
    }
}

void MyServer::slotSendDataToAllFromLine(QByteArray data)
{
    sendDataToAllFromLine(data);
}

void MyServer::sendDataToClient(int idCarteRecu, int idReception, QString actionReception)
{
    qDebug() <<"Nombre de clients: " << clients.size();
    qDebug() << "JE VAIS ENVOYER DES DONNEES";
    qDebug() << "Taille du deck restant, avant toute chose : " << whereDeck;
    /*qDebug()<< "MyServer : SendToClient : " <<  data.toStdString().c_str() << " from Server ";
    QByteArray temp = ("From server : " + QString(data)).toLatin1();*/

    //clients[id-1]->writeData(temp); // LA LISTE DE CLIENT COMMENCE A 0

    qDebug() << "actionReception : " << actionReception;

    QString aenvoyer="";

    if (whereDeck==1) // On check que le deck ne soit pas vide
    {
        while (whereDefausse>1) // On reverifie le deck
        {
            Deck[whereDeck]=Defausse[whereDefausse];
            Deck[whereDeck].setupImage(false);
            whereDeck++;
            whereDefausse--;
        }
    }

    if(actionReception=="c") // C'est encore le tour du joueur qui a posé la carte (surement un +2 etc..)
    {
        qDebug() << "Boucle if 'c'";
        if (idReception==1)
        {
            clients[0]->wait(100);
            clients[1]->wait(100);
            if (whichturn==false) clients[0]->writeData("50");
            else clients[0]->writeData("51");
        }
        else
        {
            clients[0]->wait(100);
            clients[1]->wait(100);
            if (whichturn==true) clients[1]->writeData("51");
            else clients[1]->writeData("50");
        }
        return;
    }

    if (idReception==1 and actionReception=="x") // Cartes J1 debut game
    {
        qDebug() << "Boucle if 'x'";
        aenvoyer.append("d");
        aenvoyer.append(QString::number(Joueur1[idCarteRecu].couleur));
        aenvoyer.append(QString::number(Joueur1[idCarteRecu].nombre));
        QByteArray envoie = aenvoyer.toLatin1();
        clients[0]->writeData(envoie); // Envoie de la carte
        //clients[0]->msleep(100); Potentiellement necessaire, ou alors de flush coté client
        QString stronge;
        stronge.append("z");
        stronge.append(QString::number(LastPlayed.couleur));
        stronge.append(QString::number(LastPlayed.nombre));
        clients[0]->writeData(stronge.toLatin1()); // Envoie lastPlayed (ok 7 fois mais bon)
        clients[1]->writeData(QString("a").toLatin1());
        return;
    }
    if (idReception==2 and actionReception=="x") // Cartes J2 debut game
    {
        qDebug() << "Boucle if 'x'";
        aenvoyer.append("d");
        aenvoyer.append(QString::number(Joueur2[idCarteRecu].couleur));
        aenvoyer.append(QString::number(Joueur2[idCarteRecu].nombre));
        QByteArray envoie = aenvoyer.toLatin1();
        clients[1]->writeData(envoie); // Envoie de la carte
        QString stronge;
        stronge.append("z");
        stronge.append(QString::number(LastPlayed.couleur));
        stronge.append(QString::number(LastPlayed.nombre));
        clients[1]->writeData(stronge.toLatin1()); // Envoie lastPlayed (ok 7 fois mais bon)
        clients[0]->writeData(QString("a").toLatin1());
        return;
    }
    if ((idReception==1 or idReception==2) and actionReception=="a") // Quelqu'un veux piocher
    {
        qDebug() << "Boucle if 'a'";
        aenvoyer.append("d");
        aenvoyer.append(QString::number(Deck[whereDeck].couleur));
        aenvoyer.append(QString::number(Deck[whereDeck].nombre));
        if (idReception==1) // C'est le J1
        {
            Joueur1.push_back(Deck[whereDeck]);
            QByteArray envoie = aenvoyer.toLatin1();
            clients[0]->writeData(envoie);
            clients[1]->writeData(QString("a").toLatin1()); // Dire que l'autre pioche
        }
        if (idReception==2) // C'est le J2
        {
            QByteArray envoie = aenvoyer.toLatin1();
            Joueur2.push_back(Deck[whereDeck]);
            clients[1]->writeData(envoie);
            clients[0]->writeData(QString("a").toLatin1()); // Dire que l'autre pioche
        }
        whereDeck--;
        return;
    }
    if (idReception==1 and actionReception=="b") // J1 pose une carte avec ID
    {
        qDebug() << "Boucle if 'b' de J1";

        LastPlayed=Joueur1[idCarteRecu];
        Joueur1.remove(idCarteRecu);
        aenvoyer.append("b");
        QByteArray envoie = aenvoyer.toLatin1();
        clients[1]->writeData(envoie);

        if (LastPlayed.nombre==10) // +2
        {
            QString envoie;
            for(int i=0;i<2;i++) // J2 va piocher 2 fois
            {
                envoie="d";
                Joueur2.push_back(Deck[whereDeck]); // J2 pioche
                envoie.append(QString::number(Deck[whereDeck].couleur));
                envoie.append(QString::number(Deck[whereDeck].nombre));
                whereDeck--;
                clients[1]->writeData(envoie.toLatin1());
                clients[0]->writeData("50"); // C'est encore ton tour
                clients[0]->writeData(QString("a").toLatin1()); // Dire que l'autre pioche
                clients[1]->writeData("50"); // C'est pas ton tour, tu viens de piocher
            }
        }

        if (LastPlayed.nombre==13) // +4
        {
            QString envoie;
            for(int i=0;i<4;i++) // J2 va piocher 4 fois
            {
                envoie="d";
                Joueur2.push_back(Deck[whereDeck]); // J2 pioche
                envoie.append(QString::number(Deck[whereDeck].couleur));
                envoie.append(QString::number(Deck[whereDeck].nombre));
                whereDeck--;
                clients[1]->writeData(envoie.toLatin1());
                clients[0]->writeData("50"); // C'est encore ton tour
                clients[0]->writeData(QString("a").toLatin1()); // Dire que l'autre pioche
                clients[1]->writeData("50"); // C'est pas ton tour, tu viens de piocher
            }
        }

        if (LastPlayed.nombre==11 or LastPlayed.nombre==12) // Skip ou reverse
        {
            clients[0]->writeData("50"); // C'est encore ton tour
            clients[1]->writeData("50"); // C'est pas ton tour, tu viens de te faire bloquer
        }

        QString stronge; // On met a jour la dernière carte joué
        stronge.append("z");
        stronge.append(QString::number(LastPlayed.couleur));
        stronge.append(QString::number(LastPlayed.nombre));
        clients[1]->writeData(stronge.toLatin1());
        whereDefausse++;

        return;
    }
    if (idReception==2 and actionReception=="b") // J2 pose une carte avec ID
    {
        qDebug() << "Boucle if 'b' de J2";

        LastPlayed=Joueur2[idCarteRecu];
        Joueur2.remove(idCarteRecu);
        aenvoyer.append("b");
        QByteArray envoie = aenvoyer.toLatin1();
        clients[0]->writeData(envoie);

        if (LastPlayed.nombre==10) // +2
        {
            QString envoie;
            for(int i=0;i<2;i++)
            {
                envoie="d";
                Joueur1.push_back(Deck[whereDeck]); // J1 pioche
                envoie.append(QString::number(Deck[whereDeck].couleur));
                envoie.append(QString::number(Deck[whereDeck].nombre));
                whereDeck--;
                clients[0]->writeData(envoie.toLatin1());
                clients[0]->writeData("51"); // C'est pas ton tour, tu viens de piocher
                clients[1]->writeData("51"); // C'est encore ton tour
                clients[1]->writeData(QString("a").toLatin1()); // Dire que l'autre pioche
            }
        }
        if (LastPlayed.nombre==13) // +4
        {
            QString envoie;
            for(int i=0;i<4;i++)
            {
                envoie="d";
                Joueur1.push_back(Deck[whereDeck]); // J1 pioche
                envoie.append(QString::number(Deck[whereDeck].couleur));
                envoie.append(QString::number(Deck[whereDeck].nombre));
                whereDeck--;
                clients[0]->writeData(envoie.toLatin1());
                clients[0]->writeData("51"); // C'est pas ton tour, tu viens de piocher
                clients[1]->writeData("51"); // C'est encore ton tour
                clients[1]->writeData(QString("a").toLatin1()); // Dire que l'autre pioche
            }
        }

        if (LastPlayed.nombre==11 or LastPlayed.nombre==12) // Skip ou reverse
        {
            clients[0]->writeData("51"); // C'est pas ton tour, tu viens de te faire bloquer
            clients[1]->writeData("51"); // C'est encore ton tour
        }

        QString stronge; // Refresh lastPlayed
        stronge.append("z");
        stronge.append(QString::number(LastPlayed.couleur));
        stronge.append(QString::number(LastPlayed.nombre));
        clients[0]->writeData(stronge.toLatin1());
        whereDefausse++;

        return;
    }
    if (actionReception=="l") // On recois la couleur en cas de changement, envoie au deux la vérif
    {
        qDebug() << "Boucle if 'l'";
        LastPlayed.couleur=idCarteRecu;
        aenvoyer.append("w");
        aenvoyer.append(QString::number(LastPlayed.couleur));
        QByteArray envoie = aenvoyer.toLatin1();
        clients[0]->writeData(envoie); // Il faut qu'on previenne les deux joueurs du changement de couleur
        clients[1]->writeData(envoie);
        return;
    }
    if (actionReception=="i") // Envoie de l'id au clients se connectant
    {
        qDebug() << "Boucle if 'i'";
        aenvoyer = QString::number(clients.length())+QString::number(whichturn);
        qDebug() << aenvoyer;
        QByteArray envoie = aenvoyer.toLatin1();
        clients[clients.length()-1]->writeData(envoie);
        return;
    }

    /*
    LISTE DES ACTIONS DANS LA TRAME DE DONNEES, de ce qui sera envoyé, ce qui sera sur le 1er caractère, ce qui suis ce caractère est des infos en plus
    a = adverse pioche une carte
    b = adverse a posé une carte, info carte (0-9,1-4)
    c = adverse pose une carte ET tu joue pas (joker etc), en local, l'envoie ce fait après la choix de la couleur si besoin
    1=+2 / 2=+4 / 3=tour / 4=skip; 1-4 couleur si +4
    d = tu pioche cette carte (du c, un +2 ou +4, a repeter selon la quantité)

    w = changement de couleur lastplayed
    x = CARTE A PIOCHER DEBUT DE GAME
    y = fin de partie + qui gagne
    z = info defausse / derniere carte joué
    */ // A REMPLIR BROW

}

void MyServer::Shuffle()
{
    int compteurDeck=0;
    int couleurActuel=1;
    int nombreActuel=0;
    QString Couleur="R";

    while (couleurActuel<=4 and compteurDeck<=100)
    {
        if (compteurDeck==25 or compteurDeck==50 or compteurDeck==75)
        {
            if (Couleur=="B") Couleur="V";
            if (Couleur=="J") Couleur="B";
            if (Couleur=="R") Couleur="J";
            couleurActuel++;
            nombreActuel=0;
        }

        if (nombreActuel<13)
        {
            Deck[compteurDeck].couleur=couleurActuel;
            Deck[compteurDeck].nomCouleur=Couleur;
            Deck[compteurDeck].nombre=nombreActuel;
            //Deck[compteurDeck].cheminImage=":/Uno/"+Deck[compteurDeck].nomCouleur+QString::number(nombreActuel);
            if (Deck[compteurDeck].nombre==10) Deck[compteurDeck].IsA2=true;
            if (Deck[compteurDeck].nombre==11) Deck[compteurDeck].IsAReverse=true;
            if (Deck[compteurDeck].nombre==12) Deck[compteurDeck].IsASkip=true;
            //Deck[compteurDeck].label = new QLabel;
            //Deck[compteurDeck].setupImage(false);
        }
        else
        {
            Deck[compteurDeck].couleur=couleurActuel;
            Deck[compteurDeck].nomCouleur=Couleur;
            Deck[compteurDeck].nombre=nombreActuel-12;
            //Deck[compteurDeck].cheminImage=":/Uno/"+Deck[compteurDeck].nomCouleur+QString::number(nombreActuel-12);
            if (Deck[compteurDeck].nombre==10) Deck[compteurDeck].IsA2=true;
            if (Deck[compteurDeck].nombre==11) Deck[compteurDeck].IsAReverse=true;
            if (Deck[compteurDeck].nombre==12) Deck[compteurDeck].IsASkip=true;
            //Deck[compteurDeck].label = new QLabel;
            //Deck[compteurDeck].setupImage(false);
        }
        nombreActuel++;
        compteurDeck++;

    }
    compteurDeck--;
    while (compteurDeck<=107)
    {
        Couleur="N";
        couleurActuel=5;
        if (compteurDeck<=103)
        {
            Deck[compteurDeck].couleur=couleurActuel;
            Deck[compteurDeck].nomCouleur=Couleur;
            Deck[compteurDeck].nombre=13;
            Deck[compteurDeck].IsA4=true;
            //Deck[compteurDeck].cheminImage=":/Uno/N+4";
            //Deck[compteurDeck].label = new QLabel;
            //Deck[compteurDeck].setupImage(false);
        }
        else
        {
            Deck[compteurDeck].couleur=couleurActuel;
            Deck[compteurDeck].nomCouleur=Couleur;
            Deck[compteurDeck].nombre=14;
            Deck[compteurDeck].IsAJoker=true;
            //Deck[compteurDeck].cheminImage=":/Uno/NJoker";
            //Deck[compteurDeck].label = new QLabel;
            //Deck[compteurDeck].setupImage(false);
        }
        compteurDeck++;
    }

    int i =0;
    bool tabDone[108];
    unocard tempo[108];
    while (i<108)
    {
        tabDone[i]=false;
        tempo[i]=Deck[i];
        i++;
    }
    i=0;
    while (i<108) // Melange les cartes de manière aléatoire
    {
        int aleatoire=rand()%108;
        if (tabDone[aleatoire]==false)
        {
            Deck[i]=tempo[aleatoire];
            tabDone[aleatoire]=true;
            i++;
        }

    }

    whereDeck=107;
    whereDefausse=0;

    // Initialiser le LastPlayed et affichage de la carte Deck
    LastPlayed=Deck[whereDeck];
    Defausse[whereDefausse]=Deck[whereDeck];
    whereDeck--;
    whereDefausse++;
    if (LastPlayed.couleur==5)
    {
        LastPlayed.couleur= (rand()%4)+1;
    }
    LastPlayed.IsA2=false;
    LastPlayed.IsA4=false;
    LastPlayed.IsASkip=false;
    LastPlayed.IsAReverse=false;
    LastPlayed.IsAJoker=false;

    while (Joueur1.length()>0) // Utile lorsque l'on reset le jeu
    {
        //delete Joueur1[0].label; // Supprime bien le label a la place de removeWidget, a faire avant le remove
        qWarning("Vidage J1");
        Joueur1.remove(0);
    }
    while (Joueur2.length()>0) // Utile lorsque l'on reset le jeu
    {
        //delete Joueur2[0].label; // Supprime bien le label a la place de removeWidget, a faire avant le remove
        qWarning("Vidage J2");
        Joueur2.remove(0);
    }

    for (int i=0;i<7;i++) // Distribue les cartes au J1
    {
        Joueur1.push_back(Deck[whereDeck]);
        whereDeck--;
        sendDataToClient(i,1,"x"); // Sauf que là ca envoie juste au joueur 1
    }

    for (int i=0;i<7;i++) // Distribue les cartes au J2
    {
        Joueur2.push_back(Deck[whereDeck]);
        whereDeck--;
        sendDataToClient(i,2,"x"); // Sauf que là ca envoie juste au joueur 2
    }
    for (int i=0;i<7;i++) // CHECK DE OUF
    {
        qDebug() << "Joueur 1, carte " << i <<" : couleur=" << Joueur1[i].couleur << " , nombre=" << Joueur1[i].nombre;
    }
    for (int i=0;i<7;i++) // CHECK DE OUF
    {
        qDebug() << "Joueur 2, carte " << i <<" : couleur=" << Joueur2[i].couleur << " , nombre=" << Joueur2[i].nombre;
    }
    qDebug() << "Info LastPlayed : couleur=" << LastPlayed.couleur << " , nombre=" << LastPlayed.nombre;
    sendDataToClient(5,1,"c"); // Envoie à qui est le 1er tour
    sendDataToClient(5,2,"c"); // Same bro
}

void MyServer::slotRemoveClient(MyThread * thisThread)
{
    qDebug() << "removing client ..." << thisThread->getSocketAdresse();
    clients.remove(clients.indexOf(thisThread));
}

void MyServer::slotSendDataToClient(int idCarte, int id, QString action)
{
    sendDataToClient(idCarte, id, action);
}

void MyServer::slotDataReceivedFromThread(QByteArray Data, MyThread *fromClient)
{
    qDebug()<< "emitDataReceivedFromMyServer\n";
    emit emitDataReceivedFromMyServer(Data, fromClient);
    //sendDataToAll(Data, fromClient);
}

void MyServer::incomingConnection(qintptr socketDescriptor)    //Incoming connections
{
    qDebug() << "New Connection inComing ID : " + QString::number(socketDescriptor);
    //emit emitDataReceivedFromMyServer("Nouvelle connection",0); // Balance a mainwindow qu'il y a une nouvelle co
    MyThread *thread = new MyThread(socketDescriptor,this);
    clients.append(thread);

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(emitDataReceivedFromThread(QByteArray, MyThread*)),this, SLOT(slotDataReceivedFromThread(QByteArray,MyThread*)));
    connect(thread, SIGNAL(emitRemoveClient(MyThread*)),this, SLOT(slotRemoveClient(MyThread*)));
    //Start a new thread for the connection
    thread->start();    //Which will cause the run() function
    if (clients.length()==2) // Si on a 2 clients, on peux lancer la partie
    {
        Shuffle();
    }
}
