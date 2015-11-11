/* * * * * * * * * * * * * * * *
* YHGenomics Inc.
* Author     : yang shubo
* Date       : 2015-10-22
* Description:
* * * * * * * * * * * * * * * */

#ifndef NETWORK_DATA_BUILDER_H_
#define NETWORK_DATA_BUILDER_H_

#include <memory.h>
#include <Define.h>

// in some cases two tcp message will be combined into one or chopped
// this class will solve the problem
class CircleBuffer
{
public:

    CircleBuffer();
    
    ~CircleBuffer();

    virtual bool push( const char* data, int len );

    virtual char* pop( int len );

    int length() { return this->buffer_length_; };
    int used_length() { return this->used_len_; };
    int available_length() { return this->available_len_; };

    int tail_ = 0;
    int head_ = 0;

private:

    CircleBuffer( CircleBuffer & ) = delete;
    CircleBuffer( CircleBuffer && ) = delete;
    CircleBuffer operator=( CircleBuffer ) = delete;

    int used_len_ = 0;
    int available_len_ = 0;

    const int buffer_length_ = CIRCLE_BUFFER_SIZE;

    char* circle_buffer_ = nullptr;
};

#endif // !NETWORK_DATA_BUILDER_H_