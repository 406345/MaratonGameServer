#include "Service.h"
#include "SessionManager.h"
#include "Buffer.h"
#include <memory> 
#include "SyncWorker.h"
#include <thread>
#include "HTTPService.h"
#include "HTTPSession.h"
#include "HTTPAction.h"



int main( )
{ 
    char http_req_str1[] =   "POST /abcdef/asdfasdf/ff HTTP/1.1\r\n"
                            "Content-Type:" ;
    char http_req_str2[] =  " abcdef\r";
    char http_req_str3[] =  "\n"
                            "Accept: */*\r\n"
                            "\r\n"
                            "{ 'aaaa' : 'abdef' }";

    char http_rep_str1[] =   "HTTP/1.1 200 OK\r\n"
                            "Content-Type:" ;
    char http_rep_str2[] =  " abcdef\r";
    char http_rep_str3[] =  "\n"
                            "Accept: */*\r\n"
                            "\r\n"
                            "{ 'aaaa' : 'abdef' }";

    //HTTPService* http_server = new HTTPService( );
    //http_server->listen( "localhost" , 8080 );
    //http_server->on_open_session( [ ] ( Session* session )
    //{ 
    //    HTTPSession* http_session = static_cast< HTTPSession* >( session );
    //    
    //    HTTPResponse * rep = new HTTPResponse();
    //    std::string rep_content = std::string("<body>Hello HTTP Server</body>");

    //    rep->content_length( rep_content.size() );
    //    rep->content( rep_content );
    //    rep->callback_send_complete( [ http_session ] ( HTTPAction* action )
    //    { 
    //        http_session->close( );
    //    } );

    //    http_session->send_action( rep );

    //} );


    HTTPService* http_service = new HTTPService( );
    http_service->connect( "www.baidu.com" , 80 );
    http_service->on_open_session( [ &http_service ] ( Session* session )
    { 
        HTTPSession * http_session = static_cast< HTTPSession* >( session );
        http_session->callback_close( [ ] ( Session* s )
        { 
            HTTPSession* http = static_cast< HTTPSession* >( s );
            
        } );

        HTTPRequest * req = new HTTPRequest();
        req->path( "/" );
        req->method( "GET" );
        req->content_length( 0 );
        req->callback_send_content( [ ] ( HTTPAction* action )
        { 
            return nullptr;
        } );
        http_session->send_request( req );
    } );

    //HTTPRequest * req = new HTTPRequest();
    //req->parse( Buffer(http_req_str1) );
    //req->parse( Buffer(http_req_str2) );
    //req->parse( Buffer(http_req_str3) );

    //HTTPResponse * req = new HTTPResponse();
    //req->parse( Buffer(http_rep_str1) );
    //req->parse( Buffer(http_rep_str2) );
    //req->parse( Buffer(http_rep_str3) );

    while ( true )
    {
        uv_run( Service::loop( ) , UV_RUN_DEFAULT );
    }

    return 0;
}