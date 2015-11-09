#include "Server.h"
#include "Service.h"

int main()
{
    Server server;

    Service _1234;
    _1234.init( "0.0.0.0", 1234 );

    server.add_service( &_1234 );
    server.run();

    return 0;
}