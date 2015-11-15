#include "Session.h"
#include <stdio.h>
#include "Service.h"

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

void Session::on_connected( )
{
}

void Session::on_receive_data( Buffer & buffer )
{
    char char_buf[512]= { 0 };
    sprintf( char_buf , "Session %d response\r\n" , this->id( ) );
    Buffer buf( char_buf , 512);
    this->send( buf );
}

void Session::on_close( )
{
}

void Session::on_send_finish( size_t size )
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
    write_token_t* write_token = new write_token_t;
    write_token->writer         = new uv_write_t();
    write_token->buffer         = new uv_buf_t();
    write_token->buffer->base   = new char[buffer.size()] { 0 };
    write_token->buffer->len    = buffer.size();
    write_token->writer->data   = write_token;
    write_token->session        = this;

    uv_tcp_t*   uv_tcp          = this->uv_tcp_;

    if( uv_tcp == nullptr )
    { 
        // This session uses service's uv_tcp_t 
        // to send data when it is a client
        // otherwise it uses it's own uv_tcp_t
        uv_tcp = this->service->uv_tcp_;
    }

    memcpy( write_token->buffer->base, buffer.data(), buffer.size() );
    
    auto r  = uv_write( write_token->writer, 
                        (uv_stream_t*) uv_tcp, 
                        write_token->buffer, 
                        1,  
                        Session::uv_prcoess_write_callback );

    if ( r != 0 )
    {
        LOG_DEBUG("uv errors:%s",UV_ERROR(r));
    }
}

std::string Session::host( )
{
    return std::string(this->service->host_);
}

std::string Session::ip( )
{
    return std::string(this->service->ip_);
}

void Session::uv_prcoess_write_callback( uv_write_t * req, int status )
{
    if( status < 0 )
    {
        LOG_DEBUG_UV( status );
        return;
    }

    write_token_t* write_token  = static_cast< write_token_t* >( req->data );
    uv_buf_t* buffer            = write_token->buffer;

    write_token->session->on_send_finish( buffer->len );

    SAFE_DELETE( buffer->base );
    SAFE_DELETE( buffer );
    SAFE_DELETE( write_token );
    SAFE_DELETE( req );
}

size_t Session::create_session_id( )
{
    static size_t session_id    = 0;
    session_id                  = ( session_id + 1 ) % 0xFFFFFFFF;
    return session_id;
}
