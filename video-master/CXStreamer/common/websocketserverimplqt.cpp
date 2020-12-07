#include "websocketserverimplqt.h"
#include <QJsonDocument>
#include <QJsonObject>
WebsocketServerImplQt::WebsocketServerImplQt(quint16 port):m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Websocket Server"),
                                                                                      QWebSocketServer::NonSecureMode, this))
{
    mPort = port;
    m_max_pending = 0;
    on_max_pending_changed = nullptr;
    on_max_pending_changed_user_data = nullptr;
    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Echoserver listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &WebsocketServerImplQt::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &WebsocketServerImplQt::closed);
        connect(this,SIGNAL(pushData(QByteArray)),this,SLOT(OnPushData(QByteArray)),Qt::QueuedConnection);
        moveToThread(this);
        start();
    }

}
 void WebsocketServerImplQt::closed()
 {
     qDebug() << "WebsocketServerImplQt close";
 }
void WebsocketServerImplQt::setMaxPendingChangedCallback(void (*on_max_pending_changed)(void* user_data,void* para),void* user_data)
{
    this->on_max_pending_changed = on_max_pending_changed;
    this->on_max_pending_changed_user_data = user_data;
}
void WebsocketServerImplQt::setLayoutCfgCallback(void (*on_layout_cfg)(void* user_data,void* para),void* ){
    this->on_layout_cfg = on_layout_cfg;
}
 void WebsocketServerImplQt::onNewConnection()
 {
     QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();
     connect(pSocket, &QWebSocket::disconnected, this, &WebsocketServerImplQt::socketDisconnected);
     connect(pSocket, SIGNAL(textMessageReceived(const QString& )), this, SLOT(onClientTestMsg(const QString&)));

     if(m_clients.size() >= 50)
     {
         pSocket->close();
         delete pSocket;
         return;
     }
     printf("onNewConnection >>>>>>>>>>>\n");
     m_clientsMutex.lock();
     m_clients.push_back(  pSocket);
     m_clientsMutex.unlock();
     printf("onNewConnection <<<<<<<<<<<< %d\n",m_clients.size());
 }
 void WebsocketServerImplQt::onClientTestMsg(const QString& message)
 {
     QJsonObject json = QJsonDocument::fromJson(message.toUtf8()).object();
     QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

     int inx = 0;
     m_clientsMutex.lock();
     inx = m_clients.indexOf(pClient);
     m_clientsMutex.unlock();
     if(inx >= m_client_pendings.size())
         m_client_pendings.resize(inx+1);

     if(json.contains("pendingH264pkg")){
         m_client_pendings[inx] = json["pendingH264pkg"].toInt();
         if(on_max_pending_changed != nullptr){
             auto max = *std::max_element(std::begin(m_client_pendings), std::end(m_client_pendings));
             if( max != m_max_pending || max == 0){
                 m_max_pending = max;
                 on_max_pending_changed(on_max_pending_changed_user_data,reinterpret_cast<void*>(max) );
             }
         }
     }
     else if(json.contains("topic") && json["topic"].toString() == "layout_cfg"){
         if(on_layout_cfg)
         {
             uint8_t arr[64+2];
             int rows = json["rows"].toInt();
             int cols = json["cols"].toInt();
             arr[0] = (uint8_t)cols;
             arr[1] = (uint8_t)rows;
             for(int i = 0;i < rows*cols;i++ ){
                 arr[2+i] = (uint8_t)json[QString("inx_")+i].toInt();
             }
             on_layout_cfg(nullptr,arr);
         }
     }


     printf("recv ws message:%s\n",message.toUtf8().data());
        //
 }

#include <sys/time.h>

 void WebsocketServerImplQt::socketDisconnected()
 {
     QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    printf("socketDisconnected >>>>>>>>>>>\n");
     if (pClient) {
         m_clientsMutex.lock();
         m_clients.removeAll(pClient);
         m_clientsMutex.unlock();

         pClient->deleteLater();
     }
     printf("socketDisconnected <<<<<<<<<<<<< %d\n",m_clients.size());
 }

void WebsocketServerImplQt::OnPushData(QByteArray bytes)
{
    m_clientsMutex.lock();

    {
        struct timeval tv;
        gettimeofday(&tv,nullptr);
        printf("%ld:%06ld -- WebsocketServerImplQt::OnPushData (%d KB) %d\n",tv.tv_sec,tv.tv_usec,bytes.size()/1024,m_clients.size());
    }
//    foreach (QWebSocket* ws, m_clients) {
//        if(ws->isValid())
//            ws->sendBinaryMessage(bytes);
//    }
    if(m_client_pendings.size() != m_clients.size())
        m_client_pendings.resize(m_clients.size());
    for(int i = 0;i < m_client_pendings.size()-2;i++){
        m_client_pendings[i] = 0;
    }
    for(int i = 1;i <=2;i++)
    {
        if(m_clients.size()-i >= 0)
        {
            QWebSocket* ws = m_clients[m_clients.size()-i];
            if(ws->isValid())
                ws->sendBinaryMessage(bytes);
        }
    }

    m_clientsMutex.unlock();
}
