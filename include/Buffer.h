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
    
    Buffer(size_t size);
    Buffer(std::string string);
    Buffer(const char* data, size_t size);
    ~Buffer();

    Buffer(Buffer& buffer);
    Buffer(Buffer&& buffer);

    char*   data();
    void    data( const char * value , size_t size );

    size_t  size() { return this->size_; }

private:

    char*   data_ = nullptr;
    size_t  size_ = 0;

    void clearup();
};

#endif // !BUFFER_H_
