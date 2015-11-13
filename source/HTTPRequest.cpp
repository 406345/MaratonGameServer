#include "HTTPRequest.h"


HTTPUrlParser::HTTPUrlParser()
{
}

HTTPUrlParser::HTTPUrlParser( std::string url )
{
    this->url_ = url;
    parse();
}

HTTPUrlParser::HTTPUrlParser( HTTPUrlParser & parser )
{
    this->host_     = parser.host_;
    this->path_     = parser.path_;
    this->protocol_ = parser.protocol_;
}

HTTPUrlParser::HTTPUrlParser( HTTPUrlParser && parser )
{
    this->host_      = parser.host_;
    this->path_      = parser.path_;
    this->protocol_  = parser.protocol_;
                     
    parser.host_     = nullptr;
    parser.path_     = nullptr;
    parser.protocol_ = nullptr;
}

HTTPUrlParser::~HTTPUrlParser()
{

}

void HTTPUrlParser::parse( std::string url )
{
    this->url_ = url;
    this->parse();
}

void HTTPUrlParser::parse()
{

    //http://www.baidu.com/axxx/xxx/xxxx;
    const char* purl = this->url_.c_str();
    int len = this->url_.length();

    int protocol_begin   = 0;
    int protocol_end     = 0;
    int domain_pos_begin = 0;
    int domain_pos_end   = len;

    int state = 0;


    for ( int i = 0; i < len; i++ )
    {
        if ( purl[i] == ' ' )
        {
            protocol_begin = i;
            continue;
        }
        switch ( state )
        {
            case 0: // search for '//'

                if ( i > 3 && purl[i - 1] == '/' && purl[i - 2] == '/' )
                {
                    protocol_end     = i - 3;
                    domain_pos_begin = i;
                    state++;
                }
                break;
            case 1: // search for '/'

                if ( purl[i] == '/' )
                {
                    state++;
                    domain_pos_end = i;
                }
                break;
            default:

                break;
        }
    }

    this->protocol_ = std::string( purl + protocol_begin , 
                                   protocol_end - protocol_begin );

    this->host_     = std::string( purl + domain_pos_begin , 
                                   domain_pos_end - domain_pos_begin );

    if ( len == domain_pos_end )
    {
        this->path_ = "/";
    }
    else
    {
        this->path_ = std::string( purl + domain_pos_end ,
                                   len - domain_pos_end );
    }

}

HTTPRequest::HTTPRequest()
{
    this->header( "User-Agent" , "com.maraton.yhgenomics" );
}

HTTPRequest::~HTTPRequest()
{
}

std::string HTTPRequest::build_body()
{
    return std::string();
}

std::string HTTPRequest::build_header()
{
    std::string context = "";

    url_parser_.parse( this->url() );

    if ( url_parser_.host().empty() )
    {
        return "";
    }

    char content_length_char[32] = { 0 };

    itoa( this->content_length_ , content_length_char , 10 );
    this->header( "Content-Length" , content_length_char );

    context += this->method() 
             + " " 
             + url_parser_.path() 
             + " HTTP/1.1\r\n";

    for ( auto itr = this->header_.begin(); 
          itr != this->header_.end(); 
          itr++ )
    {
        auto kv = *itr;
        context += kv.first + ": " + kv.second + "\r\n";
    }

    context += "\r\n";

    return context;
}
