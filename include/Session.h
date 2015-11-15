#ifndef SESSION_H_
#define SESSION_H_

#include <functional>

#include "uv.h"
#include "Define.h"
#include "Buffer.h"

class Service;

class Session
{
public:

    typedef std::function<void( Session* )> callback_void_session_t;

    Session( Service* service_ );
    virtual ~Session();
    
    void    close();
    virtual void    send( Buffer & buffer );

    size_t          id      ( ) { return this->session_id_; };
    std::string     host    ( );
    std::string     ip      ( );
    Service*        service ( );

    void            callback_close( callback_void_session_t callback );

protected:

    virtual void    on_connected( );
    virtual void    on_receive_data( Buffer & buffer );
    virtual void    on_close();
    virtual void    on_send_finish( size_t size );

private:

    struct write_token_t
    {
        uv_write_t *        writer;
        uv_buf_t *          buffer;
        Session *           session;
    };

    Service*                service_                = nullptr;
    uv_tcp_t*               uv_tcp_                 = nullptr;
    uv_connect_t*           uv_connect_             = nullptr;
    char*                   recive_buffer_          = nullptr;

    size_t                  session_id_;
    callback_void_session_t callback_close_      = nullptr;

    static void         uv_prcoess_write_callback( uv_write_t* req, int status );
    static size_t       create_session_id( );
    friend class Service;
};

#endif // SESSION_H_
