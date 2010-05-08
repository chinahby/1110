//FAT Filesystem Seek Cache written by Tan Shuai<tanshuai@vimicro.com>

#include <stdio.h>

#include "thin_usr.h"
#include "util.h"

#define LENGTH(array) (sizeof(array) / sizeof(array[0]))
#define MIN_INTERVAL 0xA00000 //10MBytes

#if F_SEEKCACHE

static void _f_slotcpy(SLOT *src, SLOT *dest)
{
	dest->v = src->v;
	dest->currcluster = src->currcluster;
	dest->prevcluster = src->prevcluster;
}

void _f_setcache(SLOT *insert_slot, SLOT slots[], unsigned int length)
{
	unsigned int i, oldValueLo, oldValueHi, query, hi, lo, newValue;
	unsigned char lh = 0;
	hi = lo = newValue = 0;
	oldValueLo = oldValueHi = query = insert_slot->v;

	for(i=0; i<length; i++)
	{		
		//MMDPrintf("%d...\n",i);

		if(slots[i].v == 0)
		{
			lo = hi = i;
			break;
		}

		if(query == slots[i].v)
		{
			//MMDPrintf("overlap!\n");
			return;
		}
		if(query > slots[i].v)
		{ 
			//query value need larger than others
			newValue = query - slots[i].v;
			//MMDPrintf("LO %d = %d - %d\n", newValue, query, slots[i].v);
			if(newValue <= oldValueLo) //matched the near lower
			{
				oldValueLo = newValue;
				lo = i;
			}		
		}
		else
		{
			newValue = slots[i].v - query;
			//MMDPrintf("HI %d = %d - %d\n", newValue, query, slots[i].v);
			if(newValue <= oldValueHi) //matched the near higher
			{
				oldValueHi = newValue;
				hi = i;
			}		
		}
	}

	// MMDPrintf("HI is %d, LO is %d\n", slots[hi].v, slots[lo].v);

	if((slots[hi].v - query) < MIN_INTERVAL) 
	{
		lh = 1;
	}
	if(lh) 
	{
		_f_slotcpy(insert_slot, &slots[hi]);
		// MMDPrintf("Higher Selected\n");
	}
	else 
	{
		_f_slotcpy(insert_slot, &slots[lo]);
		// MMDPrintf("Lower Selected\n");
	}	

}

//Get proximate 
int _f_getcache(SLOT *query_slot, SLOT slots[], unsigned int length)
{
	unsigned int i, mismatch = 0, newValue = 0, oldValue = query_slot->v, query = query_slot->v;

	for(i=0; i<length; i++)
	{		
		//MMDPrintf("%d...\n",i);
		if(slots[i].v == 0)
		{
			mismatch++;
			continue;
		}

		if(query >= slots[i].v)
		{ //query value need larger than others
			newValue = query - slots[i].v;
			//MMDPrintf("%d = %d - %d\n", newValue, query, slots[i].v);
			if(newValue <= oldValue) //matched
			{
				oldValue = newValue;
				_f_slotcpy(&slots[i], query_slot);

			}
		}
		else
		{
			mismatch++;
		}
	}

	if(mismatch == i) //never matched
	{
		//	MMDPrintf("Mismatchs: %d\n", query_slot->v);
		//	query_slot->v = 0;
		return 1;
	}
	
	return 0;
}

void _f_initcache(SLOT slots[], int length)
{
	int i;
	for(i=0; i<length; i++)
	{
		slots[i].v = 0;
		slots[i].currcluster = 0;
		slots[i].prevcluster = 0;
	}
}

#endif