#===================================================================
# FILE: redirect.pl
#===================================================================
use POSIX qw(uname);

my $BuildPlatform = "WIN32";

my ($uname_s, $uname_r)  = (POSIX::uname())[0,2];

if ($uname_s eq "Linux")
{
   $BuildPlatform = "LINUX";
}
 
open (FILEHANDLE, "> $ARGV[0]");
foreach $i (1..$#ARGV)
{
  $_=$ARGV[$i];

  if( $BuildPlatform eq "WIN32" )
  {
    s#\/#\\#g;
  }

  print FILEHANDLE "$_ ";
}
close FILEHANDLE;

