#include "HTTPResponse.h"

HTTPResponse::HTTPResponse( )
{
    this->parse_state_ = HTTPState::kResponseProtocolVersion;
    method_                 = "";
    path_                   = "";
    protocol_version_       = "";
}

Buffer HTTPResponse::build_message( )
{
    if ( this->message_state_ == HTTPState::kHeader )
    {
        std::string context = "";

        char content_length_char[32] = { 0 };

        itoa( this->content_length_ , content_length_char , 10 );
        this->header( "Content-Length" , content_length_char );

        context += "HTTP/1.1 200 OK\r\n";

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

        return MOVE( result );
    }
    else if ( this->message_state_ == HTTPState::kContent )
    {
        if ( this->content_length_ > this->content_pos_ )
        {
            size_t delta = this->content_length_ - this->content_pos_;
            if ( delta > MAX_CONTENT_BUILD_SIZE )
            {
                Buffer result( this->content_ , MAX_CONTENT_BUILD_SIZE );
                this->content_pos_+= MAX_CONTENT_BUILD_SIZE;
                return MOVE( result );
            }
            else
            {
                Buffer result( this->content_ , delta );
                this->content_pos_ = 0;
                this->message_state_ = HTTPState::kHeader;
                return MOVE( result );
            }
        }
    }

    return MOVE( Buffer( nullptr , 0 ) );
}
