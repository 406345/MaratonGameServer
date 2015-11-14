#include "SyncWorker.h"
#include "Define.h"

void SyncWorker::create( const size_t & loop_time , 
                         const syncworker_callback_t callback , 
                         void * data )
{
    SyncWorker* worker = new SyncWorker();
    worker->callback_ = callback;
    worker->loop_time_ = loop_time;
    worker->data( data );
    worker->start();
} 

void SyncWorker::stop()
{
}

void SyncWorker::uv_process_timer_tick_callback( uv_timer_t * handle )
{
    SyncWorker* worker = static_cast< SyncWorker* >( handle->data );

    if ( worker == nullptr )
    {
        uv_timer_stop( handle );
        return;
    }

    bool is_finished   = worker->callback_( worker );

    ++worker->loop_count_;

    if ( is_finished )
    {
        uv_timer_stop( &worker->timer_ );
        SAFE_DELETE  ( worker );
    }
}

SyncWorker::SyncWorker()
{
    this->timer_        = { 0 };
    this->timer_.data   = this;
    this->loop_count_   = 1;
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