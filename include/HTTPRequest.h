/* * * * * * * * * * * * * * * *
* YHGenomics Inc.
* Author     : yang shubo
* Date       : 2015-11-13
* Description:
* * * * * * * * * * * * * * * */

#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include <string>
#include <vector>
#include <map>

#include "Define.h"

class HTTPUrlParser
{
public:

    HTTPUrlParser();
    HTTPUrlParser( std::string url );
    HTTPUrlParser( HTTPUrlParser& parser );
    HTTPUrlParser( HTTPUrlParser&& parser );
    ~HTTPUrlParser();

    void parse( std::string url );
    std::string host() { return this->host_; };
    std::string path() { return this->path_; }
    std::string protocol() { return this->protocol_; }

private:

    std::string host_;
    std::string path_;
    std::string protocol_;
    std::string url_;

    void parse();
};



class HTTPRequest
{
public:

    HTTPRequest();
    ~HTTPRequest();

    PP_DEF( std::string, method );
   
    PP_DEF( size_t, content_length );
   
    std::string         url() { return this->url_; };
    void                url( std::string value ) { this->url_ = value; this->url_parser_.parse( value ); };

    std::string         host_ip() { return this->host_ip_; };
    void                host_ip( std::string value ) { this->host_ip_ = value; this->header( "Host", this->host_ip_ ); };

    std::string         host() { return this->url_parser_.host(); };

    void                header( std::string key,
                                std::string value ) { this->header_[key] = value; };
    std::string         header( std::string key ) { return this->header_[key]; };

    std::string         build_header();
    virtual std::string build_body();

protected:


private:


    std::map<std::string, std::string>  header_;
    std::string                         method_;
    std::string                         url_;
    std::string                         host_ip_;
    size_t                              content_length_ = 0;
    HTTPUrlParser                       url_parser_;

    friend class HTTPClient;
};

#endif // !HTTP_REQUEST_H_
