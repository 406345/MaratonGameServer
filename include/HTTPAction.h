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

    HTTPAction();
    ~HTTPAction();

    PP_DEF( std::string,    method );
    PP_DEF( size_t,         content_length );
    PP_DEF( std::string ,   protocol_version );
    PP_DEF( char* ,         content);
    PP_DEF( std::string ,   host );

    virtual bool            finished                    ( );
    
    std::string             path                        ( ) { return this->path_; };
    void                    path                        ( std::string value ) { this->path_ = value; };

    void                    header                      ( std::string key ,
                                                          std::string value );
    std::string             header                      ( std::string key );

    virtual Buffer          build_message               ( );

    size_t                  parse                       ( Buffer & buffer );

protected:
    
    HTTPState               parse_state_                = HTTPState::kMethod;
    HTTPState               message_state_              = HTTPState::kHeader; 

    std::map<std::string , std::string>     header_;
    std::string                             method_                 = "GET";
    std::string                             path_                   = "/";
    std::string                             protocol_version_       = "HTTP/1.1";
    std::string                             host_                   = "";
    std::string                             status_                 = "";
    size_t                                  status_num_             = 0;
    
    size_t                                  content_length_         = 0;
    size_t                                  content_pos_            = 0;
    char*                                   content_                = nullptr;

private:

    std::string                             parse_header_key_       = "" ;
    std::string                             parse_header_value_     = "" ;
};

#endif // !HTTP_ACTION_H_
