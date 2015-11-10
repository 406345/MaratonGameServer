//Maraton Game Server
//Create by Shubo Yang

#ifndef SESSION_H_
#define SESSION_H_

#include "uv.h"
#include "Define.h"


const int SESSION_RECIVE_BUFFER_LENGTH = 1024 * 1024;
class Service;

class Session
{
public:

    Session( Service* service );
    virtual ~Session();
    
    virtual void close();
    virtual void send( const char* data, size_t size );

    size_t id() { return this->session_id_; };

protected:

    virtual void on_recive_data( const char* data, size_t length );

private:

    Service*  service;
    uv_tcp_t* conn_;
    char*     recive_buffer_;
    size_t    session_id_;

    char*       recive_buffer() { return this->recive_buffer_; };
    static void uv_prcoess_write_callback( uv_write_t* req, int status );

    friend class Server;
};

#endif // SESSION_H_
