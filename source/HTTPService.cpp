#include "HTTPService.h"
#include "HTTPSession.h"

Session * HTTPService::create_session( )
{
    auto ret = new HTTPSession( this );
    


    return ret;
}
