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
    buf->len  = SESSION_RECIVE_BUFFER_LENGTH;
}

void Server::uv_callback_read( uv_stream_t* stream,
                              ssize_t nread,
                              const uv_buf_t* buf )
{
    Session* session = static_cast< Session* >( stream->data );

    if ( nullptr == session ) return;

    if ( 0 > nread )
    {
        uv_close( ( uv_handle_t* ) session->conn_, uv_callback_close ); 
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

void Server::uv_callback_on_new_connection( uv_stream_t* server, int status )
{
    if ( status < 0 ) {
        fprintf( stderr, "New connection error %s\n", uv_strerror( status ) );
        return;
    }
    Service* service = static_cast< Service* >( server->data );

    Session* session = service->create_session();
    uv_tcp_init( Server::loop(), session->conn_ );

    if ( uv_accept( server, ( uv_stream_t* ) session->conn_ ) == 0 )
    {
        if ( service->new_session_callback_ != nullptr )
        {
            service->new_session_callback_( session );
        }

        uv_read_start( ( uv_stream_t* ) session->conn_, uv_callback_alloc_buffer, uv_callback_read );
    }
    else 
    {
        uv_close( ( uv_handle_t* ) session->conn_, uv_callback_close );
    }
}

uv_loop_t * Server::loop()
{
    return uv_default_loop();
}

void Server::add_service( Service * srv )
{
    int r = uv_listen( ( uv_stream_t* ) &srv->sock_, 0, uv_callback_on_new_connection );
    service_list.push_back( srv );
}

void Server::run()
{
    /*for ( Service* srv : this->service_list )
    {
    }*/

    uv_run( Server::loop(), UV_RUN_DEFAULT );
}
