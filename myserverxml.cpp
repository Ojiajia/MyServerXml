#include <QObject>
#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>
#include <myserverxml.h>
#include <QList>
#include <stdio.h>
#include <stdlib.h>
#include <QListIterator>

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
//    qDebug() << "MyServer class works";
}

//***********************************************************************

// вызывается каждый раз при соединении с новым клиентом
/*virtual*/ void MyServer::slotNewConnection()
{
// для подтверждения соединения с клиентом вызывается метод
// nextPendingConnection(), который возвратит сокет

    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();



//    qDebug() << "slotNewConnection works";

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

         qDebug() << "msg from client " << str;

         QDomDocument doc;
         doc.setContent(str);
         QDomElement domElement = doc.documentElement();

         QDomNode domNode = domElement.firstChild();
         while(!domNode.isNull()) {
         traverseNode(domNode, str2);
         }

//         qDebug() << "str2 after traverseNode: " << str2;
        // str2 - ник из хмл

//        usersList.insert(pClientSocket* , str2);




//       вывод ника (.value из QMap)
//         if (usersList.contains(pClientSocket)) {

//             qDebug() << "slotReadClient" << usersList.value(pClientSocket);
//         }

//         qDebug() << "begin of users list:";

         QMap<QTcpSocket*,QString>::const_iterator i = usersList.constBegin();



//         while (i != usersList.constEnd()) {



//             i.key() = pClientSocket;
//             i.value() = str2;

////             sendToClient(i.key(),str);

//             ++i;

//         }

//         i = 0;

         usersList[pClientSocket] = str2;

         int intMsgType = 3;

         QString strMsgType = QString::number(intMsgType);

         QString msgToClient = "<data><type>" + strMsgType + "</type>";

//         qDebug() << msgToClient;

//       условное количество клиентов

         int clientId;


//         qDebug() << usersList.size();
       for (int clientId = 1; clientId <= usersList.size(); clientId++) {

//       for (int clientId = 1; clientId <= usersList.size(); clientId++) {



//         printf();
//         << " usersList.constEnd() = " << usersList.constEnd();

              while (i != usersList.constEnd()) {

//                     qDebug() << "client id = " << clientId;

                     QString clientIdStr = QString::number(clientId);

                      msgToClient = msgToClient + "<client id = \"" + clientIdStr + "\">" + i.value() + "</client>";

//                     qDebug() << " msg ends with </client> " << msgToClient;

                     ++i;

             }

             i = 0;

         }

         clientId = 0;

         msgToClient = msgToClient + "</data>";

//         qDebug() << " ends up with </data> " << msgToClient;

//         i = usersList.constBegin();



         while (i != usersList.constEnd()) {

                sendToClient(i.key(),msgToClient);

                ++i;

               }

         i = 0;

         qDebug() << "msg sent";

         //m_ptxt->append(str);

         m_nNextBlockSize = 0;

     }
}

//***********************************************************************

//***********************************************************************

QString MyServer::traverseNode(QDomNode& domNode,QString& str2)
{

        if(domNode.isElement()) {
           QDomElement domElement = domNode.toElement();
           if(!domElement.isNull()) {
//                qDebug() << "TagName: " << domElement.tagName();
//                qDebug() << "Text: "    << domElement.text();
           if(domElement.tagName() == "nickname") {
//                qDebug() << "nickname found: " << domElement.text();
                str2 = domElement.text();
//                qDebug() << "str2 from traverseNode: " << str2;
//                usersList.insert(pClientSocket, str2);
//                if (usersList.contains(str2)) {
//                    qDebug() << "slotReadClient" << usersList.value(str2);
//                }
           }
           }

        }

domNode = domNode.nextSibling();
//traverseNode(domNode);
return str2; // str2 - ник из хмл
}

//***********************************************************************

// формирование данных для отсылки клиенту

// заранее неизвестен размер блока -> нельзя сразу записать данные
// в сокет, т.к. размер блока должен быть записан в первую очередь

void MyServer::sendToClient(QTcpSocket* pSocket,QString& str)
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

    out << quint16(0) << str;
    out.device()->seek(0); // перемещение указателя на начало блока
    out << quint16(arrBlock.size() - sizeof(quint16));
    // arrBlock.size() - размер блока - 2 байта,+запись в поток out
    pSocket->write(arrBlock);
}
