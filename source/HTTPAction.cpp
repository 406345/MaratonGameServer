#include "HTTPAction.h"

HTTPAction::HTTPAction()
{
    this->header( "User-Agent" , "com.maraton.yhgenomics" );
    this->header( "Accept" , "*/*" );
}

HTTPAction::~HTTPAction()
{
}

void HTTPAction::content( Buffer content )
{
    this->content_ = content;
}

Buffer HTTPAction::content( )
{
    return this->content_;
}

size_t HTTPAction::parse( Buffer & buffer )
{
    int index = 0;
    char* pdata = buffer.data( );
    do
    {
        switch ( this->parse_state_ )
        {
            case HTTPState::kResponseProtocolVersion:

                if ( pdata[index] != ' ' )
                {
                    this->protocol_version_ += pdata[index];
                }
                else
                {
                    this->parse_state_ = HTTPState::kStatus;
                }

                break;
            case HTTPState::kStatus:

                if ( pdata[index] != ' ' )
                {
                    this->status_ += pdata[index];
                }
                else
                {
                    this->status_num_ = atoll( this->status_.c_str( ) );
                    this->parse_state_ = HTTPState::kStatusMessage;
                    this->status_= "";
                }

                break;
            case HTTPState::kStatusMessage:

                if ( pdata[index] != '\r' )
                {
                    this->status_ += pdata[index];
                }
                else
                {
                    this->parse_state_ = HTTPState::kHeaderKey;
                    ++index;
                }

                break;
            case HTTPState::kMethod:

                if ( pdata[index] != ' ' )
                {
                    this->method_ += pdata[index];
                }
                else
                {
                    this->parse_state_ = HTTPState::kPath;
                    ++index;
                }

                break;
            case HTTPState::kPath :
                
                if( pdata[index] != ' ' )
                {
                    this->path_ += pdata[index];
                }
                else
                {
                    this->parse_state_ = HTTPState::kProtocolVersion;
                    ++index;
                }

                break;
            case HTTPState::kProtocolVersion:

                if( pdata[index] != '\r' )
                {
                    this->path_ += pdata[index];
                }
                else
                {
                    this->parse_state_ = HTTPState::kHeaderKey;
                    ++index;
                }

                break;
            case HTTPState::kHeaderKey:

                if ( pdata[index] == '\r' )
                {
                    this->parse_state_ = HTTPState::kContent;
                    auto str_content_length = this->header( "Content-Length" );
                    this->content_length( atoll( str_content_length.c_str( ) ) );
                    this->content_ = Buffer( 1024 * 1024 * 5 );
                    ++index;
                    break;
                }

                if ( pdata[index] != ':' )
                {
                    this->parse_header_key_ += pdata[index];
                }
                else
                {
                    this->parse_state_ = HTTPState::kHeaderValue;
                    ++index;
                }

                break;
            case HTTPState::kHeaderValue:

                if ( pdata[index] != '\r' )
                {
                    if ( pdata[index] != ' ' )
                        this->parse_header_value_ += pdata[index];
                }
                else
                {
                    this->parse_state_ = HTTPState::kHeaderKey;
                    this->header( this->parse_header_key_ , this->parse_header_value_ );
                    this->parse_header_key_ = "";
                    this->parse_header_value_ = "";
                    ++index;
                }

                break; 
            case HTTPState::kContent:
                this->content_.push( buffer.data( ) + index , buffer.size( ) - index );
                return buffer.size( );
                break;
            default:
                break;
        }

        ++index; 
    }
    while ( index < buffer.size( ) );

    return index;
}

void HTTPAction::callback_send_content( callback_t_buffer callback )
{
    callback_send_content_ = callback;
}

void HTTPAction::callback_receive_content( callback_t_void_buffer callback )
{
    this->callback_receive_content_ = callback;
} 

void HTTPAction::callback_send_complete( callback_t callback )
{
    this->callback_send_complete_ = callback;
}
  
void HTTPAction::header( std::string key , std::string value )
{
    this->header_[key] = value;
}

void HTTPAction::data( void * value )
{
    this->data_ = value;
}

std::string HTTPAction::header( std::string key )
{
     return this->header_[key];
}

Buffer HTTPAction::build_http_header( )
{
    std::string context = "";

    char content_length_char[32] = { 0 };

    itoa( this->content_length_ , content_length_char , 10 );
    this->header( "Content-Length" , content_length_char );

    context += this->method( )
        + " "
        + this->path_
        + " "
        + this->protocol_version_
        + "\r\n";

    for ( auto itr = this->header_.begin( );
    itr != this->header_.end( );
        itr++ )
    {
        auto kv = *itr;
        context += kv.first + ": " + kv.second + "\r\n";
    }

    context += "\r\n";

    Buffer result( context.c_str( ) , context.size( ) );

    return MOVE( result );
}
