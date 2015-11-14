#include "Server.h"
#include "Service.h"
#include "SessionManager.h"
#include "HTTPClient.h"
#include "Buffer.h"
#include <memory> 
#include "SyncWorker.h"

Service* create( )
{
    Service * service2 = new Service( );
    service2->connect( "127.0.0.1" , 1234 );
    service2->on_operation_failed( [ ] ( Service * s , size_t status )
    {

    } );

    service2->on_new_session( [ ] ( Session* s )
    {
        printf( "New Session[%lld] Connected \r\n" , s->id( ) );
    } );

    service2->on_close_session( [ ] ( Session* s )
    {
        printf( "Session[%lld] Disconnected \r\n" , s->id( ) );
    } );

    return service2;
}

int main( )
{

    std::vector<Service*> v;

    for ( size_t i = 0; i < 30; i++ )
    {
        auto srv = create( );
        if ( i > 0 )
        {
            v.push_back( srv );
        }
    }

    SyncWorker::create( 100 , [ &v ] ( SyncWorker* worker )
    {
        LOG_SYS( "SyncWorker" );
        if ( worker->loop_count( ) > 50 )
        {
            for ( auto s : v )
            {
                s->stop( );
            }

            LOG_SYS( "Service2 stopped" );
            return true;
        }
        return false;
    } , nullptr );

    Server::instance( )->run( );

    return 0;
}