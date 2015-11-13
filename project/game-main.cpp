#include "Server.h"
#include "Service.h"
#include "SessionManager.h"
#include "HTTPClient.h"
#include "Buffer.h"
#include <memory> 
#include "SyncWorker.h"

int main( )
{ 
    Service* service2 = new Service( );
    service2->connect( "127.0.0.1" , 1234 );
    service2->on_operation_failed( [ ] ( Service * s , size_t status )
    {
        UV_ERROR( status );
    } );

    service2->on_new_session( [ ] ( Session* s )
    {
        printf( "New Session[%lld] Connected \r\n" , s->id( ) );
    } );

    service2->on_close_session( [ ] ( Session* s )
    {
        printf( "Session[%lld] Disconnected \r\n" , s->id( ) );
    } );

    Server::instance( )->add_service( service2 );

    Server::instance( )->run( );

    return 0;
}