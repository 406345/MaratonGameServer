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

    typedef std::function<void( Session* )>      session_callback_t;
    typedef std::function<void( Service* ,       
                                size_t status )> service_status_callback_t;
    typedef std::function<void( Service* )>      service_callback_t;
   
    Service();
    ~Service();

    void listen                 ( const char* host, int port );
    void connect                ( const char* host, int port );
    void stop                   ( );
    void on_operation_failed    ( service_status_callback_t callback );
    void on_new_session         ( session_callback_t callback );
    void on_close_session       ( session_callback_t callback );

    static uv_loop_t * loop     ( );

protected:

    virtual Session* create_session();
     
private:
    
    int                     port_                       = 0;
    char                    host_[512]                  = {0};
    ServiceType             service_type_               = ServiceType::kUnknow;
    std::vector<Session*>   session_list_;

    uv_tcp_t*               uv_tcp_                     = { 0 };
    uv_connect_t*           uv_connect_                 = { 0 };
    uv_getaddrinfo_t*       uv_resolver_                = { 0 };
    addrinfo                addrinfo_                   = { 0 };
    sockaddr_in             addr_in                     = { 0 };

    service_status_callback_t      operation_failed_callback_  = nullptr;
    session_callback_t      new_session_callback_       = nullptr;
    session_callback_t      close_session_callback_     = nullptr;

    void add_session                        ( Session * session );
    void remove_session                     ( Session * session ); 

    static void uv_callback_alloc_buffer    ( uv_handle_t* handle,
                                              size_t suggested_size,
                                              uv_buf_t* buf );

    static void uv_callback_read            ( uv_stream_t* stream,
                                              ssize_t nread,
                                              const uv_buf_t* buf );
    static void uv_callback_close           ( uv_handle_t* handle );
    static void uv_callback_service_close   ( uv_handle_t* handle );
    static void uv_callback_new_connection  ( uv_stream_t* server, 
                                              int status );
    static void uv_callback_connected       ( uv_connect_t * req, 
                                              int status );
    static void uv_process_resolved         ( uv_getaddrinfo_t* req,
                                              int status,
                                              struct addrinfo* res );

    friend class Session;
};

#endif // SERVICE_H_
