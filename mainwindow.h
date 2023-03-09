#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QUdpSocket>
#include <QDateTime>
#include <QDebug>
#include <QSettings>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals :
    void oneShot();
    void stopEmission();

public slots :
    void startBroadcasting();
    void broadcastDatagram();
    void interruption();

private slots:
        void selFichier();


private:
    Ui::MainWindow *ui;

    QUdpSocket *udpSocket;
    QTimer *timer;
    int m_recurrence;
    int m_port;
    QString m_chemin;
    QStringList m_ListeFichiers;
    QStringList m_ListeData;
    QFile *m_Fichier;
    QTextStream *m_Flux;
    int m_Position;

    QString m_adresse;

};
#endif // MAINWINDOW_H
