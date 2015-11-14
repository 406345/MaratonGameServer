#ifndef SERVER_H_
#define SERVER_H_

#include <vector>
#include "uv.h"
#include "Utils.h"
#include "Define.h"

class Service;

class Server :
    public Singleton<Server>
{
public:

    static uv_loop_t*   loop           ( );
    bool                add_service    ( Service * srv );
    bool                remove_service ( Service * srv );
    void                run            ( );

protected:

    Server() {};

private:

    
    ~Server() {};
   
    std::vector<Service *> service_list;

    static void uv_callback_alloc_buffer( uv_handle_t* handle,
                                          size_t suggested_size,
                                          uv_buf_t* buf );

    static void uv_callback_read            ( uv_stream_t* stream,
                                              ssize_t nread,
                                              const uv_buf_t* buf );
    static void uv_callback_close           ( uv_handle_t* handle );
    static void uv_callback_new_connection  ( uv_stream_t* server, 
                                              int status );
    static void uv_callback_connected       ( uv_connect_t * req, 
                                              int status );
    static void uv_process_resolved         ( uv_getaddrinfo_t* req,
                                              int status,
                                              struct addrinfo* res );
    friend class Singleton<Server>;
};

#endif // SERVER_H_
