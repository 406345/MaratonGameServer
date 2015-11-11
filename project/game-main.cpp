#include "Server.h"
#include "Service.h"
#include "SessionManager.h"

int main()
{ 
    //Service service1;
    //service1.listen( "0.0.0.0", 1234 );
    //service1.new_session_cb( [] ( Session* s ) {
    //    printf( "New Session[%lld] Connected \r\n", s->id() );
    //} );

    //service1.close_session_cb( [] ( Session* s ) {
    //    printf( "Session[%lld] Disconnected \r\n", s->id() );
    //} );

    Service service2;
    service2.connect( "127.0.0.1", 1234 );
    service2.new_session_cb( [] ( Session* s ) {
        printf( "New Session[%lld] Connected \r\n", s->id() );
    } );

    service2.close_session_cb( [] ( Session* s ) {
        printf( "Session[%lld] Disconnected \r\n", s->id() );
    } );
    
    //Server::instance()->add_service( &service1 );
    Server::instance()->add_service( &service2 );

    Server::instance()->run();

    return 0;
}