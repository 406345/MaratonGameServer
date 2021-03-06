#include "SyncWorker.h"
#include "Define.h"

void SyncWorker::create( const size_t & loop_time , 
                         const syncworker_callback_t  work_callback , 
                         const syncworker_after_callback_t  after_callback , 
                         void * data )
{
    SyncWorker* worker      = new SyncWorker();
    worker->cb_work_        = work_callback;
    worker->cb_after_work_  = after_callback;
    worker->loop_time_      = loop_time;
    worker->data( data );
    worker->start();
} 

void SyncWorker::stop()
{
}

void SyncWorker::uv_process_timer_tick_callback( uv_timer_t * handle )
{
    SyncWorker* worker = static_cast< SyncWorker* >( handle->data );

    if( worker->finished_ )
    {
        return;
    }

    if( worker->loop_count_ == 0)
    {
        worker->loop_count_++;
        return;
    }

    if ( worker == nullptr )
    {
        uv_timer_stop( handle );
        return;
    }

    worker->finished_  = worker->cb_work_( worker );

    ++worker->loop_count_;

    if ( worker->finished_ )
    {


        if ( worker->cb_after_work_ != nullptr )
        {
            worker->cb_after_work_( worker );
        }

        int result = uv_timer_stop( &worker->timer_ );
        LOG_DEBUG_UV( result );
        
        SAFE_DELETE( worker );
    }
}

SyncWorker::SyncWorker()
{
    this->timer_        = { 0 };
    this->timer_.data   = this;
    this->loop_count_   = 0;
    this->loop_time_    = 1;

    uv_timer_init( uv_default_loop() , &this->timer_ );
}

SyncWorker::~SyncWorker()
{
}

void SyncWorker::start()
{
    uv_timer_start( &this->timer_ , 
                    SyncWorker::uv_process_timer_tick_callback , 
                    0 , 
                    this->loop_time_ );
}