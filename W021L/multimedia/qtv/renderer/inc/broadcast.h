#ifndef QTV_BROADCAST_H
#define QTV_BROADCAST_H
/* =======================================================================

                          broadcast.h

DESCRIPTION
  Definition of the public APIs for QTV_Broadcast, QTV_BroadcastTransmitter,
  and QTV_BroadcastReceiver, the interfaces which allow QTV subsystems to
  broadcast information which might be useful without concern for who's
  actually paying attention.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2005 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/renderer/main/latest/inc/broadcast.h#1 $
$DateTime: 2008/05/09 08:14:38 $
$Change: 657007 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "rex.h"
#include "queue.h"
#include "qtv_utils.h"

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/*
** Define this to add diag messages about what's being broadcast.
** Careful - this is a CPU-sucker.
*/
#undef QTV_BROADCAST_VERBOSE_DIAG

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
#define QTV_BROADCAST_TRANSMITTER_MAX_NAME_LENGTH (8)

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Class & Function Declarations
** ======================================================================= */

//===========================================================================
//                          QTV_Broadcast
//
// Broadcasts encapsulate data - these are the objects that fly around
// when there's information to be passed.
//
// Beyond the Type enum, however, there are few requirements placed on
// broadcast objects.  The specifics of any given broadcast type are
// completely up to the sender(s) and receiver(s).  Well, almost.  See below.

class QTV_Broadcast
{
public:
  
  enum Type
  {
    FIRST_TYPE           = 0,
    VIDEO_FRAME_RENDERED = FIRST_TYPE, // video_renderer_broadcast.h
    VIDEO_FRAME_DROPPED,               // video_renderer_broadcast.h
    VIDEO_FRAME_FLUSHED,               // video_renderer_broadcast.h
    VIDEO_DECODE_TIMING,               // video_decoder_broadcast.h
    VIDEO_FRUC_RESULT,                 // fruc_broadcast.h
    NUM_TYPES
  };
  
  Type m_type;
  
  QTV_Broadcast( const Type type ) : m_type( type ) {}
  virtual ~QTV_Broadcast( void ) {}

  #ifdef QTV_BROADCAST_VERBOSE_DIAG

  // Every broadcast needs to be able to generate a one-line summary
  // of itself for diag output.  Returns the number of characters
  // of the buffer used.
  
  virtual int Describe( char* const pBuf, const int bufSiz );

  #endif /* QTV_BROADCAST_VERBOSE_DIAG */
};

//===========================================================================
//                       QTV_BroadcastReceiver
//
// BroadcastReceivers receive broadcasts!  And that's all there is to it.
// How they receive them, or what they do with them after they're received,
// is not defined here.

class QTV_BroadcastReceiver
{
public:
  QTV_BroadcastReceiver( void ) {}
  virtual ~QTV_BroadcastReceiver( void ) {}
  
  virtual void Receive( const QTV_Broadcast& message, void* userData ) = 0;
};

//===========================================================================
//                     QTV_BroadcastTransmitter
//
// BroadcastTransmitters are not the generators of broadcasts - they are the
// dispatchers.  Anybody can MAKE a broadcast, but BroadcastTransmitters know
// who's listening for each type and how to get it to them.
//
// Now, there's nothing preventing a BroadcastTransmitter from generating
// broadcasts and dispatching them for itself...
//
// This flexibility allows broadcast-dispatching to be done in a seperate
// task context than broadcast generation.  Or not!

class QTV_BroadcastTransmitter
{
public:
  QTV_BroadcastTransmitter( const char* szName );
  virtual ~QTV_BroadcastTransmitter( void );
  
  // Keys are nice opaque identifiers - you get one when you install
  // a sink and you use the key to refer to that installation from that
  // point on.

  typedef void* InstallationKey;

  // InstallReceiver
  //
  // When you want to listen for broadcasts from a given source, you
  // have to install a sink to receive those broadcasts.  To do that,
  // you call this function!  Once installed, a sink will receive broadcasts
  // transmitted through this source until it is uninstalled.

  InstallationKey InstallReceiver( const QTV_Broadcast::Type     type,
                                   QTV_BroadcastReceiver* const  pSink,
                                   void* const                   userData );
  bool UninstallReceiver( const InstallationKey key );

  // Anybody who has a broadcast object they want transmitted calls this
  // member to do so.  When broadcasts are passed to this function, they
  // become the property of the transmitter.  When the transmitter is done
  // with the broadcast, it will delete it via the broadcast's GetDeleter
  // mechanism.  This function auto-nulls the broadcast pointer which is 
  // passed in to help creators resist the urge to muck with broadcasts
  // that have already been submitted for transmission.
  //
  // Note that all of the above implies an assumption of this API:
  // we assume QTV_Broadcasts are on the heap.  However, because of
  // the QTV_Broadcast deleter mechanism, a clever broadcast implementer
  // could avoid this.  While such deviancy might be tempting and fun,
  // I can't come up with a condition where it'd be worth the pain.
  // Just put 'em on the heap, OK?

  virtual void Transmit( QTV_Broadcast* &pMessage );

  // Locate() allows receivers to locate transmitters based on string IDs.
  // Since the whole idea of the broadcast API is to make it easy to 
  // get interesting information without having to know where things are,
  // this static member lets anybody who understands broadcasts to find
  // the transmitter they want without having to poke around in other people's
  // systems.

  static QTV_BroadcastTransmitter* Locate( const char* szTransmitterName );
  
protected:

  struct SinkQNode
  {
    q_link_type            link;
    q_type*                pQ;
    QTV_BroadcastReceiver* pSink;
    void*                  userData;
  };

  q_type             m_aInstalledSinks[ QTV_Broadcast::NUM_TYPES ];
  rex_crit_sect_type m_sinkAccessLock;

  // 'Transmitter Registry' stuff
  //
  // This gunk allows transmitters to register and unregister themselves
  // in a static registry which allows receivers to locate transmitters
  // by name.  Subclasses should never have to muck with this.
  //
  // Why is this all in a big static struct?  To get around a compiler bug
  // which prevents the constructors for templatized classes for firing on
  // static objects, like we'd have if the registryPool were alone.
  // The empty constructor and destructors for TransmitterRegistry
  // are required to generate the correct initialization calls.

public:
  
  struct TransmitterRegistry
  {
    struct RegistryQNode
    {
      q_link_type link;
      char        name[ QTV_BROADCAST_TRANSMITTER_MAX_NAME_LENGTH + 1 ];
      QTV_BroadcastTransmitter* pTransmitter;
    };
    
    QTV_LockableQueue registryQ;

    TransmitterRegistry( void ) {}
    ~TransmitterRegistry( void ) {}
  };

protected:
  
  static TransmitterRegistry s_registry;
  
  TransmitterRegistry::RegistryQNode* m_pThisInRegistry;
};

#endif // QTV_BROADCAST_H
