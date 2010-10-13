/* =======================================================================

                          qtv_hsm.cpp

DESCRIPTION
  Implementation of types and classes to support the QTV hierarchical state
  machine framework.

Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/hsm/main/latest/src/qtv_hsm.cpp#1 $
$DateTime: 2008/05/08 13:49:08 $
$Change: 656403 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtv_hsm.h"
#include "assert.h"

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
/*lint -e641 */
static qtvhsm::event_class
  probe_event( qtvhsm::event_class::EVENT_SUPERSTATE_PROBE );
static qtvhsm::event_class entry_event( qtvhsm::event_class::EVENT_ENTRY );
static qtvhsm::event_class exit_event( qtvhsm::event_class::EVENT_EXIT );
static qtvhsm::event_class init_event( qtvhsm::event_class::EVENT_INIT );
/*lint +e641 */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ========================================================================
FUNCTION:
  hsm_class::hsm_class

DESCRIPTION:
  Constructor.

  The HSM enforces the start condition of a UML statechart ( that of an
  initial transition ) by requiring that it be passed into the constructor.
  While the initial transition is DEFINED at construction, it is not
  EXECUTED until init() is called.  This allows initialization to be
  deferred until all the needed resources are ready.

PARAMETERS:
  pseudostate_type initial
    The initial state handler.  This handler should perform any required
    one-time initialization and then trigger the initial transition
    of the state machine.

RETURN VALUE:
  None.
===========================================================================*/
qtvhsm::hsm_class::hsm_class( state_class initial )
: m_state( &qtvhsm::hsm_class::top ),
  m_initialPseudostate( initial )
{
}
/* ========================================================================
FUNCTION:
  hsm_class::~hsm_class

DESCRIPTION:
  Destructor.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
qtvhsm::hsm_class::~hsm_class( void )
{
}

/* ========================================================================
FUNCTION:
  hsm_class::init

DESCRIPTION:
  This function is called to execute the initial transition and start
  the state machine.  The initial pseudostate provided to the constructor
  must do all required initialization and set the initial state.  This
  initial state, as per the UML rules, must be a direct substate of 'top'.
  However, that state may have an initial transition defined, and
  the state referenced by that may have an initial transition defined,
  and so on...all initial transitions will be followed as part of
  the initialization process.

PARAMETERS:
  event_class* const event_ptr
    An optional event pointer, in case state machines need extra parameters
    along with the init message.  If you don't need such parameters,
    just leave it out.

RETURN VALUE:
  None.
===========================================================================*/
void qtvhsm::hsm_class::init( event_class* const event_ptr )
{
  state_class last_state = m_state;

  /* Init must only be called if we're in the top state, and we can
  ** only get into top from the constructor.  Likewise, we must have
  ** a valid initialization state as a transition source.
  */
  ASSERT( m_state == &qtvhsm::hsm_class::top && m_initialPseudostate != 0 );

  /* The first initialization in the chain (the one from 'top' )
  ** differs from the rest.  Instead of merely being an initialization
  ** transition, it's also a hook for initialization code.
  */
  ( void )dispatch( m_initialPseudostate, event_ptr );

  /* Do a quick check.  The superstate of our current state should
  ** be our old state (IE we should only have moved down one level
  ** in the state hierarchy)
  */
  ASSERT( last_state == dispatch( m_state, &probe_event ) );

  /* The transition passed our test, so send the entry even to the new state.
  */
  ( void )dispatch( m_state, &entry_event );

  /* Now to march down the rest of the init chains.  Unlike the first,
  ** these sub-transitions are not entry points and therefore get no
  ** metadata hooks.
  */
  last_state = m_state;

  while ( dispatch( m_state, &init_event ) == 0) /* 0 means handled */
  {
    /* Another check - inits are only allowed one level at a time. */
    ASSERT( last_state == dispatch( m_state, &probe_event ) );

    ( void )dispatch( m_state, &entry_event );
    last_state = m_state;
  }
}

/* ========================================================================
FUNCTION:
  hsm_class::dispatch

DESCRIPTION:
  Dispatch an event to the state machine!  This is the only means of
  interacting with the state machine after initialization.

PARAMETERS:
  event_class* const event_ptr
    The event to be dispatched to the state machine.

RETURN VALUE:
  None.
===========================================================================*/
void qtvhsm::hsm_class::dispatch( const event_class* const event_ptr )
{
  state_class handler; /* The state that can handle the event.
                       ** Can be any parent of the current state. */

  /* March up the state hierarchy until we find a state that handles
  ** this message (or hit top, which handles all messages...kinda.)
  */
  handler = m_state;
  while ( handler != 0 )
  {
    handler = dispatch( handler, event_ptr );
  }
}

/* ========================================================================
FUNCTION:
  hsm_class::init (protected)

DESCRIPTION:
  State functions execute initialization events with this member.
  More exactly, state functions call this function when they receive an
  init event to identify a further step in the initialization chain.
  If a state identifies a substate for a continuation of the init chain,
  the chain will be followed until a leaf is reached: a state which
  does not call init() when it receives an init event.

PARAMETERS:
  state_class const target_state
    The next state in the initialization chain.

RETURN VALUE:
  None.
===========================================================================*/
void qtvhsm::hsm_class::init( state_class const target_state )
{
  m_state = target_state; /* boy, initialization is easy! */
}

/* ========================================================================
FUNCTION:
  hsm_class::transition

DESCRIPTION:
  Executes a state transition.

  Here's how the state machine executes state transitions.  A transition
  consists of the following actions:
  1) sending of exit events to states up the hierarchy from the
     current state to the least common ancestor.
  2) sending of entry events to states down from the LCA to the target
     state.
  3) Change of current state to the target.

  Note that during the transition, the current state remains unchanged -
  this differs from the UML standard, which leaves the state undefined
  during the transition.  This makes me (and Samek) cranky - undefined
  states are just nonsensical.  Instead, the state change occurs
  atomically after the state hierarchy is travelled.

PARAMETERS:
  state_class target_state
    The next state in the initialization chain.

RETURN VALUE:
  None.
===========================================================================*/
void qtvhsm::hsm_class::transition( state_class target_state )
{
  static const int MAX_HSM_DEPTH = 8;

  state_class state;
  state_class superstate;

  state_class entry[ MAX_HSM_DEPTH ];
    /* The entry array contains a stack of those states which require
    ** entry as part of the state transition.  We fill it in as we walk
    ** upwards through the target's ancestry, then unroll it as we
    ** enter states down to the target. */

  state_class *e_ptr;
    /* The 'insertion point' into our entry stack.  e_ptr always points
    ** to the current head of the entry stack. */

  state_class s;
  state_class t;
    /* These are used to hold superstates of the source (s) and target (t)
    ** as we walk up the hierarchy towards the least common ancestor. */

  /*-------------------------------------------------------------------------
  ** Navigate the state heirarchy.
  ** There are six (seven) possible source/target relationships (see Samek 4.4.4)
  ** which we'll check in order of complexity & frequency.
  **
  ** A short description of these seven cases follow.  Description text
  ** is lifted directly from Samek.
  **
  ** 1) source == target
  **    self-transition.  Can be checked directly without probing superstates.
  **
  ** 2) source == target->super
  **    requires probing target superstate. Involves entry to target but no
  **    exit from source.
  **
  ** 3) source->super == target->super
  **    (most common transition topology) requires additional probing of the
  **    source superstate.  Involves exit from source and entry to target.
  **
  ** 4) source->super == target
  **    Requires no additional probing.  Involves exit from source but no
  **    entry to target (as we're already in it).
  **
  ** 5) source == any of target->super
  **    requires probing target superstates until a match is found or top
  **    is reached.  This topology is the last that does not require exiting
  **    the original source state, so if the transition does not fall into
  **    this category ( or one of those before it ) the source must be exited.
  **
  **    I disagree with Samek about the next two.  He seperates them into
  **    two separate cases, but I think that's unnecessary.
  **
  ** 6) source->super == any of target->super
  **    requires traversal of the target state hierarchy.
  **
  ** 7) any of source->super == any of target->super
  **    Requires traversal of the target state hierarchy for every superstate
  **    of the source. */

  /* Initialize the target-hierarchy list.  We'll put a 0 in the first element
  ** as a guard buffer, then build the array up from target to top as needed.
  */
  e_ptr    = &entry[ 0 ];
  *e_ptr++ = 0; /* We put a guard at the end to catch while unrolling */
  *e_ptr   = target_state;
    /* Here we're assuming we'll have to enter the target */

  /* 1) source == target
  **    self-transition.  Can be checked directly without probing superstates.
  **
  ** Remember that self transitions involve an exit and a reentry to the
  ** originating state.
  */
  if ( m_state == target_state )
  {
    ( void )dispatch( m_state, &exit_event );
    finish_transition( e_ptr );
    return;
  }

  /* 2) source == target->super
  **    requires probing target superstate. Involves entry to target but no
  **    exit from source.
  */
  t = dispatch( target_state, &probe_event );
  if ( t == m_state )
  {
    finish_transition( e_ptr );
    return;
  }

  /* 3) source->super == target->super
  **    (most common transition topology) requires additional probing of the
  **    source superstate.  Involves exit from source and entry to target.
  */
  s = dispatch( m_state, &probe_event );
  if ( s == t )
  {
    ( void )dispatch( m_state, &exit_event );
    finish_transition( e_ptr );
    return;
  }

  /* 4) source->super == target
  **    Requires no additional probing.  Involves exit from source but no
  **    entry to target (as we're already in it).
  */
  if ( s == target_state )
  {
    ( void )dispatch( m_state, &exit_event );
    --e_ptr; /* we won't be entering the target after all - we're there! */
    finish_transition( e_ptr );
    return;
  }

  /* 5) source == any of target->super
  **    requires probing target superstates until a match is found or top
  **    is reached.  This topology is the last that does not require exiting
  **    the original source state, so if the transition does not fall into
  **    this category ( or one of those before it ) the source must be exited.
  */

  /* Here we're walking up the target's inheritance chain and recording
  ** the walk in the 'e' array.  If we find a match for the source here,
  ** we can jump to the end (and do the entry walk).
  */
  for ( *(++e_ptr) = t, t = dispatch( t, &probe_event );
        t != 0;
        *(++e_ptr) = t, t = dispatch( t, &probe_event ) )
  {
    if ( t == m_state )
    {
      finish_transition( e_ptr );
      return;
    }
  }

  /* If we're here, the e array now contains the entire ancestry chain
  ** of the target state, starting with the guard zero, then the target
  ** itself, then every ancestor up to top. */

  /* Having failed all previous tests means we'll definitely be
  ** exiting the source state.
  */
  ( void )dispatch( m_state, &exit_event );

  /* 6) source->super == any of target->super
  **    requires traversal of the target state hierarchy.
  **
  ** 7) any of source->super == any of target->super
  **    Requires traversal of the target state hierarchy for every superstate
  **    of the source.
  **
  ** As previously mentioned, I disagree with Samek's separation of
  ** these two cases - they can be treated as one (which is what I am
  ** going to do here).
  */

  /* Remember that s contains a pointer to the source's superstate
  ** (from step three) so we can begin our testing there.
  */
  while ( s != 0 )
  {
    for ( state_class* i = e_ptr; *i != 0; --i )
    {
      if ( *i == s )
      {
        /* Aha!  We've found the least common ancestor.  We're done. */

        /* Because the entry-sequence will start with *e, we must set
        ** it one below our LCA, as we don't want to reenter that state.
        */
        e_ptr = i - 1;
        finish_transition( e_ptr );
        return;
      }
    }

    ( void )dispatch( s, &exit_event );
    s = dispatch( s, &probe_event );
  }
}

/* ========================================================================
FUNCTION:
  hsm_class::finish_transition

DESCRIPTION:
  This function does the 'second half' of state transitions: the
  unwinding of the state hierarchy towards the target.  This is common
  to every transition topology.

PARAMETERS:
  state_class* entry_stack_ptr
    The 'entry stack', containing all those states which need entering
    in order to finish a transition.

RETURN VALUE:
  None.
===========================================================================*/
void qtvhsm::hsm_class::finish_transition( state_class* entry_stack_ptr )
{
  state_class state;
  state_class target_state;

  for ( state = *entry_stack_ptr;
        state != 0;
        state = *( --entry_stack_ptr ) )
  {
    ( void )dispatch( state, &entry_event );
  }

  /* The target state is the one right before we hit the guard 0...
  */
  target_state = *( entry_stack_ptr + 1 );

  m_state = target_state;

  /* It looks like we should be done now, but there's one thing left to do.
  ** If the target state defines an initial transition we must follow
  ** that (just like init) until we hit a leaf node - it's not legal to end
  ** a transition in a state with an initial transition defined.
  */
  while ( dispatch( target_state, &init_event ) == 0 )
  {
    /* Just like in a regular init chain, inits may only go one level
    ** at a time.
    */
    ASSERT( target_state == dispatch( m_state, &probe_event ) );

    target_state = m_state;
    ( void )dispatch( target_state, &entry_event );
  }
}

/* ========================================================================
FUNCTION:
  hsm_class::dispatch

DESCRIPTION:
  This convenience function sends a given event to a given state.
  The process is common enough that having a function do it cleans
  things up substantially.

PARAMETERS:
  state_class target_state
    The next state in the initialization chain.

RETURN VALUE:
  None.
===========================================================================*/
qtvhsm::state_class qtvhsm::hsm_class::dispatch
(
  state_class state,
  const event_class* const event_ptr
)
{
  return state( this, event_ptr );
}

/* ========================================================================
FUNCTION:
  hsm_class::is_in_state

DESCRIPTION:
  Returns true if the state machine is in the given state.  The only thing
  that makes this interesting is the state hierarchy: when you're in a
  state, you're also in all of its superstates.

PARAMETERS:
  state_class state
    The test state.

RETURN VALUE:
  bool
    True if we're in the test state, false otherwise.
===========================================================================*/
bool qtvhsm::hsm_class::is_in_state( state_class state )
{
  while ( state != 0 )
  {
    if ( state == m_state )
    {
      return true;
    }
    state = dispatch( state, &probe_event );
  }

  return false;
}

