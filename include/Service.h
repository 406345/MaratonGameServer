#ifndef SERVICE_H_
#define SERVICE_H_

#include <functional>

#include "uv.h" 
#include "Define.h"
#include "Session.h"
#include "Utils.h"


class Service
{
public: 

    enum ServiceType
    {
        kUnknow = 0 ,
        kServer ,
        kClient
    };

    typedef std::function<void( Session* )> session_callback_t;

    Service();
    ~Service();

    void listen( const char* ip, int port );
    void connect( const char* ip, int port );

    void new_session_cb( session_callback_t callback );
    void close_session_cb( session_callback_t callback );

protected:

    virtual Session* create_session();
     
private:
    
    int port_;
    char* ip_;
    uv_tcp_t sock_ = { 0 };
    sockaddr_in addr_in = { 0 };
    uv_connect_t conn_ = { 0 };

    session_callback_t new_session_callback_;
    session_callback_t close_session_callback_;
    ServiceType service_type_ = ServiceType::kUnknow;

    friend class Server;
};

#endif // SERVICE_H_
