//Maraton Game Server
//Create by Shubo Yang

#ifndef DEFINE_H_
#define DEFINE_H_

#define PP_DEF(__type__,__name__) \
    void __name__(__type__ value){ __name__##_ = value; }; \
    __type__ __name__(){ return __name__##_; };

#define SAFE_DELETE(__x__) \
    if ( nullptr != __x__ )\
    {\
        delete __x__;\
        __x__ = nullptr;\
    }

#define SAFE_DELETE_ARRAY(__x__) \
    if ( nullptr != __x__ )\
    {\
        delete[] __x__;\
        __x__ = nullptr;\
    }

#endif // DEFINE_H_
