/****************************************************************************

   Definitions for Abstract Memory Interface

   (c) SYS TEC electronic GmbH, Greiz

   06.03.2000  -rs 
               Implementation

   16.09.2002  -as
               To save code space the functions AmiSetByte and AmiGetByte
               are replaced by macros. For targets which assign BYTE by 
               an 16Bit type, the definition of macros must changed to 
               functions.

   23.02.2005  r.d.:
               Functions included for extended data types such as UNSIGNED24,
               UNSIGNED40, ...

****************************************************************************/

#ifndef _AMI_H_
#define _AMI_H_


#if ((DEV_SYSTEM & _DEV_64BIT_SUPPORT_) == 0)
    #ifdef QWORD
        #error 'ERROR: development system does not support 64 bit operations!'
    #endif
#endif


//---------------------------------------------------------------------------
//  types
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//  Prototypen
//---------------------------------------------------------------------------

#ifdef __cplusplus
    extern "C" {
#endif 


//---------------------------------------------------------------------------
//
// read functions
//
// To save code space the function AmiSetByte is replaced by
// an macro. 
// void   PUBLIC  AmiSetByte  (void FAR* pAddr_p, BYTE bByteVal_p);

#define AmiSetByte(pAddr_p, bByteVal_p)  {*(BYTE FAR*)(pAddr_p) = (bByteVal_p);}

void   PUBLIC  AmiSetWord  (void FAR* pAddr_p, WORD wWordVal_p);
void   PUBLIC  AmiSetDword (void FAR* pAddr_p, DWORD dwDwordVal_p);


//---------------------------------------------------------------------------
//
// write functions
//
// To save code space the function AmiGetByte is replaced by
// an macro. 
// BYTE   PUBLIC  AmiGetByte  (void FAR* pAddr_p);

#define AmiGetByte(pAddr_p)  (*(BYTE FAR*)(pAddr_p))

WORD   PUBLIC  AmiGetWord  (void FAR* pAddr_p);
DWORD  PUBLIC  AmiGetDword (void FAR* pAddr_p);


//---------------------------------------------------------------------------
//
// Function:    AmiSetDword24()
//
// Description: sets a 24 bit value to a buffer
//
// Parameters:  pAddr_p         = pointer to destination buffer
//              dwDwordVal_p    = value to set
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC AmiSetDword24 (void FAR* pAddr_p, DWORD dwDwordVal_p);


//---------------------------------------------------------------------------
//
// Function:    AmiGetDword24()
//
// Description: reads a 24 bit value from a buffer
//
// Parameters:  pAddr_p         = pointer to source buffer
//
// Return:      DWORD           = read value
//
//---------------------------------------------------------------------------

DWORD PUBLIC AmiGetDword24 (void FAR* pAddr_p);


#ifdef QWORD

//---------------------------------------------------------------------------
//
// Function:    AmiSetQword40()
//
// Description: sets a 40 bit value to a buffer
//
// Parameters:  pAddr_p         = pointer to destination buffer
//              qwQwordVal_p    = quadruple word value
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC AmiSetQword40 (void FAR* pAddr_p, QWORD qwQwordVal_p);


//---------------------------------------------------------------------------
//
// Function:    AmiGetQword40()
//
// Description: reads a 40 bit value from a buffer
//
// Parameters:  pAddr_p         = pointer to source buffer
//
// Return:      QWORD
//
//---------------------------------------------------------------------------

QWORD PUBLIC AmiGetQword40 (void FAR* pAddr_p);


//---------------------------------------------------------------------------
//
// Function:    AmiSetQword48()
//
// Description: sets a 48 bit value to a buffer
//
// Parameters:  pAddr_p         = pointer to destination buffer
//              qwQwordVal_p    = quadruple word value
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC AmiSetQword48 (void FAR* pAddr_p, QWORD qwQwordVal_p);


//---------------------------------------------------------------------------
//
// Function:    AmiGetQword48()
//
// Description: reads a 48 bit value from a buffer
//
// Parameters:  pAddr_p         = pointer to source buffer
//
// Return:      QWORD
//
//---------------------------------------------------------------------------

QWORD PUBLIC AmiGetQword48 (void FAR* pAddr_p);


//---------------------------------------------------------------------------
//
// Function:    AmiSetQword56()
//
// Description: sets a 56 bit value to a buffer
//
// Parameters:  pAddr_p         = pointer to destination buffer
//              qwQwordVal_p    = quadruple word value
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC AmiSetQword56 (void FAR* pAddr_p, QWORD qwQwordVal_p);


//---------------------------------------------------------------------------
//
// Function:    AmiGetQword56()
//
// Description: reads a 56 bit value from a buffer
//
// Parameters:  pAddr_p         = pointer to source buffer
//
// Return:      QWORD
//
//---------------------------------------------------------------------------

QWORD PUBLIC AmiGetQword56 (void FAR* pAddr_p);


//---------------------------------------------------------------------------
//
// Function:    AmiSetQword64()
//
// Description: sets a 64 bit value to a buffer
//
// Parameters:  pAddr_p         = pointer to destination buffer
//              qwQwordVal_p    = quadruple word value
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC AmiSetQword64 (void FAR* pAddr_p, QWORD qwQwordVal_p);


//---------------------------------------------------------------------------
//
// Function:    AmiGetQword64()
//
// Description: reads a 64 bit value from a buffer
//
// Parameters:  pAddr_p         = pointer to source buffer
//
// Return:      void
//
//---------------------------------------------------------------------------

QWORD PUBLIC AmiGetQword64 (void FAR* pAddr_p);


//---------------------------------------------------------------------------
//
// Function:    AmiSetTimeOfDay()
//
// Description: sets a TIME_OF_DAY (CANopen) value to a buffer
//
// Parameters:  pAddr_p         = pointer to destination buffer
//              pTimeOfDay_p    = pointer to struct TIME_OF_DAY
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC AmiSetTimeOfDay (void FAR* pAddr_p, tTimeOfDay FAR* pTimeOfDay_p);


//---------------------------------------------------------------------------
//
// Function:    AmiGetTimeOfDay()
//
// Description: reads a TIME_OF_DAY (CANopen) value from a buffer
//
// Parameters:  pAddr_p         = pointer to source buffer
//              pTimeOfDay_p    = pointer to struct TIME_OF_DAY
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC AmiGetTimeOfDay (void FAR* pAddr_p, tTimeOfDay FAR* pTimeOfDay_p);


#endif // QWORD



#ifdef __cplusplus
    }
#endif 


#endif  // ifndef _AMI_H_

// Please keep an empty line at the end of this file.
