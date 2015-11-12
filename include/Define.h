#ifndef DEFINE_H_
#define DEFINE_H_

//cast define

#define CIRCLE_BUFFER_SIZE          1024 * 1024
#define SESSION_RECIVE_BUFFER_SIZE  1024 * 128

//macro function define

#define PP_DEF(__type__,__name__)                           \
    void __name__(__type__ value){ __name__##_ = value; };  \
    __type__ __name__(){ return __name__##_; };

#define SAFE_DELETE(__x__)                                  \
    if ( nullptr != __x__ )                                 \
    {                                                       \
        delete __x__;                                       \
        __x__ = nullptr;                                    \
    }

#define SAFE_DELETE_ARRAY(__x__)                            \
    if ( nullptr != __x__ )                                 \
    {                                                       \
        delete[] __x__;                                     \
        __x__ = nullptr;                                    \
    }

#define SINGLETON_BEGIN(__class__)                          \
class __class__                                             \
{                                                           \
private:                                                    \
    __class__(){};                                          \
    ~__class__(){};                                         \
public:                                                     \
    static __class__ * instance()                           \
    {                                                       \
        static __class__ * pT = nullptr;                    \
        if ( !pT )                                          \
        {                                                   \
            static __class__ instance;                      \
            pT = &instance;                                 \
        }                                                   \
        return pT;                                          \
    };

#define SINGLETON_END };

#define UV_ERROR(__x__) if ( __x__ != 0 ) printf( "uv error: %s", uv_strerror(__x__));


#endif // DEFINE_H_
