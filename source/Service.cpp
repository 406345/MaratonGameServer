#include "Service.h"
#include "SessionManager.h"

Service::Service( )
{
    this->uv_connect_  = new uv_connect_t( );
    this->uv_resolver_ = new uv_getaddrinfo_t( );
    this->uv_tcp_      = new uv_tcp_t( );

    memset( this->uv_connect_ , 0 , sizeof( uv_connect_t ) );
    memset( this->uv_resolver_ , 0 , sizeof( uv_getaddrinfo_t ) );
    memset( this->uv_tcp_ , 0 , sizeof( uv_tcp_t ) );

    this->uv_connect_->data  = this;
    this->uv_resolver_->data = this;
    this->uv_tcp_->data      = this;
}

Service::~Service( )
{
    SAFE_DELETE( this->uv_connect_ );
    SAFE_DELETE( this->uv_resolver_ );
    SAFE_DELETE( this->uv_tcp_ );
}

void Service::listen( const char * host , int port )
{
    this->service_type_             = ServiceType::kServer;
    this->port_                     = port;

    this->addrinfo_.ai_family       = PF_INET;
    this->addrinfo_.ai_socktype     = SOCK_STREAM;
    this->addrinfo_.ai_protocol     = IPPROTO_TCP;
    this->addrinfo_.ai_flags        = 0;
    this->uv_resolver_->data        = this;

    memcpy( this->host_ , host , strlen( host ) );
    int r = uv_getaddrinfo( Service::loop( ) ,
                            this->uv_resolver_ ,
                            Service::uv_process_resolved ,
                            this->host_ ,
                            NULL ,
                            &this->addrinfo_ );
}

void Service::connect( const char * host , int port )
{
    this->service_type_             = ServiceType::kClient;
    this->port_                     = port;

    this->addrinfo_.ai_family       = PF_INET;
    this->addrinfo_.ai_socktype     = SOCK_STREAM;
    this->addrinfo_.ai_protocol     = IPPROTO_TCP;
    this->addrinfo_.ai_flags        = 0;
    this->uv_resolver_->data        = this;

    memcpy( this->host_ , host , strlen( host ) );
    int r = uv_getaddrinfo( Service::loop( ) ,
                            this->uv_resolver_ ,
                            Service::uv_process_resolved ,
                            this->host_ ,
                            NULL ,
                            &this->addrinfo_ );
    LOG_DEBUG_UV( r );
}

void Service::stop( )
{
    for ( auto itr = this->session_list_.begin( );
          itr != this->session_list_.end( ); itr++)
    {
        ( *itr )->close( );
    }
}

Service::ServiceType Service::service_type( )
{
    return this->service_type_;
}

void Service::on_operation_failed( service_status_callback_t callback )
{
    this->operation_failed_callback_ = callback;
}

void Service::on_open_session( session_callback_t callback )
{
    this->new_session_callback_ = callback;
}

void Service::on_close_session( session_callback_t callback )
{
    this->close_session_callback_ = callback;
}

uv_loop_t * Service::loop( )
{
    return uv_default_loop( );
}

Session * Service::create_session( )
{
    Session * ret = new Session( this );
    return ret;
}

void Service::add_session( Session * session )
{
    this->session_list_.push_back( session );
}

void Service::remove_session( Session * session )
{

    for ( auto itr = this->session_list_.begin( );
          itr != this->session_list_.end( ); itr++)
    {
        if ( ( *itr ) == session )
        {
            this->session_list_.erase( itr );
            
            if( this->close_session_callback_ != nullptr )
            {
                this->close_session_callback_( *itr );
            }

            SAFE_DELETE( session );
            break;
        }
    }
}

void Service::uv_callback_alloc_buffer( uv_handle_t * handle ,
                                        size_t suggested_size ,
                                        uv_buf_t * buf )
{
    buf->base = new char[suggested_size];
    buf->len  = suggested_size;
}

void Service::uv_callback_read( uv_stream_t * stream ,
                                ssize_t nread ,
                                const uv_buf_t * buf )
{
    Session* session = static_cast< Session* >( stream->data );
    
    if ( nullptr == session ) return;
    
    if ( nread < 0 )
    {
        LOG_DEBUG( "uv error: %s" , UV_ERROR( nread ) );
        session->close( );
        return;
    }
    
    Buffer buffer( buf->base , nread );
    
    session->on_receive_data( buffer );

    delete buf->base;
}

void Service::uv_callback_close( uv_handle_t * handle )
{
    Session * session   = static_cast < Session* > ( handle->data );
    Service* service_    = session->service_;
   
    if( session == nullptr )
    {
        LOG_DEBUG( "Session is nullptr" );
        return;
    } 

    if( service_ == nullptr )
    {
        LOG_DEBUG( "Session owner is nullptr" );
        return;
    }

    session->on_close( );

    if( session->uv_tcp_ == nullptr )
    {
        // Service is client mode
        service_->remove_session( session );
    }
    else
    {
        // Service is server mode
        service_->remove_session( session );
    }
}

void Service::uv_callback_service_close( uv_handle_t * handle )
{
    Service * service_ = static_cast < Service* > ( handle->data );

    if ( service_ == nullptr )
    {
        LOG_DEBUG( "Service is nullptr" );
        return;
    }  
}

void Service::uv_callback_new_connection( uv_stream_t * server , int status )
{
    int result = 0;
    Service * service_ = static_cast < Service* > ( server->data );

    if( status != 0 )
    {
        LOG_DEBUG_UV( status );
        return;
    }

    if( service_ == nullptr )
    {
        LOG_DEBUG( "uv_callback_connected: Service is nullptr" );
        return;
    } 

    LOG_DEBUG( "new connection" );

    Session * session       = service_->create_session( );
    session->uv_tcp_        = new uv_tcp_t( );
    memset( session->uv_tcp_ , 0 , sizeof( uv_tcp_t ) );
    session->uv_tcp_->data  = session;

    uv_tcp_init( Service::loop( ) , session->uv_tcp_ );
    result = uv_accept( server , ( uv_stream_t* ) session->uv_tcp_ );
    LOG_DEBUG_UV( result );

    if ( result == 0 )
    {
        service_->add_session( session );
        
        if( service_->new_session_callback_ != nullptr)
        {
            service_->new_session_callback_( session );
        }

        result = uv_read_start( ( uv_stream_t* ) session->uv_tcp_ , 
                            Service::uv_callback_alloc_buffer , 
                            Service::uv_callback_read );
        LOG_DEBUG_UV( result );
    }
    else
    {
        LOG_DEBUG_UV( result );
        uv_close( ( uv_handle_t* ) session->uv_tcp_ , Service::uv_callback_close );
    }
}

void Service::uv_callback_connected( uv_connect_t * req , int status )
{
    int result = 0;
    Service * service_ = static_cast < Service* > ( req->data );

    if( status != 0 )
    {
        LOG_DEBUG_UV( status );
        
        if ( service_->operation_failed_callback_ != nullptr )
        {
            service_->operation_failed_callback_( service_ , status );
        }

        uv_close( ( uv_handle_t* ) service_->uv_tcp_ , 
                  Service::uv_callback_service_close );
        return;
    }

    if( service_ == nullptr )
    {
        LOG_DEBUG( "uv_callback_connected: Service is nullptr" );
        return;
    }

    LOG_DEBUG( "%s:%d connected" , service_->host_ , service_->port_ );

    Session* session = service_->create_session( );

    if( service_->new_session_callback_ != nullptr )
    {
        service_->new_session_callback_( session );
    }

    service_->uv_tcp_->data  = session;

    service_->add_session( session );
    session->on_connected( );

    result                  = uv_read_start( ( uv_stream_t* ) service_->uv_tcp_ , 
                                            Service::uv_callback_alloc_buffer , 
                                            Service::uv_callback_read );
    LOG_DEBUG_UV( result );
}

void Service::uv_process_resolved( uv_getaddrinfo_t * req ,
                                   int status ,
                                   addrinfo * res )
{
    int result = 0;
    Service * service_ = static_cast < Service* > ( req->data );

    if ( service_ == nullptr )
    {
        LOG_DEBUG( "Service is nullptr" );
        return;
    }
     
    service_->uv_tcp_->data       = service_;
    service_->uv_connect_->data   = service_;
    
    uv_ip4_name( ( struct sockaddr_in* ) res->ai_addr , service_->ip_ , 16 );
    uv_ip4_addr( service_->ip_ , service_->port_ , &service_->addr_in );

    if ( service_->service_type_ == Service::ServiceType::kServer )
    {
        result = uv_tcp_init( Service::loop( ) ,
                              service_->uv_tcp_ );
        LOG_DEBUG_UV( result );

        service_->uv_tcp_->data  = service_;

        result = uv_tcp_bind( service_->uv_tcp_ ,
                              ( const struct sockaddr* )&service_->addr_in ,
                              0 );
        LOG_DEBUG_UV( result );

        result = uv_listen( ( uv_stream_t* ) service_->uv_tcp_ ,
                            0 ,
                            Service::uv_callback_new_connection );
        LOG_DEBUG_UV( result );
    }
    else if ( service_->service_type_ == Service::ServiceType::kClient )
    {
        uv_tcp_init( Service::loop() , service_->uv_tcp_ );
        result = uv_tcp_connect( service_->uv_connect_ , 
                                 service_->uv_tcp_ ,
                                 ( const struct sockaddr* ) &service_->addr_in ,
                                 Service::uv_callback_connected );
        LOG_DEBUG_UV( result );
    }
}
