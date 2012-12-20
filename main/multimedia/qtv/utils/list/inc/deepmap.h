#ifndef DEEPMAP_H
#define DEEPMAP_H

/* =======================================================================
                               deepmap.h
DESCRIPTION
   This file provides definitions for a map (hashtable) class used by QTV.
   All keys and values are copied by value into the map. It works well with
   instances of ReferenceCountedPointer as either keys or values.

Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/list/main/latest/inc/deepmap.h#2 $
$DateTime: 2009/10/26 21:51:26 $
$Change: 1064894 $

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
#include "deeplist.h"

// A mapping from a key to a value.
template<class K, class V>
class DeepMapEntry
{
public:
  DeepMapEntry() { }

  DeepMapEntry(const K &key, const V &value)
    : m_key(key), m_value(value)
  {
  }

  DeepMapEntry(const DeepMapEntry &that)
    : m_key(that.m_key), m_value(that.m_value)
  {
  }

  ~DeepMapEntry() { } 

  DeepMapEntry &operator = (const DeepMapEntry &that)
  {
    m_key = that.m_key;
    m_value = that.m_value;

    return *this;
  }

  K m_key;
  V m_value;
};

template<class K, class V>
class DeepMap
{
  typedef DeepMapEntry<K, V> EntryType;

public: 

  // Construct an empty map that uses the argument hash function.
  DeepMap(uint32 (*HashFunction)(const K &key))
    : m_numBins(17), m_size(0), m_HashFunction(HashFunction)
  {
    _initialize();
  }

  // Construct an empty map that uses the argument hash function and
  // number of bins.
  DeepMap(uint32 (*HashFunction)(const K &key), uint32 numBins)
    : m_numBins(numBins), m_size(0), m_HashFunction(HashFunction)
  {
    _initialize();
  }

  // Copy constructor. All keys and values are copied.
  DeepMap(const DeepMap<K, V> &that)
    : m_numBins(that.m_numBins), m_size(that.m_size), 
      m_HashFunction(that.m_HashFunction)
  {
    _initialize();

    for (uint32 bin = 0; bin < m_numBins; bin++)
    {
      (*(m_table[bin])) += (*(that.m_table[bin]));        
    }       
  }

  // Destructor. All keys and values are released.
  ~DeepMap()
  {
    _deallocate();
  }

  // Assignment operator. All existing keys and values are released.
  // The keys and values in the argument map are copied.
  DeepMap &operator = (const DeepMap &that)
  {
    _deallocate();

    m_HashFunction = that.m_HashFunction;
    m_numBins = that.m_numBins;
    m_size = that.m_size;

    _initialize();

    for (uint32 bin = 0; bin < m_numBins; bin++)
    {
      (*(m_table[bin])) += (*(that.m_table[bin]));        
    }       

    return *this;
  }

  // Return the number of mappings.
  int32 size() const { return m_size; }

  // Return true if there exists a key k in the map such that (k == key).
  bool containsKey(const K &key) const { return get(key, NULL); }

  // Return true if there exists a key k in the map such that (k == key).
  // If there is, *pValue will point to a copy of the corresponding
  // value.
  // Otherwise, return false.
  bool get(const K &key, V *pValue = NULL) const
  {
    if (pValue != NULL)
    {
      *pValue = NULL;
    }

    uint32 hashCode = (m_HashFunction)(key);

    uint32 bin = hashCode % m_numBins;

    DeepList< EntryType > *entryList = m_table[bin];

    // Look for an equivalent key in the list of mappings. If one exists, 
    // we are done.
    for (DeepListIterator< EntryType > it(*entryList); 
         it.hasMore(); it.advance())
    {
      EntryType &entry = it.element();

      if (key == entry.m_key)
      {
        if (pValue != NULL)
        {
          // Make a copy of the value.
          *pValue = entry.m_value;
        }
        return true;
      }
    }

    return false;
  }

  // Associate a copy of value to a copy of key in the map.
  void put(const K &key, const V &value)
  {
    uint32 hashCode = (m_HashFunction)(key);

    uint32 bin = hashCode % m_numBins;

    DeepList< EntryType > *entryList = m_table[bin];

    // Look for an equivalent key in the list of mappings. If one exists, 
    // replace it.
    for (DeepListIterator< EntryType > it(*entryList); 
         it.hasMore(); it.advance())
    {
      EntryType &entry = it.element();

      if (key == entry.m_key)
      {
        entry.m_value = value;
        return;
      }
    }

    // Does not exist in list. Add a new entry to the end of the list.
    EntryType entryToAdd(key, value);
    entryList->append(entryToAdd);
    m_size++;
  }

  // Release all mappings.
  void clear()
  {
    for (uint32 bin = 0; bin < m_numBins; bin++)
    {
      m_table[bin]->clear();       
    }

    m_size = 0;
  }

private:

  void _initialize()
  {
    m_table = QTV_New_Array(DeepList< EntryType > *, m_numBins);

    for (uint32 bin = 0; bin < m_numBins; bin++)
    {
      m_table[bin] = QTV_New_Args(DeepList< EntryType >, ());
    }
  }

  void _deallocate()
  {
    for (uint32 bin = 0; bin < m_numBins; bin++)
    {
      QTV_Delete(m_table[bin]);       
      m_table[bin] = NULL;
    }    

    if(m_table != NULL)
    {
      QTV_Delete_Array(m_table);
      m_table = NULL;
    }

    m_size = 0;
  }

  uint32 m_numBins;
  int32 m_size;

  uint32 (*m_HashFunction)(const K &key);

  DeepList< EntryType > **m_table;
};

#endif /* DEEPMAP_H */
