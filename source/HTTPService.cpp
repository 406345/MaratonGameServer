#include "HTTPService.h"
#include "HTTPSession.h"

Session * HTTPService::create_session( )
{
    return new HTTPSession( this );
}
