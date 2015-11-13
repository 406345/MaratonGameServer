#ifndef SESSION_H_
#define SESSION_H_

#include "uv.h"
#include "Define.h"
#include "Buffer.h"

class Service;

class Session
{
public:

    Session( Service* service );
    virtual ~Session();
    
    virtual void    close();
    virtual void    send( Buffer buffer );

    size_t          id() { return this->session_id_; };

protected:

    virtual void    on_recive_data( Buffer buffer );

private:

    Service*        service;
    uv_tcp_t*       listener_;
    char*           recive_buffer_;
    size_t          session_id_;
    uv_connect_t*   connector_;

    char*           recive_buffer() { return this->recive_buffer_; };
    static void     uv_prcoess_write_callback( uv_write_t* req, int status );

    friend class Server;
};

#endif // SESSION_H_
