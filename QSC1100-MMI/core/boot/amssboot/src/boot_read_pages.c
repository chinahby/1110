/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          4K COPY


GENERAL DESCRIPTION
  Given an address and size this function copies the specified size containing
  that address.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/04   ktr     Created

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "boot_read_pages.h"
#include "flash.h"
#include "boot_bad_block_list.h"
#include "mibbtb.h"
#include "progressive_boot.h"


LOCAL  unsigned char page_buf[512];

/*===========================================================================
                           FUNCTIONS
===========================================================================*/


/*===========================================================================

FUNCTION map_addr_copypages

DESCRIPTION
  Given an address in RAM,NAND offset and number of full pages, and partial pages
  to copy, this function maps the logical NAND offset to Physical page and copies
  the required pages.

RETURN VALUE
  If success, return TRUE else FALSE

===========================================================================*/

int map_addr_copypages
(
	uint32 nand_offset,
	uint32 ram_offset,
	uint32 num_full_pages_2copy_frm_nand,
	uint32 size_half_page_2copy_frm_nand
)
{
	uint32 logical_page = 0;
	uint32 physical_page = 0;
	uint32 logical_block = 0;
	uint32 pages_left_incurr_block = 0;
	uint32 num_bad_blocks_before_curr_logicalpage = 0;
	uint32 i = 0;

  /* Find the logical block and page number from offset */
  logical_page = partition_start_page + (nand_offset >> page_size_shift_bits);
  
  //logical_block = (LOGICAL_DATA_SEGMENT_BLOCK + ((nand_offset + (eu_size - 1) >> block_size_shift_bits)));
  logical_block = (logical_page >> pages_in_block_shift_bits);

  /* Use the bad block List to find number of bad blocks before these logical blocks */
  /* BUT, we want to ignore bad blocks before the partition start page */
  /* (so we start counting at bb_start_index) */
   
  for(i=bb_start_index;i<(ptr_bbtb->num_entries);i++)
  {
    if(ptr_bbtb->bad_block[i] <= logical_block)
    {
      logical_block++;
      num_bad_blocks_before_curr_logicalpage++;
    }
    else{
      break;
    }
  }

	 /* calculate the exact physical page */
	 physical_page = (logical_page + (num_bad_blocks_before_curr_logicalpage * pages_in_block));

	 /* Find the pages left in current block */
	 pages_left_incurr_block  = (pages_in_block - (physical_page & (pages_in_block - 1)));


	   /* If there are enough pages left in this block we can copy from this block and we are done */
	   if(pages_left_incurr_block >= num_full_pages_2copy_frm_nand)
	   {
	   		/* copy the pages here */
	   		for(i=0;i<num_full_pages_2copy_frm_nand;i++)
	   		{

	   		     /* Read back next page in image data*/
	   		     if ((nand_device->read_page( nand_device, physical_page, (void *)ram_offset ))!= FS_DEVICE_DONE)
	   		     {
	   		       return FALSE;
	   		     }

				  ram_offset += page_size;
				  physical_page++;

	 		}

	 		if(size_half_page_2copy_frm_nand)
			{
				/* now find the number of pages left in block */
	 			pages_left_incurr_block -= num_full_pages_2copy_frm_nand;

				/* If there are pages left in block copy the partial page too */
	 			if(pages_left_incurr_block)
	 			{
                    /* Read back next page in image data*/
					if ((nand_device->read_page( nand_device, physical_page, (void *)page_buf))!= FS_DEVICE_DONE)
					{
					  return FALSE;
					}
               	}
				else
				{
              		/* Find the next good block so that u can read the pages from */
					while (nand_device->bad_block_check(nand_device,(physical_page & (pages_in_block - 1))) == FS_DEVICE_BAD_BLOCK)
					{
					       physical_page += pages_in_block;
					}

					 /* we found the good block so read the page */
					if ((nand_device->read_page( nand_device, physical_page, (void *)page_buf))!= FS_DEVICE_DONE)
					{
					  return FALSE;
					}
             	}

				/* We copied the page to a buffer, now write it to RAM */
				memcpy((void *)ram_offset,(void *)page_buf,size_half_page_2copy_frm_nand);

			}

	   } //if ends
	   else{

		   /* copy as many pages as u can here */
		   for(i=0;i<pages_left_incurr_block;i++)
		   {

		        /* Read back next page in image data*/
		        if ((nand_device->read_page( nand_device, physical_page, (void *)ram_offset))!= FS_DEVICE_DONE)
		        {
		          return FALSE;
		        }

			    ram_offset += page_size;
			    physical_page++;

		   }

		   /* Find the next good block so that u can read the pages from */
		   while (nand_device->bad_block_check(nand_device,(physical_page & (pages_in_block - 1))) == FS_DEVICE_BAD_BLOCK)
		   {
		        physical_page += pages_in_block;
		   }


		   /* We found a good block so copy remaining pages from it */
		   for(;i<num_full_pages_2copy_frm_nand;i++)
		   {
		     	/* Read next page */
			 	if ((nand_device->read_page( nand_device, physical_page, (void *)ram_offset))!= FS_DEVICE_DONE)
			 	{
			 	  return FALSE;
			 	}
			    ram_offset += page_size;
			    physical_page++;

		   }

		   /* since we already found the good block and the request comes to this with a max of 8 pages
		      we dont need to check if there are enough pages in the block */
		   if(size_half_page_2copy_frm_nand)
		   {
			   /* Read back next page in image data*/
			   if ((nand_device->read_page( nand_device, physical_page, (void *)page_buf))!= FS_DEVICE_DONE)
			   {
			     return FALSE;
			   }

			   /* We copied the page to a buffer, now write it to RAM */
			   memcpy((void *)ram_offset,(void *)page_buf,size_half_page_2copy_frm_nand);

		   }

	   }

	   return TRUE;

}


/*===========================================================================

FUNCTION boot_read_partial_pages

DESCRIPTION
  Reads all paged segments from BIB. If the start address of segment in ram is not 4k aligned
  copies the segment from start to the next 4k boundary.If the end of the segment
  is not 4k aligned copies the segment from last 4k boundary in segment to end.

RETURN VALUE
  If success, return TRUE else FALSE

===========================================================================*/

int boot_read_partial_pages(void)
{

  int i = 0;
  uint32 curr_segment_vaddr_minus_addr4k_in_paged_segment = 0;
  uint32 curr_segment_end_address = 0;
  uint32 addr4k_in_paged_segment = 0;
  struct segment_record *curr_segment;
  uint32 num_full_pages = 0;
  uint32 size_half_page = 0;
  uint32 size_2be_copied = 0;
  uint32 ram_virtual_address = 0;
  uint32 nand_offset_frm_data_section = 0;


  for(i=paged_start_index; i < (prog_hdr->numsegments);i++)
  {
    /* get a pointer to the segment */
    curr_segment = &(prog_hdr->segment[i]);
    
    /* process it only if its size is greater than zero */
    if(	curr_segment->size > 0 )
    {
      
      /* We only care about the paged segment */
      if((curr_segment->type) == MI_PBT_PAGED_SEGMENT)
      {
        /* Check if the segment starting address is 4k aligned */
        if((curr_segment->v_address) & SIZE_4K_MINUS_1)
        {
          
          /* check if the size of the segment is less than or equal 4k */
          /* we have already checked the start address is not 4k aligned */
          /* if it is copy the entire segment because we will copy it any way */
          /* This will avoid checking once for start and once for end */
          if((curr_segment->size) <= SIZE4K)
          {
            num_full_pages =  (curr_segment->size >> page_size_shift_bits);
            size_half_page =  (curr_segment->size & (SIZE_PAGE_MINUS_1));
            
            /* Map address and copy the pages here */
            if(map_addr_copypages(curr_segment->segment_offset,curr_segment->v_address,num_full_pages,size_half_page) == FALSE)
            {
              return FALSE;
            }
            continue;
            
          }
          
          
          /* calculate the next 4k boundary */
          addr4k_in_paged_segment = 	((curr_segment->v_address + SIZE_4K_MINUS_1) & (~(SIZE_4K_MINUS_1)));
          
          /* check if the end of the segment is greater than the 4k boundary */
          size_2be_copied = (addr4k_in_paged_segment -  curr_segment->v_address);
          num_full_pages =  (size_2be_copied >> page_size_shift_bits);
          size_half_page =  (size_2be_copied & (SIZE_PAGE_MINUS_1));
          
          /* Map address and copy the pages here */
          if(map_addr_copypages(curr_segment->segment_offset,curr_segment->v_address,num_full_pages,size_half_page) == FALSE)
          {
            return FALSE;
          }
          
          
        }
        
        curr_segment_end_address = (curr_segment->v_address + curr_segment->size);
        /* Check if the segment ending address is 4k aligned */
        if(curr_segment_end_address & SIZE_4K_MINUS_1)
        {
          
          /* calculate the next 4k boundary */
          addr4k_in_paged_segment = 	(curr_segment_end_address & (~(SIZE_4K_MINUS_1)));
          
          
				  /* If 4k address in RAM doesnt correspond to start of a page in NAND
			       then copy entire page from NAND that contains the 4k address in RAM
             Adjust the ramoffset and nand offset accordingly */
          curr_segment_vaddr_minus_addr4k_in_paged_segment = (addr4k_in_paged_segment - (curr_segment->v_address));
          nand_offset_frm_data_section = ((curr_segment->segment_offset)	 +  (curr_segment_vaddr_minus_addr4k_in_paged_segment  & (~(SIZE_PAGE_MINUS_1))));
          ram_virtual_address = (addr4k_in_paged_segment - (curr_segment_vaddr_minus_addr4k_in_paged_segment  & (SIZE_PAGE_MINUS_1)));
          
          size_2be_copied = (curr_segment_end_address - ram_virtual_address);
          num_full_pages =  (size_2be_copied >> page_size_shift_bits);
          size_half_page =  (size_2be_copied & (SIZE_PAGE_MINUS_1));
          
          /* Map address and copy the pages here */
          if(map_addr_copypages(nand_offset_frm_data_section,ram_virtual_address,num_full_pages,size_half_page) == FALSE)
          {
            return FALSE;
          }
          
        }
        
      }
      
    }
  }
  
  return TRUE;
  
}


/*===========================================================================

FUNCTION boot_read_pages

DESCRIPTION
  Given a virtual address at which page fault occured this function identifies
  the SIZE(4k) block in ram containing the address, and copies it from NAND to ram.

RETURN VALUE
  If success, return TRUE else FALSE

===========================================================================*/

int boot_read_pages
(
	uint32 virtual_address,
	uint32 size
)
{

  int i = 0;
  uint32 nand_offset = 0;
  uint32 logical_page = 0;
  uint32 physical_page = 0;
  uint32 logical_block = 0;
  uint32 num_bad_blocks_before_curr_logicalpage = 0;
  uint32 partial_page_size_2copy_start = 0;
  uint32 partial_page_size_2copy_end = 0;
  uint32 offset_frm_vaddress = 0;
  uint32 num_pages_2be_read_frm_nand = NUM_PAGES_IN4K;
  uint32 pages_left_incurr_block  = 0;

      /* Right now the interface supports specifying size too
         But Coding is done assuming the size to be 4K */


      /* Round it to a 4k address */
      virtual_address = (virtual_address & (~(SIZE_4K_MINUS_1)));

      /* Find the segment that contains this address */
      for(i=paged_start_index; i < prog_hdr->numsegments;i++)
      {
		  if((virtual_address >= (prog_hdr->segment[i]).v_address) & (virtual_address < ((prog_hdr->segment[i]).v_address + (prog_hdr->segment[i]).size )))
		  {
            break;
		  }
      }

      /* offset is the sum of NAND offset and difference between 4k aligned address and segment v_address */
      /* This is the offset to logical page we want to copy from nand */
      offset_frm_vaddress = (virtual_address - (prog_hdr->segment[i]).v_address);
      nand_offset = ((prog_hdr->segment[i]).segment_offset) +  offset_frm_vaddress;

      /* Check if the 4k boundary in RAM is middle of page in NAND */
      /* You have to copy 8 pages any way, check if you have to read 9 pages
         truncate 1st and 9th page and copy it */
      partial_page_size_2copy_end = (offset_frm_vaddress & (page_size - 1));
      if(partial_page_size_2copy_end)
      {
		  partial_page_size_2copy_start = (page_size - partial_page_size_2copy_end);
		  num_pages_2be_read_frm_nand++;

	  }

      /* Logical to Physical mapping */
      logical_page = (partition_start_page + (nand_offset >> page_size_shift_bits));
      logical_block = (logical_page >> pages_in_block_shift_bits);

      /* Use the bad block List to find number of bad blocks before these logical blocks */
      /* BUT, we want to ignore bad blocks before the partition start page */
      /* (so we start counting at bb_start_index) */      
      for(i=bb_start_index;i<(ptr_bbtb->num_entries);i++)
      {
         if(ptr_bbtb->bad_block[i] < logical_block)
         {
			 logical_block++;
			 num_bad_blocks_before_curr_logicalpage++;
		 }
		 else{
			 break;
		 }
	  }

      /* calculate the exact physical page */
      physical_page = (logical_page + (num_bad_blocks_before_curr_logicalpage * pages_in_block));

      /* Find the pages left in current block */
      /* pages_left_incurr_block cannot be 0 there should be atleast one page left in the block */
      pages_left_incurr_block  = (pages_in_block - (physical_page & (pages_in_block - 1)));

      /* If there are 4k+1 pages left in this block we can copy from this block and we are done */
      if(pages_left_incurr_block >= num_pages_2be_read_frm_nand)
      {

          if(partial_page_size_2copy_start)
          {
              /* Read back next page in image data*/
			   if ((nand_device->read_page( nand_device, physical_page, (void *)page_buf))!= FS_DEVICE_DONE)
			   {
				  return FALSE;
			   }

               /* copy only the partial page to 4k address */
			   memcpy((void *)virtual_address,((void *)((char *)page_buf + partial_page_size_2copy_end)),partial_page_size_2copy_start);


			   virtual_address += partial_page_size_2copy_start;
			   physical_page++;

			   /* Yes read only 7 full pages in loop and finally read last partial page */
			   num_pages_2be_read_frm_nand = num_pages_2be_read_frm_nand - 2;

		  }

		  for(i=0;i<num_pages_2be_read_frm_nand;i++)
		  {
               /* Read back next page in image data*/
			   if ((nand_device->read_page( nand_device, physical_page, (void *)virtual_address ))!= FS_DEVICE_DONE)
			   {
				  return FALSE;
			   }

				virtual_address += page_size;
			    physical_page++;
		  }

		  if(partial_page_size_2copy_end)
		  {
		      /* Read back next page in image data*/
		     if ((nand_device->read_page( nand_device, physical_page, (void *)page_buf))!= FS_DEVICE_DONE)
		     {
		  	  return FALSE;
		     }

		       /* copy only the partial page to 4k address */
		     memcpy((void *)virtual_address,(void *)page_buf,partial_page_size_2copy_end);

           }
   	  }
	  else{

		   /* pages_left_incurr_block can never be zero , it will be atleast one */
           if(partial_page_size_2copy_start)
		   {
		       /* Read back next page in image data*/
			   if ((nand_device->read_page( nand_device, physical_page, (void *)page_buf))!= FS_DEVICE_DONE)
			   {
				  return FALSE;
			   }

		        /* copy only the partial page to 4k address */
			   memcpy((void *)virtual_address,((void *)((char *)page_buf + partial_page_size_2copy_end)),partial_page_size_2copy_start);


			   virtual_address += partial_page_size_2copy_start;
			   physical_page++;

			   /* Yes read only 7 full pages in loop and finally read last partial page */
			   num_pages_2be_read_frm_nand = num_pages_2be_read_frm_nand - 2;

			   /* we have already copied one page */
			   pages_left_incurr_block--;

		   }


         /* we have have to copy as many pages as we can and search for next good block */
         for(i=0;i<pages_left_incurr_block;i++)
		 {
               /* Read next page */
			   if ((nand_device->read_page( nand_device, physical_page, (void *)virtual_address ))!= FS_DEVICE_DONE)
			   {
				  return FALSE;
			   }
			   virtual_address += page_size;
			   physical_page++;

		 }

		 /* Block ended so find the next good block */
         while (nand_device->bad_block_check(nand_device,(physical_page & (pages_in_block - 1))) == FS_DEVICE_BAD_BLOCK)
         {
               physical_page += pages_in_block;
    	 }

    	 /* We found a good block so copy remaining pages from it */
    	 for(;i<num_pages_2be_read_frm_nand;i++)
    	 {
		     /* Read next page */
			 if ((nand_device->read_page( nand_device, physical_page, (void *)virtual_address ))!= FS_DEVICE_DONE)
			 {
			   return FALSE;
			 }
			 virtual_address += page_size;
			 physical_page++;
		 }

		 if(partial_page_size_2copy_end)
		 {
		     /* Read back next page in image data*/
		    if ((nand_device->read_page( nand_device, physical_page, (void *)page_buf))!= FS_DEVICE_DONE)
		    {
		 	  return FALSE;
		    }

		      /* copy only the partial page to 4k address */
		    memcpy((void *)virtual_address,(void *)page_buf,partial_page_size_2copy_end);

		  }


	  }

	  return TRUE;

  } // boot_read_pages








