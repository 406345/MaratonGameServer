//Maraton Game Server
//Create by Shubo Yang

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
    
    typedef std::function<void( Session* )> session_callback_t;

    Service();
    ~Service();

    void listen( const char* ip, int port );

    void new_session_cb( session_callback_t callback );
    void close_session_cb( session_callback_t callback );

protected:

    virtual Session* create_session();
     
private:
    
    int port_;
    char* ip_;
    uv_tcp_t sock_;
    sockaddr_in addr_; 
    session_callback_t new_session_callback_;
    session_callback_t close_session_callback_;

    friend class Server;
};

#endif // SERVICE_H_
