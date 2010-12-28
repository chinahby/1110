# This script is used to format an applications processor image in hexidecimal
# format.  It generates a file containing the applications processor boot
# loader image in a "C" data structure (array).  The generated "C" file can
# then be linked with the OEM SBL to include the applications image within
# the OEM SBL.

open (FILEHANDLE, "+< $ARGV[0]") or die "Cannot open input file.";

printf("/*===========================================================================\n\n");
printf("GENERAL DESCRIPTION:\n");
printf("  This file was auto-generated from the aARM boot loader image for\n");
printf("  inclusion in the OEM SBL.\n\n");
printf("Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.\n");
printf("===========================================================================*/\n\n");  

printf( "const unsigned char apps_bootloader_image[] =\n{\n" );
    
while( <FILEHANDLE> )
{
  s{(..)(\s)}{0x$1,$2}g;
  
  print $_;
}

printf("};\n\n");

printf( "const unsigned long sizeof_apps_bootloader_image =\n            sizeof( apps_bootloader_image );\n" );
    
close FILEHANDLE;

