#ifndef QTV_COMMON_STATE_TABLE_H
#define QTV_COMMON_STATE_TABLE_H
/* =======================================================================

                          QtvCommonStateTable.h

DESCRIPTION

  Definition of the Common QTV state table class. This class implements the 
  interface for finite state machines with transitions.Each of these 
  transition contains next state and action. 

INITIALIZATION AND SEQUENCING REQUIREMENTS

  None.

Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/hsm/main/latest/inc/qtvcommonstatetable.h#1 $ 
$DateTime: 2008/05/08 11:03:24 $
$Change: 656211 $ 

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

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
/* ======================================================================
CLASS: QtvCommonStateTable

DESCRIPTION: Classical finite state machine class. 

PARAMETERS:

RETURN VALUE:

=========================================================================*/
// QTV Common State Table class
class QtvCommonStateTable
{
public:
  //pointer-to-member function
  typedef unsigned int (QtvCommonStateTable::*pMFnAction)(); 

  // inner struct transition (aggregate)
  struct Transition                           
  {
    pMFnAction       action;  // Transition - Action 
    unsigned int  nextState;  // Destination state 
  };

  // Constructor 
  QtvCommonStateTable(Transition const *table, 
                      unsigned int nStates, 
                      unsigned int nSignals)
  : m_aTransitionTable(table), 
    m_nMaxSignals(nSignals), 
    m_nMaxStates(nStates) 
  {
    /* Empty */
  }

  // Virtual Destructor
  virtual ~QtvCommonStateTable() 
  {
    /* Empty */
  }                                
  
  // Dispatch function which represents all the transitions from the state machine. 
  // This routine could be modified to the specific task at hand through inheritance.
  bool dispatch(unsigned const sig) 
  {
    // Check if the transition table exists and signal is within bounds
    if(m_aTransitionTable && (m_eState < m_nMaxStates) && (sig < m_nMaxSignals))
    {
      register Transition const *t = m_aTransitionTable + m_eState*m_nMaxSignals + sig;
      if(!t->action)
      {
        /* Transition Not defined yet */
        return false; 
      }
      nextEv  =(this->*(t->action))();
      m_eState = t->nextState;
      return true;
    }
    return false;
  }

  
protected:
  unsigned int m_eState;  // Current State 
  // Following event is set by every action dynamically
  unsigned  int nextEv;  // Next Event to be processed 

private:

  /* Transition table for every possible state & event */
  Transition const *m_aTransitionTable; 

  /* Max Number of signals , Need to access the correct transition */
  unsigned m_nMaxSignals; 

  /* Max Number of states , Need to access the correct state      */
  unsigned m_nMaxStates;
};
#endif

