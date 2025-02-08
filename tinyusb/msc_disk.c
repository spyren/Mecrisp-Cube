/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "bsp/board_api.h"
#include "tusb.h"
#include "fd.h"
#if SD_DRIVE == 1
#include "sd.h"
#endif

#if CFG_TUD_MSC

// Invoked to determine max LUN
uint8_t tud_msc_get_maxlun_cb(void) {
#if SD_DRIVE == 1
  return 2; // dual LUN
#else
  return 1; // single LUN
#endif
}


// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) {
  if (lun == 0) {
	  const char vid0[] = "spyr.ch";
	  const char pid0[] = "CUBE4TH Flash";
	  const char rev0[] = "1.0";
	  memcpy(vendor_id  , vid0, strlen(vid0));
	  memcpy(product_id , pid0, strlen(pid0));
	  memcpy(product_rev, rev0, strlen(rev0));
#if SD_DRIVE == 1
  } else {
	  const char vid1[] = "spyr.ch";
	  const char pid1[] = "CUBE4TH SD";
	  const char rev1[] = "1.0";
	  memcpy(vendor_id  , vid1, strlen(vid1));
	  memcpy(product_id , pid1, strlen(pid1));
	  memcpy(product_rev, rev1, strlen(rev1));
#endif
  }

}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun) {
  (void) lun;

  return true; // Flash disk is always ready
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size) {
	if (lun == 0) {
		FD_getSize();
		*block_count = FD_getBlocks()*2;
		*block_size  = 512;
#if SD_DRIVE == 1
	} else {
		SD_getSize();
		*block_count = SD_getBlocks()*2;
		*block_size  = 512;
#endif
	}

}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) {
  (void) lun;
  (void) power_condition;

  if (load_eject) {
    if (start)  {
      // load disk storage
    } else {
      // unload disk storage
    }
  }

  return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
	if (lun == 0) {
		if ( lba >= (FD_getBlocks()*2) ) {
			// out of flash disk
			return -1;
		}
		FD_ReadBlocks(buffer, lba, 1);
#if SD_DRIVE == 1
	} else {
		if ( lba >= (SD_getBlocks()*2) ) {
			// out of flash disk
			return -1;
		}
		SD_ReadBlocks(buffer, lba, 1);
#endif
	}
	return (int32_t) bufsize;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
	if (lun == 0) {
		if ( lba >= (FD_getBlocks()*2) ) {
			// out of flash disk
			return -1;
		}
		FD_WriteBlocks(buffer, lba, 1);
#if SD_DRIVE == 1
	} else {
		if ( lba >= (SD_getBlocks()*2) ) {
			// out of flash disk
			return -1;
		}
		SD_WriteBlocks(buffer, lba, 1);
#endif
	}
	return (int32_t) bufsize;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize) {
  // read10 & write10 has their own callback and MUST not be handled here

  void const* response = NULL;
  int32_t resplen = 0;

  // most scsi handled is input
  bool in_xfer = true;

  switch (scsi_cmd[0]) {
    default:
      // Set Sense = Invalid Command Operation
      tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

      // negative means error -> tinyusb could stall and/or response with failed status
      resplen = -1;
    break;
  }

  // return resplen must not larger than bufsize
  if ( resplen > bufsize ) resplen = bufsize;

  if ( response && (resplen > 0) )
  {
    if(in_xfer)
    {
      memcpy(buffer, response, (size_t) resplen);
    }else
    {
      // SCSI output
    }
  }

  return (int32_t) resplen;
}

#endif
