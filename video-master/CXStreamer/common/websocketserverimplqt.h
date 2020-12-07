#ifndef WEBSOCKETSERVERIMPLQT_H
#define WEBSOCKETSERVERIMPLQT_H
#include <websocketserverinf.h>
#include <QThread>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QList>
#include <vector>
#include <QVector>
#include <QMutex>

class WebsocketServerImplQt : public QThread
{
    Q_OBJECT
    QWebSocketServer* m_pWebSocketServer;
    QList< QWebSocket*> m_clients;
    int m_max_pending;
    QVector<int> m_client_pendings;
    QMutex m_clientsMutex;
    quint16 mPort;
    void *on_max_pending_changed_user_data;
    void (*on_max_pending_changed)(void* user_data,void* para);
    void (*on_layout_cfg)(void* user_data,void* para);

public:
    void setMaxPendingChangedCallback(void (*on_max_pending_changed)(void* user_data,void* para),void* user_data);
    void setLayoutCfgCallback(void (*on_layout_cfg)(void* user_data,void* para),void* user_data);
    WebsocketServerImplQt(quint16 port);
    void pushData(uint8_t* buff,int len){
        emit pushData(QByteArray((char*)buff,len)); //zxr 类型转换待验证
        //OnPushData(QByteArray((char*)buff,len));
    }
signals:
    void pushData(QByteArray bytes);
public slots:
    void onNewConnection();
    void onClientTestMsg(const QString& message);
    void socketDisconnected();
    void OnPushData(QByteArray bytes);
    void closed();
};
class WebsocketServerQt : public WebsocketServerInf
{
    WebsocketServerImplQt mPrivateServer;
public:
    WebsocketServerQt(quint16 port):mPrivateServer(port)
    {

    }
    void setMaxPendingChangedCallback(void (*on_max_pending_changed)(void* user_data,void* para),void* user_data){
        mPrivateServer.setMaxPendingChangedCallback(on_max_pending_changed,user_data);
    }
    void setLayoutCfgCallback(void (*on_layout_cfg)(void* user_data,void* para),void* ){
        mPrivateServer.setLayoutCfgCallback(on_layout_cfg,NULL);
    }
    int pushData(uint8_t* buff,int len)
    {
        mPrivateServer.pushData(buff,len);
        return 0;
    }
};
#endif // WEBSOCKETSERVERIMPLQT_H
