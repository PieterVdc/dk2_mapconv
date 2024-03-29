/******************************************************************************/
/** @file dk2mfile.h
 * Library for r/w of DK2 map files.
 * @par Purpose:
 *     Header file. Defines exported routines from dk2mfile.c.
 * @par Comment:
 *     None.
 * @author   qqluqq
 * @date     15 Jul 2008 - 05 Aug 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef SCRIPT_H
#define SCRIPT_H

#include "dk2mfile.h"
#include "../ADiKtEd/libadikted/adikted.h"

short dk2m_read_triggerschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags);
short dk2m_read_variableschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags);
short dk2m_print_script(const struct DK2_Level *lvlDk2,struct LEVEL *lvl);

#endif
