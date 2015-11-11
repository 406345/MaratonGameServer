#include "Session.h"
#include <stdio.h>
#include "Service.h"

Session::Session( Service* service )
{
    static size_t cur_session_id    = 0;
    cur_session_id                  = ( cur_session_id + 1 ) % 0xFFFFFFFF;
    this->session_id_               = cur_session_id;
    this->recive_buffer_            = new char[SESSION_RECIVE_BUFFER_SIZE];

    memset( this->recive_buffer_, 0, SESSION_RECIVE_BUFFER_SIZE );
    this->service = service;
}

Session::~Session()
{
    SAFE_DELETE( this->recive_buffer_ );
}

void Session::on_recive_data( const char * data, size_t length )
{
    printf( "recived size:%lld\r\n",length );
    //send( data, length );
}

void Session::close()
{

}

void Session::send( const char * data, size_t size )
{
    uv_write_t* write   = new uv_write_t();
    uv_buf_t* buffer    = new uv_buf_t();

    buffer->base         = new char[size];

    memcpy( buffer->base, data, size );

    buffer->len          = size;
    write->data         = buffer;

    auto r              = uv_write( 
                                    write, 
                                    (uv_stream_t*) this->listener_, 
                                    buffer, 
                                    1,  
                                    Session::uv_prcoess_write_callback 
                                   );

    if ( r != 0 )
    {
        printf( "write error %s" , uv_strerror(r) );
    }
}

void Session::uv_prcoess_write_callback( uv_write_t * req, int status )
{
    uv_buf_t* buffer = static_cast< uv_buf_t* >( req->data );

    SAFE_DELETE( buffer->base );
    SAFE_DELETE( buffer );
    SAFE_DELETE( req );
}
