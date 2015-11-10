#include "Session.h"
#include <stdio.h>
#include "Service.h"

Session::Session( Service* service )
{
    static size_t cur_session_id = 0;
    this->session_id_ = cur_session_id;
    cur_session_id = ( cur_session_id + 1 ) % 0xFFFFFFFF;
    
    this->conn_ = new uv_tcp_t();
    this->conn_->data = this;
    this->recive_buffer_ = new char[SESSION_RECIVE_BUFFER_LENGTH];
    this->service = service;

}

Session::~Session()
{
    SAFE_DELETE( this->conn_ );
    SAFE_DELETE( this->recive_buffer_ );
}

void Session::on_recive_data( const char * data, size_t length )
{
    send( data, length );
}

void Session::close()
{
}

void Session::send( const char * data, size_t size )
{
    uv_write_t* write = new uv_write_t();
    uv_buf_t buffer;

    buffer.base = new char[size];
    memcpy( buffer.base, data, size );
    buffer.len = size;

    write->data = this;
    auto r = uv_write( write, (uv_stream_t*) this->conn_, &buffer, 1,  Session::uv_prcoess_write_callback );
    if ( r != 0 )
    {
        printf( "write error %s" , uv_strerror(r) );
    }
}

void Session::uv_prcoess_write_callback( uv_write_t * req, int status )
{
    SAFE_DELETE( req->write_buffer.base );
    SAFE_DELETE( req );
}
