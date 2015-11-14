#include "Service.h"
#include "Server.h"
#include "SessionManager.h"

Service::Service()
{
    this->ip_           = new char[512] { 0 };
    this->host_         = new char[512] { 0 };
    this->uv_tcp_       = { 0 };
    this->addr_in       = { 0 };
    this->uv_connect_   = { 0 };
}

Service::~Service()
{
    SAFE_DELETE( this->ip_ );
    SAFE_DELETE( this->host_ );
}

void Service::listen( const char * ip, int port )
{
    this->service_type_ = ServiceType::kServer;
    memcpy( this->ip_, ip, strlen( ip ) );
    uv_tcp_init( Server::loop(), &this->uv_tcp_ );

    this->uv_tcp_.data = this;

    sockaddr_in addr;
    uv_ip4_addr( this->ip_, port, &addr );
    uv_tcp_bind( &this->uv_tcp_, ( const struct sockaddr* )&addr, 0 );

    Server::instance( )->add_service( this );
}

void Service::connect( const char * ip, int port )
{
    this->service_type_ = ServiceType::kClient;
    memcpy( this->host_, ip, strlen(ip) );
    this->port_ = port;

    uv_tcp_init( Server::loop(), &this->uv_tcp_ );
    this->uv_tcp_.data = this;

    Server::instance( )->add_service( this );
}

void Service::stop( )
{
    //Server::instance()->remove_service( this );
    //for ( auto itr = this->session_list_.begin( );
    //    itr != this->session_list_.end( );
    //    itr++ )
    //{
    //    if ( ( *itr ) == session )
    //    {
    //        
    //    }
    //}
}

void Service::on_operation_failed( service_callback_t callback )
{
    this->operation_failed_callback_ = callback;
}

void Service::on_new_session( session_callback_t callback )
{
    this->new_session_callback_ = callback;
}

void Service::on_close_session( session_callback_t callback )
{
    this->close_session_callback_ = callback;
}

Session * Service::create_session()
{
    Session * ret = new Session( this );
    return ret;
}

void Service::add_session( Session * session )
{
    this->session_list_.push_back( session );
}

void Service::remove_session( Session * session )
{
    for ( auto itr = this->session_list_.begin( );
        itr != this->session_list_.end( );
        itr++ )
    {
        if ( ( *itr ) == session )
        {
            itr = this->session_list_.erase( itr );
            SAFE_DELETE( session );
        }
    }
}
