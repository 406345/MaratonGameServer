/* * * * * * * * * * * * * * * *
* YHGenomics Inc.
* Author     : yang shubo
* Date       : 2015-11-15
* Description:
* * * * * * * * * * * * * * * */

#ifndef HTTP_SESSION_H_
#define HTTP_SESSION_H_

#include "HTTPService.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "Session.h"

class HTTPSession : 
    public Session
{
public:

    HTTPSession( HTTPService* service )
        : Session( service )
    {

    }

    ~HTTPSession()
    {

    }

    bool send_request( HTTPAction* req );

protected:

    virtual void on_send_finish     ( size_t size ) override;
    virtual void on_connected       ( ) override;
    virtual void on_receive_data    ( Buffer & buffer ) override;
    virtual void on_close           ( ) override;

private:

    HTTPAction * request   = nullptr;
    HTTPResponse* response  = nullptr;
};

#endif // !HTTP_SESSION_H_
