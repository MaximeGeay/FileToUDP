#include<QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"


#define version "FileToUDP 1.0"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowFlag(Qt::WindowMaximizeButtonHint,false);
    this->setWindowTitle(version);

    timer = new QTimer(this);
    timer->setSingleShot(true);
    udpSocket = new QUdpSocket(this);

    QSettings m_settings("FileToUDP","FileToUDP_Config");
    ui->le_Adresse->setText(m_settings.value("AdresseIp").toString());
    ui->sp_Port->setValue(m_settings.value("PortUdp").toInt());
    ui->sp_recurrence->setValue(m_settings.value("Recurrence",500).toInt());
    m_ListeFichiers=m_settings.value("ListeFichiers").toStringList();
    m_chemin=m_settings.value("chemin").toString();
    ui->le_Fichier->setText(m_chemin);

    QObject::connect(this->ui->btn_Emit, &QPushButton::clicked, this, &MainWindow::startBroadcasting);
    QObject::connect(timer, &QTimer::timeout, this, &MainWindow::broadcastDatagram);
    QObject::connect(this,&MainWindow::oneShot,this,&MainWindow::broadcastDatagram);
    QObject::connect(this->ui->btn_Stop,&QPushButton::clicked,this,&MainWindow::interruption);
    QObject::connect(this->ui->btn_Parcourir,&QPushButton::clicked,this,&MainWindow::selFichier);

    ui->btn_Stop->setEnabled(false);


}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startBroadcasting()
{
    QSettings m_settings("FileToUDP","FileToUDP_Config");
    QString sUneLigne="";
    m_settings.setValue("AdresseIp",ui->le_Adresse->text());
    m_settings.setValue("PortUdp",ui->sp_Port->value());
    m_settings.setValue("Recurrence",ui->sp_recurrence->value());
    qDebug()<<"mChemin"<<m_chemin;
    m_settings.setValue("chemin",m_chemin);
    m_settings.setValue("ListeFichiers",m_ListeFichiers);

    ui->le_Adresse->setText(m_settings.value("AdresseIp").toString());
    ui->sp_Port->setValue(m_settings.value("PortUdp").toInt());
    ui->sp_recurrence->setValue(m_settings.value("Recurrence",500).toInt());
    m_ListeFichiers=m_settings.value("ListeFichiers").toStringList();


     m_Position=0;
     m_ListeData.clear();
     qDebug()<<"ListeFichier"<<m_ListeFichiers.size();

        QListIterator<QString> it(m_ListeFichiers);
        while(it.hasNext())
        {
            m_chemin=it.next();

            m_Fichier=new QFile(m_chemin);
            if(m_Fichier->open(QIODevice::ReadOnly))
            {
                qDebug()<<"FileIsOpen";
                m_Flux=new QTextStream(m_Fichier);

                while(!m_Flux->atEnd())
                {
                    ui->statusbar->showMessage("Données en cours de tri :"+QString::number(m_ListeData.size()));

                    sUneLigne=m_Flux->readLine();
                    m_ListeData.append(sUneLigne);


                }


            }
            else
            {
                 ui->statusbar->showMessage("impossible d'ouvrir le fichier");
                 qDebug()<<"impossible d'ouvrir le fichier";
                 interruption();
                 return;
            }
        }

    m_settings.setValue("chemin",m_chemin);

    timer->setSingleShot(true);



    ui->btn_Emit->setEnabled(false);
    ui->btn_Stop->setEnabled(true);

    m_recurrence=ui->sp_recurrence->value();
    m_adresse=ui->le_Adresse->text();
    m_port=ui->sp_Port->value();


    if(udpSocket->bind(QHostAddress::Any,m_port,QAbstractSocket::ReuseAddressHint))
    {
        ui->statusbar->showMessage(QString("Connecté au port UDP %1").arg(m_port));
    }
    else
    {
        ui->statusbar->showMessage(udpSocket->errorString());

    }

    qDebug()<<"mRecurrence"<<m_recurrence;

    if (m_recurrence==0)
    {
        emit oneShot();
    }
    else
    {
        timer->start(m_recurrence);
    }

}

void MainWindow::broadcastDatagram()
{

    /*qDebug()<<"Broadcast...";
    qDebug()<<"DataSize:"<<m_ListeData.size();*/

    qint64 res=-1;
    QString sUneLigne=m_ListeData[m_Position];
    m_Position++;
    sUneLigne=sUneLigne+"\r\n";

    QByteArray datagram=sUneLigne.toLocal8Bit();

    res=udpSocket->writeDatagram(datagram.data(), datagram.size(),QHostAddress(m_adresse), m_port);

        if(res==-1)
            ui->statusbar->showMessage("Trame non émise");
        else
            ui->statusbar->showMessage(datagram);


        if(m_Position==m_ListeData.size()-1)
        {
            interruption();
            ui->statusbar->showMessage("Fichier terminé");
        }

        timer->start(m_recurrence);


}

void MainWindow::interruption()
{
    timer->stop();

    ui->btn_Emit->setEnabled(true);
    ui->btn_Stop->setEnabled(false);
    m_Fichier->close();


}
void MainWindow::selFichier()
{
    qDebug()<<"m_chemin"<<m_chemin;
    QStringList path=QFileDialog::getOpenFileNames(this,tr("Sélectionner un fichier ascii"),m_chemin.section(m_chemin.section("/",-1),0),"*.*");
    qDebug()<<"path"<<path;

    if(path.isEmpty())
    {

    }
    else
    {
        m_ListeFichiers=path;

        ui->le_Fichier->setText(m_ListeFichiers.first());

    }

}

