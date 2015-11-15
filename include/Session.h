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

    size_t          id  ( ) { return this->session_id_; };
    std::string     host( );
    std::string     ip  ( );

protected:

    virtual void    on_connected( );
    virtual void    on_receive_data( Buffer & buffer );
    virtual void    on_close();
    virtual void    on_send_finish( size_t size );

private:

    struct write_token_t
    {
        uv_write_t *    writer;
        uv_buf_t *      buffer;
        Session *       session;
    };

    Service*            service         = nullptr;
    uv_tcp_t*           uv_tcp_         = nullptr;
    uv_connect_t*       uv_connect_     = nullptr;
    char*               recive_buffer_  = nullptr;

    size_t              session_id_;

    static void         uv_prcoess_write_callback( uv_write_t* req, int status );
    static size_t       create_session_id( );
    friend class Service;
};

#endif // SESSION_H_
