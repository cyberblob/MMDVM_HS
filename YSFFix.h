/*
 *   Copyright (C) 2015,2016,2017 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016,2017 by Andy Uribe CA6JAU
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#if !defined(YSFFIX_H)
#define  YSFFIX_H

#include "YSFDefines.h"
#include "YSFFICH.h"

// FICH byte 1 offset in YSF frame (5 bytes sync + 31 bits FICH = byte offset 36)
const uint8_t YSF_FICH_BYTE1_OFFSET = 36U;

/*
 * setRepeaterAccessBit - Simple function to enable WIRESX repeater operation
 * 
 * This is the bare minimum needed to fix WIRESX one-way communication:
 * 1. Sets bit 6 of FICH byte 1 to 1 (Repeater Access = enabled)
 * 2. Recalculates BCH checksum to ensure frame validity
 * 
 * Parameters:
 *   frame - Pointer to 120-byte YSF frame (modified in place)
 * 
 * Memory usage:
 *   RAM: ~0 bytes (modifies frame in place)
 *   Flash: ~50 bytes for the wrapper function
 * 
 * Configuration:
 *   YSF_REPEATER_SET_FICH_BIT - Set to 1 to enable FICH modification (default),
 *                               set to 0 to disable (pass-through, no change)
 */
static inline void setRepeaterAccessBit(uint8_t* frame)
{
#if defined(YSF_REPEATER_MODE) && (YSF_REPEATER_SET_FICH_BIT == 1)
    // Set bit 6 of FICH byte 1 (Repeater Access = 1)
    // This tells WIRESX that this is a repeater-access transmission
    frame[YSF_FICH_BYTE1_OFFSET] = 0x40U;
    
    // Recalculate BCH checksum after FICH modification
    CYSF_FICH_Encoder::recalculateBCH(frame);
#endif
}

#endif