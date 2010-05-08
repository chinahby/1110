#ifndef AUDMPEG_H
#define AUDMPEG_H
/*===========================================================================

           A U D I O   F O R M A T   M P E G   H E A D E R   F I L E

DESCRIPTION
  This header file contains external interface definitions common to
  both MP3 and AAC files.

Copyright (c) 2002-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audmpeg.h#2 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/08    kk     CR159736-Added support to play Multiple Id3tag Mp3 files.
08/30/07    hs     Fixed crash issue when we have APIC frame and added
                   checking invalid frame len
08/06/07    kk     Merged the Fix CR 124244 - Full ID3V2.3 support, 
                   support for URL link (W)frames .
05/23/07    kk     Fixed  CR 113932 - CMX get spec problem due to APIC 
                   frame in MP3 ID3V2.3 tag.
11/29/06    kk     Added another parameter for audmpeg_id3v2_check_padding().
08/15/06    sg     Modified to use uint32 for find_sync API.
07/28/06    hz     Added support for ID3V2 and AAC ADTS/VBR song length.
07/19/05    pl     Added error_status to audmpeg_parse_ctl_type
05/17/05    ac     Revised audmpeg_parse_ctl_type definition to include padding
                   information 
04/23/05    ac     Revised the audmpeg_find_sync_func_type() to include a 
                   boolean argument for the first buffer
08/27/04    sg     Restored previous ID3 comment length.
08/11/04    sg     Defined macros for ID3 tag items lengths. Fixed ID3 comment
                   length.
06/17/02    sg     Initial version.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */

#ifdef FEATURE_MPEG_AUDIO
#include "audmain.h"    /* Audio Format Data definitions and protos        */
#include "audmaini.h"
#include "snd.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
                         MPEG AUDIO DECLARATIONS
                    (Data structures common to both MP3 and AAC)
===========================================================================*/

/* Resync states */ 
typedef enum {
  AUDMPEG_RESYNC_START,
  AUDMPEG_RESYNC_DUMP,
  AUDMPEG_RESYNC_LOAD,
  AUDMPEG_RESYNC_DONE
} audmpeg_resync_type;

typedef int (*audmpeg_find_sync_func_type)(byte **buffer_ptr_ptr, 
                                           uint32 buf_length, 
                                           boolean first_buffer);

/* enum type to keep track of current get operations - get time or get spec
 * rewind or fast forward. 
 */
typedef enum {
  AUDMPEG_PROC_OPS_NONE = 0,
  
  AUDMPEG_PROC_OPS_PROCESSING_GETSPEC  = 0x0001 << 1,
  AUDMPEG_PROC_OPS_PROCESSING_GETTIME  = 0x0001 << 2,
  AUDMPEG_PROC_OPS_PROCESSING_PLAYBACK = 0x0001 << 3,
  AUDMPEG_PROC_OPS_PROCESSING_REWIND   = 0x0001 << 4,
  AUDMPEG_PROC_OPS_PROCESSING_FFWD     = 0x0001 << 5,
  AUDMPEG_PROC_OPS_PROCESSING_GETTAG   = 0x0001 << 6,
  AUDMPEG_PROC_OPS_PROCESSING_GETVBR   = 0x0001 << 7,
  
  AUDMPEG_GET_OPS_MAX
} audmpeg_proc_ops_type;

/* VBR pre-allocated memory to store played frames' offset to first sync */
#define AUDMPEG_VBR_PRE_ALLOC_MEM   500

/* Structure used to parse the VBR file
 */
typedef struct {
  /* ===================================================================
   *   This section is used to search the buffer to calculate the whole
   *   number of frames in the clip 
   * ===================================================================*/
  uint32  total_read_len;        /* used to searh buffer                   */
  uint32  just_read_len;         /* used to searh buffer                   */
  uint32  frame_num;             /* used as index to frame_info            */
  uint32  vbr_recorded;          /* used to search buffer                  */
  uint32  leftover_size;         /* bytes of latest frame leftover         */
  uint8   *leftover;             /* used to search buffer                  */
  boolean failure;               /* indicate whether there is any failure  */
  uint32  prealloc_index;        /* index of buffer allocated for the pos  */
  uint32  *frame_pos;            /* frame offset to first_sync position    */

  /* ===================================================================
   *   This item is used for rewind 
   * ===================================================================*/
  uint32  rrwnd_num;              /* VBR rewind  frame number              */

  /* ===================================================================
   *   This section is for VBR fast forward
   * ===================================================================*/
  uint32  ffwd_num;               /* VBR forward frame number              */
  uint32  adjusted_ms;            /* In case we need to do further parse,  */ 
  uint32  elapsed_ms;             /* this time will not be updated right   */
  uint32  ms_forward;             /* away, let's store them here           */
  void    *state_type;            /* store the playback hander information */
} audmpeg_vbr_parse_type; 

/* This structure contains all the information on a particular MP3 or AAC file
** that is being parsed.
*/
typedef struct {
  snd_af_access_type  *audmpeg_handle;     /* Info to callback client         */
  uint8               *track_buf_1;       /* First data buffer                */
  uint8               *track_buf_2;       /* Second data buffer               */
  uint32              track_1_start;      /* Position in file of track 1      */
  uint32              track_2_start;      /* Position in file of track 2      */
  uint32              track_1_length;     /* Track 1 data length              */
  uint32              track_2_length;     /* Track 2 data length              */
  uint32              track_length;       /* Length, in bytes, of this track  */
  boolean             track_1_loading;    /* Track 1 is being loaded          */
  boolean             track_2_loading;    /* Track 2 is being loaded          */
  uint32              file_location;      /* Location in file being processed */
  uint32              file_len;           /* Length of file being processed   */

  uint32              first_sync;   /* Position of the first sync in the file */
  boolean             read_failed;        /* Read failure flag                */
  audmpeg_resync_type resync_state;

  boolean             file_pad_req;
  /* File pad length. Use only when file_pad_req is true */
  uint16              file_pad_length;

  /* pointer to heap buffer, require for padding         */ 
  uint8               *track_buf_dynamic;

  /* Each MPEG frame contains sync data and unchanging header data which can be
  ** searched for to more accurately locate the next sync.
  */
  boolean             frame_data_found;   /* TRUE = found an MPEG frame       */
  byte                mask_size;          /* Frame header mask size in bytes  */
  byte                frame_mask[4];      /* Frame header mask                */
  byte                frame_data[4];      /* Fixed header bits for the file   */
  audmain_status_type error_status;       /* Indicate the parse error         */
  audmpeg_find_sync_func_type find_sync_func;
  audmpeg_proc_ops_type  ops;             /* Current operations to clip       */

  audmpeg_vbr_parse_type vbr_parse;       /* VBR clip parse information       */
  uint32              id3v2_tag_start;    /* start of the id3 valid tag       */
} audmpeg_parse_ctl_type;

extern const uint32 audmpeg_samplerate_values_table[];


/*===========================================================================

                         ID3 STRUCTURE DECLARATIONS

===========================================================================*/
/* The ID3V1 structure can be found at the last 128 bytes of the MP3/AAC file,
** if included.  Character elements are not NULL terminated.
*/
#define AUDMPEG_ID3_TITLE_LENGTH   30
#define AUDMPEG_ID3_ARTIST_LENGTH  30
#define AUDMPEG_ID3_ALBUM_LENGTH   30
#define AUDMPEG_ID3_YEAR_LENGTH    4
#define AUDMPEG_ID3_COMMENT_LENGTH 29 /* See comment in audmpeg_id3v1_tag_type*/

#define AUDMPEG_ID3V2_TAG_ID_LEN         3 /* size of ID3V2 Tag ID            */
#define AUDMPEG_ID3V2_HEADER_FOOTER_LEN 10 /* size of ID3V2 Header/Footer     */
                                                                              
#define AUDMPEG_ID3V2_FRAME_ID_LEN    4  /* ID3V2 frame id   len - 4 Bytes    */
#define AUDMPEG_ID3V2_FRAME_SIZE_LEN  4  /* ID3V2 frame id   len - 4 Bytes    */
#define AUDMPEG_ID3V2_FRAME_FLAG_LEN  2  /* ID3V2 frame flag len - 2 Bytes    */
#define AUDMPEG_ID3V2_FRAME_HEADER_LEN 10 /* ID3V2 frame header len - 10
                                             (AUDMPEG_ID3V2_FRAME_ID_LEN   +
                                              AUDMPEG_ID3V2_FRAME_SIZE_LEN +
                                              AUDMPEG_ID3V2_FRAME_FLAG_LEN ) */

/* Macros do find out id3v2 version and reversion
 */
#define AUDMPEG_ID3V2_TAG_VERSION_MAJOR(x) (((x) >> 8) & 0xff) 
#define AUDMPEG_ID3_TAG_VERSION_MINOR(x)   (((x) >> 0) & 0xff)

/* ID3TAG v1 content
 */
typedef PACKED struct {
  char tag_id[3];   /* ASCII "TAG" marks the ID3v1 block */
  char title[AUDMPEG_ID3_TITLE_LENGTH];     /* Title of song */
  char artist[AUDMPEG_ID3_ARTIST_LENGTH];   /* Artist performing the song */
  char album[AUDMPEG_ID3_ALBUM_LENGTH];     /* Album the song can be found on */
  char year[AUDMPEG_ID3_YEAR_LENGTH];       /* Year of release */
  char comment[AUDMPEG_ID3_COMMENT_LENGTH]; /* Comment field */
  byte track;       /* ID3 1.1 extension, otherwise, it's the last char of the
                       ID3 1.0 comment field */
  byte genre;       /* Genre of music (defined in ID3 specifications) */
} audmpeg_id3v1_tag_type;

/* ID3 version type. It should be implemented backward compatible.
 */
typedef enum {
  AUDMPEG_TAGTYPE_NONE = 0,
  AUDMPEG_TAGTYPE_ID3V1,               /* ID3V1 only existing                */
  AUDMPEG_TAGTYPE_ID3V2,               /* ID3V1 only existing                */

  AUDMPEG_TAGTYPE_ID3V2_FOOTER,

  AUDMPEG_TAGTYPE_MAX
} audmpeg_id3_tag_version_type;

typedef enum {
  AUDMPEG_ID3V2_TAG_FLAG_UNSYNCHRONISATION     = 0x80,
  AUDMPEG_ID3V2_TAG_FLAG_EXTENDEDHEADER        = 0x40,
  AUDMPEG_ID3V2_TAG_FLAG_EXPERIMENTALINDICATOR = 0x20,
  AUDMPEG_ID3V2_TAG_FLAG_FOOTERPRESENT         = 0x10,

  AUDMPEG_ID3V2_TAG_FLAG_KNOWNFLAGS            = 0xf0
} audmpeg_id3v2_frame_flag_type ;

/* enum define of id3v2 frames ids which are supported  
 */
typedef enum {
  AUDMPEG_ID3V2_FRAME_TITLE = 0, /* Title      "TIT2"    */
  AUDMPEG_ID3V2_FRAME_SUBTITLE,  /* Subtitle   "TIT3"    */ 
  AUDMPEG_ID3V2_FRAME_COPYRIGHT, /* Copyright  "TCOP"    */
  AUDMPEG_ID3V2_FRAME_PRODUCED,  /* Produced   "TPRO"    */
  AUDMPEG_ID3V2_FRAME_COMPOSER,  /* Composer   "TCOM"    */ 
  AUDMPEG_ID3V2_FRAME_ARTIST,    /* Artist     "TPE1"    */
  AUDMPEG_ID3V2_FRAME_ORCHESTRA, /* Orchestra  "TPE2"    */ 
  AUDMPEG_ID3V2_FRAME_Conductor, /* Conductor  "TPE3"    */ 
  AUDMPEG_ID3V2_FRAME_LYRICIST,  /* Lyricist   "TEXT"    */ 
  AUDMPEG_ID3V2_FRAME_ALBUM,     /* Album      "TALB"    */ 
  AUDMPEG_ID3V2_FRAME_TRACK,     /* Track      "TRCK"    */ 
  AUDMPEG_ID3V2_FRAME_YEAR,      /* Year       "TDRC"    */ 
  AUDMPEG_ID3V2_FRAME_PUBLISHER, /* Publisher  "TPUB"    */ 
  AUDMPEG_ID3V2_FRAME_GENRE,     /* Genre      "TCON"    */ 
  AUDMPEG_ID3V2_FRAME_STATION,   /* Station    "TRSN"    */ 
  AUDMPEG_ID3V2_FRAME_ENCODER,   /* Encoder    "TENC"    */  
  AUDMPEG_ID3V2_FRAME_LENGTH,    /* Length     "TLEN"    */

  AUDMPEG_ID3V2_FRAME_MAX
} audmpeg_id3v2_frame_supported_type;

/* pre-allocated memory to store Generic frames */
#define AUDMPEG_GFRAME_PRE_ALLOC_MEM   9

/* ID3V2 tag inforamtion */
typedef enum {
  AUDMPEG_ID3V2_TAG_INFO_BY_POSITION,
  AUDMPEG_ID3V2_TAG_INFO_BY_CONTENT
} audmpeg_id3v2_tag_info_type;

/* ID3V2 Tag Generic frame content for other frames */

typedef struct {
  audmpeg_id3v2_tag_info_type info_type;
  char    id[5];             /* id of this frame              */
  int16   flags;             /* flags of this frame            */
  union {
    uint8   *frame_content;    /* content of this frame         */
    uint32  file_position;     /* Frame content position in file */
  } frame_data;
  uint32  frame_size;        /* content size                  */
} audmpeg_id3v2_generic_frame_type; 

/* supported frame content with id
 */
typedef struct {
  char    id[5];          /* id of this frame              */
  boolean found;             /* whether this frame existing   */
  int16   flag;              /* flag of this frame            */
  uint8   *frame_content;    /* content of this frame         */
  uint32  frame_size;        /* content size                  */
} audmpeg_id3v2_frame_type; 

/* ID3V2 Tag frame info for other frames.
 *
 */
typedef struct {
  char    id[5];             /* id of this frame              */
  boolean found;             /* whether this frame existing   */
  uint16   flags;            /* flags of this frame           */
  uint32  file_position;     /* Frame content position in file */
  uint32 size;               /* Size of the content */
} audmpeg_id3v2_frame_info_type;

/* SEEK frame type
 */
typedef struct {
  char       id[5];
  boolean    found;
  uint32     offset;
  boolean    parse;  /* Whether "SEEK" frame parsing started */
} audmpeg_id3v2_seek_frame_type; 

/* Place to hold the tag to parse
 */
typedef struct {
  uint8  *buf;
  uint32 buf_len;
  audmpeg_id3v2_seek_frame_type seek_frame;
} audmpeg_id3v2_tag_holder_type;

/* Enum type to describe how does the tag located in the file
 */
typedef enum {
  ID3V2_TAG_LOCATE_NONE = 0,
  ID3V2_TAG_LOCATE_PREPEND_ONLY,
  ID3V2_TAG_LOCATE_PREPEND_POSTPEND,
  ID3V2_TAG_LOCATE_POSTPEND_ONLY,

  ID3V2_TAG_LOCATE_MAX
} audmpeg_id3v2_tag_locate_type; 

/* The ID3V2 structure can be found at the beginning of the MP3/AAC file,
** if included.
*/
typedef struct {
  uint16 version;       /* Tag version: version + revision                   */
  int8   hdr_flag;      /* Header flag                                       */
  uint32 tag_size;      /* First Tag size: EXCLUDING header 10 and 
                           footer 10 bytes and and consequent tag(s) pointed 
                           by "SEEK" frame                                   */
  uint8  *ext_hdr;      /* extended tag header, if exists                    */
  uint32 ext_hdr_size;  /* extended tag header size                          */

  uint32 total_tag_size;/* Total size of all tags found                      */
  uint32 postpend_size; /* Tags size at the end of file                      */
                                                                           
  audmpeg_id3v2_frame_type      frames[AUDMPEG_ID3V2_FRAME_MAX];
  audmpeg_id3v2_tag_locate_type tag_locate;

  uint32 next_file_pos;  /* next file position need to read to parse the tag  */

  uint32  already_parsed_tag_size; /* Eventually should = v2_tag.tag_size +  
                                                          ext hdr size        */
  boolean first_seek_buf;
  audmpeg_id3v2_tag_holder_type holder;  /* place to hold current parsed info */
  audmpeg_id3v2_frame_info_type apic;    /* holds information of APIC frame   */
  audmpeg_id3v2_generic_frame_type *gframes; /* Generic frames */
  uint32   gframes_cnt; /* Generic frame count */   
  uint16  gframes_prealloc_index; /* index of buffer allocated for wframes */
} audmpeg_id3v2_tag_type;

#define AUDMPEG_ID3V1_TAG_LEN sizeof(audmpeg_id3v1_tag_type)

/* Each ID3V2 tag contains any number of frame headers with the format
** described below.
*/
typedef struct {
  char frame_id[4]; /* "X" "Y" and "Z" are experimental and can be ignored.   */
  byte size[4];     /* excludes frame header                                  */
  byte flags[2];    /* [0]7:tag preservation 6:file preservation 5:read only
                    **    4-0:cleared
                    ** [1]7:zlib compression 6:encrypted 5:group identity
                    **    4-0:cleared
                    */
} audmpeg_id3v2_frame_header_type;

/* Generic ID3 tag container */
typedef struct {
  /*
  union {
    audmpeg_id3v1_tag_type  id3v1_tag;
    audmpeg_id3v2_tag_type  id3v2_tag;
  };
  audmpeg_id3_version_type  version;
  */
  audmpeg_id3v1_tag_type  v1_tag;
  boolean         v1_tag_found;

  boolean         v2_tag_found;
  uint32          v2_tag_length;       /* Length, in bytes, of the ID3V2 tag  */

  audmpeg_id3_tag_version_type tag_type;      /* Field indicates tag version */
  audmpeg_id3v2_tag_type v2_tag;              /* Place holds tag content     */
} audmpeg_id3_tag_type;


#define AUDMPEG_MAX_FILE_LEN (uint32)(-1)

#define IS_ID3V1_TAG(x) \
  (((byte *)(x))[0] == 0x54 && ((byte *)(x))[1] == 0x41 && \
                                                      ((byte *)(x))[2] == 0x47)
#define IS_ID3V2_TAG(x) \
  (((byte *)(x))[0] == 0x49 && ((byte *)(x))[1] == 0x44 && \
                                                      ((byte *)(x))[2] == 0x33)

#define IS_ID3V2_WFRAME(x) (((byte *)(x))[0] == 'W' )
#define IS_ID3V2_TFRAME(x) (((byte *)(x))[0] == 'T' )


/* <EJECT> */
/*---------------------------------------------------------------------------
** AudMPEG Public Function Prototypes
**---------------------------------------------------------------------------
*/

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_get_data_cb_req

DESCRIPTION
  Callback function which handles the response to a Data request.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
extern boolean audmpeg_get_data_cb_req(
  audmpeg_parse_ctl_type *parse_ctl,
  uint8                  *buf, 
  uint32                 length
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_adec_adjust

DESCRIPTION
  This function adjusts adec buffers to reflect their newly used data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audmpeg_adec_adjust (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *bytes_used
); 

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_parse

DESCRIPTION
  This function parses one event of the MPEG file.  Prepares buffers for data
  transfer.

DEPENDENCIES
  Must call audmpeg_age() before calling this function!

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audmpeg_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_age

DESCRIPTION
  This function checks to see if the buffers are ready for use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audmpeg_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_reset

DESCRIPTION
  This function resets internal buffers after a song is unloaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void audmpeg_reset (
  audmpeg_parse_ctl_type  *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_load_id3v1_tag

DESCRIPTION
  This function loads the id3v1 tag from the MPEG Audio file into a
  temporary buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void audmpeg_load_id3v1_tag(
  snd_af_access_type     *handle,
  audmpeg_parse_ctl_type *parse_ctl,
  audmpeg_id3_tag_type   *id3_tag
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_find_first_frame

DESCRIPTION
  Processes any ID3v2 tags and finds the first frame

DEPENDENCIES
  None

RETURN VALUE
  TRUE if a frame was found

SIDE EFFECTS

===========================================================================*/
extern boolean audmpeg_find_first_frame(
  uint8                       *buffer,
  snd_af_access_type          *handle,
  audmain_parse_ctl_type      *main_parse_ctl,
  audmpeg_parse_ctl_type      *mpeg_parse_ctl,
  audmpeg_id3_tag_type        *id3_tag
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_setup_track2

DESCRIPTION
  Sets up the track2 buffer.

DEPENDENCIES
  Should be called after track1 has been setup using audmpeg_find_first_frame()

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audmpeg_setup_track2(
  snd_af_access_type          *handle,
  audmain_parse_ctl_type      *main_parse_ctl,
  audmpeg_parse_ctl_type      *mpeg_parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_setup_streaming

DESCRIPTION
  This function sets up the buffers to handle streaming audio.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern boolean audmpeg_setup_streaming (
   uint8                       *buffer,
   snd_af_access_type          *handle,
   audmain_parse_ctl_type      *main_parse_ctl,
   audmpeg_parse_ctl_type      *mpeg_parse_ctl
);

/*===========================================================================

FUNCTION audmpeg_process_idtag

DESCRIPTION
  This function processes the ID3 tag.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - Valid supported ID3 tag is found and processed; 
  FALSE - Valid supported ID3 tag is not found.

SIDE EFFECTS
  If valid ID3 tag has been found, corresponding id3_tag data structure is
  updated.
===========================================================================*/
extern boolean audmpeg_process_idtag (
  snd_af_access_type     *handle,
  audmpeg_parse_ctl_type *parse_ctl,
  audmpeg_id3_tag_type   *id3_tag,
  uint32                 *tag_size
);

/*===========================================================================

FUNCTION audmpeg_find_tag_type

DESCRIPTION
  Findes the tag type/version - v1 or v2, based on the key word.

DEPENDENCIES
  None

RETURN VALUE
  audmpeg_id3_tag_version_type

SIDE EFFECTS
 
===========================================================================*/
extern audmpeg_id3_tag_version_type audmpeg_find_tag_type (
  const uint8 *buf
);

/*===========================================================================

FUNCTION audmpeg_id3v2_process_tag_frame

DESCRIPTION
  Parse the whole content into id3_tag structure based on the required fields.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -- Parsing is successful
  FALSE -- Parsing is failed

SIDE EFFECTS
 
===========================================================================*/
extern boolean audmpeg_id3v2_process_tag_frame (
  audmpeg_parse_ctl_type        *parse_ctl,
  audmpeg_id3v2_tag_type        *v2_tag
);

/*===========================================================================

FUNCTION audmpeg_id3v2_process_tag

DESCRIPTION
  Parse the whole content into id3_tag structure based on the required fields.

DEPENDENCIES
  None

RETURN VALUE
  ID3V2 tag size (including tag header and tag footer)                                
SIDE EFFECTS
 
===========================================================================*/
extern boolean audmpeg_id3v2_process_tag (
  snd_af_access_type            *handle,
  audmpeg_parse_ctl_type        *parse_ctl,
  audmpeg_id3v2_tag_holder_type *holder,
  audmpeg_id3v2_tag_type        *v2_tag,
  audmpeg_id3v2_seek_frame_type *seek_frame 
);

/*===========================================================================

FUNCTION audmpeg_id3v2_parse_tag_header

DESCRIPTION
  Parse the ID3V2 header to get tag version, flags and tag size.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -- Parsing is successful
  FALSE -- Parsing is failed

SIDE EFFECTS
 
===========================================================================*/
extern boolean audmpeg_id3v2_parse_tag_header (
  uint8  *buf,
  uint16 *version,
  int8   *flag,
  uint32 *size
);

/*===========================================================================

FUNCTION audmpeg_id3v2_parse_uint

DESCRIPTION
  Parse the byte(s) to get the corresponding integer value.

DEPENDENCIES
  None

RETURN VALUE
  integer value of the byte(s).

SIDE EFFECTS
  ptr is updated
 
===========================================================================*/
extern uint32 audmpeg_id3v2_parse_uint (
  uint8  **ptr,
  uint8  bytes
);

/*===========================================================================

FUNCTION audmpeg_id3v2_parse_syncsafe

DESCRIPTION
  Parse the syncsafe byte2 to get the corresponding integer value.

DEPENDENCIES
  None

RETURN VALUE
  integer value of the byte(s).

SIDE EFFECTS
  ptr is updated
 
===========================================================================*/
extern uint32 audmpeg_id3v2_parse_syncsafe (
  uint8  **ptr,
  uint8  bytes
);

/*===========================================================================

FUNCTION audmpeg_id3v2_parse_frame

DESCRIPTION
  Parse the syncsafe byte2 to get the corresponding integer value.

DEPENDENCIES
  None

RETURN VALUE
  integer value of the byte(s) been parsed

SIDE EFFECTS
  ptr is updated
 
===========================================================================*/
uint32 audmpeg_id3v2_parse_frame (
  audmpeg_id3v2_tag_type *v2_tag
);

/*===========================================================================

FUNCTION audmpeg_id3v2_findframe

DESCRIPTION
  Find the frame ID.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - 

SIDE EFFECTS
  ptr is updated
 
===========================================================================*/
boolean audmpeg_id3v2_findframe (
  char   *data,
  uint8  *buf,
  uint32 len
);

/*===========================================================================

FUNCTION audmpeg_id3tag_process

DESCRIPTION
  This function processes the ID3 tag.

DEPENDENCIES
  None

RETURN VALUE
  TURE  - Success
  FALSE - Failed

SIDE EFFECTS
  If valid ID3 tag has been found, corresponding id3_tag data structure is
  updated.
===========================================================================*/
extern void audmpeg_id3tag_process (
  snd_af_access_type     *handle,
  audmpeg_parse_ctl_type *parse_ctl,
  audmpeg_id3_tag_type   *id3_tag
);

/*===========================================================================

FUNCTION audmpeg_id3v2_parse_tag

DESCRIPTION
  Process the ID3V2 tag.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - Success
  FALSE - Failed

SIDE EFFECTS
  None 

===========================================================================*/
extern void audmpeg_id3v2_parse_tag ( 
  audmpeg_parse_ctl_type *parse_ctl,
  uint8                  *tag_buf,
  uint32                 tag_buf_len,
  audmpeg_id3v2_tag_type *v2_tag
);

/*===========================================================================

FUNCTION audmpeg_id3v2_reset_tag

DESCRIPTION
  Reset whole tags structure or content fields only.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
void audmpeg_id3v2_reset_tag ( 
  audmpeg_id3v2_tag_type *v2_tag,
  boolean                reset_all
);

/*===========================================================================

FUNCTION audmpeg_id3v2_check_buf_2

DESCRIPTION
  If buffer 2 is not free, free it up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
void audmpeg_id3v2_check_buf_2 (
  audmpeg_parse_ctl_type *parse_ctl
);

/*===========================================================================

FUNCTION audmpeg_id3v2_check_ext_hdr

DESCRIPTION
  Check whether the extended header exists

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
boolean audmpeg_id3v2_check_ext_hdr (
  uint8                  *tag_buf,
  audmpeg_id3v2_tag_type *v2_tag
);

/*===========================================================================

FUNCTION audmpeg_id3v2_check_padding

DESCRIPTION
  Check whether this is padding data

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If it is padding
  FALSE - If it is not padding

SIDE EFFECTS
  None 

===========================================================================*/
boolean audmpeg_id3v2_check_padding (
  uint8                     *buf,
  audmpeg_id3v2_tag_type *v2_tag
);

/*===========================================================================

FUNCTION audmpeg_id3v2_parse_async

DESCRIPTION
  Process the leftover ID3V2 tag through ASYNC read.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
boolean audmpeg_id3v2_parse_async (
  audmpeg_parse_ctl_type *parse_ctl,
  uint8                  *buf, 
  uint32                 length,
  audmpeg_id3v2_tag_type *v2_tag
);
#endif /* FEATURE_MPEG_AUDIO */
#endif /* AUDMPEG_H */
