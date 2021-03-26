/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  Header file for CANopen API for DS402
                drives and motion control

  -------------------------------------------------------------------------

                $RCSfile: Ccm402.h,v $
                
                $Author: F.Jungandreas $
                
                $Revision: 1.2 $  $Date: 2006/06/30 07:13:16 $
                
                $State: Exp $
                
                Build Environment:
                KEIL uVision 3

  -------------------------------------------------------------------------

  Revision History:
  2006/06/15 f.j.:  start of implementation
  2006/06/30 f.j.:  tested for 1 or 2 axis, not tested for more than 1 instances


****************************************************************************/

#ifndef _CCM402_H_
#define _CCM402_H_


//select drive operation modes
#define CCM402_USE_PROFILE_POSITION_MODE        /*TRUE*/ FALSE
#define CCM402_USE_VELOCITY_MODE                TRUE /*FALSE*/
#define CCM402_USE_PROFILE_VELOCITY_MODE        TRUE /*FALSE*/
#define CCM402_USE_TORQUE_PROFILE_MODE          TRUE /*FALSE*/
#define CCM402_USE_HOMING_MODE                  /*TRUE*/ FALSE
#define CCM402_USE_INTERPOLATED_POSITION_MODE   /*TRUE*/ FALSE


//select the number of drives on the device
#define CCM402_MAX_AXIS                         1 /* attention: please select the correct object dictionary */


#if (CCM402_MAX_AXIS > 8)
    #error 'only 8 axis possible'
#endif

//instance makros for DS402, must not change
#if (CCM402_MAX_AXIS > 1)

    #define CCM402_DECL_AXIS_                   ,BYTE bAxis_p
    #define CCM402_DECL_AXIS                    BYTE bAxis_p
    #define CCM402_AXIS                         bAxis_p
    #define CCM402_AXIS_                        ,bAxis_p

    #if (COP_MAX_INSTANCES > 1)
        #define COMMA                           ,
    #else
        #define COMMA
        #undef  CCM_DECL_INSTANCE_HDL
        #define CCM_DECL_INSTANCE_HDL
    #endif
#else
    #define CCM402_DECL_AXIS
    #define CCM402_DECL_AXIS_
    #define CCM402_AXIS
    #define CCM402_AXIS_
    #define COMMA
#endif

//---------------------------------------------------------------------------
// typedef
//---------------------------------------------------------------------------

#define AXIS_OFFSET                     0x800 //offset between 2 axis in the object dictionary

// Define the states of the DS402 statemachine, see DS402
typedef enum
{
    kDs402State_START                   = 0,    //after Power On or Reset   
    kDs402State_NOT_READY_TO_SWITCH_ON  = 1,
    kDs402State_SWITCH_ON_DISABLED      = 2,
    kDs402State_READY_TO_SWITCH_ON      = 3,
    kDs402State_SWITCHED_ON             = 4,
    kDs402State_OPERATION_ENABLE        = 5,
    kDs402State_QUICK_STOP_ACTIV        = 6,
    kDs402State_FAULT_REACTION_ACTIV    = 7,
    kDs402State_FAULT                   = 8
} tDs402States;


// Define the drive operation modes, see DS402
typedef enum
{
    kOpMode_reserved0                   = 0,    //reserved
    kOpMode_ppProfilePositionMode       = 1,
    kOpMode_vlVelocityMode              = 2,
    kOpMode_pvProfileVelocityMode       = 3,
    kOpMode_tqTorqueProfileMode         = 4,
    kOpMode_reserved5                   = 5,    //reserved
    kOpMode_hm_HomingMode               = 6,  
    kOpMode_InterpolatedPositionMode    = 7,
    kOpMode_reserved8                   = 8     //reserved
} tDs402OperationMode;


typedef struct{

    tDs402States        m_Ds402State_g;       // include the actual state of the statemachine
    WORD  MEM           m_wOldControlword_g;  // previously controlword for axis
}tAxis;

// Define bits and masks for controlword
/*
 controlword
 +----+----+----+----+----+----+---+---+---+---+---+---+---+---+---+---+
 | 15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 +----+----+----+----+----+----+---+---+---+---+---+---+---+---+---+---+
    ^    ^    ^    ^    ^    ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^ 
    |    |    |    |    |    |   |   |   |   |   |   |   |   |   |   |
    |    |    |    |    |    |   |   |   |   |   |   |   |   |   |   +- switch on (M)
    |    |    |    |    |    |   |   |   |   |   |   |   |   |   +----- enable voltage (M)
    |    |    |    |    |    |   |   |   |   |   |   |   |   +--------- quick stop (M)
    |    |    |    |    |    |   |   |   |   |   |   |   +------------- enable operation (M)
    |    |    |    |    |    |   |   |   |   +---+---+----------------- operation mode specific (O)
    |    |    |    |    |    |   |   |   +----------------------------- fault reset (M)
    |    |    |    |    |    |   |   +--------------------------------- halt (O)
    |    |    |    |    |    +---+------------------------------------- reserved (O)
    +----+----+----+----+---------------------------------------------- manufacturer specific (O)

*/
#define CTRLW_SWITCH_ON                 0x0001  //bit 0     0000 0000 0000 0001 mandatory
#define CTRLW_ENABLE_VOLTAGE            0x0002  //bit 1     0000 0000 0000 0010 mandatory
#define CTRLW_QUICK_STOP                0x0004  //bit 2     0000 0000 0000 0100 mandatory
#define CTRLW_ENABLE_OPERATION          0x0008  //bit 3     0000 0000 0000 1000 mandatory
#define CTRLW_OPERATION_MODE_SPECIFIC   0x0070  //bit 4-6   0000 0000 0111 0000 optional
#define CTRLW_FAULT_RESET               0x0080  //bit 7     0000 0000 1000 0000 mandatory
#define CTRLW_HALT                      0x0100  //bit 8     0000 0001 0000 0000 optional
#define CTRLW_RESERVED                  0x0600  //bit 9-10  0000 0110 0000 0000 optional
#define CTRLW_MANUFACTURER_SPECIFIC     0xF100  //bit 11-15 1111 1000 0000 0000 optional

// define commands triggered by the bit patterns in the controlword
#define CMD_SHUTDOWN_MAKS               0x0087                  //xxxx xxxx 1xxx x111
#define CMD_SHUTDOWN                    CTRLW_ENABLE_VOLTAGE\
                                      + CTRLW_QUICK_STOP        //xxxx xxxx 0xxx x110

#define CMD_SWITCH_ON_MAKS              0x0087                  //xxxx xxxx 1xxx x111
#define CMD_SWITCH_ON                   CTRLW_SWITCH_ON\
                                      + CTRLW_ENABLE_VOLTAGE\
                                      + CTRLW_QUICK_STOP        //xxxx xxxx 0xxx x111

#define CMD_DISABLE_VOLTAGE_MAKS        0x0082                  //xxxx xxxx 1xxx xx1x
#define CMD_DISABLE_VOLTAGE             0x0000                  //xxxx xxxx 0xxx xx0x

#define CMD_QUICK_STOP_MAKS             0x0086                  //xxxx xxxx 1xxx x11x
#define CMD_QUICK_STOP                  CTRLW_ENABLE_VOLTAGE    //xxxx xxxx 0xxx x01x


#define CMD_DISABLE_OPERATION_MAKS      0x008F                  //xxxx xxxx 1xxx 1111
#define CMD_DISABLE_OPERATION           CTRLW_SWITCH_ON\
                                      + CTRLW_ENABLE_VOLTAGE\
                                      + CTRLW_QUICK_STOP        //xxxx xxxx 0xxx 0111

#define CMD_ENABLE_OPERATION_MAKS       0x008F                  //xxxx xxxx 1xxx 1111
#define CMD_ENABLE_OPERATION            CTRLW_SWITCH_ON\
                                      + CTRLW_ENABLE_VOLTAGE\
                                      + CTRLW_QUICK_STOP\
                                      + CTRLW_ENABLE_OPERATION  //xxxx xxxx 0xxx 1111

#define CMD_FAULT_RESET_MAKS            CTRLW_FAULT_RESET       //xxxx xxxx 1xxx xxxx
#define CMD_FAULT_RESET                 CTRLW_FAULT_RESET       //rising edge


//operation mode specific defines for controlword
//profile position mode
#define CTRLW_PP_NEW_SET_POINT          0x0010  //bit 4     0000 0000 0001 0000 optional
#define CTRLW_PP_CHANGE_SET_IMMEDIATELY 0x0020  //bit 5     0000 0000 0010 0000 optional
#define CTRLW_PP_ABS_REL                0x0040  //bit 6     0000 0000 0100 0000 optional

//profile velocity mode
#define CTRLW_PV_RFG_ENABLE             0x0010  //bit 4     0000 0000 0001 0000 optional
#define CTRLW_PV_RFG_UNLOCK             0x0020  //bit 5     0000 0000 0010 0000 optional
#define CTRLW_PV_RFG_USE_REF            0x0040  //bit 6     0000 0000 0100 0000 optional

//homing mode
#define CTRLW_HM_HOMING_OPERATION_START 0x0010  //bit 4     0000 0000 0001 0000 optional

//interpolated position mode
#define CTRLW_ENABLE_IP_MODE            0x0010  //bit 4     0000 0000 0001 0000 optional


// Define bits and masks for statusword
/*
 statusword
 +----+----+----+----+----+----+---+---+---+---+---+---+---+---+---+---+
 | 15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 +----+----+----+----+----+----+---+---+---+---+---+---+---+---+---+---+
    ^    ^    ^    ^    ^    ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^ 
    |    |    |    |    |    |   |   |   |   |   |   |   |   |   |   |
    |    |    |    |    |    |   |   |   |   |   |   |   |   |   |   +- ready to switch on (M)
    |    |    |    |    |    |   |   |   |   |   |   |   |   |   +----- switched on (M)
    |    |    |    |    |    |   |   |   |   |   |   |   |   +--------- operation enable (M)
    |    |    |    |    |    |   |   |   |   |   |   |   +------------- fault (M)
    |    |    |    |    |    |   |   |   |   |   |   +----------------- voltage enable (M)
    |    |    |    |    |    |   |   |   |   |   +----------------------quick stop (M)
    |    |    |    |    |    |   |   |   |   +------------------------- switch on disable (M)
    |    |    |    |    |    |   |   |   +----------------------------- warning (O)
    |    |    |    |    |    |   |   +--------------------------------- manufacturer specific (O)
    |    |    |    |    |    |   +------------------------------------- remote (M)
    |    |    |    |    |    +----------------------------------------- target reached (M)
    |    |    |    |    +---------------------------------------------- internal limit active (M)
    |    |    +----+--------------------------------------------------- operation mode specific (O)
    +----+------------------------------------------------------------- manufacturer specific (O)

*/
#define STW_READY_TO_SWITCH_ON          0x0001  //bit 0     0000 0000 0000 0001 mandatory
#define STW_SWITCHED_ON                 0x0002  //bit 1     0000 0000 0000 0010 mandatory
#define STW_OPERATION_ENABLE            0x0004  //bit 2     0000 0000 0000 0100 mandatory
#define STW_FAULT                       0x0008  //bit 3     0000 0000 0000 1000 mandatory
#define STW_VOLTAGE_ENABLE              0x0010  //bit 4     0000 0000 0001 0000 mandatory
#define STW_QUICK_STOP                  0x0020  //bit 5     0000 0000 0010 0000 mandatory
#define STW_SWITCH_ON_DISABLED          0x0040  //bit 6     0000 0000 0100 0000 mandatory
#define STW_WARNING                     0x0080  //bit 7     0000 0000 1000 0000 optional
#define STW_MANUFACTURER_SPECIFIC1      0x0100  //bit 8     0000 0001 0000 0000 optional
#define STW_REMOTE                      0x0200  //bit 9     0000 0010 0000 0000 mandatory
#define STW_TARGET_REACHED              0x0400  //bit 10    0000 0100 0000 0000 mandatory
#define STW_INTERNAL_LIMIT_ACTIVE       0x0800  //bit 11    0000 1000 0000 0000 mandatory
#define STW_OPERATION_MODE_SPECIFIC     0x3000  //bit 12-13 0011 0000 0000 0000 optional
#define STW_MANUFACTURER_SPECIFIC2      0xC000  //bit 14-15 1100 0000 0000 0000 optional


#define STW_NOT_READY_TO_SWITCH_ON_MASK 0x004F                  // xxxx xxxx x1xx 1111
#define STW_NOT_READY_TO_SWITCH_ON      0x0000                  // xxxx xxxx x0xx 0000

#define STW_SWITCH_ON_DISABLED_MASK     0x004F                  // xxxx xxxx x1xx 1111
#define STW_SWITCH_ON_DISABLED_VALUE    STW_SWITCH_ON_DISABLED  // xxxx xxxx x1xx 0000

#define STW_READY_TO_SWITCH_ON_MASK     0x006F                  // xxxx xxxx x11x 1111
#define STW_READY_TO_SWITCH_ON_VALUE    STW_READY_TO_SWITCH_ON\
                                      + STW_QUICK_STOP          // xxxx xxxx x01x 0001

#define STW_SWITCHED_ON_MASK            0x006F                  // xxxx xxxx x11x 1111
#define STW_SWITCHED_ON_VALUE           STW_READY_TO_SWITCH_ON\
                                      + STW_SWITCHED_ON\
                                      + STW_QUICK_STOP          // xxxx xxxx x01x 0011


#define STW_OPERATION_ENABLE_MASK       0x006F                  // xxxx xxxx x11x 1111
#define STW_OPERATION_ENABLE_VALUE      STW_READY_TO_SWITCH_ON\
                                      + STW_SWITCHED_ON\
                                      + STW_OPERATION_ENABLE\
                                      + STW_QUICK_STOP          // xxxx xxxx x01x 0111


#define STW_QUICK_STOP_ACTIVE_MASK      0x006F                  // xxxx xxxx x11x 1111
#define STW_QUICK_STOP_ACTIVE_VALUE     STW_READY_TO_SWITCH_ON\
                                      + STW_SWITCHED_ON\
                                      + STW_OPERATION_ENABLE    // xxxx xxxx x00x 0111


#define STW_FAULT_REACTION_ACTIVE_MASK  0x004F                  // xxxx xxxx x1xx 1111
#define STW_FAULT_REACTION_ACTIVE_VALUE STW_READY_TO_SWITCH_ON\
                                      + STW_SWITCHED_ON\
                                      + STW_OPERATION_ENABLE\
                                      + STW_FAULT               // xxxx xxxx x0xx 1111

#define STW_FAULT_MASK                  0x004F                  // xxxx xxxx x1xx 1111
#define STW_FAULT_VALUE                 STW_FAULT               // xxxx xxxx x0xx 1000


//operation mode specific defines for statusword
#define STW_OPERATION_MODE_SPECIFIC     0x3000  //bit 12-13 0011 0000 0000 0000 optional
//profile position mode

//profile velocity mode
#define STW_PV_SPEED                    0x1000  //bit 12    0001 0000 0001 0000 optional
#define STW_PV_MAX_SLIPPAGE_ERROR       0x2000  //bit 13    0010 0000 0001 0000 optional

//homing mode
#define STW_HM_HOMING_ATTAINED          0x1000  //bit 12    0001 0000 0001 0000 optional
#define STW_HM_HOMING_ERROR             0x2000  //bit 13    0010 0000 0001 0000 optional

//interpolated position mode
#define STW_IP_MODE_ACTIVE              0x1000  //bit 12    0001 0000 0001 0000 optional

//---------------------------------------------------------------------------
// function prototypes
//---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//
// Function:    Ccm402Init()
//
// Description: initialize the DS402 statemachine
//              
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              CCM402_DECL_AXIS        = number of axis
//
// Returns:     tCopKernel = error code
//
// State:       
//
// ----------------------------------------------------------------------------
CDRVDLLEXPORT tCopKernel PUBLIC Ccm402Init     (CCM_DECL_INSTANCE_HDL COMMA
                                                CCM402_DECL_AXIS);


// ----------------------------------------------------------------------------
//
// Function:    Ccm402SetState()
//
// Description: set the state of the  DS402 statemachine
//              
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              NewState_p              = new Ds402 state
//              pwStatusword_p          = pointer to statusword e.g. to object 0x6041
//              CCM402_DECL_AXIS        = number of axis
//
// Returns:     void
//
// State:       
//
// ----------------------------------------------------------------------------
CDRVDLLEXPORT void PUBLIC Ccm402SetState       (CCM_DECL_INSTANCE_HDL_
                                                tDs402States NewState_p,
                                                WORD MEM *pwStatusword_p
                                                CCM402_DECL_AXIS_);


// ----------------------------------------------------------------------------
//
// Function:    Ccm402GetState()
//
// Description: set the state of the  DS402 statemachine
//              
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              CCM402_DECL_AXIS        = number of axis
//
// Returns:     tDs402States            = actual Ds402 state
//
// State:       
//
// ----------------------------------------------------------------------------
CDRVDLLEXPORT tDs402States PUBLIC Ccm402GetState (CCM_DECL_INSTANCE_HDL
                                                 COMMA CCM402_DECL_AXIS);

// ----------------------------------------------------------------------------
//
// Function:    Ccm402Process()
//
// Description: include the DS402 statemachine
//              should call cyclic for every axis
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              Controlword_p           = controlword e.g. object 0x6040
//              pwStatusword_p          = pointer to statusword e.g. to object 0x6041
//              CCM402_DECL_AXIS        = number of axis
//
// Returns:     tCopKernel = error code
//
// State:       
//
// ----------------------------------------------------------------------------
CDRVDLLEXPORT tCopKernel PUBLIC Ccm402Process      (CCM_DECL_INSTANCE_HDL_
                                                    WORD Controlword_p,
                                                    WORD MEM *pwStatusword_p 
                                                    CCM402_DECL_AXIS_);

//---------------------------------------------------------------------------
// Function:    Ccm402OperationModeChanged()
//
// Description: function is called, if the DS602 operation mode ist chanded
//              see object 0x6060
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              bNewOperationMode_p     = new operation mode see index e.g. 0x6060 
//              CCM402_DECL_AXIS        = number of axis
//
// Returns:     tCopKernel          = error code
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC Ccm402OperationModeChanged( CCM_DECL_INSTANCE_HDL_
                                                            tObdInteger8 bNewOperationMode_p
                                                            CCM402_DECL_AXIS_);

// ----------------------------------------------------------------------------
//
// Function:    Ccm402Operation()
//
// Description: DS402 statemachine called this function in the state OPERATION_ENABLE 
//              cyclic
//              
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              CCM402_DECL_AXIS        = number of axis
//
// Returns:     TRUE    operation enable allowed
//              FALSE   operation enable NOT allowed
//
// State:       
//
// ----------------------------------------------------------------------------
CDRVDLLEXPORT void PUBLIC Ccm402Operation(CCM_DECL_INSTANCE_HDL_ WORD Controlword_p, 
                                            WORD MEM *pwStatusword_p
                                            CCM402_DECL_AXIS_);
// ----------------------------------------------------------------------------
//
// Function:    Ccm402QuickStopOperation()
//
// Description: DS402 statemachine called this function in the state QUICK_STOP_ACTIVE 
//              cyclic
//              
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              CCM402_DECL_AXIS        = number of axis
//
// Returns:     TRUE    operation enable allowed
//              FALSE   operation enable NOT allowed
//
// State:       
//
// ----------------------------------------------------------------------------
CDRVDLLEXPORT void PUBLIC Ccm402QuickStopOperation(CCM_DECL_INSTANCE_HDL_ WORD Controlword_p, 
                                            WORD MEM *pwStatusword_p
                                            CCM402_DECL_AXIS_);

// ----------------------------------------------------------------------------
//
// Function:    Ccm402OperationEnableAllowed()
//
// Description: DS402 statemachine called this function before the 
//              statemachine changed the state from SWITCHED_ON into
//              OPERATION_ENABEL (tansition 4 or 16)
//              so it is possible for the application, to prohibit to change
//              the state (i.e. the drive ist not ready)
//              
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              CCM402_DECL_AXIS        = number of axis
//
// Returns:     TRUE    operation enable allowed
//              FALSE   operation enable NOT allowed
//
// State:       
//
// ----------------------------------------------------------------------------
CDRVDLLEXPORT BOOL PUBLIC Ccm402OperationEnableAllowed (CCM_DECL_INSTANCE_HDL COMMA
                                                        CCM402_DECL_AXIS);


// ----------------------------------------------------------------------------
//
// Function:    Ccm402QuickStopAllowed()
//
// Description: DS402 statemachine called this function before the 
//              statemachine changed the state from OPERATION_ENABEL into
//              QUICK_STOP_ACTIV (tansition 11)
//              so it is possible for the application, to prohibit to change
//              the state (i.e. the drive ist not ready)
//              
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              CCM402_DECL_AXIS        = number of axis
//
// Returns:     TRUE    operation enable allowed
//              FALSE   operation enable NOT allowed
//
// State:       
//
// ----------------------------------------------------------------------------
CDRVDLLEXPORT BOOL PUBLIC Ccm402QuickStopAllowed (CCM_DECL_INSTANCE_HDL COMMA
                                                  CCM402_DECL_AXIS);


// ----------------------------------------------------------------------------
//
// Function:    Ccm402SwitchedOnAllowed()
//
// Description: DS402 statemachine called this function before the 
//              statemachine changed the state into  SWITCHED_ON
//              (tansition 3 or 5)
//              so it is possible for the application, to prohibit to change
//              the state (i.e. the drive ist not ready)
//              
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              CCM402_DECL_AXIS        = number of axis
//
// Returns:     TRUE    operation enable allowed
//              FALSE   operation enable NOT allowed
//
// State:       
//
// ----------------------------------------------------------------------------
CDRVDLLEXPORT BOOL PUBLIC Ccm402SwitchedOnAllowed (CCM_DECL_INSTANCE_HDL COMMA
                                                   CCM402_DECL_AXIS);

// ----------------------------------------------------------------------------
//
// Function:    Ccm402ReadyToSwitchOnAllowed()
//
// Description: DS402 statemachine called this function before the 
//              statemachine changed the state into  READY_TO_SWITCH_ON
//              (tansition 2, 6 or 8)
//              so it is possible for the application, to prohibit to change
//              the state (i.e. the drive ist not ready)
//              
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              CCM402_DECL_AXIS        = number of axis
//
// Returns:     TRUE    operation enable allowed
//              FALSE   operation enable NOT allowed
//
// State:       
//
// ----------------------------------------------------------------------------
CDRVDLLEXPORT BOOL PUBLIC Ccm402ReadyToSwitchOnAllowed (CCM_DECL_INSTANCE_HDL COMMA
                                                        CCM402_DECL_AXIS);


// ----------------------------------------------------------------------------
//
// Function:    Ccm402SwitchOnDisabledAllowed()
//
// Description: DS402 statemachine called this function before the 
//              statemachine changed the state into  SWITCH_ON_DISABLED
//              (tansition 7, 9, 10, 12 or 15)
//              so it is possible for the application, to prohibit to change
//              the state (i.e. the drive ist not ready)
//              
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              CCM402_DECL_AXIS        = number of axis
//
// Returns:     TRUE    operation enable allowed
//              FALSE   operation enable NOT allowed
//
// State:       
//
// ----------------------------------------------------------------------------
CDRVDLLEXPORT BOOL PUBLIC Ccm402SwitchOnDisabledAllowed (CCM_DECL_INSTANCE_HDL COMMA
                                                         CCM402_DECL_AXIS);

#endif

// Die letzte Zeile mu?unbedingt eine leere Zeile sein, weil manche Compiler
// damit ein Problem haben, wenn das nicht so ist (z.B. GNU oder Borland C++ Builder).
