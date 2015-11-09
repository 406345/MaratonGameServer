//Maraton Game Server
//Create by Shubo Yang

#ifndef SERVICE_H_
#define SERVICE_H_

#include "uv.h" 
#include "Define.h"
#include "Session.h"

class Service
{
public: 

    Service();
    ~Service();

    void init( const char* ip, int port );
    virtual Session* create_session();
     
private:
    
    int port_;
    char* ip_;
    uv_tcp_t sock_;
    sockaddr_in addr_; 

    friend class Server;
};

#endif // SERVICE_H_
