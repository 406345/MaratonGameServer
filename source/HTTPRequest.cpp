#include "HTTPRequest.h"

HTTPRequest::HTTPRequest( )
{
    this->parse_state_ = HTTPState::kMethod;
    method_                 = "GET";
    path_                   = "/";
    protocol_version_       = "HTTP/1.1";
}

Buffer HTTPRequest::build_http_header( )
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