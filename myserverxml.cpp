#include <QObject>
#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>
#include <myserverxml.h>
#include <QList>
#include <stdio.h>
#include <stdlib.h>
#include <QListIterator>
#include <QtXml>
#include <QDebug>
#include <QtXml>

//nPort - номер порта
MyServer::MyServer(int nPort, QWidget* pwgt /*=0*/) : QWidget(pwgt)
                                                    , m_nNextBlockSize(0)
{
    m_ptcpServer = new QTcpServer(this);
    //вызов метода listen() для запуска сервера
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
        // !m_ptcpServer->... ! дает результат переменная типа bool
        QMessageBox::critical(0,
                              "Server Error",
                              "Unable to start the server:"
                              + m_ptcpServer->errorString()
                              );
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()),
    this,                 SLOT(slotNewConnection())
    );

    m_ptxt = new QTextEdit;    // виджет многострочного текстового поля
    m_ptxt->setReadOnly(true); // режим только просмотра информации

    //Layout setup

    // чтобы привязать компоновщик к виджету, нужно создать объект
    // компоновщика, а потом передать его в метод виджета setLayout().

    QVBoxLayout* pvbxLayout = new QVBoxLayout; //pvbxLayout устанавливает
                                               //себя компоновщиком для
                                               //QVBoxLayout
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(m_ptxt);
    setLayout(pvbxLayout);
    qDebug() << "MyServer class works";
}

//***********************************************************************

// вызывается каждый раз при соединении с новым клиентом
/*virtual*/ void MyServer::slotNewConnection()
{
// для подтверждения соединения с клиентом вызывается метод
// nextPendingConnection(), который возвратит сокет

    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();



    qDebug() << "slotNewConnection works";

    connect(pClientSocket, SIGNAL(disconnected()), //standart
            pClientSocket, SLOT(deleteLater())     //standart
           );

    connect(pClientSocket, SIGNAL(readyRead()),    //standart
            this,          SLOT(slotReadClient())  //описан ниже отдельно
           );

    //sendToClient(pClientSocket, "Server Response: You are connected!");

}

//***********************************************************************

void MyServer::slotReadClient()
{
     QTcpSocket* pClientSocket = (QTcpSocket*)sender(); 
     QDataStream in(pClientSocket);
     in.setVersion(QDataStream::Qt_5_3);
     for (;;) { // бесконечный цикл
         if (!m_nNextBlockSize) { // если 0 т.е. неизвестен
             if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                 break;
             }
             in >> m_nNextBlockSize; // считывается размер блока
         }
         // размер доступных для чтения данных >/= размеру блока
         if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
         break;
         }
         QTime   time;
         // переменная in - объект потока QDataStream
         in >> str; // получение <xml>
         //QString strMessage = str;
         qDebug() << "msg in <xml>: " << str;
         QDomDocument doc;
         doc.setContent(str);
         QDomElement domElement = doc.documentElement();

         QDomNode domNode = domElement.firstChild();
         while(!domNode.isNull()) {
         traverseNode(domNode);
         }

         qDebug() << "after traverseNode";
         m_ptxt->append(str);

         m_nNextBlockSize = 0;
         // сообщение клиенту о прочтении данных
//         sendToClient(pClientSocket,"Server Response: " + str2, " sent: " + str1);
     }
}

//***********************************************************************

void MyServer::traverseNode(QDomNode& domNode)
{

        if(domNode.isElement()) {
           QDomElement domElement = domNode.toElement();
           if(!domElement.isNull()) {
                qDebug() << "TagName: " << domElement.tagName();
                qDebug() << "Text: "    << domElement.text();
           }
        }

domNode = domNode.nextSibling();
//traverseNode(domNode);
}

//***********************************************************************

// формирование данных для отсылки клиенту

// заранее неизвестен размер блока -> нельзя сразу записать данные
// в сокет, т.к. размер блока должен быть записан в первую очередь

void MyServer::sendToClient(QTcpSocket* pSocket, QString str)
 {
    QByteArray  arrBlock; // массив байтов
    QDataStream out(&arrBlock, QIODevice::WriteOnly);

// Класс QDataStream обеспечивает сериализацию
// двоичных данных в QIODevice .
// cериализация это процесс сохранения состояния объекта
// в последовательность байт;
// Создает поток данных, который работает с массивом байтов arrBlock
// QIODevice::WriteOnly описывает,kак устройство должно быть
// использовано.

    out.setVersion(QDataStream::Qt_5_3);

    out << quint16(0) << QTime::currentTime() << str;
    out.device()->seek(0); // перемещение указателя на начало блока
    out << quint16(arrBlock.size() - sizeof(quint16));
    // arrBlock.size() - размер блока - 2 байта,+запись в поток out
    pSocket->write(arrBlock);
}
