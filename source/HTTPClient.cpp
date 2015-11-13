#include "HTTPClient.h"
#include "Define.h"

std::string HTTPClient::http_header_ =
                                        "Accept: */*\r\n" 
                                        "Accept-Language: zh-cn\r\n"
                                        "User-Agent: YHGenomics\r\n";


HTTPClient::HTTPClient()
{
    this->uv_addrinfo_              = { 0 };
    this->uv_buffer_                = { 0 };
    this->uv_connect_               = { 0 };
    this->uv_getnameinfo_req_       = { 0 };
    this->uv_getnameinfo_sockaddr_  = { 0 };
    this->uv_buffer_.base           = new char[1024];
    this->uv_buffer_.len            = 1024;

    memset( this->uv_buffer_.base, 0, this->uv_buffer_.len );
}

HTTPClient::~HTTPClient()
{

}

void HTTPClient::request( HTTPRequest * req )
{
    this->request_ = req;

    this->status_ = HTTPClient::HTTPClientStatus::kQueryDNS;

    auto loop = uv_default_loop();

    this->uv_addrinfo_.ai_family = PF_INET;
    this->uv_addrinfo_.ai_socktype = SOCK_STREAM;
    this->uv_addrinfo_.ai_protocol = IPPROTO_TCP;
    this->uv_addrinfo_.ai_flags = 0;
    this->uv_resolver_.data = this;

    int r = uv_getaddrinfo( loop, 
                            &this->uv_resolver_, 
                            HTTPClient::uv_process_resolved, 
                            this->request_->host().c_str(), 
                            NULL, 
                            &this->uv_addrinfo_ );

    UV_ERROR( r );

}

void HTTPClient::write_data(const char * data, size_t len )
{
    if ( this->status_ != HTTPClient::HTTPClientStatus::kSending )return;

    int r = 0;
    uv_write_t* write_req = new uv_write_t();
    memset( write_req, 0, sizeof( uv_write_t ) );
    write_req->data = this;

    uv_buf_t* buffer = new uv_buf_t();

    buffer->len = len;
    buffer->base = new char[buffer->len];
    memset( buffer->base, 0, buffer->len );
    memcpy( buffer->base, data, len );
    
    printf( "%s", buffer->base );

    r = uv_write( write_req, 
                  ( uv_stream_t* ) &this->uv_socket_, 
                  buffer, 
                  1, 
                  HTTPClient::uv_process_write );
    UV_ERROR( r );

}

void HTTPClient::uv_process_resolved( uv_getaddrinfo_t * req,
                                      int status, 
                                      addrinfo * res )
{
    if ( status < 0 ) {
        fprintf( stderr, "getaddrinfo callback error %s\n", uv_err_name( status ) );
        return;
    }

    HTTPClient* client          = static_cast< HTTPClient* >( req->data );

    char addr[17]               = { '\0' };
    uv_ip4_name     ( ( struct sockaddr_in* ) res->ai_addr, addr, 16 );
    struct sockaddr_in recv_addr;
    uv_ip4_addr( addr, 80, &recv_addr );
    uv_connect_t *connect_req   = &client->uv_connect_;
    connect_req->data           = client;

    uv_tcp_init     ( uv_default_loop(), &client->uv_socket_ );

    uv_tcp_connect  ( connect_req, &client->uv_socket_, 
                      ( const struct sockaddr* ) &recv_addr, 
                      HTTPClient::uv_process_connected );

    uv_freeaddrinfo ( res );
    client->status_             = HTTPClient::HTTPClientStatus::kConnecting;

    client->request_->host_ip( addr );
}

void HTTPClient::uv_process_connected( uv_connect_t * req, int status )
{
    if ( status < 0 )
    {
        UV_ERROR( status );
        return;
    }
    int r                   = 0;
    HTTPClient* client      = static_cast< HTTPClient* >( req->data );
    client->uv_socket_.data = client;
    client->status_         = HTTPClient::HTTPClientStatus::kSending;  
    std::string content     = client->request_->build_header();
    
    client->write_data( content.c_str(), content.length() );

    r = uv_read_start( ( uv_stream_t* ) &client->uv_socket_,
                       HTTPClient::uv_process_alloc, 
                       HTTPClient::uv_process_read );
    UV_ERROR( r );
}

void HTTPClient::uv_process_write( uv_write_t * req, int status )
{
    HTTPClient* client = static_cast< HTTPClient* >( req->data );

    delete req;
}

void HTTPClient::uv_process_alloc( uv_handle_t * handle, 
                                   size_t suggested_size, 
                                   uv_buf_t * buf )
{
    buf->base = new char[suggested_size] {0};
    buf->len = (int)suggested_size;
}

void HTTPClient::uv_process_read( uv_stream_t * stream, 
                                  ssize_t nread, 
                                  const uv_buf_t * buf )
{
    if ( stream == NULL )return;
    if ( nread < 0 )
    {
        delete buf->base;
        UV_ERROR( (int)nread );
        uv_close( ( uv_handle_t* ) stream , HTTPClient::uv_process_close );
        return;
    }

    HTTPClient* client = static_cast< HTTPClient* >( stream->data );
     
    printf( "%s", buf->base );

    delete buf->base;
}

void HTTPClient::uv_process_close( uv_handle_t * handle )
{
    HTTPClient* client = static_cast< HTTPClient* >( handle->data );
    SAFE_DELETE( client );
}
 