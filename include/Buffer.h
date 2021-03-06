/* * * * * * * * * * * * * * * *
* YHGenomics Inc.
* Author     : yang shubo
* Date       : 2015-11-13
* Description:
* * * * * * * * * * * * * * * */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <string>
#include "Define.h"

class Buffer
{
public:
    
    Buffer( );
    Buffer( size_t size );
    Buffer( std::string string );
    Buffer( const char* data , size_t size );
    Buffer( Buffer& buffer );
    Buffer( Buffer&& buffer );
    Buffer& operator =( Buffer& buffer );
    Buffer& operator =( Buffer&& buffer );
    ~Buffer();
    
    char*   data();
    void    data( const char * value , size_t size );

    size_t  size() { return this->size_; }

    void    push( const char* data , size_t len );

private:

    char*   data_  = nullptr;
    size_t  size_  = 0;

    char*   pdata_ = nullptr;

    void clearup();
};

#endif // !BUFFER_H_
