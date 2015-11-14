#include "stdio.h"
#include "Server.h"
#include "Session.h"
#include "Service.h"
#include "Buffer.h"

void Server::uv_callback_alloc_buffer( uv_handle_t* handle ,
                                       size_t suggested_size ,
                                       uv_buf_t* buf )
{
    Session* session = static_cast< Session* >( handle->data );

    if ( nullptr == session ) return;

    buf->base = session->recive_buffer( );
    buf->len  = SESSION_RECIVE_BUFFER_SIZE;
}

void Server::uv_callback_read( uv_stream_t* stream ,
                               ssize_t nread ,
                               const uv_buf_t* buf )
{
    Session* session = static_cast< Session* >( stream->data );

    if ( nullptr == session ) return;

    if ( 0 > nread )
    {
        LOG_DEBUG( "uv error: %s" , UV_ERROR( nread ) );
        uv_close( ( uv_handle_t* ) session->listener_ , uv_callback_close );
        return;
    }

    Buffer buffer( buf->base , nread );

    session->on_recive_data( std::move( buffer ) );
}

void Server::uv_callback_close( uv_handle_t* handle )
{
    Session* session = static_cast< Session* >( handle->data );

    if ( nullptr == session ) return;

    auto service = session->service;

    session->close( );
    session->service->close_session_callback_( session );

    if ( session->service->service_type_ == Service::ServiceType::kClient )
    {
        session->service->stop( );
        service->remove_session( session );
        SAFE_DELETE( service );
    }
}

void Server::uv_callback_new_connection( uv_stream_t* server , int status )
{
    if ( status < 0 )
    {
        LOG_DEBUG( "uv error: %s" , UV_ERROR( status ) );
        return;
    }

    Service* service = static_cast< Service* >( server->data );

    if ( service == nullptr )
    {
        LOG_DEBUG( "service has been relesed" );
        return;
    }

    Session* session = service->create_session( );
    uv_tcp_init( Server::loop( ) , session->listener_ );

    session->listener_ = new uv_tcp_t( );

    if ( uv_accept( server , ( uv_stream_t* ) session->listener_ ) == 0 )
    {
        if ( service->new_session_callback_ != nullptr )
        {
            service->new_session_callback_( session );
        }

        uv_read_start( ( uv_stream_t* ) session->listener_ ,
                       uv_callback_alloc_buffer ,
                       uv_callback_read );
    }
    else
    {
        uv_close( ( uv_handle_t* ) session->listener_ , uv_callback_close );
    }
}

void Server::uv_callback_connected( uv_connect_t * req , int status )
{
    Service* service = static_cast< Service* > ( req->data );

    if ( service == nullptr )
    {
        LOG_DEBUG( "service has been relesed" );
        return;
    }

    if ( status < 0 )
    {
        LOG_DEBUG( "uv error: %s" , UV_ERROR( status ) );

        if ( service->operation_failed_callback_ != nullptr )
        {
            service->operation_failed_callback_( service , status );
        }

        Server::instance( )->remove_service( service );
        return;
    }

    Session* session = service->create_session( );
    session->listener_ = &service->uv_tcp_;
    session->listener_->data = session;

    service->session_list_.push_back( session );
    service->new_session_callback_( session );

    uv_read_start( ( uv_stream_t* ) session->listener_ ,
                   uv_callback_alloc_buffer ,
                   uv_callback_read );
}

void Server::uv_process_resolved( uv_getaddrinfo_t * req , int status , addrinfo * res )
{
    Service* service            = static_cast< Service* >( req->data );
    char addr[17]               = { 0 };
    service->uv_tcp_.data       = service;
    service->uv_connect_.data   = service;

    uv_ip4_name( ( struct sockaddr_in* ) res->ai_addr , addr , 16 );
    uv_ip4_addr( addr , service->port_ , &service->addr_in );



    auto result                 = uv_tcp_connect( &service->uv_connect_ , &service->uv_tcp_ ,
                                                  ( const struct sockaddr* ) &service->addr_in ,
                                                  Server::uv_callback_connected );
    LOG_UV_ERROR( result );
}

uv_loop_t * Server::loop( )
{
    return uv_default_loop( );
}

bool Server::add_service( Service * srv )
{
    int result = 0;

    if ( srv->service_type_ == Service::ServiceType::kUnknow )
        return false;

    if ( srv->service_type_ == Service::ServiceType::kServer )
    {
        result = uv_listen( ( uv_stream_t* ) &srv->uv_tcp_ ,
                            0 ,
                            uv_callback_new_connection );
    }
    else if ( srv->service_type_ == Service::ServiceType::kClient )
    {
        srv->uv_addrinfo_.ai_family     = PF_INET;
        srv->uv_addrinfo_.ai_socktype   = SOCK_STREAM;
        srv->uv_addrinfo_.ai_protocol   = IPPROTO_TCP;
        srv->uv_addrinfo_.ai_flags = 0;
        srv->uv_resolver_.data = srv;

        int r = uv_getaddrinfo( Server::loop( ) ,
                                &srv->uv_resolver_ ,
                                Server::uv_process_resolved ,
                                srv->host_ ,
                                NULL ,
                                &srv->uv_addrinfo_ );
        LOG_UV_ERROR( r );
    }

    if ( result != 0 )
    {
        printf( "error: %s" , uv_strerror( result ) );
    }

    service_list.push_back( srv );
    return true;
}

bool Server::remove_service( Service * srv )
{
    if ( srv->service_type_ == Service::ServiceType::kClient )
    {
        uv_read_stop( ( uv_stream_t* ) &srv->uv_tcp_ );
    }
    else if ( srv->service_type_ == Service::ServiceType::kClient )
    {

    }

    for ( auto itr = service_list.begin( ); itr != service_list.end( ); itr++ )
    {
        if ( ( *itr ) == srv )
        {
            service_list.erase( itr );
            return true;
        }
    }

    return false;
}

void Server::run( )
{
    uv_run( Server::loop( ) , UV_RUN_DEFAULT );
}
