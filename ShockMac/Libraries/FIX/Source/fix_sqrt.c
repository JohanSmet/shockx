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
//=================================================================
//
//		System Shock - ©1994-1995 Looking Glass Technologies, Inc.
//
//		FIX_SQRT.c	-	Square root routine for fixed-point numbers.  Adapted from 80386 asm.
//
//=================================================================


//--------------------
//  Includes
//--------------------
#include "fix.h"
#include <math.h>

//-----------------------------------------------------------------
//  Calculate the square root of a fixed-point number.
//-----------------------------------------------------------------
fix fix_sqrt(fix num)
{
	// convert to float, use c-libraries sqrt function, convert back to fix
	float fnum = (float) num / 65536.0f;
	if (fnum > 0.0f) {
		return (fix) (sqrtf(fnum) * 65536.0f);
	} else {
		return 0;
	}
}

//-----------------------------------------------------------------
//  Calculate the square root of a wide (64-bit) number.
//-----------------------------------------------------------------
fix quad_sqrt(int32_t hi, int32_t lo)
{
	double num = (double) hi + ((double) lo / 4294967296.0);
	if (num > 0.0f) {
		return (fix) (sqrt(num) * 65536.0);
	} else {
		return 0;
	}
}

