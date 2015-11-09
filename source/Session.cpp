#include "Session.h"

Session::Session()
{
    this->conn_ = new uv_tcp_t();
    this->conn_->data = this;
    this->recive_buffer_ = new char[SESSION_RECIVE_BUFFER_LENGTH];
}

Session::~Session()
{
    SAFE_DELETE( this->conn_ );
    SAFE_DELETE( this->recive_buffer_ );
}

void Session::on_recive_data( const char * data, size_t length )
{
}

void Session::close()
{
}
