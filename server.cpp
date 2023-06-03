#include "server.h"
#include "ui_server.h"

#include <ui_server.h>

Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    // función para inicializar el servidor
    initServer();

    connect(ui->btn_close, &QAbstractButton::clicked, this, &QWidget::close);
    connect(serverSocket, &QTcpServer::newConnection, this, &Server::sendRandomMessage);

    // fill vector
    for (int i = 1; i <= 50; i++) {
        randomMessages << "Mensaje aleatorio " + QString::number(i);
    }
    // qInfo() << randomMessage;
}

Server::~Server()
{
    delete ui;
}


void Server::initServer()
{
    qInfo() << "initServer()...";
    serverSocket = new QTcpServer(this);
    if (!serverSocket->listen()) {
        qInfo() << "Server error: " + serverSocket->errorString();
        return;
    }
    QString port = QString::number(serverSocket->serverPort());
    QString ipAddress;
    QList<QHostAddress> ipAddressList = QNetworkInterface::allAddresses();
    //qInfo() << ipAddressList;
    for (int i = 0; i < ipAddressList.size(); i++){
        // Filtrar localhost e ipv6
        if (ipAddressList.at(i) != QHostAddress::LocalHost && ipAddressList.at(i).toIPv4Address()) {
            ipAddress = ipAddressList.at(i).toString();
            break;
        }
    }
    QString info = "ip: " + ipAddress + ", port: " + port;
    qInfo() << info;
    ui->label->setText(info);
}

void Server::sendRandomMessage()
{
    qInfo() << "sendRandomMessage() ...";
    // stream and channel
    QByteArray qByteArray;
    QDataStream qDataStream(&qByteArray, QIODevice::WriteOnly);
    // random message
    QString randomMessage = randomMessages[QRandomGenerator::global()->bounded(randomMessages.size())];
    // write to label
    ui->lbl_message->setText(randomMessage);
    // write to stream
    qDataStream << randomMessage;
    // send it
    QTcpSocket *clientSocket = serverSocket->nextPendingConnection();
    connect(clientSocket, &QAbstractSocket::disconnected, clientSocket, &QObject::deleteLater);
    clientSocket->write(qByteArray);
    clientSocket->disconnectFromHost();
}
