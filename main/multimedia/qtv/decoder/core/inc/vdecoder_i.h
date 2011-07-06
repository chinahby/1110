#ifndef _VDECODER_I_H_
#define _VDECODER_I_H_
/* =======================================================================
                               vdecoder_i.h
DESCRIPTION
  Internal header for VDEC API.  Here we define the base class for all
  media-specific decoders.  The API function are C-language shims
  which call member functions of this base class.

Copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/decoder/core/rel/2.0/inc/vdecoder_i.h#6 $
$DateTime: 2009/10/14 01:39:25 $
$Change: 1052854 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/08/09    vs      Remove the dependencies of other modules.
--------   ---      ---------------------------------------------------------
========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "vdecoder_types.h"
#include "vdecoder_types_i.h"
#include "list.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include "qdspext.h"
#ifdef __cplusplus
}
#endif

#include "vdecoder_utils.h"
#include "vdecoder_log.h"
/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

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
FUNCTION
  SAMPLE_FUNC

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

/* =======================================================================
**                        Class Declarations
** ======================================================================= */
//----------------------------------------------------------------------------
// Vdec_LockableQueue associates a q_type with a rex_crit_sect_type, forcing
// users to lock the CS before messing with the queue.
//
class Vdec_LockableQueue
{
public:
  Vdec_LockableQueue( void )
  {
    q_init( &m_q );
    rex_init_crit_sect( &m_cs );
  }

  virtual ~Vdec_LockableQueue( void )
  {
    if ( q_cnt( &m_q ) > 0 )
    {
      VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_ERROR, "~Vdec_LockableQueue with items queued" );
    }
  }

  q_type* Lock( void )
  {
    rex_enter_crit_sect( &m_cs );

    return &m_q;
  }

  void Unlock( q_type* &pQ )
  {
    pQ = NULL;
    rex_leave_crit_sect( &m_cs );
  }

  int GetCnt( void )
  {
    return q_cnt( &m_q );
  }

private:
  q_type             m_q;
  rex_crit_sect_type m_cs;

};

/* ======================================================================
CLASS
  VideoDecoder

DESCRIPTION
  Abstract base class for all video decoders.  Allows the C-language
  API shims to have something to call.

========================================================================== */
class VideoDecoder
{
  // Video decoder subclasses 'register' themselves with this base class
  // to perform the following ftyp-keyed tasks:
  // - decoder creation
  // - media header parsing
  //
  // In order to register itself to handle these creation tasks, a subclass
  // must, at static initialization time, create a structure of the
  // VDEC_FTYP_HANDLER_RECORD type.
  //
  // Then, you must add a line to VideoDecoder::InitializeFtypRegistry
  // to insert this structure into the vdec registry.
  //
  // Each subclass has to put the following stuff into its handler record:
  // - a string containing all the ftyps supported by that subclass
  //   (for instance, a subclass that handles 'mp40' and 'mp42' would use the
  //    string "mp40mp42")
  // - a function of type VDEC_CREATION_FUNC which creates a decoder object
  //   of that subclass
  // - a function of type VDEC_HEADER_PARSER_FUNC which parses media headers
  //   for ftyps supported by that subclass
  //
  // When this base class is asked to parse a media header or create a decoder,
  // it will check in the registry for a node with the proper ftyp in it,
  // and it will use the functions provided there to do the work.
  //
  // For example:
  //
  // Let's say I'm creating a subclass which can handle the 'bull' and 'bear' ftyps.
  // I would have the following code in my header file:
  //
  // class StockDecoder : public VideoDecoder
  // {
  // ...
  //   static const char* const        sm_ftypsSupported;
  //
  //   static VDEC_ERROR ParseHeader( const VDEC_BYTE * const pBuf,
  //                                  const unsigned long,    bufSize,
  //                                  unsigned long * const   pBufConsumed_out,
  //                                  VDEC_CONFIG * const     pConfig_out );
  //   static VideoDecoder* Create( const VDEC_CONFIG * const pConfig,
  //                                VDEC_FRAME_CB_FN          pFrameCb,
  //                                void * const              pFrameCbData,
  //                                VDEC_ERROR * const        pErr_out );
  //
  // In my implementation file, I would have the following:
  //
  // char * const StockDecoder::sm_ftypsSupported = "bullbear";
  // VDEC_FTYP_HANDLER_RECORD StockDecoder_handlerRecord =
  //   {
  //     NULL, /* link'll get set up when we register this struct */
  //     StockDecoder::sm_ftypsSupported,
  //     StockDecoder::ParseHeader,
  //     StockDecoder::Create
  //   };
  //
  // Notice that the handler record structure is global, and not
  // class static!  That allows us to add the handler to the base class's
  // registry without having to include the subclass's header file...
  // read on...
  //
  // In VideoDecoder::InitializeFtypRegistry, I add:
  //
  // #ifdef FEATURE_STOCK_DECODER
  //   extern VDEC_FTYP_HANDLER_RECORD StockDecoder_handlerRecord;
  //   RegisterWithFtypRegistry( &StockDecoder_handlerRecord );
  // #endif
  //
  // And that's all there is to it!  Now the base class will defer to the
  // StockDecoder class for the 'bull' and 'bear' ftyps.
  //
public:
  typedef
    VDEC_ERROR (*VDEC_HEADER_PARSER_FUNC)( const char * const      fourcc,
                                           const VDEC_BYTE * const pBuf,
                                           const unsigned long     bufSize,
                                           unsigned long * const   pBufConsumed_out,
                                           VDEC_BLOB ** const      ppConfig_out );

  typedef
  VideoDecoder* (*VDEC_CREATION_FUNC)(VDEC_ERROR * const  pErr_out);

  typedef struct
  {
    list_link_type          link;      /* used by list services             */
    const char *            ftyps;     /* All supported ftyps, concatenated */
    VDEC_CREATION_FUNC      pCreateFn; /* to create a vdec                  */
  }
  VDEC_FTYP_HANDLER_RECORD;

protected:

  static list_type sm_ftypRegistry;
  static bool      sm_registryIsInitialized;

  static bool InitializeFtypRegistry( void );
  static bool RegisterWithFtypRegistry( VDEC_FTYP_HANDLER_RECORD* const pRecord );

  static int  GetLayerIDWithEarliestTimeStamp( uint32 NumBytes[MAX_MP4_LAYERS],
                                               uint64 TimeStamp[MAX_MP4_LAYERS],
                                               uint8  nLayers );
private:

  static VDEC_FTYP_HANDLER_RECORD* FindFtypInRegistry( const char* const ftyp );

public:

  // This function is a static function called by the API to create the
  // appropriate decoder. This helps keep the API independent of specific
  // decoder implementations.
  //
  // This function looks in the vdec registry to find the proper function
  // to do the construction.
  //

  static int IsInstanceValid(VDEC_STREAM_ID stream);
  
  static VideoDecoder * Create
  (
	const char * const      pFourcc,
	VDEC_ERROR *            pErr_out
  );


 virtual VDEC_ERROR Initialize
 (
    VDEC_BLOB * const               pConfig,
    const unsigned short            nLayers,
    VDEC_MALLOC_FN                  pfnFrameBufferMalloc,
    VDEC_FREE_FN                    pfnFrameBufferFree,
    VDEC_EVENT_CB_FN                pEventCb,
    void * const                    pEventCbData,
    VDEC_DIMENSIONS                &frameSize,
    VDEC_CONCURRENCY_CONFIG         concurrencyConfig
  )=0;

  // Subclasses are expected to override Dump to serialize their own
  // specialized stuff, but subclasses, remember to call the base-class
  // dump to get all the common stuff serialized...
  //
  // Used when we're providing our own buffers - there's a protected
  // member which allows subclasses to provide buffers for us
  // so we don't need to concatenate lots of little dumps for the final
  // output...
  //
  virtual VDEC_ERROR Dump( VDEC_BYTE ** const ppBuf_ret,
                           unsigned long * const pBufSize_ret );

  // Data input functions
  //
  virtual VDEC_ERROR ReuseFrameBuffer( VDEC_FRAME * const pFrame );

  virtual VDEC_ERROR Flush( void );

  // Subclasses are required to implement Suspend and Resume;
  // currently there is no common behavior for the base class
  virtual VDEC_ERROR Suspend( void );
  virtual VDEC_ERROR Resume( VDEC_CONCURRENCY_CONFIG concurrencyConfig);

  virtual VDEC_ERROR Scale( VDEC_DIMENSIONS outFrameSize,
                            VDEC_SCALE_CB_FN scaleCbFn,
                            void * const     pCbData );

  // Decode is the only function which subclasses MUST override.  Well,
  // this and the static functions above...and you SHOULD override Dump.
  //
  virtual VDEC_ERROR Decode( void ) = 0;

  virtual VDEC_ERROR GetStats( VDEC_STATS * const pStats_out );

  virtual void Destroy (void) {return ;}

  virtual void ClearStats( void );

  virtual VDEC_ERROR SetParameter( VDEC_PARAMETER_ID           parameter,
                                   VDEC_PARAMETER_DATA * const info ) = 0;
  virtual VDEC_ERROR GetParameter( VDEC_PARAMETER_ID           parameter,
                                   VDEC_PARAMETER_DATA *       info ) = 0;

  virtual VDEC_ERROR Queue( VDEC_INPUT_BUFFER * const pInput,
                            VDEC_QUEUE_CB_FN          pFnCb,
                            void * const              pCbData );
  
  /* EOS related functions */
  virtual VDEC_ERROR Queue_eOS();
  virtual VDEC_ERROR Queue_eOS_Buffer(uint32 nLayers);
  static void eOSBufferFreeCallback(const VDEC_STREAM_ID      /* unused */ ,
                             VDEC_INPUT_BUFFER * const pBuffer,
                             void * const              pCbData);
  void eOSBufferFreeHandler( VDEC_INPUT_BUFFER * const pBuffer );

protected:
  int m_maxStatsQueueLength;

  VDEC_STATS m_stats;  // everybody needs a statistics struct

  //=========================================================================

  /* Information about the 'frame-available' callback.  All subclasses
  ** use this, so it's stored centrally here.
  */
  VDEC_EVENT_CB_FN m_pEventCb;
  void *     m_pEventCbData;

  //=========================================================================
  // A generic DSP-watchdog callback.  This just creates a
  // MP4_DECODE_FATAL_ERROR message and sends it to the application via
  // m_pFrameCb.
  //
  static void WatchdogCallback( void* pThis );

  //=========================================================================
  // Functions to allocate and free memory stored in BLOBs.
  // Since BLOBs are only to be allocated by decoders, the allocation function
  // is protected.  Anybody can release memory associated with a BLOB they've
  // been given, so that one is public.
  //
protected:
  static VDEC_BLOB* AllocateBLOB( const char * const  fourcc,
                                  const VDEC_BLOBTYPE type,
                                  uint32              size );
public:
  static VDEC_ERROR FreeBLOB( VDEC_BLOB *pBlob );

  //
  //=========================================================================
  // Subclass identification.
  //
  // This section exists only because the ARM1.2 compiler does not support
  // dynamic_cast.  We need a mechanism to determine the exact type of
  // objects referenced with a VideoDecoder pointer, and since the
  // compiler can't do it, we must do it ourselves.
  //
  // To that end, I'm defining an enum and a const data member which subclasses
  // can initialize via our constructor.  There is also a public function
  // which can be called to determine an object's type.
  //
  // This can all go away if we ever get a real compiler.
  //
public:
  enum DECODER_TYPE
  {
    VDEC_TYPE_MP4,
    VDEC_TYPE_H264,
    VDEC_TYPE_JPEG,
    VDEC_TYPE_WMV,
    VDEC_TYPE_OSCAR,
    VDEC_TYPE_REAL
  };

 /* An internal state which helps us keep track of the DSP.
 */
  enum
  {
    VDEC_STATE_ERROR         = -1,
    VDEC_STATE_UNINITIALIZED,
    VDEC_STATE_INITIALIZED,
    VDEC_STATE_DESTRUCTING
  }
  m_state;

  DECODER_TYPE DecoderType( void ) const;

protected:
  const DECODER_TYPE cm_decoderType;

  //
  //=========================================================================
  // Constructor and destructor
  //
  // These are way down here to make sure they're after all relevant typedefs.
  //
public:
  VideoDecoder(const DECODER_TYPE decoderType);
  virtual ~VideoDecoder( void );

  /*  Enum to specify which layer is decoded  */
  enum vdec_decoded_layer_type
  {
    VDEC_BASE_LAYER = 0,
    VDEC_ENH_LAYER = 1,
    VDEC_BASE_PLUS_ENH_LAYER = 2
  };

  /* enum to specify type of frame decoded */
  enum vdec_decoded_frame_type
  {
    VDEC_FT_I_FRAME=1,
    VDEC_FT_P_FRAME = 2,
    VDEC_FT_B_FRAME = 3,
    VDEC_FT_FRUC_FRAME = 4
  };

  /* Structure to collect frame statistics for logging */
  struct vdec_frame_decode_info 
  {
    vdec_decoded_frame_type FrameType;
    // (this enum will have Base, enhanced, base+enhanced and num. of layers as members)
    vdec_decoded_layer_type LayerDecoded; 

    uint32  intra_mb_count[MAX_MP4_LAYERS]; 
    uint32  inter_mb_count[MAX_MP4_LAYERS]; 
    uint32  error_mb_count[MAX_MP4_LAYERS];

    uint32  SuperFrameID; // Super frame ID, if present
    uint32  FloID;        // FLO ID, if present

    unsigned char  isInterpolated;
  };
  vdec_frame_decode_info  vdec_framedecode_info;

  /* For logging the frame statistics */
  void LogQTVFrameDecodeStats(uint32 framePTSInfo );


  //
  //=========================================================================
  // The input queue
  //
  // Queued input data is stored on the input queue.  Everybody has one of
  // these, so the base class implements it.
  //
protected:
  /* These structures, while having redundant queue features, can NOT be
  ** subclassed from a common 'queueable' because the queue services defined
  ** in queue.h do NOT perform as advertised - the item pointer in q_link
  ** is ignored, and q_get returns a pointer to the link no matter what.
  ** So if I want these structures to work with queue services, I need
  ** to have the link at the very top with no vtable pointer or anything
  ** else cluttering up the works.
  */
  struct InputQNode
  {
    q_link_type        link;
    VDEC_INPUT_BUFFER *pBuf;
    VDEC_QUEUE_CB_FN   pFnCb;
    void              *pCbData;
    VideoDecoder      *pDecoder;
  };

  // How InputQNodes are expected to be used:
  //
  // 1) Call AllocateInputNode to obtain a node from the free-pool and
  //    populate the structure.  At this point, the InputQNode is
  //    the owner of the input buffer.
  // 2) Place the node on the input queue.
  // 3) Do stuff.
  // 4) When you're ready to release the input buffer and return it to
  //    the application, call ReleaseNode on it.  This returns it to
  //    the app and puts the node structure back in the free pool for reuse.

  InputQNode* AllocateInputNode( VDEC_INPUT_BUFFER * const pBuf,
                                 VDEC_QUEUE_CB_FN          pFnCb,
                                 void * const              pCbData,
                                 VideoDecoder * const      pDecoder );
  void ReleaseNode( InputQNode* const pNode );

  Vdec_LockableQueue        m_inputQ;

  /* EOS buffers */
  VDEC_INPUT_BUFFER  m_eOS_InputBuffer;
  VDEC_INPUT_BUFFER  *m_eOS_pInputBuffer;

  /* we will queue it only once */
  boolean isEoSBufferQueued;

  //-------------------------------------------------------------------------
  // VirtualInputBuffer
  //
  // This class provides a 'virtual' input buffer produced from multiple
  // nodes on the input queue.  For each layer, the virtual node contains
  // a pointer to the first node on the input queue with valid data.
  //
  // When the virtual buffer is destroyed, it releases all the nodes on the
  // input queue which are 'exhausted' (all layers in the node are consumed).
  //
  // Why am I bothering with this?  Because the vdec API does not require
  // queue and decode calls to be in lock-step.  If an application wants to
  // call queue a bunch and then decode a bunch, we must support that.
  // In multilayer cases, however, this can give us a headache.  Take, for
  // example, the following TS clip:
  //
  // time increases ------------------------->
  // layer 1: -- 1 --- 2 --- 4 --- 5 --- 7 --- 8 --- ...
  // layer 0: 0 --------- 3 --------- 6 --------- 9 -...
  //
  // Since videofmt produces input buffers containing the next available
  // sample per layer, our first getNextLayeredSample call will result
  // in an input buffer containing frames 0 & 1:  Our second call would
  // produce an input buffer containing samples 2 & 3.  If we queue both,
  // we end up with an input queue which looks like this:
  //
  // inputQ: 0/1 -> 3/2
  //
  // Assume a decode call which always returns the input buffer when it
  // completes.  We call decode, consume frame 0, and return the input
  // buffer to the application with frame 0 consumed: x/1.  The application
  // hands the buffer to videofmt for refilling, and it replaces the consumed
  // layers with the next available sample for those layers.  The buffer
  // becomes 6/1, and we requeue it.  The input queue is now:
  //
  // inputQ: 3/2 -> 6/1
  //
  // Now we've got a problem.  The next frame to decode is in layer 1
  // in the second input node.  This means that, in order to find the next
  // frame to decode, we'll have to do a linear search of the entire input
  // queue on every decode call. Ick.
  //
  // What if we just hold onto input buffers until all layers are empty?
  //
  // Going back to our example, given the input queue
  //
  // inputQ: 0/1 -> 3/2 -> 6/4 -> 9/5
  //
  // The first decode consumes frame 0, just as before:
  //
  // inputQ: x/1 -> 3/2
  //
  // The second decode consumes frame 1 (it's the only non-consumed frame
  // in the first node, so there's no contest) and we return the buffer.
  // This seems to be healthy, but keep this up until we get to the following
  // state:
  //
  // inputQ: 6/x -> 9/5
  //
  // Now we'll decode frame 6, as it's the only frame left available
  // in the lead node, and that's wrong.  The correct solution, again,
  // involves searching the entire list for the lowest available timestamp.
  //
  // The correct solution which covers all cases is to produce a 'virtual'
  // node at the queue head which contains at every layer a reference
  // to the first unconsumed layer, like this:
  //
  //  virtual   inputQ
  //    [6] -----> 6        9        12
  //     -         - -----> - -----> -
  //    [5] ------ x -----> 5        7
  //   node
  //
  // That's what this class does.  It creates a virtual node containing
  // the earliest available frames in all layers and propogates changes
  // to those frames resulting from decodes back into the correct nodes
  // in the inputQ.  If entire input buffers end up consumed by a decode
  // (think 'x/x') the virtualNode returns those to the application for
  // reuse.

  // The class is friended because it needs access to VideoDecoder-internal
  // types.  This is fine - the class is just a tool for abstracting
  // an operation the VideoDecoder needs to do, anyway.
  //
  // This three-declaration sequence gets around the chicken-and-egg problem
  // of having to declare a class before you can friend it, but being unable
  // to define it using private types until it's friended.

  class VirtualInputBuffer;
  friend class VirtualInputBuffer;

  class VirtualInputBuffer
  {
  public:
    // We could be explicit here and pass in references just to the
    // input queue and the input free-pool, but that just produces
    // a clunky interface.  We've already friended this class, so there's
    // no harm just passing in a reference to the decoder using it.

    VirtualInputBuffer( VideoDecoder &decoder );
    ~VirtualInputBuffer( void );

    VDEC_INPUT_BUFFER* operator->( void ); // Masquerading as input buffer
    VDEC_INPUT_BUFFER*  GetBuffer( void ); // Same idea, old-school syntax

  private:
    VideoDecoder &m_decoder;

    // Our composite buffer, created during construction.  This is what
    // we let clients see.

    VDEC_INPUT_BUFFER m_virtualBuffer;

    // Extra internal accounting lets us keep track of which nodes we're
    // representing - this lets us propogate changes to the virtual node
    // back to the real nodes during destruction.

    InputQNode* m_aNodes[ VDEC_COMMON_MAX_LAYERS ];
  };

  //
  //=========================================================================
  // The frame queue
  //
  // decoded frames which have been passed to the application are tracked
  // on this queue.
  //
  // This queue is treated like the input queue - fetch and initialize
  // a buffer from AllocateFrameNode, release it with ReleaseNode.
  // You know the drill.

protected:
  struct FrameQNode
  {
    q_link_type link;
    VDEC_FRAME  frame;
  };

  FrameQNode* AllocateFrameNode( void );
  void ReleaseNode( FrameQNode* const pNode );

  Vdec_LockableQueue m_frameQ;        // the frame-buffer queue
  
  /* Stats maintained for video decoders. Since we need this info
  ** for ARM based decode stats.
  */
  struct vdecoderstats
  {
     uint32 cumulative_dec_time_ms;
     uint32 max_dec_time_ms;
     uint32 dec_frame_count;
#ifdef FEATURE_FRE
     uint32 dec_interpolated_frame_count;
#endif
  }; 
  vdecoderstats vdecoder_stats;

#ifndef PLATFORM_LTK
  /* Logs the Peak and Average Arm decode time for decoding. */
  void LogArmDecodeTime_stats(const long average_dec_time_ms,
                              const long peak_dec_time_ms,
                              const uint8 videotype) ;

#endif /* PLATFORM_LTK */

protected:
  VDEC_BLOB *              m_pConfig;
  unsigned short           m_nLayers;
  VDEC_MALLOC_FN           m_pfnFrameBufferMalloc;
  VDEC_FREE_FN             m_pfnFrameBufferFree;
  VDEC_DIMENSIONS          m_frameSize;
  VDEC_CONCURRENCY_CONFIG  m_concurrencyConfig;
  boolean                  m_bDeblockingEnable;
  boolean		   m_bExternalClock;	

  static char              m_fourcc[5];
  bool                     m_bDecoderInitialized;

  /* base class flag to determine whether or not xScaler is enabled by OEM */
  bool m_isxScalerEnabled;
};

#endif /* _VDECODER_I_H_ */
