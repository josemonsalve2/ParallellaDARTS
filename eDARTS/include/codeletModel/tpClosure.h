/**
 * @file tpClosure.h
 * @author Jose M Monsalve
 * @date 12 July 2017
 * @brief Helper file for tpClosure creation
 * @todo Add copyright
 *
 */
#ifndef TP_CLOSURE_H
#define TP_CLOSURE_H

#include "e_darts_print.h"

// Forward declaration of _tp_metadata_t
typedef struct __attribute__ ((__packed__)) __attribute__ ((__packed__)) _tp_metadata_s _tp_metadata_t;
typedef struct __attribute__ ((__packed__)) __attribute__ ((__packed__)) genericTpClosure_s genericTpClosure_t;

typedef struct __attribute__ ((__packed__)) __attribute__ ((__packed__)) genericTpClosure_s {
    _tp_metadata_t (*_metadataCtor)(unsigned _TPid);
    void (*_userInitCtorExec)(_tp_metadata_t*, genericTpClosure_t*);
    unsigned sizeOfArgs;
} genericTpClosure_t;

// For macro overloading. This way it is possible to call the same macro no matter how many
// parameters we pass
#define GET_MACRO_TPCLOSURE(TPname,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,\
                                   _17,_18,_19,_20,_21,_22,_23,_24,_25,NAME,...) NAME
#define DEFINE_TP_CLOSURE_ARGS(TPname, ...) \
        GET_MACRO_TPCLOSURE(TPname,##__VA_ARGS__,DEFINE_TP_CLOSURE_ARGS25,DEFINE_TP_CLOSURE_ARGS24,\
        DEFINE_TP_CLOSURE_ARGS23,DEFINE_TP_CLOSURE_ARGS22,DEFINE_TP_CLOSURE_ARGS21,DEFINE_TP_CLOSURE_ARGS20,\
        DEFINE_TP_CLOSURE_ARGS19,DEFINE_TP_CLOSURE_ARGS18,DEFINE_TP_CLOSURE_ARGS17,DEFINE_TP_CLOSURE_ARGS16,\
        DEFINE_TP_CLOSURE_ARGS15,DEFINE_TP_CLOSURE_ARGS14,DEFINE_TP_CLOSURE_ARGS13,DEFINE_TP_CLOSURE_ARGS12,\
        DEFINE_TP_CLOSURE_ARGS11,DEFINE_TP_CLOSURE_ARGS10,DEFINE_TP_CLOSURE_ARGS9,DEFINE_TP_CLOSURE_ARGS8,\
        DEFINE_TP_CLOSURE_ARGS7,DEFINE_TP_CLOSURE_ARGS6,DEFINE_TP_CLOSURE_ARGS5,DEFINE_TP_CLOSURE_ARGS4,\
        DEFINE_TP_CLOSURE_ARGS3,DEFINE_TP_CLOSURE_ARGS2,DEFINE_TP_CLOSURE_ARGS1,DEFINE_TP_CLOSURE_ARGS0) (TPname,##__VA_ARGS__)


#define DEFINE_TP_CLOSURE_ARGS0(TPname) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        }args;\
    } TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t * _tp, genericTpClosure_t* tpClosure) { \
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure; \
        _##TPname##_userInitCtor(_tp); \
    }\
    \
    TPname##_tpClosure_t _invoke_##TPname() {\
        TPname##_tpClosure_t newClosure;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\



#define DEFINE_TP_CLOSURE_ARGS1(TPname, ArgType0) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
    	ArgType0 A0;\
        }args;\
    } TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0);\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS2(TPname, ArgType0, ArgType1) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1);\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS3(TPname, ArgType0, ArgType1, ArgType2) \
    typedef struct __attribute__ ((__packed__)) __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2);\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS4(TPname, ArgType0, ArgType1, ArgType2, ArgType3) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3);\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.args.A3 = A3;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\


#define DEFINE_TP_CLOSURE_ARGS5(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4);\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.args.A3 = A3;\
        newClosure.args.A4 = A4;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS6(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5);\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.args.A3 = A3;\
        newClosure.args.A4 = A4;\
        newClosure.args.A5 = A5;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS7(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6);\\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.args.A3 = A3;\
        newClosure.args.A4 = A4;\
        newClosure.args.A5 = A5;\
        newClosure.args.A6 = A6;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS8(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7);\\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.args.A3 = A3;\
        newClosure.args.A4 = A4;\
        newClosure.args.A5 = A5;\
        newClosure.args.A6 = A6;\
        newClosure.args.A7 = A7;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS9(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8);\\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.args.A3 = A3;\
        newClosure.args.A4 = A4;\
        newClosure.args.A5 = A5;\
        newClosure.args.A6 = A6;\
        newClosure.args.A7 = A7;\
        newClosure.args.A8 = A8;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS10(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9);\\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.args.A3 = A3;\
        newClosure.args.A4 = A4;\
        newClosure.args.A5 = A5;\
        newClosure.args.A6 = A6;\
        newClosure.args.A7 = A7;\
        newClosure.args.A8 = A8;\
        newClosure.args.A9 = A9;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS11(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10);\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.args.A3 = A3;\
        newClosure.args.A4 = A4;\
        newClosure.args.A5 = A5;\
        newClosure.args.A6 = A6;\
        newClosure.args.A7 = A7;\
        newClosure.args.A8 = A8;\
        newClosure.args.A9 = A9;\
        newClosure.args.A10 = A10;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS12(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11);\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.args.A3 = A3;\
        newClosure.args.A4 = A4;\
        newClosure.args.A5 = A5;\
        newClosure.args.A6 = A6;\
        newClosure.args.A7 = A7;\
        newClosure.args.A8 = A8;\
        newClosure.args.A9 = A9;\
        newClosure.args.A10 = A10;\
        newClosure.args.A11 = A11;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS13(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12);\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, rgType12 A12) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.args.A3 = A3;\
        newClosure.args.A4 = A4;\
        newClosure.args.A5 = A5;\
        newClosure.args.A6 = A6;\
        newClosure.args.A7 = A7;\
        newClosure.args.A8 = A8;\
        newClosure.args.A9 = A9;\
        newClosure.args.A10 = A10;\
        newClosure.args.A11 = A11;\
        newClosure.args.A12 = A12;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS14(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12, ArgType13) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        ArgType13 A13;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12, tpClosureCast->args.A13);\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, ArgType12 A12) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.args.A3 = A3;\
        newClosure.args.A4 = A4;\
        newClosure.args.A5 = A5;\
        newClosure.args.A6 = A6;\
        newClosure.args.A7 = A7;\
        newClosure.args.A8 = A8;\
        newClosure.args.A9 = A9;\
        newClosure.args.A10 = A10;\
        newClosure.args.A11 = A11;\
        newClosure.args.A12 = A12;\
        newClosure.args.A13 = A13;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS15(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12, ArgType13, ArgType14) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        ArgType13 A13;\
        ArgType14 A14;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12, tpClosureCast->args.A13, tpClosureCast->args.A14);\
    }\
        TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, ArgType12 A12, ArgType13 A13, ArgType14 A14) {\
            TPname##_tpClosure_t newClosure;\
            newClosure.args.A0 = A0;\
            newClosure.args.A1 = A1;\
            newClosure.args.A2 = A2;\
            newClosure.args.A3 = A3;\
            newClosure.args.A4 = A4;\
            newClosure.args.A5 = A5;\
            newClosure.args.A6 = A6;\
            newClosure.args.A7 = A7;\
            newClosure.args.A8 = A8;\
            newClosure.args.A9 = A9;\
            newClosure.args.A10 = A10;\
            newClosure.args.A11 = A11;\
            newClosure.args.A12 = A12;\
            newClosure.args.A13 = A13;\
            newClosure.args.A14 = A14;\
            newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
            newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
            newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
        }\

#define DEFINE_TP_CLOSURE_ARGS16(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12, ArgType13, ArgType14, ArgType15) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        ArgType13 A13;\
        ArgType14 A14;\
        ArgType15 A15;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12, tpClosureCast->args.A13, tpClosureCast->args.A14, tpClosureCast->args.A15);\
    }\
        TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, ArgType12 A12, ArgType13 A13, ArgType14 A14, ArgType15 A15) {\
            TPname##_tpClosure_t newClosure;\
            newClosure.args.A0 = A0;\
            newClosure.args.A1 = A1;\
            newClosure.args.A2 = A2;\
            newClosure.args.A3 = A3;\
            newClosure.args.A4 = A4;\
            newClosure.args.A5 = A5;\
            newClosure.args.A6 = A6;\
            newClosure.args.A7 = A7;\
            newClosure.args.A8 = A8;\
            newClosure.args.A9 = A9;\
            newClosure.args.A10 = A10;\
            newClosure.args.A11 = A11;\
            newClosure.args.A12 = A12;\
            newClosure.args.A13 = A13;\
            newClosure.args.A14 = A14;\
            newClosure.args.A15 = A15;\
            newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
            newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
            newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
        }\

#define DEFINE_TP_CLOSURE_ARGS17(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12, ArgType13, ArgType14, ArgType15, ArgType16) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        ArgType13 A13;\
        ArgType14 A14;\
        ArgType15 A15;\
        ArgType16 A16;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12, tpClosureCast->args.A13, tpClosureCast->args.A14, tpClosureCast->args.A15, tpClosureCast->args.A16);\
    }\
        TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, ArgType12 A12, ArgType13 A13, ArgType14 A14, ArgType15 A15, ArgType16 A16) {\
            TPname##_tpClosure_t newClosure;\
            newClosure.args.A0 = A0;\
            newClosure.args.A1 = A1;\
            newClosure.args.A2 = A2;\
            newClosure.args.A3 = A3;\
            newClosure.args.A4 = A4;\
            newClosure.args.A5 = A5;\
            newClosure.args.A6 = A6;\
            newClosure.args.A7 = A7;\
            newClosure.args.A8 = A8;\
            newClosure.args.A9 = A9;\
            newClosure.args.A10 = A10;\
            newClosure.args.A11 = A11;\
            newClosure.args.A12 = A12;\
            newClosure.args.A13 = A13;\
            newClosure.args.A14 = A14;\
            newClosure.args.A15 = A15;\
            newClosure.args.A16 = A16;\
            newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
            newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
            newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
        }\

#define DEFINE_TP_CLOSURE_ARGS18(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12, ArgType13, ArgType14, ArgType15, ArgType16, ArgType17) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        ArgType13 A13;\
        ArgType14 A14;\
        ArgType15 A15;\
        ArgType16 A16;\
        ArgType17 A17;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12, tpClosureCast->args.A13, tpClosureCast->args.A14, tpClosureCast->args.A15, tpClosureCast->args.A16, tpClosureCast->args.A17);\
    }\
        TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, ArgType12 A12, ArgType13 A13, ArgType14 A14, ArgType15 A15, ArgType16 A16, ArgType17 A17) {\
            TPname##_tpClosure_t newClosure;\
            newClosure.args.A0 = A0;\
            newClosure.args.A1 = A1;\
            newClosure.args.A2 = A2;\
            newClosure.args.A3 = A3;\
            newClosure.args.A4 = A4;\
            newClosure.args.A5 = A5;\
            newClosure.args.A6 = A6;\
            newClosure.args.A7 = A7;\
            newClosure.args.A8 = A8;\
            newClosure.args.A9 = A9;\
            newClosure.args.A10 = A10;\
            newClosure.args.A11 = A11;\
            newClosure.args.A12 = A12;\
            newClosure.args.A13 = A13;\
            newClosure.args.A14 = A14;\
            newClosure.args.A15 = A15;\
            newClosure.args.A16 = A16;\
            newClosure.args.A17 = A17;\
            newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
            newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
            newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
        }\

#define DEFINE_TP_CLOSURE_ARGS19(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12, ArgType13, ArgType14, ArgType15, ArgType16, ArgType17, ArgType18) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        ArgType13 A13;\
        ArgType14 A14;\
        ArgType15 A15;\
        ArgType16 A16;\
        ArgType17 A17;\
        ArgType18 A18;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12, tpClosureCast->args.A13, tpClosureCast->args.A14, tpClosureCast->args.A15, tpClosureCast->args.A16, tpClosureCast->args.A17, tpClosureCast->args.A18);\
    }\
    TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, ArgType12 A12, ArgType13 A13, ArgType14 A14, ArgType15 A15, ArgType16 A16, ArgType17 A17, ArgType18 A18) {\
        TPname##_tpClosure_t newClosure;\
        newClosure.args.A0 = A0;\
        newClosure.args.A1 = A1;\
        newClosure.args.A2 = A2;\
        newClosure.args.A3 = A3;\
        newClosure.args.A4 = A4;\
        newClosure.args.A5 = A5;\
        newClosure.args.A6 = A6;\
        newClosure.args.A7 = A7;\
        newClosure.args.A8 = A8;\
        newClosure.args.A9 = A9;\
        newClosure.args.A10 = A10;\
        newClosure.args.A11 = A11;\
        newClosure.args.A12 = A12;\
        newClosure.args.A13 = A13;\
        newClosure.args.A14 = A14;\
        newClosure.args.A15 = A15;\
        newClosure.args.A16 = A16;\
        newClosure.args.A17 = A17;\
        newClosure.args.A18 = A18;\
        newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
        newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
        newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
        return newClosure;\
    }\

#define DEFINE_TP_CLOSURE_ARGS20(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12, ArgType13, ArgType14, ArgType15, ArgType16, ArgType17, ArgType18, ArgType19) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        ArgType13 A13;\
        ArgType14 A14;\
        ArgType15 A15;\
        ArgType16 A16;\
        ArgType17 A17;\
        ArgType18 A18;\
        ArgType19 A19;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12, tpClosureCast->args.A13, tpClosureCast->args.A14, tpClosureCast->args.A15, tpClosureCast->args.A16, tpClosureCast->args.A17, tpClosureCast->args.A18, tpClosureCast->args.A19);\
    }\
        TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, ArgType12 A12, ArgType13 A13, ArgType14 A14, ArgType15 A15, ArgType16 A16, ArgType17 A17, ArgType18 A18, ArgType19 A19) {\
            TPname##_tpClosure_t newClosure;\
            newClosure.args.A0 = A0;\
            newClosure.args.A1 = A1;\
            newClosure.args.A2 = A2;\
            newClosure.args.A3 = A3;\
            newClosure.args.A4 = A4;\
            newClosure.args.A5 = A5;\
            newClosure.args.A6 = A6;\
            newClosure.args.A7 = A7;\
            newClosure.args.A8 = A8;\
            newClosure.args.A9 = A9;\
            newClosure.args.A10 = A10;\
            newClosure.args.A11 = A11;\
            newClosure.args.A12 = A12;\
            newClosure.args.A13 = A13;\
            newClosure.args.A14 = A14;\
            newClosure.args.A15 = A15;\
            newClosure.args.A16 = A16;\
            newClosure.args.A17 = A17;\
            newClosure.args.A18 = A18;\
            newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
            newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
            newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
            return newClosure;\
        }\

#define DEFINE_TP_CLOSURE_ARGS21(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12, ArgType13, ArgType14, ArgType15, ArgType16, ArgType17, ArgType18, ArgType19, ArgType20) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        ArgType13 A13;\
        ArgType14 A14;\
        ArgType15 A15;\
        ArgType16 A16;\
        ArgType17 A17;\
        ArgType18 A18;\
        ArgType19 A19;\
        ArgType20 A20;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12, tpClosureCast->args.A13, tpClosureCast->args.A14, tpClosureCast->args.A15, tpClosureCast->args.A16, tpClosureCast->args.A17, tpClosureCast->args.A18, tpClosureCast->args.A19, tpClosureCast->args.A20);\
    }\
        TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, ArgType12 A12, ArgType13 A13, ArgType14 A14, ArgType15 A15, ArgType16 A16, ArgType17 A17, ArgType18 A18, ArgType19 A19, ArgType20 A20) {\
            TPname##_tpClosure_t newClosure;\
            newClosure.args.A0 = A0;\
            newClosure.args.A1 = A1;\
            newClosure.args.A2 = A2;\
            newClosure.args.A3 = A3;\
            newClosure.args.A4 = A4;\
            newClosure.args.A5 = A5;\
            newClosure.args.A6 = A6;\
            newClosure.args.A7 = A7;\
            newClosure.args.A8 = A8;\
            newClosure.args.A9 = A9;\
            newClosure.args.A10 = A10;\
            newClosure.args.A11 = A11;\
            newClosure.args.A12 = A12;\
            newClosure.args.A13 = A13;\
            newClosure.args.A14 = A14;\
            newClosure.args.A15 = A15;\
            newClosure.args.A16 = A16;\
            newClosure.args.A17 = A17;\
            newClosure.args.A18 = A18;\
            newClosure.args.A19 = A19;\
            newClosure.args.A20 = A20;\
            newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
            newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
            newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
            return newClosure;\
        }\

#define DEFINE_TP_CLOSURE_ARGS22(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12, ArgType13, ArgType14, ArgType15, ArgType16, ArgType17, ArgType18, ArgType19, ArgType20, ArgType21) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        ArgType13 A13;\
        ArgType14 A14;\
        ArgType15 A15;\
        ArgType16 A16;\
        ArgType17 A17;\
        ArgType18 A18;\
        ArgType19 A19;\
        ArgType20 A20;\
        ArgType21 A21;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12, tpClosureCast->args.A13, tpClosureCast->args.A14, tpClosureCast->args.A15, tpClosureCast->args.A16, tpClosureCast->args.A17, tpClosureCast->args.A18, tpClosureCast->args.A19, tpClosureCast->args.A20, tpClosureCast->args.A21);\
    }\
        TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, ArgType12 A12, ArgType13 A13, ArgType14 A14, ArgType15 A15, ArgType16 A16, ArgType17 A17, ArgType18 A18, ArgType19 A19, ArgType20 A20, ArgType21 A21) {\
            TPname##_tpClosure_t newClosure;\
            newClosure.args.A0 = A0;\
            newClosure.args.A1 = A1;\
            newClosure.args.A2 = A2;\
            newClosure.args.A3 = A3;\
            newClosure.args.A4 = A4;\
            newClosure.args.A5 = A5;\
            newClosure.args.A6 = A6;\
            newClosure.args.A7 = A7;\
            newClosure.args.A8 = A8;\
            newClosure.args.A9 = A9;\
            newClosure.args.A10 = A10;\
            newClosure.args.A11 = A11;\
            newClosure.args.A12 = A12;\
            newClosure.args.A13 = A13;\
            newClosure.args.A14 = A14;\
            newClosure.args.A15 = A15;\
            newClosure.args.A16 = A16;\
            newClosure.args.A17 = A17;\
            newClosure.args.A18 = A18;\
            newClosure.args.A19 = A19;\
            newClosure.args.A20 = A20;\
            newClosure.args.A21 = A21;\
            newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
            newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
            newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
            return newClosure;\
        }\

#define DEFINE_TP_CLOSURE_ARGS23(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12, ArgType13, ArgType14, ArgType15, ArgType16, ArgType17, ArgType18, ArgType19, ArgType20, ArgType21, ArgType22) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        ArgType13 A13;\
        ArgType14 A14;\
        ArgType15 A15;\
        ArgType16 A16;\
        ArgType17 A17;\
        ArgType18 A18;\
        ArgType19 A19;\
        ArgType20 A20;\
        ArgType21 A21;\
        ArgType22 A22;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12, tpClosureCast->args.A13, tpClosureCast->args.A14, tpClosureCast->args.A15, tpClosureCast->args.A16, tpClosureCast->args.A17, tpClosureCast->args.A18, tpClosureCast->args.A19, tpClosureCast->args.A20, tpClosureCast->args.A21, tpClosureCast->args.A22);\
    }\
        TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, ArgType12 A12, ArgType13 A13, ArgType14 A14, ArgType15 A15, ArgType16 A16, ArgType17 A17, ArgType18 A18, ArgType19 A19, ArgType20 A20, ArgType21 A21, ArgType22 A22) {\
            TPname##_tpClosure_t newClosure;\
            newClosure.args.A0 = A0;\
            newClosure.args.A1 = A1;\
            newClosure.args.A2 = A2;\
            newClosure.args.A3 = A3;\
            newClosure.args.A4 = A4;\
            newClosure.args.A5 = A5;\
            newClosure.args.A6 = A6;\
            newClosure.args.A7 = A7;\
            newClosure.args.A8 = A8;\
            newClosure.args.A9 = A9;\
            newClosure.args.A10 = A10;\
            newClosure.args.A11 = A11;\
            newClosure.args.A12 = A12;\
            newClosure.args.A13 = A13;\
            newClosure.args.A14 = A14;\
            newClosure.args.A15 = A15;\
            newClosure.args.A16 = A16;\
            newClosure.args.A17 = A17;\
            newClosure.args.A18 = A18;\
            newClosure.args.A19 = A19;\
            newClosure.args.A20 = A20;\
            newClosure.args.A21 = A21;\
            newClosure.args.A22 = A22;\
            newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
            newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
            newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
            return newClosure;\
        }\

#define DEFINE_TP_CLOSURE_ARGS24(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12, ArgType13, ArgType14, ArgType15, ArgType16, ArgType17, ArgType18, ArgType19, ArgType20, ArgType21, ArgType22, ArgType23) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        ArgType13 A13;\
        ArgType14 A14;\
        ArgType15 A15;\
        ArgType16 A16;\
        ArgType17 A17;\
        ArgType18 A18;\
        ArgType19 A19;\
        ArgType20 A20;\
        ArgType21 A21;\
        ArgType22 A22;\
        ArgType23 A23;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12, tpClosureCast->args.A13, tpClosureCast->args.A14, tpClosureCast->args.A15, tpClosureCast->args.A16, tpClosureCast->args.A17, tpClosureCast->args.A18, tpClosureCast->args.A19, tpClosureCast->args.A20, tpClosureCast->args.A21, tpClosureCast->args.A22, tpClosureCast->args.A23);\
    }\
        TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, ArgType12 A12, ArgType13 A13, ArgType14 A14, ArgType15 A15, ArgType16 A16, ArgType17 A17, ArgType18 A18, ArgType19 A19, ArgType20 A20, ArgType21 A21, ArgType22 A22, ArgType23 A23) {\
            TPname##_tpClosure_t newClosure;\
            newClosure.args.A0 = A0;\
            newClosure.args.A1 = A1;\
            newClosure.args.A2 = A2;\
            newClosure.args.A3 = A3;\
            newClosure.args.A4 = A4;\
            newClosure.args.A5 = A5;\
            newClosure.args.A6 = A6;\
            newClosure.args.A7 = A7;\
            newClosure.args.A8 = A8;\
            newClosure.args.A9 = A9;\
            newClosure.args.A10 = A10;\
            newClosure.args.A11 = A11;\
            newClosure.args.A12 = A12;\
            newClosure.args.A13 = A13;\
            newClosure.args.A14 = A14;\
            newClosure.args.A15 = A15;\
            newClosure.args.A16 = A16;\
            newClosure.args.A17 = A17;\
            newClosure.args.A18 = A18;\
            newClosure.args.A19 = A19;\
            newClosure.args.A20 = A20;\
            newClosure.args.A21 = A21;\
            newClosure.args.A22 = A22;\
            newClosure.args.A23 = A23;\
            newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
            newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
            newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
            return newClosure;\
        }\

#define DEFINE_TP_CLOSURE_ARGS25(TPname, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6, ArgType7, ArgType8, ArgType9, ArgType10, ArgType11, ArgType12, ArgType13, ArgType14, ArgType15, ArgType16, ArgType17, ArgType18, ArgType19, ArgType20, ArgType21, ArgType22, ArgType23, ArgType24) \
    typedef struct __attribute__ ((__packed__)) TPname##_tpClosure_s {\
        genericTpClosure_t baseClosure;\
        struct __attribute__ ((__packed__)) {\
        ArgType0 A0;\
        ArgType1 A1;\
        ArgType2 A2;\
        ArgType3 A3;\
        ArgType4 A4;\
        ArgType5 A5;\
        ArgType6 A6;\
        ArgType7 A7;\
        ArgType8 A8;\
        ArgType9 A9;\
        ArgType10 A10;\
        ArgType11 A11;\
        ArgType12 A12;\
        ArgType13 A13;\
        ArgType14 A14;\
        ArgType15 A15;\
        ArgType16 A16;\
        ArgType17 A17;\
        ArgType18 A18;\
        ArgType19 A19;\
        ArgType20 A20;\
        ArgType21 A21;\
        ArgType22 A22;\
        ArgType23 A23;\
        ArgType24 A24;\
        }args;\
    }TPname##_tpClosure_t;\
    typedef TPname##_tpClosure_t TPname##_closure;\
    \
    void TPname##_closureExec(_tp_metadata_t* _tp, genericTpClosure_t* tpClosure) {\
        TPname##_tpClosure_t * tpClosureCast = (TPname##_tpClosure_t *) tpClosure;\
        _##TPname##_userInitCtor(_tp, tpClosureCast->args.A0, tpClosureCast->args.A1, tpClosureCast->args.A2, tpClosureCast->args.A3, tpClosureCast->args.A4, tpClosureCast->args.A5, tpClosureCast->args.A6, tpClosureCast->args.A7, tpClosureCast->args.A8, tpClosureCast->args.A9, tpClosureCast->args.A10, tpClosureCast->args.A11, tpClosureCast->args.A12, tpClosureCast->args.A13, tpClosureCast->args.A14, tpClosureCast->args.A15, tpClosureCast->args.A16, tpClosureCast->args.A17, tpClosureCast->args.A18, tpClosureCast->args.A19, tpClosureCast->args.A20, tpClosureCast->args.A21, tpClosureCast->args.A22, tpClosureCast->args.A23, tpClosureCast->args.A24);\
    }\
        TPname##_tpClosure_t _invoke_##TPname(ArgType0 A0, ArgType1 A1, ArgType2 A2, ArgType3 A3, ArgType4 A4, ArgType5 A5, ArgType6 A6, ArgType7 A7, ArgType8 A8, ArgType9 A9, ArgType10 A10, ArgType11 A11, ArgType12 A12, ArgType13 A13, ArgType14 A14, ArgType15 A15, ArgType16 A16, ArgType17 A17, ArgType18 A18, ArgType19 A19, ArgType20 A20, ArgType21 A21, ArgType22 A22, ArgType23 A23, ArgType24 A24) {\
            TPname##_tpClosure_t newClosure;\
            newClosure.args.A0 = A0;\
            newClosure.args.A1 = A1;\
            newClosure.args.A2 = A2;\
            newClosure.args.A3 = A3;\
            newClosure.args.A4 = A4;\
            newClosure.args.A5 = A5;\
            newClosure.args.A6 = A6;\
            newClosure.args.A7 = A7;\
            newClosure.args.A8 = A8;\
            newClosure.args.A9 = A9;\
            newClosure.args.A10 = A10;\
            newClosure.args.A11 = A11;\
            newClosure.args.A12 = A12;\
            newClosure.args.A13 = A13;\
            newClosure.args.A14 = A14;\
            newClosure.args.A15 = A15;\
            newClosure.args.A16 = A16;\
            newClosure.args.A17 = A17;\
            newClosure.args.A18 = A18;\
            newClosure.args.A19 = A19;\
            newClosure.args.A20 = A20;\
            newClosure.args.A21 = A21;\
            newClosure.args.A22 = A22;\
            newClosure.args.A23 = A23;\
            newClosure.args.A24 = A24;\
            newClosure.baseClosure._metadataCtor = _##TPname##_metadataCtor;\
            newClosure.baseClosure._userInitCtorExec = TPname##_closureExec;\
            newClosure.baseClosure.sizeOfArgs = sizeof(newClosure.args);\
            return newClosure;\
        }\

#endif /* TP_CLOSURE_H */
