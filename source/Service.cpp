#include "Service.h"
#include "Server.h"

Service::Service()
{
    this->ip_ = new char[4];
}

Service::~Service()
{
    SAFE_DELETE( this->ip_ );
}

void Service::listen( const char * ip, int port )
{
    memcpy( this->ip_, ip, 4 );
    uv_tcp_init( Server::loop(), &this->sock_ );

    this->sock_.data = this;

    sockaddr_in addr;
    uv_ip4_addr( this->ip_, port, &addr );
    uv_tcp_bind( &this->sock_, ( const struct sockaddr* )&addr, 0 );
}

void Service::new_session_cb( session_callback_t callback )
{
    this->new_session_callback_ = callback;
}

void Service::close_session_cb( session_callback_t callback )
{
    this->close_session_callback_ = callback;
}

Session * Service::create_session()
{
    return new Session(this);
}
 