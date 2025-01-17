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
/*
 * $Source: n:/project/lib/src/h/RCS/types.h $
 * $Revision: 1.2 $
 * $Author: kaboom $
 * $Date: 1993/09/28 01:12:47 $
 *
 * extra typedefs and macros for use by all code.
 *
 * $Log: types.h $
 * Revision 1.2  1993/09/28  01:12:47  kaboom
 * Converted #include "xxx" to #include <xxx> for watcom.
 *
 * Revision 1.1  1993/03/19  18:19:27  matt
 * Initial revision
 */

#ifndef __TYPES_H
#define __TYPES_H

#include <stdbool.h>
#include <stdint.h>

/* these are convenience typedefs so we don't always have to keep typing
   `unsigned.' */
/*
JS: don't define these (stdlib has conflicting typedefs) -- replace usage with uint8_t, uint16_t etc.
typedef unsigned char uchar;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint32_t ulong;
typedef uint8_t  ubyte;
*/

#ifndef NULL
#define NULL 0
#endif /* !NULL */

#ifndef TRUE
#define TRUE 1
#endif /* !TRUE */

#ifndef FALSE
#define FALSE 0
#endif /* !FALSE */

#endif /* !__TYPES_H */
