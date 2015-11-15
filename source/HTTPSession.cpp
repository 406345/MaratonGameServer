#include "HTTPSession.h"

HTTPSession::HTTPSession( HTTPService * service_ )
    : Session( service_ )
{
}

bool HTTPSession::send_request( HTTPRequest * req )
{
    if ( this->request_ != nullptr )
    {
        this->request_ = req;
    }

    this->request_->header( "Host", this->ip() );
    auto msg = this->request_->build_http_header( );
    this->send( msg );
    LOG_DEBUG( "%s" , msg.data( ) );
    return true;
}

bool HTTPSession::send_response( HTTPResponse * rep )
{
    if ( this->response_ != nullptr )
    {
        this->response_ = rep;
    } 

    this->response_->header( "Host", this->ip() );
    auto msg = this->response_->build_http_header( );
    this->send( msg );
    LOG_DEBUG( "%s" , msg.data( ) );
    return true;
}

void HTTPSession::send( Buffer & buffer )
{
    Session::send( buffer );
}

void HTTPSession::on_send_finish( size_t size )
{
    HTTPAction* action_ = this->request_ == nullptr ?
        static_cast< HTTPAction* >( this->response_ ) :
        static_cast< HTTPAction* >( this->request_ );

    if ( action_ == nullptr )
    {
        return;
    }

    if( action_->callback_send_content_ == nullptr )
    {
        if( action_->callback_send_complete_ != nullptr )
        {
            action_->callback_send_complete_( action_ );
        }

        return;
    }

    auto msg = action_->callback_send_content_( action_ );

    if( msg == nullptr )
    {
        if( action_->callback_send_complete_ != nullptr )
        {
            action_->callback_send_complete_( action_ );
        }
        return;
    }

    this->send( *msg );
    SAFE_DELETE( msg );
}

void HTTPSession::on_connected( )
{ 
}

void HTTPSession::on_receive_data( Buffer & buffer )
{
    if( this->service()->service_type() == Service::ServiceType::kClient )
    {
        SAFE_DELETE( this->response_ );
        this->response_ = new HTTPResponse( );
        this->response_->parse( buffer );
    }
    else
    {
        SAFE_DELETE( this->request_ );
        this->request_ = new HTTPRequest( );
        this->request_->parse( buffer );
    }

    LOG_DEBUG( "%s" , buffer.data( ) );
}

void HTTPSession::on_close( )
{

}
