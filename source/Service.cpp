#include "Service.h"
#include "Server.h"
#include "SessionManager.h"

Service::Service()
{
    this->ip_ = new char[128];
    memset( this->ip_, 0, 128 ); 
    this->sock_ = { 0 };
    this->addr_in = { 0 };
    this->conn_ = { 0 };
}

Service::~Service()
{
    SAFE_DELETE( this->ip_ );
}

void Service::listen( const char * ip, int port )
{
    this->service_type_ = ServiceType::kServer;
    memcpy( this->ip_, ip, strlen( ip ) );
    uv_tcp_init( Server::loop(), &this->sock_ );

    this->sock_.data = this;

    sockaddr_in addr;
    uv_ip4_addr( this->ip_, port, &addr );
    uv_tcp_bind( &this->sock_, ( const struct sockaddr* )&addr, 0 );
}

void Service::connect( const char * ip, int port )
{
    this->service_type_ = ServiceType::kClient;
    memcpy( this->ip_, ip, strlen(ip) );
    uv_tcp_init( Server::loop(), &this->sock_ );

    this->sock_.data = this;
    this->conn_.data = this;

    uv_ip4_addr( this->ip_, port, &this->addr_in ); 
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
    auto ret = new Session(this); 
    return ret;
}
 