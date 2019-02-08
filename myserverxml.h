#ifndef MYSERVER_H
#define MYSERVER_H
#pragma once
#include <QObject>
#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>
#include <QList>
#include <QtXml>

class QTcpServer;
class QTextEdit;
class QTcpSocket;
// ======================================================================
class MyServer : public QWidget {
Q_OBJECT
private:
    QTcpServer* m_ptcpServer;     //атрибут
    QTextEdit*  m_ptxt;           //длина следующего текстового
                                  //блока от сокета
    quint16     m_nNextBlockSize; //многострочное текстовое поле
                                  //для информирования о происходящих
                                  //соединениях
private:

    void sendToClient(QTcpSocket* pSocket, QString str);
    void traverseNode(QDomNode& domNode);

public:

    MyServer(int nPort, QWidget* pwgt = 0);

    QString str; // message with data


public slots:

    virtual void slotNewConnection();
            void slotReadClient();

};
#endif // MYSERVER_H
