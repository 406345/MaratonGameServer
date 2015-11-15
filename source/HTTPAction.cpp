#include "HTTPAction.h"

HTTPAction::HTTPAction()
{
    this->header( "User-Agent" , "com.maraton.yhgenomics" );
    this->header( "Accept" , "*/*" );
}

HTTPAction::~HTTPAction()
{
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
            default:
                break;
        }

        ++index;

        if( this->parse_state_ == HTTPState::kContent )
        {
            return index;
        }
    }
    while ( index < buffer.size( ) );

    return index;
}

bool HTTPAction::finished( )
{
    return this->message_state_ == HTTPState::kHeader;
}

void HTTPAction::header( std::string key , std::string value )
{
    this->header_[key] = value;
}

std::string HTTPAction::header( std::string key )
{
     return this->header_[key];
}

Buffer HTTPAction::build_message( )
{
    if ( this->message_state_ == HTTPState::kHeader )
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

        this->message_state_ = HTTPState::kContent;
        
        Buffer result( context.c_str( ) , context.size( ) );

        return MOVE(result);
    }
    else if ( this->message_state_ == HTTPState::kContent )
    {
        if ( this->content_length_ > this->content_pos_ )
        {
            size_t delta = this->content_length_ - this->content_pos_;
            if( delta > MAX_CONTENT_BUILD_SIZE )
            {
                Buffer result( this->content_ , MAX_CONTENT_BUILD_SIZE );
                this->content_pos_+= MAX_CONTENT_BUILD_SIZE;
                return MOVE(result);
            }
            else
            {
                Buffer result( this->content_ , delta );
                this->content_pos_ = 0;
                this->message_state_ = HTTPState::kHeader;
                return MOVE(result);
            }
        }
    }

    return MOVE(Buffer( nullptr , 0 ));
}
