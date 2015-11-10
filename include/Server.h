//Maraton Game Server
//Create by Shubo Yang

#ifndef SERVER_H_
#define SERVER_H_

#include <vector>
#include "uv.h"
#include "Utils.h"

class Service;

class Server :
    public Singleton<Server>
{
public:

    static uv_loop_t* loop();
    void add_service( Service * srv );
    void run();

protected:

    Server() {};

private:

    
    ~Server() {};
   
    std::vector<Service*> service_list;

    static void uv_callback_alloc_buffer( uv_handle_t* handle,
                                          size_t suggested_size,
                                          uv_buf_t* buf );

    static void uv_callback_read( uv_stream_t* stream,
                                  ssize_t nread,
                                  const uv_buf_t* buf );
    static void uv_callback_close( uv_handle_t* handle );
    static void uv_callback_on_new_connection( uv_stream_t* server, int status );

    friend class Singleton<Server>;
};

#endif // SERVER_H_
