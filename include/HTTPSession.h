/* * * * * * * * * * * * * * * *
* YHGenomics Inc.
* Author     : yang shubo
* Date       : 2015-11-15
* Description:
* * * * * * * * * * * * * * * */

#ifndef HTTP_SESSION_H_
#define HTTP_SESSION_H_

#include <functional>

#include "HTTPService.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "Session.h"

class HTTPSession : 
    public Session
{
public:

    typedef std::function<void( HTTPRequest* )> req_callback_t;
    typedef std::function<void( HTTPResponse* )> rep_callback_t;

    HTTPSession( HTTPService* service_ );
    ~HTTPSession()
    {
        SAFE_DELETE( this->request_ );
        SAFE_DELETE( this->response_ );
    }

    bool            send_request    ( HTTPRequest* req ); 
    bool            send_response   ( HTTPResponse* rep ); 

protected:

    virtual void    send( Buffer & buffer ) override;
    virtual void    on_send_finish     ( size_t size ) override;
    virtual void    on_connected       ( ) override;
    virtual void    on_receive_data    ( Buffer & buffer ) override;
    virtual void    on_close           ( ) override;

private:

    req_callback_t  callback_req_   = nullptr;
    rep_callback_t  callback_rep_   = nullptr;
    HTTPResponse*   response_       = nullptr;
    HTTPRequest*    request_        = nullptr;
};

#endif // !HTTP_SESSION_H_
