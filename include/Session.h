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
    
    void    close();
    virtual void    send( Buffer & buffer );

    size_t          id() { return this->session_id_; };

protected:

    virtual void    on_recive_data( Buffer & buffer );
    virtual void    on_close();

private:

    Service*        service         = nullptr;
    uv_tcp_t*       uv_tcp_         = nullptr;
    uv_connect_t*   uv_connect_     = nullptr;
    char*           recive_buffer_  = nullptr;

    size_t          session_id_;

    static void     uv_prcoess_write_callback( uv_write_t* req, int status );
    static size_t   create_session_id( );
    friend class Service;
};

#endif // SESSION_H_
