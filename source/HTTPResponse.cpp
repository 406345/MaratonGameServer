#include "HTTPResponse.h"

HTTPResponse::HTTPResponse( )
{
    this->parse_state_ = HTTPState::kResponseProtocolVersion;
    method_                 = "";
    path_                   = "";
    protocol_version_       = "";
}

Buffer HTTPResponse::build_http_header( )
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
    context += std::string( this->content_.data( ) , this->content_.size( ) );

    Buffer result( context.c_str( ) , context.size( ) );

    return MOVE( result );
}
