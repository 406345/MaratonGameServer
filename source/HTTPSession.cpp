#include "HTTPSession.h"

bool HTTPSession::send_request( HTTPAction * req )
{
    if ( request != nullptr )
    {
        if ( request->finished() )
        {
            SAFE_DELETE( request );
        }
        else
        {
            return false;
        }
    }

    request = req;
    request->header( "Host", this->ip() );
    auto msg = request->build_message( );
    this->send( msg );
    LOG_DEBUG( "%s" , msg.data( ) );
    return true;
}

void HTTPSession::on_send_finish( size_t size )
{
    if ( request == nullptr )
    {
        return;
    }

    if( request->finished() )
    {
        return;
    }

    auto msg = request->build_message( );

    if( msg.size() == 0 )
    {
        return;
    }

    this->send( msg );
    LOG_DEBUG( "%s" , msg.data( ) );
}

void HTTPSession::on_connected( )
{

}

void HTTPSession::on_receive_data( Buffer & buffer )
{
    LOG_DEBUG( "%s" , buffer.data( ) );
}

void HTTPSession::on_close( )
{

}
