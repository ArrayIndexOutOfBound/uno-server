#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MyServer *server = new MyServer;
    connect(server, SIGNAL(emitDataReceivedFromMyServer(QByteArray,MyThread *)),this, SLOT(slotDataReceivedFromMyServer(const QByteArray, MyThread *)));
    connect(this, SIGNAL(emitSendDataToClient(int,int,QString)), server, SLOT(slotSendDataToClient(int,int,QString)));
    connect(this, SIGNAL(emitSendDataToAllFromLine(QByteArray)),server, SLOT(slotSendDataToAllFromLine(QByteArray)));
    server->StartServer();
    qDebug()<< "MainWindow : Lancement du Serveur demarré";
    ui->teTexte->append("Tout est initialisé");
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotDataReceivedFromMyServer(const QByteArray Data, MyThread * source)
{
    qDebug() << "J'AI RECU UN TRUC AU DESSUS";
    QString reception = QString(Data);

    qDebug() << reception;

    // 1er caractère : la source (1 ou 2)
    /*
    LISTE DES ACTIONS DANS LA TRAME DE DONNEES, de ce qui sera recu (envoyé par client)
    a = je pioche une carte
    b = je pose une carte; suite étant soit la carte, soit son ID vectoriel (a voir ce qui semble le plus efficace)
    c = en cas de changement de couleur, changer le lastplayed, UTILISE ID CARTE POUR LA COULEUR


    */
    // CA DEPEND DE L'IP RECU

    if (reception.length()<=3) // C'est une action légitime, et pas juste du texte pour la fenêtre de debug du serveur
    {
        QString ids = reception.mid(0,1);
        QString action = reception.mid(1,1);
        QString idCard = reception.mid(2,1);
        ui->teTexte->append(ids);
        ui->teTexte->append(action);
        ui->teTexte->append(idCard);

        qDebug() << "VRAIMENT ICI CE QUIL FAUT" << source->getSocketAdresse();

        int idCarte = idCard.toInt();
        int id = ids.toInt();
        emit emitSendDataToClient(idCarte,id,action);
    }
    else // C'est du debug, on le log ici
    {
        ui->teTexte->append(QString(Data));
    }

}


void MainWindow::on_pushButton_clicked()
{
    qDebug() << "Push Bouton appuyé";
    qDebug() << ui->lineEdit->text().toLatin1();
    QByteArray data = "S:"+ui->lineEdit->text().toLatin1();
    emit emitSendDataToAllFromLine(data);
}
