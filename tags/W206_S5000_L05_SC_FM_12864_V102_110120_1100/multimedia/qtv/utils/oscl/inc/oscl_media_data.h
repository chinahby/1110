#ifndef OSCL_MEDIA_DATA_H
#define OSCL_MEDIA_DATA_H
/* =======================================================================
                               oscl_media_data.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/oscl/main/latest/inc/oscl_media_data.h#7 $
$DateTime: 2008/05/08 14:04:55 $
$Change: 656428 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_types.h"
#include "oscl_media_status.h"
#include <string.h> // needed for memcpy, memset, and memmove

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef void (*BufferFreeFuncPtr) (void *);

typedef uint32 MediaTimestamp;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS 
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/* ======================================================================
CLASS 
  MemAllocator

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
template <class T> class MemAllocator
{
public:
  typedef T* pointer;
  //  virtual pointer allocate(size_t size, void *hint = 0) = 0;
  virtual pointer allocate(void * hint = 0, const int num_reserved_frags = 1) = 0;
  virtual void deallocate(pointer p) = 0;
};

/* ======================================================================
CLASS 
  BufferState

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
struct BufferState
{
  BufferState(BufferFreeFuncPtr the_free_function  = 0, void *bufptr = 0) : 
  refcount(0), ptr(bufptr), free_function(the_free_function)
  {
  };

  void increment_refcnt()
  {
    ++refcount;
  };
  void decrement_refcnt()
  {
    if ( !(--refcount) )
    {
      if ( free_function ) free_function(ptr);
      ptr = 0; 
    }
  };

  void bind(void *in_ptr, BufferFreeFuncPtr in_free_function)
  {
    refcount = 0; ptr = in_ptr, free_function = in_free_function;
  };

  void *get_ptr()
  {
    return ptr;
  };
  int32 get_refcount()
  {
    return refcount;
  };

  BufferFreeFuncPtr get_free_function()
  {
    return free_function;
  };

  void reset()
  {
    refcount=0; ptr=0;
  };


  private:
  int32 refcount;
  void *ptr;
  BufferFreeFuncPtr free_function; // called to free the buffer
};

/* ======================================================================
CLASS 
  BufferFragment

DESCRIPTION
// BufferFragment is a special case of a memory 
// fragment since it refers to fragments of a reference
// counted memory.  The name is used to make this explicit.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class BufferFragment : public MemoryFragment
{
};

/* ======================================================================
CLASS 
  BufFragGroup

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
template <class ChainClass, uint32 max_frags> class BufFragGroup
{
public:


  BufFragGroup(): next(0), num_fragments(0), length(0)
  {
    memset(fragments, 0, sizeof(fragments));
    memset(buffer_states, 0, sizeof(buffer_states));
  };



  int32 GetMaxFrags() const
  {
    return max_frags;
  };
  int32 GetNumFrags() const
  {
    return num_fragments;
  };
  uint32 GetLength() const
  {
    return length;
  };

  BufferFragment * GetFragment(const int32 idx);

  BufferState * GetBufferState(const int32 idx); 

  void AppendNext(ChainClass* next_ptr)
  {
    next = next_ptr;
  };
  ChainClass* GetNext() const
  {
    return next;
  };

  // make fragment array accessible to allow usage within socket 
  // send functions.

protected:


  // BufferFragment * GetFragment(const uint32 idx) const;



  virtual void Clear()
  {
    for ( uint ii = 0 ; ii < num_fragments; ++ii )
    {
      if ( buffer_states[ii] )
      {
        buffer_states[ii]->decrement_refcnt();
      }
    }
    num_fragments = length = 0;
    memset(buffer_states, 0, sizeof(buffer_states));
    memset(fragments, 0, sizeof(fragments));
  };

  BufFragStatus AddFragment(const BufferFragment& frag, BufferState* in_buffer_state, 
                            int32 location_offset = max_frags)
  {
    if ( num_fragments > max_frags )
    {
      return BufFragStatusClass::TOO_MANY_FRAGS;
    }
    if ( frag.ptr == NULL )
    {
      return BufFragStatusClass::EMPTY_FRAGMENT;
    }

    if ( location_offset >= 0 && location_offset < (int32) num_fragments )
    {
      if ( fragments[location_offset].ptr )
      {
        // make room for the new fragment
        memmove(fragments+location_offset+1, fragments+location_offset, 
                (num_fragments-location_offset)*sizeof(BufferFragment));
        memmove(buffer_states+location_offset+1, buffer_states+location_offset, 
                (num_fragments-location_offset)*sizeof(BufferState*));
        ++num_fragments; /* increment the number of fragments if a move 
                          * is necessary.  If the location was empty, then 
                          * has already been reserved and counted.
                          */
      }
      fragments[location_offset] = frag;
      buffer_states[location_offset] = in_buffer_state;
      if ( in_buffer_state ) in_buffer_state->increment_refcnt();
      length += frag.len;
      return BufFragStatusClass::SUCCESS;
    }

    // append the new fragment to the end
    fragments[num_fragments] = frag; 
    buffer_states[num_fragments++] = in_buffer_state;
    length += frag.len;
    if ( in_buffer_state ) in_buffer_state->increment_refcnt();
    return BufFragStatusClass::SUCCESS;
  };

  BufferFragment fragments[max_frags];
  BufferState* buffer_states[max_frags];
  ChainClass* next;
  uint32 num_fragments;
  uint32 length;
};

/* ======================================================================
FUNCTION 
  BufFragGroup<ChainClass, max_frags> :: GetFragment

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
template <class ChainClass, uint32 max_frags> inline BufferFragment *  BufFragGroup<ChainClass, max_frags> :: GetFragment(const int32 idx)
{
  if ( idx < num_fragments && idx >= 0 )
  {
    return & fragments[idx];
  }
  else
  {
    return NULL;
  }
}

/* ======================================================================
CLASS 
  MediaData

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
template <class ChainClass, uint32 max_frags, uint32 local_bufsize> class MediaData : 
public BufFragGroup<ChainClass, max_frags>
{

public:
  MediaData(): timestamp(0), available_localbuf(local_bufsize), num_reserved_fragments(0)
  {
  };

  uint32 GetLocalBufsize() const
  {
    return local_bufsize;
  };
  MediaTimestamp GetTimestamp() const
  {
    return timestamp;
  };
  void SetTimestamp(MediaTimestamp in_timestamp)
  {
    timestamp = in_timestamp;
  };
  uint32 GetAvailableBufferSize() const
  {
    return available_localbuf;
  };



  // The input should be initialized so that the fragment length is the 
  // request size.  The returned value will have a non-NULL pointer if the 
  // request could be fulfilled.
  MediaStatus GetLocalFragment(BufferFragment& fragment)
  {
    if ( fragment.len > available_localbuf )
    {
      fragment.ptr = NULL;
      fragment.len = available_localbuf;
      return MediaStatusClass::NOT_ENOUGH_SPACE;
    }

    fragment.ptr = localbuf + (local_bufsize - available_localbuf);
    available_localbuf -= fragment.len;
    return MediaStatusClass::SUCCESS;
  };

  virtual void Clear()
  {
    for ( uint ii = 0 ; ii < this->num_fragments; ++ii )
    {
      if ( this->buffer_states[ii] )
      {
        this->buffer_states[ii]->decrement_refcnt();
      }
    }
    this->num_fragments = 0;
    this->length = 0;
    memset(this->buffer_states, 0, sizeof(this->buffer_states));
    memset(this->fragments, 0, sizeof(this->fragments));
    this->available_localbuf = local_bufsize;
  };


  bool IsLocalData(const MemoryFragment& frag) const
  {
    if ( frag.ptr == NULL )
      return true;
    if ( (uint8 *)frag.ptr >= localbuf )
    {
      if ( ((uint8 *)frag.ptr)+frag.len < localbuf+local_bufsize )
      {
        return true;
      }
      return false;
    }
    return false;
  }

  int GetMediaSize () const
  {
    int num_reserved = 0;
    for ( int ii=0; ii< num_reserved_fragments; ii++ )
      num_reserved += this->fragments[ii].len;

    return(this->length - num_reserved);
  }

  BufferFragment * GetMediaFragment(const uint32 idx)
  {
    uint32 tmp = idx + this->num_reserved_fragments;
    if ( tmp > this->num_fragments )
    {
      return NULL;
    }
    else
    {
      return & this->fragments[tmp];
    }
  }     

  uint32 GetNumMediaFrags(void) const
  {
    if ( this->num_fragments >= this->num_reserved_fragments )
      return this->num_fragments - this->num_reserved_fragments;
    else return 0;
  }
protected:
  // This function copies the fragment data to the local buffer 
  MediaStatus AddLocalFragment(const BufferFragment& frag, 
                               int32 location_offset)
  {

    if ( this->num_fragments > max_frags )
    {
      return MediaStatusClass::TOO_MANY_FRAGS;
    }

    if ( frag.ptr == NULL || frag.len == 0 )
    {
      return MediaStatusClass::EMPTY_FRAGMENT;
    }

    // see if there is enough room in the local buffer
    if ( frag.len > available_localbuf )
    {
      return MediaStatusClass::NOT_ENOUGH_SPACE;
    }

    // get the fragment
    BufferFragment local_fragment;
    local_fragment.len = frag.len;
    local_fragment.ptr = localbuf + (local_bufsize - available_localbuf);
    available_localbuf -= frag.len;
    this->length += frag.len;

    memcpy(local_fragment.ptr, frag.ptr, frag.len*sizeof(uint8));

    if ( location_offset >= 0 && location_offset < (int32) this->num_fragments )
    {
      if ( this->fragments[location_offset].ptr )
      {
        // make room for the new fragment
        memmove(this->fragments+location_offset+1, this->fragments+location_offset, 
                (this->num_fragments-location_offset)*sizeof(BufferFragment));
        memmove(this->buffer_states+location_offset+1, this->buffer_states+location_offset, 
                (this->num_fragments-location_offset)*sizeof(BufferState*));
        ++this->num_fragments; /* increment the number of fragments if a move 
                                * is necessary.  If the location was empty, then 
                                * has already been reserved and counted.
                                */
      }
      this->fragments[location_offset] = local_fragment;
      this->buffer_states[location_offset] = NULL;
      return MediaStatusClass::SUCCESS;
    }

    // append the new fragment to the end
    this->fragments[this->num_fragments] = local_fragment; 
    this->buffer_states[this->num_fragments++] = NULL;
    return MediaStatusClass::SUCCESS;
  };

  MediaTimestamp timestamp;
  uint8 localbuf[local_bufsize];
  uint32 available_localbuf;

  int num_reserved_fragments;
};

#endif
