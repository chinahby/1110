# ------------------------------------------------------------------------
# File: FormatT32F3Trace.pl
# Desc: Parses the trace files into a format similar to that seen on QXDM
#
# Usage: perl FormatT32F3Trace.pl > <Output filename>
#
#
# Copyright (c) 2005 QUALCOMM Inc. All Rights Reserved.
# ------------------------------------------------------------------------

#============================================================================
#
#                        EDIT HISTORY FOR MODULE
#
# $Header: //depot/asic/qsc1100/build/ms/FormatT32F3Trace.pl#1 $
#
# when       who     what, where, why
# -------    ---     --------------------------------------------------------
# 12/01/05   ty      Pass in filename as argument.  Fill in zeros within
#                    timestamp
# 11/08/05   eav     Put zeros in the timestamp
# 06/??/04   eav     Initial file
# 
#============================================================================
if ($#ARGV < 0)
{
  `cat trace*.txt > output.txt`;
  open(DAT, "output.txt") || die("Could not open file!");
}
else
{
  open(DAT, $ARGV[0]) || die("Could not open $ARGV[0]");
}

my @args = ();
@raw_data=<DAT>;

for ($i = 0; $i < scalar @raw_data; $i++)
{
 $message = $raw_data[$i];
 chop($message);

 if ($message =~ /\|(.*?)\|(.*?)\|.+\"(.*?)\".+\"(.*?)\"/g)
   {
     $timestamp = $1;

     while ($raw_data[$i+1] =~ /#+ *(\d{1,})/)
     {
       push(@args, $1);
       $i++;
     }

     # insert the arguments into the format string
     $format_string = $3;
     while (scalar @args > 0)
     { 
       $arg = shift(@args);

       if ($format_string =~ /(\%[0123456789udlx]{1,})/i)
       {
         $formatted_arg = sprintf($1, $arg);
         $format_string =~ s/\%[0123456789udlx]{1,}/$formatted_arg/i;
       }
     }

     $line_num = $2;
     $file_name = $4;
     $timestamp =~ s/ /0/g;

     printf("%s\t%s\t%s\t%s\n",$timestamp,$line_num,$file_name,$format_string);
   }
 elsif ( $message =~ /\|(.*?)\|/g )
   {
      printf("** ERROR T32 msg cut off **");
      printf("\t %s", $message);
      while ($raw_data[$i+1] =~ /#+ *(\d{1,})/)
      {
        push(@args, $1);
        $i++;
      }

      printf(" | args: ");
      while (scalar @args > 0)
      { 
        $arg = shift(@args);
        printf("$arg, ");
      }
      printf("\n");
   }
}

close(DAT);

`rm output.txt`
