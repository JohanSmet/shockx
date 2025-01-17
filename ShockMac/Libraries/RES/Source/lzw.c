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
//		LZW.C		New improved super-duper LZW compressor & decompressor
//		This module by Greg Travis and Rex Bradford
//
//		This module implements LZW-based compression and decompression.
//		It has flexible control over the source and destination of the
//		data it uses.  Data is read from a "source" and written to a
//		"destination".  In the case of compression, the source is uncompressed
//		and the destination is compressed; for expansion the reverse is true.
//		Sources and destinations deal in byte values, even though the LZW
//		routine works in 14-bit compression codes.
//
//		Sources may be one of the following standard types:
//
//		BUFF		A memory block
//		FD			A file descriptor (fd = open()), already positioned with seek()
//		FP			A file ptr (fp = fopen()), already positioned with lseek()
//		USER		A user-supplied function
//
//		Destinations may be any of the above 4 types, plus additionally:
//
//		NULL		The bit-bucket.  Used to determine the size of destination
//					data without putting it anywhere.
//
//		The LZW module consists of these public routines:
//
//		LzwInit() - Just sets LzwTerm() to be called on exit.
//
//		LzwTerm() - Just calls LzwFreeBuffer(), to free lzw buffer if it
//						has been malloc'ed.
//
//		LzwSetBuffer() - Sets buffer for lzw compression & expansion routines
//							to use.  The buffer must be at least LZW_BUFF_SIZE
//							in size, which for 14-bit lzw is about 91K.
//
//		LzwMallocBuffer() - Allocates buffer for lzw compression & expansion
//							routines to use.  This routine will be called auto-
//							matically the first time LzwCompress() or LzwExpand()
//							is used if a buffer has not been set or allocated.
//
//		LzwFreeBuffer() - Frees current buffer if allocated.
//
//		LzwCompress() - Compresses data, reading from an uncompressed source
//						and writing compressed bytes to a destination.  Returns
//						the size of the compressed data.  A maximum destination
//						size may be specified, in which case a destination which
//						is about to exceed this will be aborted, returning -1.
//
//		LzwExpand() - Expands data, reading from a compressed source and
//						writing decompressed bytes to a destination.  Returns the
//						size of the decompressed data.  Parameters may be used
//						to capture a subsection of the uncompressed stream (by
//						skipping the first n1 destination bytes and then taking
//						the next n2).
//
//		Lzw.h supplies a large set of macros of the form:
//
//			LzwCompressSrc2Dest(...)  and  LzweExpandSrc2Dest(...)
//
//		which implement all combinations of source and destination types,
//		such as buffer->buffer, fd->buffer, buffer->null, fp->user, etc.
//
//		User types are handy when there is a need to transform the data
//		on its way to or from compression (to enhance the compression, or
//		just to massage the data into a usable form).  For example, a map
//		may want to transform elevations to delta format on the way to
//		and from compression in order to enhance the compression.
//
//		User sources supply two functions of the form:
//
//		void f_SrcCtrl(int32_t srcLoc, LzwCtrl ctrl);
//		uint8_t f_SrcGet();
//
//		The control function is used to set up and tear down the Get()
//		function, which is used to supply the next byte of data.  Before
//		any compression or decompression begins, the SrcCtrl() function
//		is called with the srcLoc argument (supplied at the call to
//		LzwCompress or LzwExpand, its meaning is user-defined), and the
//		ctrl argument set to BEGIN.  After all compression and decompression
//		is done, cleanup is invoked by calling SrcCtrl() with ctrl equal
//		to END.  Between BEGIN and END, the SrcGet() function is called
//		repeatedly to get the next byte from the user input stream.
//
//		User destinations work similarly.  Again, two functions:
//
//		void f_DestCtrl(int32_t destLoc, LzwCtrl ctrl);
//		void f_DestPut(uint8_t byte);
//
//		The control function is called with BEGIN and END just like the
//		source function.  The DestPut() function is called repeatedly to
//		put the next byte to the user output stream.
//
//		Note that user sources can be used for both compression (source of
//		uncompressed bytes) and expansion (source of compressed bytes).
//		Similarly, user destinations can be used for both compression
//		(destination of compressed bytes) and expansion (destination of
//		uncompressed bytes).  This is true of standard sources and
//		destinations as well, of course.

/*
* $Header: r:/prj/lib/src/res/rcs/lzw.c 1.5 1994/09/21 09:34:37 rex Exp $
* $log$
*/

//	------------------------------------------------------------
//		HEADER SECTION
//	------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef __LZW_H
#include "lzw.h"
#endif

//	Important constants

#define MAX_VALUE ((1 << LZW_BITS) - 1)	// end-of-compress-data code
#define MAX_CODE (MAX_VALUE - 2)				// maximum real code allows
#define FLUSH_CODE (MAX_VALUE - 1)			// code to lzw string table
#define HASHING_SHIFT (LZW_BITS-8)			// # bits to shift when hashing
#define FLUSH_PAUSE 1000						// wait on full table before flush

//	Overall lzw buffer info

void *lzwBuffer;						// total buffer
bool lzwBufferMalloced;				// buffer malloced?

//	Global tables used for compression & expansion

int16_t *lzwCodeValue;					// code value array
uint16_t *lzwPrefixCode;				// prefix code array
uint8_t *lzwAppendChar;				// appended chars array
uint8_t *lzwDecodeStack;				// decoded string

uint8_t *lzwFdReadBuff;				// buffer for file descriptor source
uint8_t *lzwFdWriteBuff;				// buffer for file descriptor dest

//	Prototypes of internal routines

int32_t LzwFindMatch(int32_t hash_prefix, uint32_t hash_character);
uint8_t *LzwDecodeString(uint8_t *buffer, uint32_t code);

//	--------------------------------------------------------
//		INITIALIZATION AND TERMINATION
//	--------------------------------------------------------
//
//	LzwInit()  needs to be called once before any of the compression
//	 routines are used.

void LzwInit( void )
{
//	AtExit(LzwTerm);
}

//	------------------------------------------------------------
//
// LzwTerm() needs to be called once when the lzw compression
//	 routines are no longer needed.

void LzwTerm( void )
{
	LzwFreeBuffer();
}

//	------------------------------------------------------------
//		BUFFER SETTING
//	--------------------------------------------------------
//
//	LzwSetBuffer() inits and sets buffer to use.
//
//	Returns: 0 if ok, -1 if buffer not ok

int32_t LzwSetBuffer(void *buff, int32_t buffSize)
{
//	Check buffer size

	if (buffSize < LZW_BUFF_SIZE)
		{
//		Warning(("LzwSetBuffer: buffer too small!\n"));
		return(-1);
		}

//	De-allocate current buffer if malloced

	LzwTerm();

//	Set buffer pointers

	lzwBuffer = buff;
	lzwDecodeStack = lzwBuffer;
	lzwFdReadBuff = ((uint8_t *) lzwDecodeStack) + LZW_DECODE_STACK_SIZE;
	lzwFdWriteBuff = ((uint8_t *) lzwFdWriteBuff) + LZW_FD_READ_BUFF_SIZE;
	lzwCodeValue = (int16_t *) (((uint8_t *) lzwDecodeStack) + LZW_FD_WRITE_BUFF_SIZE);
	lzwPrefixCode = (uint16_t *) (((uint8_t *) lzwCodeValue) + (LZW_TABLE_SIZE * sizeof(uint16_t)));
	lzwAppendChar = ((uint8_t *) lzwPrefixCode) + (LZW_TABLE_SIZE * sizeof(uint16_t));
	lzwBufferMalloced = FALSE;
	return(0);
}

//	------------------------------------------------------------
//
//	LzwMallocBuffer() allocates buffer with Malloc.
//
//	Returns: 0 if success, -1 if error.

int32_t LzwMallocBuffer()
{
	void *buff;

	if ((lzwBuffer == NULL) || (!lzwBufferMalloced))
		{
		buff = malloc(LZW_BUFF_SIZE);
		if (buff == NULL)
			{
//			Warning(("LzwMallocBuffer: failed to allocate buffers\n"));
			return(-1);
			}
		else
			{
			LzwSetBuffer(buff, LZW_BUFF_SIZE);
			lzwBufferMalloced = TRUE;
			}
		}
	return(0);
}

//	------------------------------------------------------------
//
//	LzwFreeBuffer() frees buffer.

void LzwFreeBuffer()
{
	if (lzwBufferMalloced)
		{
		free(lzwBuffer);
		lzwBuffer = NULL;
		lzwBufferMalloced = FALSE;
		}
}

//	------------------------------------------------------------
//		COMPRESSION
//	------------------------------------------------------------
//
//	LzwCompress() does lzw compression.  It reads uncompressed bytes
//	from an input source and outputs compressed bytes to an output
//	destination.  It returns the number of bytes the compressed data
//	took up, or -1 if the compressed data size exceeds the allowed space.
//
//		f_ScrCtrl    = routine to call to control source data stream
//		f_SrcGet     = routine to call to get next input data byte
//		srcLoc       = source data "location", actual type undefined
//		srcSize      = size of source (input) data
//		f_DestCtrl   = routine to call to control destination data stream
//		f_DestPut    = routine to call to put next output data byte
//		destLoc      = dest data "location", actual type undefined
//		destSizeMax  = maximum allowed size of output data
//
//	Returns: actual output compressed size, or -1 if exceeded outputSizeMax
//		(in which case compression has been aborted)

//	This macro is used to accumulate output codes into a bit buffer
//	and call the destination put routine whenever more than 8 bits
//	are available.  If the output data size ever exceeds the alloted
//	size, the source and destination are shut down and -1 is returned.

typedef struct {
	uint32_t next_code;		// next available string code
	uint32_t character;		// current character read from source
	uint32_t string_code;	// current string compress code
	uint32_t index;			// index into string table
	int32_t lzwInputCharCount;		// input character count
	int32_t lzwOutputSize;			// current size of output
	int32_t lzwOutputBitCount;		// current bit location in output
	uint32_t lzwOutputBitBuffer;	// 32-bit buffer holding output bits
} LzwC;

LzwC lzwc;		// current compress state

#define LzwOutputCode(code) { \
	lzwc.lzwOutputBitBuffer |= ((uint32_t) code) << (32-LZW_BITS-lzwc.lzwOutputBitCount); \
	lzwc.lzwOutputBitCount += LZW_BITS; \
	while (lzwc.lzwOutputBitCount >= 8) \
		{ \
		(*f_DestPut)(lzwc.lzwOutputBitBuffer >> 24); \
		if (++lzwc.lzwOutputSize > destSizeMax) \
			{ \
			(*f_SrcCtrl)(srcLoc, END); \
			(*f_DestCtrl)(destLoc, END); \
			return -1L; \
			} \
		lzwc.lzwOutputBitBuffer <<= 8; \
		lzwc.lzwOutputBitCount -= 8; \
		} \
}

int32_t LzwCompress(
	void (*f_SrcCtrl)(intptr_t srcLoc, LzwCtrl ctrl),	// func to control source
	uint8_t (*f_SrcGet)(),						// func to get bytes from source
	int32_t srcLoc,								// source "location" (ptr, FILE *, etc.)
	int32_t srcSize,								// size of source in bytes
	void (*f_DestCtrl)(intptr_t destLoc, LzwCtrl ctrl),	// func to control dest
	void (*f_DestPut)(uint8_t byte),		// func to put bytes to dest
	int32_t destLoc,								// dest "location" (ptr, FILE *, etc.)
	int32_t destSizeMax							// max size of dest (or LZW_MAXSIZE)
)
{

//	If not already initialized, do it

	if (lzwBuffer == NULL)
		{
		if (LzwMallocBuffer() < 0)
			return(0);
		}

//	Set up for compress loop

	lzwc.next_code = 256;             // skip over real 256 char values
	memset(lzwCodeValue, -1, sizeof(int16_t) * LZW_TABLE_SIZE);

	lzwc.lzwOutputSize = 0;
	lzwc.lzwOutputBitCount = 0;
	lzwc.lzwOutputBitBuffer = 0;

	(*f_SrcCtrl)(srcLoc, BEGIN);
	(*f_DestCtrl)(destLoc, BEGIN);

	lzwc.string_code = (*f_SrcGet)();
	lzwc.lzwInputCharCount = 1;

// This is the main loop where it all happens.  This loop runs until all of
// the input has been exhausted.  Note that it stops adding codes to the
// table after all of the possible codes have been defined.

	while (TRUE)
		{

//	Get next input char, if read all data then exit loop

		lzwc.character = (*f_SrcGet)();
		if (lzwc.lzwInputCharCount++ >= srcSize)
			break;

//	See if string is in string table.  If it is, get the code value.

		lzwc.index = LzwFindMatch(lzwc.string_code, lzwc.character);
		if (lzwCodeValue[lzwc.index] != -1)
			lzwc.string_code = lzwCodeValue[lzwc.index];

//	Else if string not in string table, try to add it.

		else
			{
			if (lzwc.next_code <= MAX_CODE)
				{
				lzwCodeValue[lzwc.index] = lzwc.next_code++;
				lzwPrefixCode[lzwc.index] = lzwc.string_code;
				lzwAppendChar[lzwc.index] = lzwc.character;
				LzwOutputCode(lzwc.string_code);
				lzwc.string_code = lzwc.character;
				}

//	Else if table is full and has been for a while, flush it, and drain
//	the code value table too.

			else if (lzwc.next_code > MAX_CODE + FLUSH_PAUSE)
				{
				LzwOutputCode(lzwc.string_code);
				LzwOutputCode(FLUSH_CODE);
				memset(lzwCodeValue, -1, sizeof(int16_t) * LZW_TABLE_SIZE);
			   lzwc.string_code = lzwc.character;
				lzwc.next_code = 256;
				}

//	Else if can't add but table not full, just output the code.

			else
				{
				lzwc.next_code++;
				LzwOutputCode(lzwc.string_code);
				lzwc.string_code = lzwc.character;
				}
			}
		}

//	Done with processing loop, output current code, end-of-data code,
//	and a final 0 to flush the buffer.

	LzwOutputCode(lzwc.string_code);
	LzwOutputCode(MAX_VALUE);
	LzwOutputCode(0);

//	Shut down source and destination and return size of output

	(*f_SrcCtrl)(srcLoc, END);
	(*f_DestCtrl)(destLoc, END);

	return(lzwc.lzwOutputSize);
}

//	-----------------------------------------------------------
//		EXPANSION
//	-----------------------------------------------------------
//
//	LzwExpand() does lzw expansion.  It reads compressed bytes
//	from an input source and outputs uncompressed bytes to an output
//	destination.  It returns the number of bytes the uncompressed data
//	took up.
//
//		f_ScrCtrl    = routine to call to control source data stream
//		f_SrcGet     = routine to call to get next input data byte
//		srcLoc       = source data "location", actual type undefined
//		f_DestCtrl   = routine to call to control destination data stream
//		f_DestPut    = routine to call to put next output data byte
//		destLoc      = dest data "location", actual type undefined
//		destSkip     = # bytes of output to skip over before storing
//		destSize     = # bytes of output to store (if 0, everything)
//
//	Returns: # bytes in uncompressed output

typedef struct {
	int32_t lzwInputBitCount;
	uint32_t lzwInputBitBuffer;
	uint32_t next_code;		// next available string code
	uint32_t new_code;		// next code from source
	uint32_t old_code;		// last code gotten from source
	uint32_t character;		// current char for string stack
	uint8_t *string;					// used to output string in reverse order
	int32_t outputSize;				// size of uncompressed data
	int32_t destSkip;					// # bytes to skip over
	int32_t destSize;					// destination size
} LzwE;

LzwE lzwe;		// current expand state


static uint32_t LzwInputCode(uint8_t (*f_SrcGet)())
{
	uint32_t return_value;

	while (lzwe.lzwInputBitCount <= 24)
		{
		lzwe.lzwInputBitBuffer |= ((uint32_t) (*f_SrcGet)()) << (24 - lzwe.lzwInputBitCount);
		lzwe.lzwInputBitCount += 8;
		}
	return_value = lzwe.lzwInputBitBuffer >> (32 - LZW_BITS);

	lzwe.lzwInputBitBuffer <<= LZW_BITS;
	lzwe.lzwInputBitCount -= LZW_BITS;

	return(return_value);
}


int32_t LzwExpand(
	void (*f_SrcCtrl)(intptr_t srcLoc, LzwCtrl ctrl),	// func to control source
	uint8_t (*f_SrcGet)(),						// func to get bytes from source
	int32_t srcLoc,								// source "location" (ptr, FILE *, etc.)
	void (*f_DestCtrl)(intptr_t destLoc, LzwCtrl ctrl),	// func to control dest
	void (*f_DestPut)(uint8_t byte),		// func to put bytes to dest
	int32_t destLoc,								// dest "location" (ptr, FILE *, etc.)
	int32_t destSkip,								// # dest bytes to skip over (or 0)
	int32_t destSize								// # dest bytes to capture (if 0, all)
)
{

//	If not already initialized, do it

	if (lzwBuffer == NULL)
		{
		if (LzwMallocBuffer() < 0)
			return(0);
		}

//	Set up for expansion loop

	lzwe.lzwInputBitCount = 0;
	lzwe.lzwInputBitBuffer = 0;
	lzwe.next_code = 256;			// next available char after regular 256 chars
	lzwe.outputSize = 0;
	lzwe.destSkip = destSkip;
	lzwe.destSize = destSize ? destSize : LZW_MAXSIZE;

//	Notify the control routines

	(*f_SrcCtrl)(srcLoc, BEGIN);
	(*f_DestCtrl)(destLoc, BEGIN);

//	Get first code & output it.

	lzwe.old_code = LzwInputCode(f_SrcGet);
	lzwe.character = lzwe.old_code;

	if (--lzwe.destSkip < 0)
		{
		(*f_DestPut)(lzwe.old_code); lzwe.outputSize++;
		}

//	This is the expansion loop.  It reads in codes from the source until
//	it sees the special end-of-data code.

	while ((lzwe.new_code = LzwInputCode(f_SrcGet)) != MAX_VALUE)
		{

//	If flush code, flush the string table & restart from top of loop

		if (lzwe.new_code == FLUSH_CODE)
			{
			lzwe.next_code = 256;
			lzwe.old_code = LzwInputCode(f_SrcGet);
			lzwe.character = lzwe.old_code;
			if (--lzwe.destSkip < 0)
				{
				if (lzwe.outputSize++ >= lzwe.destSize)
					break;
				(*f_DestPut)(lzwe.old_code);
				}
			continue;
			}

//	Check for the special STRING+CHARACTER+STRING+CHARACTER+STRING, which
//	generates an undefined code.  Handle it by decoding the last code,
//	adding a single character to the end of the decode string.

		if (lzwe.new_code >= lzwe.next_code)
			{
			*lzwDecodeStack = lzwe.character;
			lzwe.string = LzwDecodeString(lzwDecodeStack + 1, lzwe.old_code);
			}

//	Otherwise we do a straight decode of the new code.

		else
			{
			lzwe.string = LzwDecodeString(lzwDecodeStack, lzwe.new_code);
			}

//	Output the decode string to the destination, in reverse order.

		lzwe.character = *lzwe.string;
		while (lzwe.string >= lzwDecodeStack)
			{
			if (--lzwe.destSkip < 0)
				{
				if (lzwe.outputSize++ >= lzwe.destSize)
					goto DONE_EXPAND;
				(*f_DestPut)(*lzwe.string);
				}
			--lzwe.string;
			}

//	If possible, add a new code to the string table.

		if (lzwe.next_code <= MAX_CODE)
			{
			lzwPrefixCode[lzwe.next_code] = lzwe.old_code;
			lzwAppendChar[lzwe.next_code] = lzwe.character;
			lzwe.next_code++;
			}
		lzwe.old_code = lzwe.new_code;
		}

//	When break out of expansion loop, shut down source & dest & return size.

DONE_EXPAND:

	(*f_SrcCtrl)(srcLoc, END);
	(*f_DestCtrl)(destLoc, END);

	return(lzwe.outputSize);
}

//	--------------------------------------------------------------
//		STANDARD INPUT SOURCES
//	--------------------------------------------------------------
//
//	LzwBuffSrcCtrl() and LzwBuffSrcGet() implement a memory buffer
//	source for lzw compression and expansion.

static uint8_t *lzwBuffSrcPtr;

void LzwBuffSrcCtrl(intptr_t srcLoc, LzwCtrl ctrl)
{
	if (ctrl == BEGIN)
		lzwBuffSrcPtr = (uint8_t *) srcLoc;
}

uint8_t LzwBuffSrcGet()
{
	return(*lzwBuffSrcPtr++);
}

//	---------------------------------------------------------------
//
//	LzwFdSrcCtrl() and LzwFdSrcGet() implement a file-descriptor
//	source (fd = open()) for lzw compression and expansion.

static int32_t lzwFdSrc;
static int32_t lzwReadBuffIndex;

void LzwFdSrcCtrl(intptr_t srcLoc, LzwCtrl ctrl)
{
	if (ctrl == BEGIN)
		{
		lzwFdSrc = (int) srcLoc;
		lzwReadBuffIndex = LZW_FD_READ_BUFF_SIZE;
		}
}

uint8_t LzwFdSrcGet()
{
	if (lzwReadBuffIndex == LZW_FD_READ_BUFF_SIZE)
		{
		read(lzwFdSrc, lzwFdReadBuff, LZW_FD_READ_BUFF_SIZE);
		lzwReadBuffIndex = 0;
		}
	return(lzwFdReadBuff[lzwReadBuffIndex++]);
}

//	---------------------------------------------------------------
//
//	LzwFpSrcCtrl() and LzwFpSrcGet() implement a file-ptr source
//	(fp = fopen()) for lzw compression and expansion.

static FILE *lzwFpSrc;

void LzwFpSrcCtrl(intptr_t srcLoc, LzwCtrl ctrl)
{
	if (ctrl == BEGIN)
		lzwFpSrc = (FILE *) srcLoc;
}

uint8_t LzwFpSrcGet()
{
	return(fgetc(lzwFpSrc));
}

//	---------------------------------------------------------------
//		STANDARD OUTPUT SOURCES
//	---------------------------------------------------------------
//
//	LzwBuffDestCtrl() and LzwBuffDestPut() implement a memory
//	buffer destination for lzw compression and expansion.

static uint8_t *lzwBuffDestPtr;

void LzwBuffDestCtrl(intptr_t destLoc, LzwCtrl ctrl)
{
	if (ctrl == BEGIN)
		lzwBuffDestPtr = (uint8_t *) destLoc;
}

void LzwBuffDestPut(uint8_t byte)
{
	*lzwBuffDestPtr++ = byte;
}

//	---------------------------------------------------------------
//
//	LzwFdDestCtrl() and LzwFdDestPut() implement a file-descriptor
//	destination (fd = open()) for lzw compression and expansion.

static intptr_t lzwFdDest;
static int32_t lzwWriteBuffIndex;

void LzwFdDestCtrl(intptr_t destLoc, LzwCtrl ctrl)
{
	if (ctrl == BEGIN)
		{
		lzwFdDest = (int) destLoc;
		lzwWriteBuffIndex = 0;
		}
	else if (ctrl == END)
		{
		if (lzwWriteBuffIndex)
			write(lzwFdDest, lzwFdWriteBuff, lzwWriteBuffIndex);
		}
}

void LzwFdDestPut(uint8_t byte)
{
	lzwFdWriteBuff[lzwWriteBuffIndex++] = byte;
	if (lzwWriteBuffIndex == LZW_FD_WRITE_BUFF_SIZE)
		{
		write(lzwFdDest, lzwFdWriteBuff, LZW_FD_WRITE_BUFF_SIZE);
		lzwWriteBuffIndex = 0;
		}
}

//	---------------------------------------------------------------
//
//	LzwFpDestCtrl() and LzwFpDestPut() implement a file-ptr destination
//	(fp = fopen()) for lzw compression and expansion.

static FILE *lzwFpDest;

void LzwFpDestCtrl(intptr_t destLoc, LzwCtrl ctrl)
{
	if (ctrl == BEGIN)
		lzwFpDest = (FILE *) destLoc;
}

void LzwFpDestPut(uint8_t byte)
{
	fputc(byte, lzwFpDest);
}

//	---------------------------------------------------------------
//
//	LzwNullDestCtrl() and LzwNullDestPut() implement a bit-bucket
//	destination for lzw compression and expansion.  Used to size
//	results of compression or expansion.

void LzwNullDestCtrl(intptr_t destLoc, LzwCtrl ctrl)
{
}

void LzwNullDestPut(uint8_t byte)
{
}

//	-----------------------------------------------------------
//		INTERNAL ROUTINES - COMPRESSION
//	-----------------------------------------------------------
//
//	LzwFindMatch() is the hashing routine.  It tries to find a match
//	for the prefix+char string in the string table.  If it finds it,
//	the index is returned.  If the string is not found, the first available
//	index in the string table is returned instead.
//
//		hash_prefix    = prefix to this code
//		hash_character = new character
//
//	Returns: string table index

int32_t LzwFindMatch(int32_t hash_prefix, uint32_t hash_character)
{
	int32_t index;
	int32_t offset;

	index = (hash_character << HASHING_SHIFT) ^ hash_prefix;
	if (index == 0)
		offset = 1;
	else
		offset = LZW_TABLE_SIZE - index;
	while (1)
		{
		if (lzwCodeValue[index] == -1)
			return(index);
		if ((lzwPrefixCode[index] == hash_prefix) &&
			(lzwAppendChar[index] == hash_character))
				return(index);
		index -= offset;
		if (index < 0)
			index += LZW_TABLE_SIZE;
		}
}

//	------------------------------------------------------------
//		INTERNAL ROUTINES - EXPANSION
//	------------------------------------------------------------
//
//	LzwDecodeString() decodes a string from the string table,
//	storing it in a buffer.  The buffer can then be output in
//	reverse order by the expansion program.

uint8_t *LzwDecodeString(uint8_t *buffer, uint32_t code)
{
#ifdef DBG_ON
	int32_t i = 0;
#endif

	while (code > 255)
		{
		*buffer++ = lzwAppendChar[code];
		code = lzwPrefixCode[code];

#ifdef DBG_ON
		if (i++ >= 4094)
			Warning(("LzwDecodeString: Fatal error during code expansion\n"));
#endif
		}

	*buffer = code;
	return(buffer);
}

