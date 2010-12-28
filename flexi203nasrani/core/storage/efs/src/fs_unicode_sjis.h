/***********************************************************************
 * fs_unicode_sjis.h
 *
 * Shared function for Shift-JIS conversion.
 *
 * Copyright (C) 2007 QUALCOMM, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_unicode_sjis.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-07-10   sh    Added Shift-JIS short filename conversion support.

===========================================================================*/

#ifndef __FS_UNICODE_SJIS_H__
#define __FS_UNICODE_SJIS_H__

/* This Unicode value is substituted for any invalid S-JIS word */
#define FS_SJIS_UNKNOWN 0x0058     // ASCII 'X'

/* This function takes a Shift-JIS DCBS word and returns the UTF-16
 * Unicode equivalent.  The Shift-JIS word is split into a high (lead)
 * byte, and a low byte.  Unrecognized values return the character
 * defined in FS_SJIS_UNKNOWN. */
UTF16 fs_sjis_dcbs_to_utf16 (unsigned char high_byte, unsigned char low_byte);

#endif /* not __FS_UNICODE_SJIS_H__ */
