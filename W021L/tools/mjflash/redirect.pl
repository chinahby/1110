#===================================================================
# FILE: redirect.pl
#===================================================================
open (FILEHANDLE, "> $ARGV[0]");
foreach $i (1..$#ARGV)
{
  $_=$ARGV[$i];
  s#\/#\\#g;
  print FILEHANDLE "$_ ";
}
close FILEHANDLE;

