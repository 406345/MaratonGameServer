#include "CircleBuffer.h"

CircleBuffer::CircleBuffer()
{
    this->circle_buffer_ = new char[this->buffer_length_];

    this->available_len_ = this->buffer_length_;
    this->used_len_ = 0;
}

CircleBuffer::~CircleBuffer()
{
    if ( this->circle_buffer_ != nullptr )
    {
        delete this->circle_buffer_;
        this->circle_buffer_ = nullptr;
    }
}

bool CircleBuffer::push( const char * data, int len )
{
    if ( available_length() < len )
    {
        throw "[CircleBuffer.cpp] not enough buffer";
    }

    if ( ( tail_ + len ) > this->buffer_length_ )
    {
        char* ptail = this->circle_buffer_ + this->tail_;
        int delta_len = this->buffer_length_ - tail_;
        memcpy( ptail, data, delta_len );
        memcpy( this->circle_buffer_, data + delta_len, len - delta_len );
    }
    else
    {
        char* ptail = this->circle_buffer_ + this->tail_;

        memcpy( ptail, data, len );
    }

    this->tail_ = ( this->tail_ + len ) % this->buffer_length_;

    this->used_len_ += len;

    this->available_len_ = this->buffer_length_ - this->used_len_;

    return true;
}

char * CircleBuffer::pop( int len )
{
    if ( len > this->used_length() )
    {
        return nullptr;
    }

    char* result = new char[len];
    int result_pos = 0;
    char* phead = this->circle_buffer_ + this->head_;

    // there are two pices data
    // one is at end of circle buffer
    // antoher is at front of circle buffer
    if ( ( this->head_ + len ) > this->buffer_length_ )
    {
        int end_len = this->buffer_length_ - this->head_;
        memcpy( result, phead + result_pos, end_len );
        result_pos += this->buffer_length_ - this->head_;
        memcpy( result + result_pos, this->circle_buffer_, ( len - end_len ) );
    }
    // circle buffer contains all data inside
    else
    {
        memcpy( result, phead + result_pos, len );
    }

    this->head_ = ( this->head_ + len ) % this->buffer_length_;

    this->used_len_ -= len;

    this->available_len_ = this->buffer_length_ - this->used_len_;

    return result;
}
