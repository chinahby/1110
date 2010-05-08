#============================================================================
#  Name:
#    jnand_ext_error.pl
#
#  Description:
#    Perl script to process extended error code header file into a CMM
#    script that will print out extended errors in human readable form
#
#  Execution:									  
#    To execute script execute the command line 
#        "cat jnand_ext_error.h | perl jnand_ext_error.pl".
#
#  Copyright (c) 2004 by QUALCOMM, Incorporated.  
#  All Rights Reserved.
#----------------------------------------------------------------------------

#============================================================================
#
#                          EDIT HISTORY FOR MODULE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
#  $Header: //depot/asic/msmshared/tools/jflash/JFLASH.03.00/jflash_ext_error.pl#1 $ $DateTime: 2007/10/01 18:50:43 $ $Author: opham $
#  
# when       who     what, where, why
# --------   ---     --------------------------------------------------------
# 09/26/04   drh     Created.
# 
#============================================================================

# Print a one time block at the top of the file
  
printf "local &ExtError\n";
printf "\n";
printf ";==========================================\n";
printf "; Get the arguments passed in.\n";
printf ";==========================================\n";
printf "ENTRY &ExtError\n";
printf "\n";
printf "\n";
printf "\n";


printf "print \"  \"\n";
printf "print \"  \"\n";
printf "print \"Extended error is &ExtError\"\n";
printf "\n";
printf "\n";
printf "\n";


# For each error code in the enum definition, create a function
while (<>) {
      if (/^\s*(ERR_\S*)=(0[xX][abcdefABCDEF01234567890]*),\s*\/\*(.*)\*\//) {
         # printf "%s    %s     %s\n", $1, $2, $3;
         printf "if &ExtError==$2\n";
         printf "(\n";
         printf "  print \"Error name is $1\"\n";
         printf "  print \"     $3\"\n";
         printf "  enddo\n";
         printf ")\n";
         printf "\n";
      }
}


# Print a one time line at the end

printf "\n\nenddo\n\n";

