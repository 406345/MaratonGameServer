#include "stdio.h"
#include "Server.h"
#include "Session.h"
#include "Service.h"

void Server::uv_callback_alloc_buffer( uv_handle_t* handle,
                                      size_t suggested_size,
                                      uv_buf_t* buf )
{
    Session* session = static_cast< Session* >( handle->data );

    if ( nullptr == session ) return; 

    buf->base = session->recive_buffer();
    buf->len  = SESSION_RECIVE_BUFFER_SIZE;
}

void Server::uv_callback_read( uv_stream_t* stream,
                              ssize_t nread,
                              const uv_buf_t* buf )
{
    Session* session = static_cast< Session* >( stream->data );

    if ( nullptr == session ) return;

    if ( 0 > nread )
    {
        uv_close( ( uv_handle_t* ) session->listener_, uv_callback_close ); 
        return;
    }

    session->on_recive_data( buf->base, nread );
}

void Server::uv_callback_close( uv_handle_t* handle )
{
    Session* session = static_cast< Session* >( handle->data );

    if ( nullptr == session ) return;
 
    session->close();
    session->service->close_session_callback_( session );

    SAFE_DELETE( session );
}

void Server::uv_callback_new_connection( uv_stream_t* server, int status )
{
    if ( status < 0 ) {
        fprintf( stderr, "New connection error %s\n", uv_strerror( status ) );
        return;
    }
    Service* service = static_cast< Service* >( server->data );

    Session* session = service->create_session();
    uv_tcp_init( Server::loop(), session->listener_ );

    session->listener_ = new uv_tcp_t();

    if ( uv_accept( server, ( uv_stream_t* ) session->listener_ ) == 0 )
    {
        if ( service->new_session_callback_ != nullptr )
        {
            service->new_session_callback_( session );
        }

        uv_read_start( ( uv_stream_t* ) session->listener_, uv_callback_alloc_buffer, uv_callback_read );
    }
    else 
    {
        uv_close( ( uv_handle_t* ) session->listener_, uv_callback_close );
    }
}

void Server::uv_callback_connected( uv_connect_t * req, int status )
{
    Service* service = static_cast< Service* > ( req->data );
    if ( status < 0 )
    {
        auto sock = ( uv_tcp_t* ) req->data;
         
        return;
    }

    Session* session = service->create_session();
    session->listener_ = &service->sock_;
    session->listener_->data = session;
    service->new_session_callback_( session );
    uv_read_start( ( uv_stream_t* ) session->listener_, uv_callback_alloc_buffer, uv_callback_read );
}

uv_loop_t * Server::loop()
{
    return uv_default_loop();
}

bool Server::add_service( Service * srv )
{
    int result = 0;

    if ( srv->service_type_ == Service::ServiceType::kUnknow )
        return false;

    if ( srv->service_type_ == Service::ServiceType::kServer )
    {
        result = uv_listen( ( uv_stream_t* ) &srv->sock_, 0, uv_callback_new_connection );
    }
    else if ( srv->service_type_ == Service::ServiceType::kClient )
    {
        result = uv_tcp_connect( &srv->conn_, &srv->sock_, ( const struct sockaddr* ) &srv->addr_in, Server::uv_callback_connected );
    }

    if ( result != 0 )
    {
        printf( "error: %s", uv_strerror( result ) );
    }

    service_list.push_back( srv );
    return true;
}

void Server::run()
{ 
    uv_run( Server::loop(), UV_RUN_DEFAULT );
}
