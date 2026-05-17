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

#if !defined(YSFFICH_H)
#define  YSFFICH_H

#include "YSFDefines.h"

// FICH bit offsets within the YSF frame (bits 40-239 = 200 bits)
// Frame structure: [Sync 40 bits][FICH 200 bits][DCH ~720 bits]
const unsigned int YSF_FICH_BIT_OFFSET     = 40U;
const unsigned int YSF_FICH_BYTE_OFFSET    = YSF_FICH_BIT_OFFSET / 8U;  // 5
const unsigned int YSF_FICH_LENGTH_BYTES   = YSF_FICH_LENGTH_BITS / 8U;  // 25

// Minimum frame length for valid FICH extraction (320 bits = 40 bytes)
// Includes: 5 bytes sync + 25 bytes FICH
const unsigned int YSF_MIN_FRAME_LENGTH_BITS = 320U;

// FICH field bit offsets within the FICH data (relative to FICH start)
const unsigned int YSF_FICH_FRAMETYPE_BIT     = 0U;
const unsigned int YSF_FICH_FRAMETYPE_LEN     = 4U;
const unsigned int YSF_FICH_DNMODE_BIT        = 4U;
const unsigned int YSF_FICH_DNMODE_LEN        = 2U;
const unsigned int YSF_FICH_DEVIATION_BIT     = 6U;
const unsigned int YSF_FICH_REPEATER_BIT      = 6U;    // Bit 6 of byte 1 (absolute bit 14)
const unsigned int YSF_FICH_FRAME_counter_BIT = 16U;
const unsigned int YSF_FICH_DEST_ID_BIT       = 32U;
const unsigned int YSF_FICH_SRC_ID_BIT        = 64U;
const unsigned int YSF_FICH_MISC_FLAGS_BIT    = 96U;

// FICH byte positions
const unsigned int YSF_FICH_BYTE0 = 0U;  // Frame Type (4), DN Mode (2), Deviation (1), Unused (1)
const unsigned int YSF_FICH_BYTE1 = 1U;  // Frame Type (4), DN Mode (2), Repeater Access (1), Unused (1)
const unsigned int YSF_FICH_BYTE2 = 2U;  // Frame Counter low byte
const unsigned int YSF_FICH_BYTE3 = 3U;  // Frame Counter high byte
const unsigned int YSF_FICH_BYTE4 = 4U;  // Destination ID bits 0-7
const unsigned int YSF_FICH_BYTE5 = 5U;  // Destination ID bits 8-15
const unsigned int YSF_FICH_BYTE6 = 6U;  // Destination ID bits 16-23
const unsigned int YSF_FICH_BYTE7 = 7U;  // Destination ID bits 24-31
const unsigned int YSF_FICH_BYTE8 = 8U;  // Source ID bits 0-7
const unsigned int YSF_FICH_BYTE9 = 9U;  // Source ID bits 8-15
const unsigned int YSF_FICH_BYTE10 = 10U; // Source ID bits 16-23
const unsigned int YSF_FICH_BYTE11 = 11U; // Source ID bits 24-31
const unsigned int YSF_FICH_BYTE12 = 12U; // Miscellaneous flags

// Return codes
const uint8_t YSF_FICH_OK                = 0U;
const uint8_t YSF_FICH_ERROR_INVALID_FRAME = 1U;
const uint8_t YSF_FICH_ERROR_INVALID_DGID  = 2U;
const uint8_t YSF_FICH_ERROR_INVALID_CHECKSUM = 3U;

// YSF_FICH - Frame Information Channel data structure
// Maps to bits 40-239 in YSF frame (200 bits = 25 bytes)
struct YSF_FICH {
    // Byte 0: Frame Type (4 bits), DN Mode (2 bits), Deviation (1 bit), Unused (1 bit)
    uint8_t frameType : 4;
    uint8_t dnMode    : 2;
    uint8_t deviation : 1;
    uint8_t unused0   : 1;

    // Byte 1: Frame Type (4 bits), DN Mode (2 bits), Repeater Access (1 bit), Unused (1 bit)
    // Note: Bit 6 is the Repeater Access bit - KEY FIELD FOR REPEATER OPERATION
    uint8_t frameType1 : 4;
    uint8_t dnMode1    : 2;
    uint8_t repeaterAccess : 1;  // Bit 6 - When set, indicates repeater operation
    uint8_t unused1    : 1;

    // Bytes 2-3: 16-bit Frame Counter (little-endian)
    uint16_t frameCounter;

    // Bytes 4-7: 32-bit Destination ID (DG-ID) (little-endian)
    uint32_t destId;

    // Bytes 8-11: 32-bit Source ID (little-endian)
    uint32_t srcId;

    // Byte 12: Miscellaneous flags
    uint8_t miscFlags;
};

// CYSF_FICH_Decoder - Decoder for YSF Frame Information Channel
class CYSF_FICH_Decoder {
public:
    // Decode - Extract FICH data from a YSF frame
    // Parameters:
    //   frame - Pointer to 120-byte YSF frame
    //   fick - Reference to YSF_FICH struct to populate
    // Returns: YSF_FICH_OK on success, error code on failure
    static uint8_t decode(const uint8_t* frame, YSF_FICH& fick);

    // Validate BCH checksum of FICH
    // Parameters:
    //   frame - Pointer to 120-byte YSF frame
    // Returns: true if BCH checksum is valid, false otherwise
    static bool validateBCH(const uint8_t* frame);

    // Get individual fields from raw frame (without full decode)
    static uint8_t getRepeaterAccess(const uint8_t* frame);
    static uint16_t getFrameCounter(const uint8_t* frame);
    static uint32_t getDestId(const uint8_t* frame);
    static uint32_t getSrcId(const uint8_t* frame);
    static uint8_t getFrameType(const uint8_t* frame);
    static uint8_t getDNMode(const uint8_t* frame);
};

// CYSF_FICH_Encoder - Encoder for YSF Frame Information Channel
class CYSF_FICH_Encoder {
public:
    // Encode - Write FICH data to a YSF frame
    // Parameters:
    //   frame - Pointer to 120-byte YSF frame buffer
    //   fick - Reference to YSF_FICH struct containing data to encode
    // Returns: YSF_FICH_OK on success, error code on failure
    static uint8_t encode(uint8_t* frame, const YSF_FICH& fick);

    // Set Repeater Access bit (bit 6 of byte 1)
    // Parameters:
    //   frame  - Pointer to 120-byte YSF frame buffer
    //   enable - true to enable repeater access, false to disable
    static void setRepeaterAccess(uint8_t* frame, bool enable);

    // Set Frame Counter (16-bit counter in bytes 2-3)
    // Parameters:
    //   frame  - Pointer to 120-byte YSF frame buffer
    //   counter - 16-bit frame counter value
    static void setFrameCounter(uint8_t* frame, uint16_t counter);

    // Set Destination ID / DG-ID (32-bit in bytes 4-7)
    // Parameters:
    //   frame - Pointer to 120-byte YSF frame buffer
    //   dgId  - 32-bit destination ID
    // Returns: YSF_FICH_OK on success, YSF_FICH_ERROR_INVALID_DGID if value > 0xFF
    static uint8_t setDestId(uint8_t* frame, uint32_t dgId);

    // Set Source ID (32-bit in bytes 8-11)
    // Parameters:
    //   frame - Pointer to 120-byte YSF frame buffer
    //   srcId - 32-bit source ID
    static void setSrcId(uint8_t* frame, uint32_t srcId);

    // Recalculate BCH parity after FICH modification
    // Parameters:
    //   frame - Pointer to 120-byte YSF frame buffer (modified)
    static void recalculateBCH(uint8_t* frame);

    // Helper: Get current FICH byte 1 value with mask for Repeater Access bit
    static uint8_t getRepeaterAccessMask() { return 0x40U; }  // Bit 6 mask
};

#endif