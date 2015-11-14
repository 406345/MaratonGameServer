#ifndef SERVICE_H_
#define SERVICE_H_

#include <functional>
#include <vector>

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
    typedef std::function<void( Service* , size_t status )> service_callback_t;
    
    Service();
    ~Service();

    void listen                 ( const char* ip, int port );
    void connect                ( const char* ip, int port );
    void stop                   ( );
    void on_operation_failed    ( service_callback_t callback );
    void on_new_session         ( session_callback_t callback );
    void on_close_session       ( session_callback_t callback );

protected:

    virtual Session* create_session();
     
private:
    
    int                     port_                       = 0;
    char*                   ip_                         = nullptr;
    char*                   host_                       = nullptr;
    ServiceType             service_type_               = ServiceType::kUnknow;
    std::vector<Session*>   session_list_;

    sockaddr_in             addr_in                     = { 0 };
    uv_tcp_t                uv_tcp_                     = { 0 };
    uv_connect_t            uv_connect_                 = { 0 };
    addrinfo                uv_addrinfo_                = { 0 };
    uv_getaddrinfo_t        uv_resolver_                = { 0 };

    service_callback_t      operation_failed_callback_  = nullptr;
    session_callback_t      new_session_callback_       = nullptr;
    session_callback_t      close_session_callback_     = nullptr;

    void add_session            ( Session * session );
    void remove_session         ( Session * session );

    friend class Server;
};

#endif // SERVICE_H_
