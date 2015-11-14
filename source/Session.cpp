#include "Session.h"
#include <stdio.h>
#include "Service.h"
#include "Server.h"

Session::Session( Service * service )
{
    this->session_id_               = create_session_id();
    this->service                   = service;
}

Session::~Session()
{
    SAFE_DELETE( this->uv_connect_ );
    SAFE_DELETE( this->uv_tcp_ );
}

void Session::on_recive_data( Buffer & buffer )
{
    char char_buf[512]= { 0 };
    sprintf( char_buf , "Session %d response\r\n" , this->id( ) );
    Buffer buf( char_buf , 512);
    this->send( buf );
}

void Session::on_close( )
{
}

void Session::close()
{
    this->on_close( );
    uv_tcp_t* uv_tcp  = this->uv_tcp_;

    if( uv_tcp == nullptr )
    {
        uv_tcp = this->service->uv_tcp_;
    }
    
    uv_close( (uv_handle_t* ) uv_tcp , Service::uv_callback_close );
}

void Session::send( Buffer & buffer )
{
    uv_write_t* write   = new uv_write_t();
    uv_buf_t*   buf     = new uv_buf_t();
    uv_tcp_t*   uv_tcp  = this->uv_tcp_;
    buf->base           = new char[buffer.size()] { 0 };
    buf->len            = buffer.size();
    write->data         = buf;

    if( uv_tcp == nullptr )
    { 
        // This session uses service's uv_tcp_t 
        // to send data when it is a client
        // otherwise it uses it's own uv_tcp_t
        uv_tcp = this->service->uv_tcp_;
    }

    memcpy( buf->base, buffer.data(), buffer.size() );
    
    auto r  = uv_write( write, 
                        (uv_stream_t*) uv_tcp, 
                        buf, 
                        1,  
                        Session::uv_prcoess_write_callback );

    if ( r != 0 )
    {
        LOG_DEBUG("uv errors:%s",UV_ERROR(r));
    }
}

void Session::uv_prcoess_write_callback( uv_write_t * req, int status )
{
    uv_buf_t* buffer = static_cast< uv_buf_t* >( req->data );

    SAFE_DELETE( buffer->base );
    SAFE_DELETE( buffer );
    SAFE_DELETE( req );
}

size_t Session::create_session_id( )
{
    static size_t session_id    = 0;
    session_id                  = ( session_id + 1 ) % 0xFFFFFFFF;
    return session_id;
}
