#include "Server.h"
#include "Service.h"
#include "SessionManager.h"
#include "HTTPClient.h"
#include "Buffer.h"
#include <memory> 
#include "SyncWorker.h"
#include <thread>

void test( );

Service* create( )
{
    Service* srv = new Service( );
    srv->connect( "localhost" , 1234 ); 
    return srv;
}
static std::vector<Service*> v;
static bool finished = false;
bool syncworker_callback( SyncWorker* worker )
{
    for ( auto s : v )
    {
        s->stop( );
        SAFE_DELETE( s );
    }
    v.clear( );

    return true;
}
void syncworker_after_callback( SyncWorker* worker )
{
    finished=true;
}

void test( )
{
    finished=false;
    for ( size_t i = 0; i < 10; i++ )
    {
        auto srv = create( );
        if ( i > 0 )
        {
            v.push_back( srv );
        }
    }

    SyncWorker::create( 5000 , syncworker_callback , syncworker_after_callback , nullptr );
}
int main( )
{
    test( );
    SyncWorker::create( 1000 , [ ] ( SyncWorker* worker )
    { 
        if( finished )
        {
            test( );
        }
        return false;
    } , 
    nullptr , nullptr );

    while ( true )
    {
        uv_run( Service::loop( ) , UV_RUN_DEFAULT );
    }

    return 0;
}