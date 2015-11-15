/* * * * * * * * * * * * * * * *
* YHGenomics Inc.
* Author     : yang shubo
* Date       : 2015-11-15
* Description:
* * * * * * * * * * * * * * * */

#ifndef HTTP_SERVICE_H_
#define HTTP_SERVICE_H_

#include "Service.h"

class HTTPService : 
    public Service
{
protected:

    Session* create_session() override;
};

#endif // !HTTP_SERVICE_H_
