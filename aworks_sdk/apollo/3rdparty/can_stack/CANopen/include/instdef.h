/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5 / CAN driver

  Description:  definitions for generating instances

  -------------------------------------------------------------------------

                $RCSfile: InstDef.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.19 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

  r.d.: first implementation

****************************************************************************/

#ifndef _INSTDEF_H_
#define _INSTDEF_H_

#ifndef COP_MAX_INSTANCES
    #define COP_MAX_INSTANCES   CDRV_MAX_INSTANCES
#endif

#if (COP_MAX_INSTANCES == 0)
    #error "ERROR: invalid value for COP_MAX_INSTANCES / CDRV_MAX_INSTANCES!"
#endif


// =========================================================================
// types and macros for generating instances
// =========================================================================

typedef enum
{
    kStateUnused    = 0,
    kStateDeleted   = 1,
    kStateUsed      = 0xFF

} tInstState;

//------------------------------------------------------------------------------------------

typedef void MEM* tCopPtrInstance;
typedef BYTE      tCopInstanceHdl;

// define const for illegale values
#define CCM_ILLINSTANCE      NULL
#define CCM_ILLINSTANCE_HDL  0xFF

//------------------------------------------------------------------------------------------
// if more than one instance then use this macros
#if (COP_MAX_INSTANCES > 1)

    //--------------------------------------------------------------------------------------
    // macro definition for instance table definition
    //--------------------------------------------------------------------------------------

    // memory attributes for instance table
    #define INST_NEAR                               // faster access to variables
    #define INST_FAR                                // variables wich have to located in xdata
    #define STATIC                                  // prevent warnings for variables with same name

    #define INSTANCE_TYPE_BEGIN     typedef struct {
    #define INSTANCE_TYPE_END       } tCopInstanceInfo;


    //--------------------------------------------------------------------------------------
    // macro definition for API interface
    //--------------------------------------------------------------------------------------

    // declaration:

    // macros for declaration within function header or prototype of API functions
    #define CCM_DECL_INSTANCE_HDL                   tCopInstanceHdl InstanceHandle
    #define CCM_DECL_INSTANCE_HDL_                  tCopInstanceHdl InstanceHandle,

    // macros for declaration of pointer to instance handle within function header or prototype of API functions
    #define CCM_DECL_PTR_INSTANCE_HDL               tCopInstanceHdl MEM* pInstanceHandle
    #define CCM_DECL_PTR_INSTANCE_HDL_              tCopInstanceHdl MEM* pInstanceHandle,

    // macros for declaration instance as lokacl variable within functions
    #define CCM_DECL_INSTANCE_PTR_LOCAL             tCcmInstanceInfo MEM* pInstance;
    #define CCM_DECL_PTR_INSTANCE_HDL_LOCAL         tCopInstanceHdl  MEM* pInstanceHandle;

    #define CCM_DECL_INSTANCE_PTR                   tCcmInstanceInfo MEM* pInstance
    #define CCM_DECL_INSTANCE_PTR_                  tCcmInstanceInfo MEM* pInstance,

    // reference:

    // macros for reference of instance handle for function parameters
    #define CCM_INSTANCE_HDL                        InstanceHandle
    #define CCM_INSTANCE_HDL_                       InstanceHandle,

    // macros for reference of instance parameter for function parameters
    #define CCM_INSTANCE_PARAM(par)                 par
    #define CCM_INSTANCE_PARAM_(par)                par,

    #define CCM_INSTANCE_PTR                        pInstance
    #define CCM_INSTANCE_PTR_                       pInstance,

    // macros for reference of instance parameter for writing or reading values
    #define CCM_INST_ENTRY                          (*((tCopPtrInstance)pInstance))

    #define CCM_INSTANCE_ARRAY_IDX                  InstanceHandle

    // processing:

    // macros for process instance handle
    #define CCM_CHECK_INSTANCE_HDL()                if (InstanceHandle >= COP_MAX_INSTANCES) \
                                                        {return (kCopIllegalInstance);}


    // macros for process pointer to instance handle
    #define CCM_CHECK_PTR_INSTANCE_HDL()            if (pInstanceHandle == NULL) \
                                                        {return (kCopInvalidInstanceParam);}

    // This macro returned the handle and pointer to next free instance.
    #define CCM_GET_FREE_INSTANCE_AND_HDL()         pInstance = CcmGetFreeInstanceAndHandle (pInstanceHandle); \
                                                    ASSERT (*pInstanceHandle != CCM_ILLINSTANCE_HDL);

    #define CCM_CHECK_INSTANCE_PTR()                if (pInstance == CCM_ILLINSTANCE) \
                                                        {return (kCopNoFreeInstance);}

    #define CCM_GET_INSTANCE_PTR()                  pInstance = CcmGetInstancePtr (InstanceHandle);
    #define CCM_GET_FREE_INSTANCE_PTR()             pInstance = GetFreeInstance (); \
                                                    ASSERT (pInstance != CCM_ILLINSTANCE);


    //--------------------------------------------------------------------------------------
    // macro definition for stack interface
    //--------------------------------------------------------------------------------------

    // macros for declaration within the function header, prototype or local var list
    // Declaration of pointers within function paramater list must defined as void MEM*
    // pointer. 
    #define MCO_DECL_INSTANCE_PTR                   void MEM* pInstance  
    #define MCO_DECL_INSTANCE_PTR_                  void MEM* pInstance, 
    #define MCO_DECL_INSTANCE_PTR_LOCAL             tCopPtrInstance  pInstance;

    // macros for reference of pointer to instance
    // These macros are used for parameter passing to called function.
    #define MCO_INSTANCE_PTR                        pInstance
    #define MCO_INSTANCE_PTR_                       pInstance,
    #define MCO_ADDR_INSTANCE_PTR_                  &pInstance,

    // macro for access of struct members of one instance
    // An access to a member of instance table must be casted by the local
    // defined type of instance table.
    #define MCO_INST_ENTRY                          (*(tCopPtrInstance)pInstance)
    #define MCO_GLB_VAR(var)                        (((tCopPtrInstance)pInstance)->var)

    // macros for process pointer to instance
    #define MCO_GET_INSTANCE_PTR()                  pInstance = (tCopPtrInstance) GetInstancePtr (InstanceHandle);
    #define MCO_GET_FREE_INSTANCE_PTR()             pInstance = (tCopPtrInstance) GetFreeInstance (); \
                                                    ASSERT (pInstance != CCM_ILLINSTANCE);

    // This macro should be used to check the passed pointer to an public function
    #define MCO_CHECK_INSTANCE_STATE()              ASSERT (pInstance != NULL); \
                                                    ASSERT (((tCopPtrInstance)pInstance)->m_InstState == kStateUsed);

    // macros for declaration of pointer to instance pointer
    #define MCO_DECL_PTR_INSTANCE_PTR               void MEM*  MEM* pInstancePtr
    #define MCO_DECL_PTR_INSTANCE_PTR_              void MEM*  MEM* pInstancePtr,

    // macros for reference of pointer to instance pointer
    // These macros are used for parameter passing to called function.
    #define MCO_PTR_INSTANCE_PTR                    pInstancePtr 
    #define MCO_PTR_INSTANCE_PTR_                   pInstancePtr,

    // macros for process pointer to instance pointer
    #define MCO_CHECK_PTR_INSTANCE_PTR()            ASSERT (pInstancePtr != NULL);
    #define MCO_SET_PTR_INSTANCE_PTR()              (*pInstancePtr = pInstance);

    #define MCO_INSTANCE_PARAM(a)                   (a)
    #define MCO_INSTANCE_PARAM_(a)                  (a),
    #define MCO_INSTANCE_PARAM_IDX_()               MCO_INSTANCE_PARAM_ (MCO_GLB_VAR (m_bInstIndex))
    #define MCO_INSTANCE_PARAM_IDX()                MCO_INSTANCE_PARAM (MCO_GLB_VAR (m_bInstIndex))
    #define MCO_WRITE_INSTANCE_STATE(a)             MCO_GLB_VAR (m_InstState) = a;

    // this macro deletes all instance entries as unused
    #define MCO_DELETE_INSTANCE_TABLE()                                    \
    {                                                                      \
        tCopInstanceInfo MEM*   pInstance       = &aCopInstanceTable_g[0]; \
        tFastByte               InstNumber      = 0;                       \
        tFastByte               i               = COP_MAX_INSTANCES;       \
        do {                                                               \
            pInstance->m_InstState = (BYTE) kStateUnused;                  \
            pInstance->m_bInstIndex = (BYTE) InstNumber;                   \
            pInstance++; InstNumber++; i--;                                \
        } while (i != 0);                                                  \
    }

    // definition of functions which has to be defined in each module of CANopen stack
    #define MCO_DEFINE_INSTANCE_FCT() \
        static tCopPtrInstance GetInstancePtr (tCopInstanceHdl InstHandle_p);  \
        static tCopPtrInstance GetFreeInstance (void);
    #define MCO_DECL_INSTANCE_FCT()                                            \
        static tCopPtrInstance GetInstancePtr (tCopInstanceHdl InstHandle_p) { \
            return &aCopInstanceTable_g[InstHandle_p]; }                       \
        static tCopPtrInstance GetFreeInstance (void) {                        \
            tCopInstanceInfo MEM*   pInstance   = &aCopInstanceTable_g[0];     \
            tFastByte               i           = COP_MAX_INSTANCES;           \
            do { if (pInstance->m_InstState != kStateUsed) {                   \
                    return (tCopPtrInstance) pInstance; }                      \
                pInstance++; i--; }                                            \
            while (i != 0);                                                    \
            return CCM_ILLINSTANCE; }

    // this macro defines the instance table. Each entry is reserved for an instance of CANopen.
    #define MCO_DECL_INSTANCE_VAR() \
        static tCopInstanceInfo MEM aCopInstanceTable_g [COP_MAX_INSTANCES];

    // this macro defines member variables in instance table which are needed in
    // all modules of CANopen stack
    #define MCO_DECL_INSTANCE_MEMBER() \
        STATIC  BYTE                            m_InstState; \
        STATIC  BYTE                            m_bInstIndex;

    #define MCO_INSTANCE_PARAM_IDX_()           MCO_INSTANCE_PARAM_ (MCO_GLB_VAR (m_bInstIndex))
    #define MCO_INSTANCE_PARAM_IDX()            MCO_INSTANCE_PARAM (MCO_GLB_VAR (m_bInstIndex))

#else       // only one instance is used

    // Memory attributes for instance table.
    #define INST_NEAR   NEAR                        // faster access to variables
    #define INST_FAR    MEM                         // variables wich have to located in xdata
    #define STATIC      static                      // prevent warnings for variables with same name

    #define INSTANCE_TYPE_BEGIN
    #define INSTANCE_TYPE_END

// macros for declaration, initializing and member access for instance handle
// This class of macros are used by API function to inform CCM-modul which 
// instance is to be used.

    // macros for reference of instance handle
    // These macros are used for parameter passing to CANopen API function.
    #define CCM_INSTANCE_HDL
    #define CCM_INSTANCE_HDL_

    #define CCM_DECL_INSTANCE_PTR_LOCAL

    // macros for declaration within the function header or prototype    
    #define CCM_DECL_INSTANCE_HDL                   void
    #define CCM_DECL_INSTANCE_HDL_

    #define CCM_DECL_INSTANCE_PTR                   void
    #define CCM_DECL_INSTANCE_PTR_

    // macros for declaration of pointer to instance handle
    #define CCM_DECL_PTR_INSTANCE_HDL               void
    #define CCM_DECL_PTR_INSTANCE_HDL_

    #define CCM_INSTANCE_PTR
    #define CCM_INSTANCE_PTR_

    // macros for process instance handle
    #define CCM_CHECK_INSTANCE_HDL()

    // macros for process pointer to instance handle
    #define CCM_CHECK_PTR_INSTANCE_HDL()

    // This macro returned the handle and pointer to next free instance.
    #define CCM_GET_FREE_INSTANCE_AND_HDL()

    #define CCM_CHECK_INSTANCE_PTR()

    #define CCM_GET_INSTANCE_PTR()
    #define CCM_GET_FREE_INSTANCE_PTR()

    #define CCM_INSTANCE_PARAM(par)
    #define CCM_INSTANCE_PARAM_(par)

    #define CCM_INST_ENTRY                          aCcmInstanceTable_g[0]
    #define CCM_INSTANCE_ARRAY_IDX                  0

// macros for declaration, initializing and member access for instance pointer
// This class of macros are used by CANopen internal function to point to one instance.

    // macros for declaration within the function header, prototype or local var list
    #define MCO_DECL_INSTANCE_PTR                   void
    #define MCO_DECL_INSTANCE_PTR_
    #define MCO_DECL_INSTANCE_PTR_LOCAL

    // macros for reference of pointer to instance
    // These macros are used for parameter passing to called function.
    #define MCO_INSTANCE_PTR
    #define MCO_INSTANCE_PTR_
    #define MCO_ADDR_INSTANCE_PTR_

    // macros for process pointer to instance
    #define MCO_GET_INSTANCE_PTR()
    #define MCO_GET_FREE_INSTANCE_PTR()

    // This macro should be used to check the passed pointer to an public function
    #define MCO_CHECK_INSTANCE_STATE()

    // macros for declaration of pointer to instance pointer
    #define MCO_DECL_PTR_INSTANCE_PTR               void
    #define MCO_DECL_PTR_INSTANCE_PTR_

    // macros for reference of pointer to instance pointer
    // These macros are used for parameter passing to called function.
    #define MCO_PTR_INSTANCE_PTR
    #define MCO_PTR_INSTANCE_PTR_

    // macros for process pointer to instance pointer
    #define MCO_CHECK_PTR_INSTANCE_PTR()
    #define MCO_SET_PTR_INSTANCE_PTR()

    #define MCO_INSTANCE_PARAM(a)
    #define MCO_INSTANCE_PARAM_(a)
    #define MCO_INSTANCE_PARAM_IDX_()
    #define MCO_INSTANCE_PARAM_IDX()

    // macro for access of struct members of one instance
    #define MCO_INST_ENTRY                          aCopInstanceTable_g[0]
    #define MCO_GLB_VAR(var)                        (var)
    #define MCO_WRITE_INSTANCE_STATE(a)

    // this macro deletes all instance entries as unused
    #define MCO_DELETE_INSTANCE_TABLE()

    // definition of functions which has to be defined in each module of CANopen stack
    #define MCO_DEFINE_INSTANCE_FCT()
    #define MCO_DECL_INSTANCE_FCT()

    // this macro defines the instance table. Each entry is reserved for an instance of CANopen.
    #define MCO_DECL_INSTANCE_VAR()

    // this macro defines member variables in instance table which are needed in
    // all modules of CANopen stack
    #define MCO_DECL_INSTANCE_MEMBER()

#endif



#if (CDRV_MAX_INSTANCES > 1)

    #define CDRV_REENTRANT                          REENTRANT

#else

    #define CDRV_REENTRANT

#endif



#endif // _INSTDEF_H_

// Please keep an empty line at the end of this file.

