/* * * * * * * * * * * * * * * *
* YHGenomics Inc.
* Author     : yang shubo
* Date       : 2015-11-15
* Description:
* * * * * * * * * * * * * * * */

#ifndef HTTP_REQEUST_H_
#define HTTP_REQEUST_H_

#include <functional>
#include "HTTPAction.h"

class HTTPRequest : 
    public HTTPAction
{
public:
     
    HTTPRequest( );
    virtual Buffer build_http_header( ) override;
    

private:

};

#endif // !HTTP_REQEUST_H_
