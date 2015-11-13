/* * * * * * * * * * * * * * * *
* YHGenomics Inc.
* Author     : yang shubo
* Date       : 2015-11-12
* Description:
* * * * * * * * * * * * * * * */

#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include <string>
#include <stdio.h>
#include <functional>
#include "uv.h"
#include "HTTPRequest.h"

class HTTPClient
{
public:
    
    typedef std::function<void( HTTPClient* client, std::string html)> http_context_callback_t;

    HTTPClient();
    ~HTTPClient();

    void request( HTTPRequest * req );

private:

    enum HTTPClientStatus
    {
        kUnknow,
        kQueryDNS,
        kConnecting,
        kSending,
        kWorking
    }; 
    
    static std::string      http_header_;
    
    sockaddr                uv_getnameinfo_sockaddr_;
    uv_getnameinfo_t        uv_getnameinfo_req_;
    addrinfo                uv_addrinfo_;
    uv_getaddrinfo_t        uv_resolver_;
    uv_tcp_t                uv_socket_;
    uv_buf_t                uv_buffer_;
    uv_connect_t            uv_connect_;

    http_context_callback_t context_callback_;
    HTTPClientStatus        status_ = HTTPClientStatus::kUnknow;
    HTTPRequest *           request_;

    void write_data( const char* data, size_t len);
   
    static void uv_process_resolved     ( uv_getaddrinfo_t* req,
                                          int status,
                                          struct addrinfo* res );
    static void uv_process_connected    ( uv_connect_t* req, 
                                          int status );
    static void uv_process_write        ( uv_write_t* req, 
                                          int status );
    static void uv_process_alloc        ( uv_handle_t* handle,
                                          size_t suggested_size,
                                          uv_buf_t* buf );
    static void uv_process_read         ( uv_stream_t* stream,
                                          ssize_t nread,
                                          const uv_buf_t* buf );
    static void uv_process_close        ( uv_handle_t* handle );

};

#endif // !HTTP_CLIENT_H_
