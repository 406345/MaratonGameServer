//Maraton Game Server
//Create by Shubo Yang

#ifndef SESSION_H_
#define SESSION_H_

#include "uv.h"
#include "Define.h"

const int SESSION_RECIVE_BUFFER_LENGTH = 1024 * 1024;

class Session
{
public:

    Session();
    virtual ~Session();
    virtual void close(); 

protected:

    virtual void on_recive_data( const char* data, size_t length );

private:

    uv_tcp_t *conn_;
    char* recive_buffer_;

    char* recive_buffer() { return this->recive_buffer_; };
   
    friend class Server;
};

#endif // SESSION_H_
