/* * * * * * * * * * * * * * * *
* YHGenomics Inc.
* Author     : yang shubo
* Date       : 2015-11-15
* Description:
* * * * * * * * * * * * * * * */

#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include "HTTPAction.h"

class HTTPResponse :
    public HTTPAction
{
public:

    HTTPResponse( );
    virtual Buffer build_message( ) override;

private:


};

#endif // !HTTP_RESPONSE_H_
