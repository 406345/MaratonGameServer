#include "Buffer.h"

Buffer::Buffer(size_t size)
{
    SAFE_DELETE( this->data_ );
    this->data_ = new char[size] { 0 };
    this->size_ = size;
}

Buffer::Buffer( std::string string )
{
   this->data( string.c_str() , string.size() );
}

Buffer::Buffer( const char * data , size_t size )
{
    this->data( data , size );
}

Buffer::~Buffer()
{
    SAFE_DELETE( this->data_ );
}

Buffer::Buffer( Buffer & buffer )
{
    this->data( buffer.data_ , buffer.size_ );
}

Buffer::Buffer( Buffer && buffer )
{
    this->data_  = buffer.data_;
    this->size_  = buffer.size_;

    buffer.data_ = nullptr;
    buffer.size_ = 0;
}

char * Buffer::data()
{
    return this->data_;
}

void Buffer::data( const char * value , size_t size )
{
    SAFE_DELETE( this->data_ );
  
    this->data_ = new char[size] { 0 };
    this->size_ = size;

    memcpy( this->data_ , value , size );
}

void Buffer::clearup()
{
    SAFE_DELETE( this->data_ );
    this->size_ = 0;
}
