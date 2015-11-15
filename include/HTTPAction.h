/* * * * * * * * * * * * * * * *
* YHGenomics Inc.
* Author     : yang shubo
* Date       : 2015-11-13
* Description: 
* * * * * * * * * * * * * * * */

#ifndef HTTP_ACTION_H_
#define HTTP_ACTION_H_

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "Define.h"
#include "Buffer.h"

class HTTPAction
{
public:
    
    const size_t            MAX_CONTENT_BUILD_SIZE  = 1024;
    
    enum HTTPState
    {
        kMethod = 1 ,
        kPath ,
        kProtocolVersion ,
        kResponseProtocolVersion ,
        kStatus ,
        kStatusMessage ,
        kHeader ,
        kHeaderKey ,
        kHeaderValue ,
        kContent ,
        kFinish ,
        kUnknown
    };

    typedef std::function<Buffer*( HTTPAction* )> callback_t_buffer;
    typedef std::function<void( HTTPAction*,Buffer& )> callback_t_void_buffer;
    typedef std::function<void( HTTPAction* )> callback_t;

    HTTPAction();
    ~HTTPAction();

    PP_DEF( std::string,    method );
    PP_DEF( size_t,         content_length );
    PP_DEF( std::string ,   protocol_version );
    PP_DEF( std::string ,   host );

    void                    content                     ( Buffer content );
    Buffer                  content                     ( );

    std::string             path                        ( ) { return this->path_; };

    void                    path                        ( std::string value ) { this->path_ = value; };

    void                    header                      ( std::string key ,
                                                          std::string value );
    
    void                    data                        ( void* value );

    std::string             header                      ( std::string key );
    size_t                  parse                       ( Buffer & buffer );
    virtual void            callback_send_content       ( callback_t_buffer callback );
    virtual void            callback_receive_content    ( callback_t_void_buffer callback );
    virtual void            callback_send_complete      ( callback_t callback );
protected:
    
    HTTPState               parse_state_                = HTTPState::kMethod;
    
    std::map<std::string , std::string> header_;
    std::string                         method_                    = "GET";
    std::string                         path_                      = "/";
    std::string                         protocol_version_          = "HTTP/1.1";
    std::string                         host_                      = "";
    std::string                         status_                    = "";
    size_t                              status_num_                = 0;
    
    size_t                              content_length_            = 0;
    size_t                              content_pos_               = 0;
    Buffer                              content_;
    
    callback_t_buffer                   callback_send_content_     = nullptr;
    callback_t_void_buffer              callback_receive_content_  = nullptr;
    callback_t                          callback_send_complete_    = nullptr;

    virtual Buffer                      build_http_header( );

private:

    std::string                         parse_header_key_          = "" ;
    std::string                         parse_header_value_        = "" ;
    void*                               data_                      = nullptr;

    friend class HTTPSession;
};

#endif // !HTTP_ACTION_H_
