/*

Copyright (C) 2015-2018 Night Dive Studios, LLC.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
//		RES_.H	Resource System internal header file
//		Rex E. Bradford
/*
* $Header: r:/prj/lib/src/res/rcs/res_.h 1.4 1994/11/30 20:39:39 xemu Exp $
* $Log: res_.h $
 * Revision 1.4  1994/11/30  20:39:39  xemu
 * cd spoof drive structs
 *
 * Revision 1.3  1994/09/22  10:47:01  rex
 * Got rid of offset alignment stuff (offset field no longer needs shifting).
 *
 * Revision 1.2  1994/05/26  13:54:52  rex
 * Added stuff for installable & default pager
 *
 * Revision 1.1  1994/02/17  11:22:56  rex
 * Initial revision
 *
*/

#ifndef __RES__H
#define __RES__H

#ifndef __RES_H
#include "res.h"
#endif
//#ifndef ___RES_H
//#include <_res.h>
//#endif

//	----------------------------------------------------------
//		FOR RESOURCE SYSTEM INTERNAL USE - DON'T BE BAD!
//	----------------------------------------------------------

//	Checking id's and ref's (resacc.c and refacc.c)

bool ResCheckId(Id id);				// returns TRUE if id ok, else FALSE + warns
bool RefCheckRef(Ref ref);			// returns TRUE if ref ok, else FALSE & warns

//	Resource loading (resload.c)

void *ResLoadResource(Id id);
bool ResRetrieve(Id id, void *buffer);

//	Resource paging (resmem.c)

void *ResDefaultPager(int32_t size);
extern void *(*f_pager)(int32_t size);
extern Id idBeingLoaded;
#define RES_PAGER(size) (*f_pager)(size)

//	Grow descriptor table (res.c)

void ResGrowResDescTable(Id id);

#define ResExtendDesc(id) {if ((id) > resDescMax) ResGrowResDescTable(id);}

#define DEFAULT_RES_GROWDIRENTRIES 128		// must be power of 2

//	Data alignment aids

#define RES_OFFSET_ALIGN(offset) (((offset)+3)&0xFFFFFFFCL)
#define RES_OFFSET_PADBYTES(size) ((4-(size))&3)

#define RES_OFFSET_REAL2DESC(offset) (offset)
#define RES_OFFSET_DESC2REAL(offset) (offset)

//#define RES_OFFSET_REAL2DESC(offset) ((offset)>>2)
//#define RES_OFFSET_DESC2REAL(offset) ((offset)<<2)

#define RES_OFFSET_PENDING 1	// offset of resource not yet written

//	LRU chain link management macros

#define ResRemoveFromLRU(prd) { \
	gResDesc[(prd)->next].prev = (prd)->prev; \
	gResDesc[(prd)->prev].next = (prd)->next; }

#define ResAddToTail(prd) { \
	(prd)->prev = gResDesc[ID_TAIL].prev; \
	(prd)->next = ID_TAIL; \
	gResDesc[(prd)->prev].next = RESDESC_ID(prd); \
	gResDesc[ID_TAIL].prev = RESDESC_ID(prd); }

#define ResMoveToTail(prd) { \
	if ((prd)->next != ID_TAIL) \
		{ \
		ResRemoveFromLRU(prd); \
		ResAddToTail(prd); \
		} \
	}

//	Statistics tables

#ifdef DBG_ON

typedef struct {
	uint32_t numGets;				// # ResGet()'s or RefGet()'s
	uint32_t numLocks;			// # ResLock()'s or RefLock()'s
	uint16_t numExtracts;		// # ResExtract()'s or RefExtract()'s
	uint16_t numLoads;			// # ResLoad()'s
	uint16_t numOverwrites;	// # times resource overwritten by one in new file
	uint16_t numPageouts;		// # times paged out of ram
} ResCumStat;

extern ResCumStat *pCumStatId;						// ptr to cumulative stats by id
extern ResCumStat cumStatType[NUM_RESTYPENAMES]; // table of cum. stats by type

#define CUMSTATS(id,field) DBG(DSRC_RES_CumStat, { \
	ResCumStat *prcs;				\
	if (pCumStatId == NULL)		\
		ResAllocCumStatTable();	\
	prcs = pCumStatId + (id);	\
	prcs->field++;					\
	prcs = &cumStatType[RESDESC2(id)->type]; \
	prcs->field++;					\
	})

typedef struct {
	uint16_t numPageouts;			// # times ResPageOut() called
	uint32_t totSizeNeeded;			// total # bytes asked for across calls
	uint32_t totSizeGotten;			// total # bytes paged out across calls
} ResPageStats;

extern ResPageStats resPageStats;	// paging statistics

void ResAllocCumStatTable();			// internal stat routine prototypes
void ResSpewCumStats();					// these are in rescum.c

#else

#define CUMSTATS(id,field)

#endif

typedef struct {
   char *path;
   void (*callback)(int size, Id id);
} ResSpoofDrive;

extern ResSpoofDrive gSpoofDrive;

#endif

