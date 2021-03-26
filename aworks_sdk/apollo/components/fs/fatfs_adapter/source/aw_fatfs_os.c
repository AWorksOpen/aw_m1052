/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief FatFs AWorks adapter layer
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-07  deo, first implementation
 * \endinternal
 */


#include "apollo.h"
#include "aw_sem.h"
#include "aw_mem.h"
#include "aw_fatfs.h"
#include "string.h"

#if FF_FS_REENTRANT    /* Mutal exclusion */

/*------------------------------------------------------------------------*/
/* Create a Synchronization Object
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to create a new
/  synchronization object for the volume, such as semaphore and mutex.
/  When a 0 is returned, the f_mount() function fails with FR_INT_ERR.
*/

//const osMutexDef_t Mutex[FF_VOLUMES];    /* CMSIS-RTOS */

/* 1:Function succeeded, 0:Could not create the sync object */
int ff_cre_syncobj (  
    BYTE vol,         /* Corresponding volume (logical drive number) */
    FF_SYNC_t *sobj   /* Pointer to return the created sync object */
)
{
  *sobj = (FF_SYNC_t)aw_mem_alloc(sizeof(aw_fatfs_mutex_t));

  if (*sobj == NULL) {
      return 0;
  }

  memset(*sobj, 0, sizeof(aw_fatfs_mutex_t));

  if (NULL == AW_MUTEX_INIT((*sobj)->lock, AW_SEM_Q_PRIORITY)) {
      aw_mem_free((void *)(*sobj));
      return 0;
  }

  return 1;
}



/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to delete a synchronization
/  object that created with ff_cre_syncobj() function. When a 0 is returned,
/  the f_mount() function fails with FR_INT_ERR.
*/
/* 1:Function succeeded, 0:Could not delete due to an error */
int ff_del_syncobj (    
    FF_SYNC_t sobj        /* Sync object tied to the logical drive to be deleted */
)
{
  AW_MUTEX_TERMINATE(sobj->lock);
  aw_mem_free((void *)sobj);

  return 1;
}



/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/
/* 1:Got a grant to access the volume, 0:Could not get a grant */
int ff_req_grant (    
    FF_SYNC_t sobj    /* Sync object to wait */
)
{

  if (AW_OK == AW_MUTEX_LOCK(sobj->lock, FF_FS_TIMEOUT)) {
      return 1;
  } else {
      return 0;
  }
}



/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
*/

void ff_rel_grant (
    FF_SYNC_t sobj    /* Sync object to be signaled */
)
{
    AW_MUTEX_UNLOCK(sobj->lock);
}
#endif


#if FF_USE_LFN == 3   /* LFN with a working buffer on the heap */
/*------------------------------------------------------------------------*/
/* Allocate a memory block                                                */
/*------------------------------------------------------------------------*/
/* Returns pointer to the allocated memory block (null on not enough core) */
void* ff_memalloc (    
    UINT msize        /* Number of bytes to allocate */
)
{
    return aw_mem_alloc(msize);
}


/*------------------------------------------------------------------------*/
/* Free a memory block                                                    */
/*------------------------------------------------------------------------*/

void ff_memfree (
    void* mblock    /* Pointer to the memory block to free */
)
{
    aw_mem_free(mblock);
}

#endif

/* end of file */
