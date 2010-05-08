#ifndef QTV_HSM_H
#define QTV_HSM_H
/* =======================================================================

                          qtv_hsm.h

DESCRIPTION
  Definition of types and classes to support the QTV hierarchical state
  machine (HSM) framework.

INITIALIZATION AND SEQUENCING REQUIREMENTS
 To use an HSM, the following initialization order must be used:
   1) instantiate object
   2) call init().  This transitions the state machine from the initial
      pseudostate.

 After these steps are complete, events may be dispatched to
 the state machine.

NOTES
  These classes are a modified implementation of the design presented in
  "Practical Statecharts in C/C++", by Samek.  For a thorough discussion
  of hierarchical state machines and the ideas behind the design, see
  chapters 1-4 of that text.

DETAILED USAGE INFORMATION
To use the QTV HSM classes, do the following:

0)  Get familiar with the concepts behind UML statecharts and hierarchical
    state machines.  A good quick article can be found at
    http://www.wisdom.weizmann.ac.il/~dharel/SCANNED.PAPERS/Statecharts.pdf

1)  (optional, but recommended) figure out your state machine.  Drawing it
    can be mighty helpful. Doesn't have to be in ASCII. ;)

    Example:

    A state machine for locating C-style comments.

         *-.
           |
           v
          +- code -------------+
          |                    |
          | +- slash -+  SLASH |
          | |         |<-------+
          | |         +------->|
       +->| +-------+-+  CHAR  |
       |  |         |          |
       |  +-------- | ---------+
       |            |
       | SLASH      | STAR
       |            v
       |  +- comment ----------+
       |  |                    |
       |  | +- star --+  STAR  |
       |  | |         |<-------+
       +----+         +------->|
          | +---------+  CHAR  |
          |                    |
          +--------------------+

2)  Define event types.  If events require parameterization, create
    subclasses of event_class for those event types.  Events below EVENT_USER
    are reserved for the framework.  Do not use them!

    Example:

    From the state machine, we see events SLASH, STAR, and CHAR.  None require
    parameterization, but I'll show a parameterized version of CHAR
    as an example.

    enum my_events
    {
      SLASH = qtvhsm::event_class::EVENT_USER,
      STAR,
      CHAR
    };

    And, a hypothetical parameterized CHAR event:

    class char_event_class : public qtvhsm::event_class
    {
    public:
      char_event_class( char c_in ) : event_class( CHAR ), c( c_in ) {}
      char c;
    };

3)  Define your state-machine class as a subclass of hsm_class.

    Example:

    class my_hsm : public qtvhsm::hsm_class
    {
    public:

3a) Declare states for your HSM.  States are represented as functions
    which take an event parameter and return a state pointer.
    Remember to define an initial state, which will be used to perform
    the machine's initial transition and startup actions.
    The HSM_DECLARE_STATE macro can be very helpful for this.

    Example:

    private:
      HSM_DECLARE_STATE( initial );
      HSM_DECLARE_STATE( code );
      HSM_DECLARE_STATE( slash );
      HSM_DECLARE_STATE( star );
      HSM_DECLARE_STATE( comment );

3b) Define a constructor for your HSM which passes the initial state
    you defined in 3a to the hsm_class constructor.  The initial transition
    will not be executed when the state machine is instantiated.
    Initialization (the initial transition) occurs when init() is called.
    You'll need to cast the initial function pointer to state_type
    and make sure to fully qualify the function name.

    Example:

    public:
      my_hsm( void ) : hsm_class( ( qtvhsm::state_class::state_fn_type )&my_hsm::initial ) {}

3c) Add a call to the HSM_DEFINE_CALL_STATE_FN macro in the public section.
    This is boilerplate - just put it in your class and ignore it.

    Example:

    public:
      HSM_DEFINE_CALL_STATE_FN();

3d) At this point, your HSM subclass is complete.  Add any other data
    members it needs.  Be careful of adding more public interface functions:
    all the input to a state machine should be via dispatched events.

    Example:

    class my_hsm : public qtvhsm::hsm_class
    {
    public:
      my_hsm( void ) : hsm_class( ( qtvhsm::state_class::state_fn_type )&my_hsm::initial ) {}
      HSM_DEFINE_CALL_STATE_FN();

    private:
      HSM_DECLARE_STATE( initial );
      HSM_DECLARE_STATE( code );
      HSM_DECLARE_STATE( slash );
      HSM_DECLARE_STATE( star );
      HSM_DECLARE_STATE( comment );
    };

*** INTERMISSION 1 ***

A note about states and events....a state is a function which receives
events and returns 0 if it processed it, or a pointer to its parent state
if it is unable to handle the event.  State functions should use the
HSM_RETURN macro to do this neatly.

There are four system-defined events which are dispatched to states
by the framework.  These require special handling.  They are:

EVENT_SUPERSTATE_PROBE
  This event is used by the framework to navigate state machine
  hierarchies.  No state may handle this event.  In other words,
  states which receive this event must always return their parent state.
  Your state handlers should always ignore this event.

EVENT_ENTRY
  This event is sent by the framework when a state is entered.

EVENT_EXIT
  This event is sent by the framework when a state is exited.

EVENT_INIT
  This event is sent by the framework after an EVENT_ENTRY to query a
  state for the presence of an 'initial stransition'.  States with
  initial transitions are not valid transition endpoints.  If you
  handle EVENT_INIT, you must use the HSM_INIT macro to specify the target
  of your initial transition.  This state *must* be a direct substate.
  There's a code example in step 4, but here's an example in UML:

      +- s0 -------+
  *-->|            |
      |  *-.       |
      |    |       |
      |    v       |
      | +---- s1 -+|
      | |         ||
      | +---------+|
      +------------+

  The system (top) defines an initial transition to state s0, which defines
  an initial transition to s1.  This means that any time a transition is
  made to s0, the state machine must automatically transition into s1
  as well: no transition may end in s0.

  If all of this seems terribly obscure and pointless, worry not.
  Lots of great state machines have only the main initial transition.
  Until you need an internal initial transition (and when it happens, you'll
  know) just ignore EVENT_INIT in all states but the main initial
  pseudostate.  See step 4 for details on this.

Therefore, when entering a state, the framework will send two events
to that state:

  - EVENT_ENTRY, to say 'this state is being entered'.
  - EVENT_INIT, to query for the presence of an initial transition.
    If this event is unhandled, this state is a valid transition endpoint.
    If it is handled, the initial transition (specified with HSM_INIT)
    is followed.

When leaving a state, the framework sends an EVENT_EXIT to the state
being left.

There is one big rule to handling system events:
- Do NOT cause transitions directly from the handling of these events!
  To do so will cause undefined behavior.  There's nothing wrong with
  queueing an event for later consumption which will cause a transition,
  but don't do it directly from the handling for the system events.

*** END INTERMISSION 1 ***

4)  Implement the initial state.  This state is special, as it only needs
    to process EVENT_INIT, and it does all your startup initialization.
    Make sure this event returns 0.  If you use the event parameter,
    you can use the HSM_DEFINE_STATE macro to provide the boilerplate.
    If you don't, you can use HSM_DEFINE_EVENTLESS_STATE - this is the
    same as HSM_DEFINE_STATE, but the parameter is unnamed so the compiler
    won't complain if you don't use it.  This really only makes sense
    for initial-transition pseudostates.

    Example:

    HSM_DEFINE_EVENTLESS_STATE( my_hsm::initial )
    {
      ..initialize data members...
      ..allocate needed memory...
      ..grab needed hardware resources...

      HSM_INIT( my_hsm::code );
      return 0;
    }

    This is the only state which MUST define an initial transition via
    the HSM_INIT macro.

5)  Define your states!  If a state handles an event, return 0.  If not,
    return the parent state via HSM_RETURN.  Top level states must return
    hsm_class::top.  To initiate a state transition, use HSM_TRANSITION.
    Just like the initial transition, you can use HSM_DEFINE_STATE
    to provide the boilerplate.

    If your state processes an event, return 0.  If not, return
    its parent state using the HSM_RETURN macro.
    This allows the framework to delegate event handling to supertypes
    as appropriate.  If your state doesn't HAVE a parent (if it is at the
    top level of your design), then return qtv_vrend_class::top as its
    parent.  Top is the implicit root of all HSM hierarchies.

    Example:

      HSM_DEFINE_STATE( star )
      {
        switch( event_ptr->kind )
        {
          case SLASH:
          {
            do_stuff();
            HSM_TRANSITION( my_hsm::code );
            return 0;
          }
          case CHAR:
          {
            do_other_stuff();
            HSM_TRANSITION( my_hsm::comment );
            return 0;
          }
        }

        HSM_RETURN( my_hsm::comment )
      }

      HSM_DEFINE_STATE( comment )
      {
        switch( event_ptr->kind )
        {
          case STAR:
          {
            do_star_stuff();
            HSM_TRANSITION( my_hsm::star );
            return 0;
          }
        }

        HSM_RETURN( qtv_vrend_class::top );
      }

    None of our example states needed to process the system events, but
    you certainly could.  Here's a hypothetical state handler to control
    turning on the light bulb in the refrigerator when the door is opened.

      HSM_DEFINE_STATE( fridge_hsm::door_open )
      {
        switch( event_ptr->kind )
        {
          case EVENT_ENTRY:
          {
            turn_on_light();
            return 0;
          }
          case EVENT_EXIT:
          {
            turn_off_light();
            return 0;
          }

          ..Additional event handling would go here...
        }

        HSM_RETURN( parent state )
      }

    Notice that since all of these examples are valid transition destinations,
    none handle EVENT_INIT.

6)  Use your state machine!  Initialize with init, then dispatch messages.

    Example:

      static my_hsm hsm;

      int main()
      {
        qtvhsm::event_class* event_ptr;

        hsm.init();

        while ( ...still more to do... )
        {
          ...get an event, point to it with event_ptr ...

          hsm.dispatch( event_ptr );

          ...release or reuse the event ...
        }
      }

    Your state machine need not be static.  It was made so in this example
    to demonstrate the separation of creation/construction and initialization
    for state machines.

Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/hsm/main/latest/inc/qtv_hsm.h#1 $
$DateTime: 2008/05/08 11:03:24 $
$Change: 656211 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* The test harness is a win32 app, and knows nothing of qtv-standard
** types.
*/
#ifdef WIN32

typedef unsigned char uint8;
typedef unsigned long uint32;

#else /* WIN32 */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "comdef.h" /* for basic qtv primitive types */

#endif /* WIN32 */

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
**                        Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Class & Function Declarations
** ======================================================================= */

class qtvhsm /* this class only serves as a namespace */
{
public:
  /* Subclasses of this class can be produced to specialize HSM events
  ** to provide event paramenters.
  */
  class event_class
  {
  public:
    typedef uint8 event_kind_type;

    /* The following kinds of event are defined by the framework and must
    ** remain reserved.  HSM implementations may begin defining specialized
    ** kinds of event beginning with EVENT_USER.
    */
    enum event_kind_enum
    {
      EVENT_SUPERSTATE_PROBE = 0,
        /* used by the framework to navigate state hierarchies. */
      EVENT_ENTRY,
        /* sent when entering a state */
      EVENT_EXIT,
        /* sent when leaving a state */
      EVENT_INIT,
        /* sent when initializing a state machine to a given state.  this
        ** is different from entry/exit because there's no state transition
        ** associated with initialization - we're not exiting another state. */
      EVENT_USER
        /* Concrete HSMs may begin defining event types with this value
        ** ( MY_AWESOME_FIRST_EVENT = qtvhsm::EVENT_USER ... ) */
    };

    event_kind_type kind;

    event_class( const event_kind_type k ) : kind( k ) {}

    /* The default constructor is for those who'd rather build events
    ** as structs instead of constructable classes. Make sure to set kind
    ** correctly if you do this!
    */
    event_class( void ) : kind( 0 ) {}

  }; /* end of event_class */

  class hsm_class; /* forward decl */

  /* The state_class abstracts state member functions in such a way
  ** as to allow for multiple inheritance - instead of calling hsm member
  ** functions directly through pointers, we hand those pointers
  ** to the hsm objects and let THEM call them via a polymorphic
  ** 'call_state_fn' function.  This way, pointers don't get tangled
  ** as we cast them up and down the hsm hierarchy. */
  class state_class
  {
  public:
    /* state functions return state class objects instead of direct
    ** pointers to their parent classes.  This is transparent to users
    ** because state_classes can be constructed silently from
    ** hsm_class member function pointers of the right type. */
    typedef state_class ( hsm_class::*state_fn_type )( const event_class* const );

    /* This constructor lets us produce state_classes from state functions,
    ** or encapsulate said functions in a state_class. */
    state_class( state_fn_type fn = 0 ) : m_state_fn_( fn ) {}

    /* Comparing state_classes really means comparing their state functions. */
    bool operator==( state_class const &s )
      { return m_state_fn_ == s.m_state_fn_;}
    bool operator!=( state_class const &s )
      { return m_state_fn_ != s.m_state_fn_;}

    /* The state_class, since it masquerades as a state function, supports
    ** operator() so it can be called as a function! */
    state_class operator()( hsm_class* hsm_ptr,
                            const event_class* const evt_ptr )
    {
      return hsm_ptr->call_state_fn( m_state_fn_, evt_ptr );
    }

  private:
    state_fn_type m_state_fn_;
  };

  class hsm_class
  {
  public:
    /* The HSM enforces the start condition of a UML statechart ( that of an
    ** initial transition ) by requiring that it be passed into the constructor.
    ** While the initial transition is DEFINED at construction, it is not
    ** EXECUTED until init() is called.  This allows initialization to be
    ** deferred until all the needed resources are ready.
    */
    hsm_class( state_class initial );
    virtual ~hsm_class( void );

    /* This function is called to execute the initial transition and start
    ** the state machine.  The initial pseudostate provided to the constructor
    ** must do all required initialization and set the initial state.  This
    ** initial state, as per the UML rules, must be a direct substate of 'top'.
    ** However, that state may have an initial transition defined, and
    ** the state referenced by that may have an initial transition defined,
    ** and so on...all initial transitions will be followed as part of
    ** the initialization process.
    **
    ** If subsystem initialization requires parameter information, it can
    ** be passed via the optional event pointer.
    */
    void init( event_class* const event_ptr = 0 );

    /* Dispatch an event to the state machine!  This is the only means of
    ** interacting with the state machine after initialization.
    */
    void dispatch( const event_class* const event_ptr );

    /* External clients can find out about our state if they really want to. */
    state_class get_state( void ) const { return m_state; }

    /* When you're in a state, you're also in all of its superstates. */
    bool is_in_state( state_class state );

    /* In order for state_classes to do their jobs, we must give them
    ** a polymorphic way to call our state functions.  This is it.
    ** All subclasses of hsm_class can use the DEFINE_CALL_STATE_FN
    ** macro to get this automatically. */
    virtual state_class call_state_fn( state_class::state_fn_type fn,
                                       const event_class * const evt_ptr ) = 0;

    #define HSM_DEFINE_CALL_STATE_FN() \
    virtual qtvhsm::state_class call_state_fn( qtvhsm::state_class::state_fn_type fn, \
                                               const qtvhsm::event_class * const evt_ptr ) \
    { \
      return ( this->*fn )( evt_ptr ); \
    }

    /* state functions can use HSM_RETURN to return their parent state
    ** functions without worrying about all the casting and such. */
    #define HSM_RETURN( tgt ) \
       return (( qtvhsm::state_class::state_fn_type )( &tgt ))

  protected:
    /* State functions execute initialization events with this member.
    ** More exactly, state functions call this function when they receive an
    ** init event to identify a further step in the initialization chain.
    ** If a state identifies a substate for a continuation of the init chain,
    ** the chain will be followed until a leaf is reached: a state which
    ** does not call init() when it receives an init event.
    */
    void init( state_class const state );
    #define HSM_INIT( tgt ) \
      init(( qtvhsm::state_class::state_fn_type )( &tgt ))

    /* Here's how the state machine executes state transitions.  A transition
    ** consists of the following actions:
    ** 1) sending of exit events to states up the hierarchy from the
    **    current state to the least common ancestor.
    ** 2) sending of entry events to states down from the LCA to the target
    **    state.
    ** 3) Change of current state to the target.
    **
    ** Note that during the transition, the current state remains unchanged -
    ** this differs from the UML standard, which leaves the state undefined
    ** during the transition.  This makes me (and Samek) cranky - undefined
    ** states are just nonsensical.  Instead, the state change occurs
    ** atomically after the state hierarchy is travelled.
    */
    void transition( state_class target_state );
    #define HSM_TRANSITION( tgt ) \
      transition(( qtvhsm::state_class::state_fn_type )( &tgt ))

    /* Here is the top state, as specified by the UML standard.  All
    ** states must be substates of top.  As you can see, it doesn't do much.
    ** Its purpose is twofold:
    ** - To guarantee a least-common-ancestor between any two states during
    **   a state transition, and
    ** - To make sure all events end up 'handled'.  Of course, at this level,
    **   'handling' = 'throwing away', but if you really cared you'd have
    **   handled the event before it got to top anyway, right?  Right.
    */
    state_class top( const event_class* const ) { return 0;}

    /* HSM subclasses can use these functions to ease declaration
    ** and definition of state functions. */
    #define HSM_DECLARE_STATE( name_ ) \
      qtvhsm::state_class name_( const qtvhsm::event_class* const event_ptr )

    #define HSM_DEFINE_STATE( fullname_ ) \
      qtvhsm::state_class fullname_( const qtvhsm::event_class* const event_ptr )

    /* Remember that this macro is really only useful for the top-level
    ** initial pseudostate, as it's the only one which makes sense if
    ** it ignores the event it gets.
    ** "Real" states should use HSM_DEFINE_STATE */
    #define HSM_DEFINE_EVENTLESS_STATE( fullname_ ) \
      qtvhsm::state_class fullname_( const qtvhsm::event_class* const )

  private:
    /* This convenience function sends a given event to a given state.
    ** The process is common enough that having a function do it cleans
    ** things up substantially.
    */
    state_class dispatch( state_class state,
                          const event_class* const event_ptr );

    /* This function does the 'second half' of state transitions: the
    ** unwinding of the state hierarchy towards the target.  This is common
    ** to every transition topology.
    */
    void finish_transition( state_class* entry_stack_ptr );

    state_class m_state; /* current state */
    /* The initial transition (made during the init() call) is actually
    ** a state class.  But since it's not a state in the true sense
    ** (and it's only used once at startup) it's a pseudo-state */
    state_class m_initialPseudostate;
  }; /* end of hsm_class */

}; /* end of qtvhsm namespace */

#endif /* QTV_HSM_H */
