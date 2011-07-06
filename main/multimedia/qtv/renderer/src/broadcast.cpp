/* =======================================================================

                          broadcast.cpp

DESCRIPTION
  Implementation of the base QTV_Broadcast classes.

Copyright 2005 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/renderer/main/latest/src/broadcast.cpp#6 $
$DateTime: 2009/11/30 03:18:44 $
$Change: 1098040 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "broadcast.h"
#include "assert.h"
#include "qtvsystem.h"
#include "AEEstd.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
QTV_BroadcastTransmitter::TransmitterRegistry
  QTV_BroadcastTransmitter::s_registry;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

#ifdef QTV_BROADCAST_VERBOSE_DIAG

/* ======================================================================
FUNCTION:
  QTV_Broadcast::Describe

DESCRIPTION:
  Every broadcast needs to be able to generate a one-line summary
  of itself for diag output.  Returns the number of characters
  of the buffer used.

PARAMETERS:
  char* const pBuf
    The buffer into which to place our description

  const int bufSiz
    The size of the above buffer.

RETURN VALUE:
  int.
    The number of characters (not including the final '\0') which would
    be required to print everything we wanted.  If this number is < bufSiz,
    that means it all fit.

    Conveniently, this is the same as the return code from snprintf.

    Negative numbers mean something went wrong.

===========================================================================*/
int QTV_Broadcast::Describe( char* const pBuf, const int bufSiz )
{
  int rc( -1 );

  static const char* const aTypesAsText[ NUM_TYPES - FIRST_TYPE ] = /*lint!e656 */
  {
    "VID_FR_RENDER",
    "VID_FR_DROP",
    "VID_FR_FLUSH",
    "VID_DEC_TIMING",
    "VID_FRUC_RESULT"
  };

#ifndef PLATFORM_LTK
  if ( pBuf )
  {
    rc = snprintf( pBuf, bufSiz, "%s", aTypesAsText[ m_type - FIRST_TYPE ] ); /*lint!e656 */
  }
#endif /* !PLATFORM_LTK */

  return rc;
}

#endif /* QTV_BROADCAST_VERBOSE_DIAG */

/* ======================================================================
FUNCTION:
  QTV_BroadcastTransmitter::QTV_BroadcastTransmitter

DESCRIPTION:
  Constructor.

PARAMETERS:
  const char* szName
    Name of the transmitter.  Optional.  If a name is provided,
    receivers will be able to locate this transmitter with the static
    Locate function.  If NULL...well, they won't.

RETURN VALUE:
  None.

===========================================================================*/
QTV_BroadcastTransmitter::QTV_BroadcastTransmitter( const char* szName )
: m_pThisInRegistry( NULL )
{
  q_type*                             pRegistryQ   ( NULL );
  TransmitterRegistry::RegistryQNode* pRegistryNode( NULL );

  QCUtils::InitCritSect(&m_sinkAccessLock);

  for
  (
    int i = (int)QTV_Broadcast::FIRST_TYPE;
    i < (int)QTV_Broadcast::NUM_TYPES;
    ++i
  )
  {
    (void)q_init( &( m_aInstalledSinks[ i ] ) );
  }

  if ( szName != NULL )
  {
    pRegistryNode = ( TransmitterRegistry::RegistryQNode* )
      QTV_Malloc( sizeof( TransmitterRegistry::RegistryQNode ) );
    if ( pRegistryNode != NULL )
    {
      (void)q_link( pRegistryNode, &( pRegistryNode->link ) );
      (void)std_strlcpy( pRegistryNode->name,
                         szName,
                         QTV_BROADCAST_TRANSMITTER_MAX_NAME_LENGTH + 1);
      pRegistryNode->pTransmitter = this;

      pRegistryQ = s_registry.registryQ.Lock();
      q_put( pRegistryQ, &( pRegistryNode->link ) );
      s_registry.registryQ.Unlock( pRegistryQ );

      m_pThisInRegistry = pRegistryNode;
    }
    else
    {
      QTV_MSG_PRIO( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                    "QTV_BroadcastTransmitter out of memory in construction!" );
    }
  }
}

/* ======================================================================
FUNCTION:
  QTV_BroadcastTransmitter::~QTV_BroadcastTransmitter

DESCRIPTION:
  Destructor

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
QTV_BroadcastTransmitter::~QTV_BroadcastTransmitter( void )
{
  q_type* pRegistryQ( NULL );

  // Policy decision:
  //
  // Do I uninstall any remaining sinks?  Do I complain if I find some?
  // If complaining is appropriate, why not let the object pool do it?
  //
  // At this time, I am going to assume that complaints are appropriate,
  // and that the default complaint from the object pool will also be fine.
  // Therefore, this destructor doesn't need to do anything.

  // However, we do need to bother removing ourselves from the registry.
  if ( m_pThisInRegistry != NULL )
  {
    pRegistryQ = s_registry.registryQ.Lock(); /*lint !e1551 */
    q_delete( pRegistryQ, &( m_pThisInRegistry->link ) );
    s_registry.registryQ.Unlock( pRegistryQ );/*lint !e1551 */

    QTV_Free( m_pThisInRegistry ); /*lint !e1551 */

    m_pThisInRegistry = NULL;
  }
  QCUtils::DinitCritSect(&m_sinkAccessLock);
}

/* ======================================================================
FUNCTION:
  QTV_BroadcastTransmitter::InstallReceiver

DESCRIPTION:
  When you want to listen for broadcasts from a given source, you
  have to install a sink to receive those broadcasts.  To do that,
  you call this function!  Once installed, a sink will receive broadcasts
  transmitted through this source until it is uninstalled.

PARAMETERS:
  const QTV_Broadcast::Type type
    The type of event we wish to receive.

  QTV_BroadcastReceiver* const  pSink
    The sink which is to receive those events.

  void* const               userData
    User data - this value will be passed to the sink along with the event
    when events are transmitted.  It is unused by the source object.

RETURN VALUE:
  InstallationKey
    The key used to identify this sink registration.  This will be used
    to uninstall the registration later.  If installation fails,
    this will be 0.

===========================================================================*/
QTV_BroadcastTransmitter::InstallationKey QTV_BroadcastTransmitter::InstallReceiver
(
  const QTV_Broadcast::Type     type,
  QTV_BroadcastReceiver* const  pSink,
  void* const                   userData
)
{
  QTV_BroadcastTransmitter::InstallationKey key   ( 0 );
  SinkQNode*                                pNode ( NULL );

  if ( type >= QTV_Broadcast::NUM_TYPES )
  {
    QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "InstallReceiver given out of bounds type: %d",
                   type );
    return 0;
  }

  QCUtils::EnterCritSect( &m_sinkAccessLock );

  // To install a sink, we just put a record for it on the queue
  // associated with the event type.

  pNode = QTV_New( SinkQNode );
  if ( pNode != NULL )
  {
    (void)q_link( pNode, &( pNode->link ) );
    pNode->pQ       = &( m_aInstalledSinks[ type ] );
    pNode->pSink    = pSink;
    pNode->userData = userData;

    q_put( pNode->pQ, &( pNode->link ) );

    // We just use the node address as the key for this registration.
    // It makes uninstallation a cinch.

    key = pNode;
  }
  else
  {
    // Being out of memory makes us very cranky.
    QTV_MSG_PRIO( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                  "InstallReceiver out of memory!" );
  }

  QCUtils::LeaveCritSect( &m_sinkAccessLock );

  return key;
}

/* ======================================================================
FUNCTION:
  QTV_BroadcastTransmitter::UninstallReceiver

DESCRIPTION:
  Uninstalls a previously installed sink.

PARAMETERS:
  const InstallationKey key
    The key which was returned from the original installation.

RETURN VALUE:
  bool
    true if we found and removed a registration, false otherwise.

===========================================================================*/
bool QTV_BroadcastTransmitter::UninstallReceiver( const InstallationKey key )
{
  bool       rc    ( false );
  SinkQNode* pNode ( ( SinkQNode* )key );

  if ( pNode != NULL )
  {
    QCUtils::EnterCritSect( &m_sinkAccessLock );

    q_delete( pNode->pQ, &( pNode->link ) );
    QTV_Delete( pNode );

    rc = true;

    QCUtils::LeaveCritSect( &m_sinkAccessLock );
  }

  return rc;
}

/* ======================================================================
FUNCTION:
  QTV_BroadcastTransmitter::Transmit

DESCRIPTION:
  Anybody who has a broadcast object they want transmitted calls this
  member to do so.  When broadcasts are passed to this function, they
  become the property of the transmitter.  When the transmitter is done
  with the broadcast, it will delete it via the broadcast's GetDeleter
  mechanism.  This function auto-nulls the broadcast pointer which is
  passed in to help creators resist the urge to muck with broadcasts
  that have already been submitted for transmission.

PARAMETERS:
  const QTV_Broadcast* &pMsg
    The message to broadcast.

RETURN VALUE:
  None.

===========================================================================*/
void QTV_BroadcastTransmitter::Transmit( QTV_Broadcast* &pMsg )
{
  SinkQNode*               pNode  ( NULL );

  if ( pMsg == NULL )
  {
    return;
  }

  QCUtils::EnterCritSect( &m_sinkAccessLock );

  for
  (
    pNode  = ( SinkQNode* )q_check( &m_aInstalledSinks[ pMsg->m_type ] );
    pNode != NULL;
    pNode  = ( SinkQNode* )q_next( &m_aInstalledSinks[ pMsg->m_type ],
                                   &( pNode->link ) )
  )
  {
    pNode->pSink->Receive( *pMsg, pNode->userData );
  }

  QCUtils::LeaveCritSect( &m_sinkAccessLock );

  // We're done with the message now, so delete it.
  QTV_Delete( pMsg );
  pMsg = NULL; // Object's gone - NULL the pointer to prevent trouble.
}

/* ======================================================================
FUNCTION:
  QTV_BroadcastTransmitter::Locate

DESCRIPTION:
  Allows receivers to locate transmitters based on string IDs.
  Since the whole idea of the broadcast API is to make it easy to
  get interesting information without having to know where things are,
  this static member lets anybody who understands broadcasts to find
  the transmitter they want without having to poke around in other people's
  systems.

PARAMETERS:
  const char* szTransmitterName
    The text ID - the name - of the transmitter we're seeking.

RETURN VALUE:
  QTV_BroadcastTransmitter*
    A pointer to the matching transmitter, or NULL if no match.

===========================================================================*/
QTV_BroadcastTransmitter* QTV_BroadcastTransmitter::Locate
(
  const char* szTransmitterName
)
{
  QTV_BroadcastTransmitter*           pResult      ( NULL );
  q_type*                             pRegistryQ   ( NULL );
  TransmitterRegistry::RegistryQNode* pRegistryNode( NULL );

  pRegistryQ = s_registry.registryQ.Lock();

  for
  (
    pRegistryNode  = ( TransmitterRegistry::RegistryQNode* )
                       q_check( pRegistryQ );
    pRegistryNode != NULL;
    pRegistryNode  = ( TransmitterRegistry::RegistryQNode* )
                       q_next( pRegistryQ, &( pRegistryNode->link ) )
  )
  {
    if ( strncmp( szTransmitterName,
                  pRegistryNode->name,
                  QTV_BROADCAST_TRANSMITTER_MAX_NAME_LENGTH ) == 0 )
    {
      pResult = pRegistryNode->pTransmitter;
      break; // We found our match!  No need to look more.
    }
  }

  s_registry.registryQ.Unlock( pRegistryQ );

  return pResult;
}
