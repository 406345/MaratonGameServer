#include "Buffer.h"

Buffer::Buffer( )
{
    this->data_  = nullptr;
    this->pdata_ = this->data_;
    this->size_  = 0;
}

Buffer::Buffer(size_t size)
{
    SAFE_DELETE( this->data_ );
    this->data_  = new char[size] { 0 };
    this->pdata_ = this->data_;
    this->size_  = size;
}

Buffer::Buffer( std::string string )
{
   this->data( string.c_str() , string.size() );
}

Buffer::Buffer( const char * data , size_t size )
{
    this->data( data , size );
}

Buffer & Buffer::operator=( Buffer & buffer )
{
    this->data( buffer.data_ , buffer.size_ );
    return *this;
}

Buffer & Buffer::operator=( Buffer && buffer )
{
    this->data_  = buffer.data_;
    this->size_  = buffer.size_;
    this->pdata_ = this->data_;

    buffer.data_ = nullptr;
    buffer.size_ = 0;

    return *this;
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
    this->pdata_ = this->data_;

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
    this->size_ = size;
  
    if( value == nullptr )
    {
        return;
    }

    this->data_ = new char[size] { 0 };
    this->pdata_ = this->data_;
    memcpy( this->data_ , value , size );
}

void Buffer::push( const char * data , size_t len )
{
    if ( this->data_ == nullptr )
        return;

    size_t delta = this->size_ - ( size_t )( this->pdata_ - this->data_ );

    if ( delta > len )
    {
        memcpy( this->pdata_ , data , len );
    }
    else
    {
        memcpy( this->pdata_ , data , delta );
    }
}

void Buffer::clearup()
{
    SAFE_DELETE( this->data_ );
    this->size_ = 0;
}
