#include "Server.h"
#include "Service.h"

int main()
{ 
    Service service;
    service.listen( "0.0.0.0", 1234 );
    service.new_session_cb( [] ( Session* s ) { 
        printf( "New Session[%ld] Connected \r\n" , s->id() );
    } );

    service.close_session_cb( [] ( Session* s ) { 
        printf( "Session[%ld] Disconnected \r\n" , s->id() );
    } );
    
    Server::instance()->add_service( &service );

    Server::instance()->run();

    return 0;
}