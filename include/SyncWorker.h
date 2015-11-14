/* * * * * * * * * * * * * * * *
* YHGenomics Inc.
* Author     : yang shubo
* Date       : 2015-11-11
* Description:
* * * * * * * * * * * * * * * */

#ifndef SYNC_WORKER_H_
#define SYNC_WORKER_H_

#include <functional>
#include "uv.h"
#include "Define.h"

class SyncWorker
{
public:

    typedef std::function<bool( SyncWorker* )> syncworker_callback_t;

    static void create      ( const size_t & loop_time , 
                             const syncworker_callback_t  callback , 
                             void* data );
     
    void*       data( ) { return this->data_; };
    void        data( void* value ) { this->data_ = value; };
    void        stop( );
    size_t      loop_count( ) { return loop_count_; };

private:

    static void uv_process_timer_tick_callback( uv_timer_t* handle );

    SyncWorker();
    ~SyncWorker();

    void start();

    void*                   data_ = nullptr;
    uv_timer_t              timer_;
    size_t                  loop_time_ = 1;

    syncworker_callback_t   callback_;
    size_t                  loop_count_ = 1;
};

#endif // !SYNC_WORKER_H_