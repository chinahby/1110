#!/usr/bin/perl
# ------------------------------------------------------------------------
# Generate feature documentation in HTML
# Copyright (c) 2003 QUALCOMM Inc. All Rights Reserved.
# $Header: //depot/asic/qsc1100/tools/build/featlist.pl#1 $
# 
# - Extracts any feature token from a #if* statement
# - Extracts #define tokens but not the right hand values
# - Write feature comments into HTML output
# - Write list of features and which file the feature is referenced
# - Write the list of files and which features are in each file
# ------------------------------------------------------------------------
use BuildTools;
use File::Find;
use Getopt::Long;
use File::Basename;

GetOptions( "d|directory:s","a|byfile","b|byfeature","f|features","c|comments","h|html","s|sort","g|groupfile:s","i|image:s" );

my %groupInfoHash;
# ------------------------------------------------------------------------
#
# ------------------------------------------------------------------------

if( !$opt_d )
{
   Usage();
   die "Must specify a directory\n";
}

my $StartDir = $opt_d;

my $CopyRight =  "Copyright (c) 2003 QUALCOMM Inc.\n";

# ------------------------------------------------------------------------
# SUBROUTINE: Usage 
#
# ------------------------------------------------------------------------
sub Usage 
{
   print <<usage;
   $0 - Copyright (c) 2003 QUALCOMM Inc.
      % perl $0 -d <dir>
      % perl $0 -d <dir> -a      - List features by file
      % perl $0 -d <dir> -b      - List features by feature (then file)
      % perl $0 -d <dir> -f      - List features names
      % perl $0 -d <dir> -f -c   - List features and associated comments
                                     FEATURE<left tick>comment
      % perl $0 -d <dir> -f -h   - List features and associated comments
                                     HTML format
      % perl $0 -d <dir> -f -h -s - List features and associated comments
                                     Sorted HTML format
usage
   return;
} # END Usage

# ------------------------------------------------------------------------
# Features
#    Hash of feature tokens, has the format:
#       Feature{feature}{name}
#                       @{locations} ==> {file}
#                                        {line}
#    This allows simple reporting for a feature of which file/line number
#    pairs reference the feature
#
# Files
#    Hash of files with features used in the file, has the format:
#       Files{filename}{name}
#                      {features}{feature}{name}
#                                         @{lines}
#    This allows simple reporting for a file of the features named in
#    that file and the line numbers where the feature is used.
# ------------------------------------------------------------------------
my %Features = ();
my %Files    = ();

# ------------------------------------------------------------------------
#
# ------------------------------------------------------------------------
my $DispLine = "-----------------------------------------------------------------\n";

# ------------------------------------------------------------------------
# SUBROUTINE: AddFeatures
#
# ------------------------------------------------------------------------
sub AddFeatures
{
   my( $features, $filename, $line, $usage, $comment ) = @_;

   # print $DispLine;
   # print "FILENAME: $filename\n";
   # print "LINE:     $line\n";
   # print "USAGE:    $usage\n";

   foreach $feature (@$features)
   {
     # do not output obvious header file inclusion guard defines
      if( $feature ne "" && $feature !~ /_H\s*$/) 
      {
         # print "ADD: $feature\n";
         my $loc  = {};
   
         if ( $Features{$feature}{comment} eq '' && $comment ne '')
         {
           $Features{$feature}{name}    = $feature;
           $Features{$feature}{comment} = $comment;
         }
         $loc            = {};
         $loc->{file}    = $filename;
         $loc->{line}    = $line;
         push @{ $Features{$feature}{locations} }, $loc;

         $Files{$filename}{name} = $filename;
         $Files{$filename}{features}{$feature}{name} = $feature;
         push @{ $Files{$filename}{features}{$feature}{lines} }, $line;
      }
   }
}

# ------------------------------------------------------------------------
# SUBROUTINE: ParseForFeatureTokens
#
# ------------------------------------------------------------------------
sub ParseForFeatureTokens
{
   local( $cur_line )    = $_[0];
   local( *FILE )        = $_[1];
   local( $line )        = $_[2];

   # Remove leading spaces
   #
   $cur_line =~ s/^\s+//g;

   my( @feature_tokens ) = ();
   my( $line_update )    = 0;
   my( $scan_line )      = $cur_line;

   # print $DispLine;
   # print "Parse for Tokens\n";

   # Multi-Line Statements
   #
   if( index( $cur_line, "\\" ) > -1 )
   {
      $scan_line = substr( $scan_line, 0, index( $cur_line, "\\" ) );

      # print "   LINE is continued .\n";
      while( <FILE> )
      {
         chomp;
         $line_update++; 

         if(  index( $_, "\\" ) > -1 )
         {
            # print "LINE is continued .. [$_]\n";
            $scan_line .= " " . substr( $_, 0, index( $_, "\\" ) );
         }
         else
         {
            # print "                     [$_]\n";
            $scan_line .= " $_";
            last;
         }
      }
      # print "   MULTI [$scan_line]\n";
   } # END if the line is continued

   $$line = $$line + $line_update;

   # Handle #defines
   #
   # if starts with #define, then drop after second space
   #
   if( index( $scan_line, "#define" ) > -1 )
   {
      my $define_token_1 = "";
      my $define_token_2 = "";
      ($define_token_1, $define_token_2) = split /\s+|\(/, $scan_line;
      # print "   DEFINE: $define_token_1\n";
      # print "   DEFINE: $define_token_2\n";
      $scan_line = $define_token_1 . " " . $define_token_2;
   }

   # Remove nufarious non feature token stuff
   #
   $scan_line =~ s/(defined)|(#define)|(#ifndef)|(#ifdef)|(#if)|(<)|(>)|(\()|(\)|(\|\|)|(\&\&)|(==)|(!=)|(>=)|(<=)|(!))/ /g;

   # print $DispLine;

   # Capture comments
   #   Scan for // or /*, for // drop all chars till end of line
   #   for /* drop until */ or end of line
   #
   if( index( $scan_line, "//" ) > -1 )
   {
      $scan_line = substr( $scan_line, 0, index( $scan_line, "//" ) );
      # print "INLINE [$scan_line]\n";
   }
   elsif( index( $scan_line, "/*" ) > -1 )
   {
      # !!! ASSUMPTION !!!
      #   Any preprocessor statement with a /* style comment obeys:
      #   1. There's no code after the /* till the end of the line
      #   2. There's no code related to the statement after a */
      #   3. The statement is synctactically correct to an ANSI C preprocessor
      #
      # print "MULTI [$scan_line]\n";     
      $scan_line = substr( $scan_line, 0, index( $scan_line, "/*" ) );
   }

   # Remove all extra spaces, add to feature_tokens
   #
   my $feature = "";
   @feature_tokens = split /\s+/, $scan_line;

   # print "   CURLINE  [$cur_line]\n";
   # print "   SCANLINE [$scan_line]\n";

   # foreach $feature (@feature_tokens)
   # {
   #   print "      [$feature]\n" if( $feature ne "" );
   # }

   # print $DispLine;
   return @feature_tokens;
   
} # END ParseForFeatureTokens

my @LastComment        = ();
my @LastCleanComment   = ();

# ------------------------------------------------------------------------
# SUBROUTINE: CleanComment 
#
# ------------------------------------------------------------------------
sub CleanComment 
{
    my( $line ) = @_;

    if( ( index( $line, "--------------------" ) > -1 ) ||
        ( index( $line, "====================" ) > -1 )    )
    {
        $line = "";
    }
    else
    {
        $line =~ s/(^\s*)//g;
        $line =~ s/(^\/\*)|(^\*\*)|(\*\/)|(^\/\/)//g;
        if( index( $line, "//" ) > -1 )
        {
           $line = substr( $line, index( $line, "//" ) + 2, length( $line ) - index( $line, "//" ) );
        }
        $line =~ s/(^\s*)//g;
    }
    return $line; 
}


# ------------------------------------------------------------------------
# SUBROUTINE: ParseForFeatures
#
# ------------------------------------------------------------------------
sub ParseForFeatures
{
   my( $filename ) = @_;
   # print "$filename\n";

   my @features = ();
   my $line     = 0;
   my $nest     = 0;
   my $neston   = 0;

   my $LastCleanedComment = "";

   local $CURFILE;

   open( CURFILE, "<$filename\n" ) or die "Couldn't open $filename\n";
   while( <CURFILE> )
   {
      chomp;
      $line++;
      

      # ------------------------------------------------------------------
      # Blank Lines
      # ------------------------------------------------------------------
      if( m/^[  ]*$/ )
      {
         $LastCleanedComment = "";
         next;
      }

      # ------------------------------------------------------------------
      # Comments
      # ------------------------------------------------------------------
      @LastComment = ();
      if( /\/\// )
      {
         $LastComment[ 0 ] = CleanComment( $_ );
      }
      elsif( index( $_, "/*" ) > -1 )
      {
         my $i        = 0;

         $LastComment[ $i++ ] = CleanComment( $_ );

         if( index( $_, "*/" ) > -1 )
         {
            if( $i != 1 )
            {
               $LastComment[ $i++ ] = CleanComment( $_ );
            }
         }
         else
         {
            while( <CURFILE> )
            {
               chomp;
               $LastComment[ $i++ ] = CleanComment( $_ );
               if( index( $_, "*/" ) > -1 )
               {
                  last;
               }
            }
         }
      }

      #if( $#LastComment > 0 )
      {
         my $j = 0;
         $LastCleanComment   = ();
         my $LCC = "";
         foreach $comment ( @LastComment )
         {
            if( $comment ne "" )
            {
               $LastCleanComment[ $j++ ] = $comment;
               # print "$comment\n";
               $LCC .= $comment . " ";
            }
         }
   
         if( $LCC ne "" )
         {
            $LastCleanedComment = $LCC;
            # print "LCC [$LastCleanedComment]\n";
         }
      }
 
      # ------------------------------------------------------------------
      # Statements 
      # ------------------------------------------------------------------
      @features = ();
     
      if( ( /#else/) or ( /#elif/) or ( /#endif/ ) )
      {
        $nest-- if ($neston == 1);
      }
      elsif( /#error/ )
      {
      }
      elsif( /#include/ )
      {
      }
      elsif( /#undef/ )
      {
      }
      elsif( /#line/ )
      {
      }
      elsif( ( /#if/ ) )
      {
         # These tell us if a feature is used to control conditional
         #   compilation - this is what we want to know
         # From these, we want to check if the feature is asserted or not
         #

         # @features = ParseForFeatureTokens( $_, *CURFILE, \$line );
         # AddFeatures( \@features, $filename, $line, "USE", $LastCleanedComment );
#         $LastCleanedComment = "";
         $nest++;
         $neston = 1;
      }
      elsif( /#define/ )
      {
         if ( $nest <= 0 && $neston == 0)
         {
           $neston = 0;
           $nest = 0;
           $LastCleanedComment = "";
         }
         # These turn features on or define macros or other manifest
         #   constants.  We ignore these.
         # print "Before adding features: [$LastCleanedComment]\n";
         # print "                  here: [$_]\n";
         # print "                  line: [$line]\n";
         @features = ParseForFeatureTokens( $_, *CURFILE, \$line );
         AddFeatures( \@features, $filename, $line, "DEFINE", $LastCleanedComment );
         $LastCleanedComment = "";
      }
      # print "LINE: $_\n";
   }
   close( CURFILE ); 
}

# ------------------------------------------------------------------------
# SUBROUTINE: 
#
# ------------------------------------------------------------------------
sub Wanted
{
   my( $name, $path, $suffix );

   if( ! -d $_ )
   {
      $suffix = "";
      ( $name, $path, $suffix ) = fileparse( $_, ".c", ".h", ".s" );

      if( $suffix ne "" )
      {
         # print "PARSE $name$suffix\n";
         ParseForFeatures( $_ );
      }
   }
}

sub HtmlHeader
{
   print "<head>\n";
   print "<title>AMSS Feature List</title>\n";
   print "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n";
   print "</head>\n";
   print "\n";
   print "<body>\n";
   print "<p align=\"center\"><strong>AMSS Feature List<a name=\"AMSS Feature List\"></a></strong></p>\n";
   print "<p align=\"center\"><strong>$CopyRight<a name=\"$CopyRight\"></a></strong></p>\n";
}

sub HtmlFooter
{
  print "</body>\n";
  print "</html>\n";
}

sub HtmlFeatureHeader
{
   local( $header )    = $_[0];
   my $description = $groupInfoHash{ lc($header) } || "NO DESCRIPTON AVAILABLE";

  print "<div align='left'>\n";
  print "<table bgcolor ='fff000' border=0 width='800'>\n";
  print "<tr><td align='center'><h2>$header</h2></td></tr>\n";
  print "<tr><td align='center'><h3>$description</h3></td></tr>\n";
  print "</h1>";
  print "</table>\n";
  print "</div>\n";
}
sub HtmlFeature
{
   local( $feature )    = $_[0];
   local( $comment )    = $_[1];

   print "<p align=\"left\"><b>$feature</b></p>\n";
   print "</div>\n";
   print "<blockquote>\n";
   print "<div align=\"left\">\n";
   print "<p>$comment</p>\n";
   print "</div>\n";
   print "</blockquote>\n";
   print "</div>\n";
}

# ------------------------------------------------------------------------
# MAIN SCRIPT
#
# ------------------------------------------------------------------------
# print "Parsing files...\n";
%groupInfoHash = AMSSBuildTools::ParseXMLgroupFile($opt_g);
find( \&Wanted, $StartDir );

my $feature;
my $locations;

if( $opt_a )
{
   print $DispLine;
   print "File/Line by Feature\n";
   print $DispLine;

   foreach $feature ( sort keys %Features )
   {
      print $DispLine;
      print "$feature\n";
      for $locations ( @{ $Features{$feature}{locations} } )
      {
         print "   $locations->{file}\t\t\t$locations->{line}\n";
      }
   }
   print "\n";
}

my $file;
my $line;

if( $opt_b )
{
   print $DispLine;
   print "Feature by File\n";
   print $DispLine;

   foreach $file ( sort keys %Files )
   {
      print $DispLine;
      print "$file\n";
      foreach $feature ( sort keys %{ $Files{$file}{features} } )
      {
         print "   $feature\n";
         for $line ( @{$Files{$file}{features}{$feature}{lines} } )
         {
            print "      $line\n"
         }
      }
   }
}

if( $opt_f && ! $opt_s )
{
   # print "List features...\n";
   AMSSBuildTools::HTML_FileHeader(STDOUT,"AMSS Feature List","AMSS Feature List",$opt_i) if ($opt_h);
   foreach $feature ( sort keys %Features )
   {
      if( $opt_h )
      {
         HtmlFeature($feature, $Features{$feature}{comment});
      }
      elsif( $opt_c )
      {
         print "$feature`$Features{$feature}{comment}\n";
      }
      else
      {
         print "$feature\n";
      }
   }
   HtmlFooter() if ($opt_h);
}


if( $opt_f && $opt_s )
{
   AMSSBuildTools::HTML_FileHeader(STDOUT,"AMSS Feature List","AMSS Feature List",$opt_i) if ($opt_h);
   #output index first

   my $count=1;
   print "<hr>\n";
   print "<center>\n";
   print "<table>\n";
   foreach $file ( sort keys %Files )
   {
      my ($header) = ($file =~ /cust(.*?)\.h/);
      my $description = $groupInfoHash{ lc($header) } 
                                                  || "NO DESCRIPTON AVAILABLE";
      if ( $count%2 )
      {
        #print "<tr bgcolor='#FAEBD7'>\n";
        #print "<tr bgcolor='#FFF000'>\n";
        print "<tr bgcolor='#FFFFCC'>\n";
      }
      else
      {
        #print "<tr bgcolor='#FAEBD7'>\n";
        print "<tr>\n";
      }
      print "  <td><a href='\#$count'>$header</a></td>\n";
      print "  <td>$description</td>\n";
      print "</tr>\n";
      $count++;
   }
   print "</table>\n";
   print "</center>\n";
   print "<hr>\n";
   $count=1;
   print "<table><tr><td>\n";
   foreach $file ( sort keys %Files )
   {
      $f = $file;
      $f =~ s/cust//;
      $f =~ s/\.h//;
      $f = uc $f;
      print "$f\n" if (!$opt_h);
      print "<a name='\#$count'>";
      HtmlFeatureHeader($f) if ($opt_h);
      print "</a>";
      $count++;
      foreach $feature ( sort keys %{ $Files{$file}{features} } )
      {
        if( $opt_h )
        {
           HtmlFeature($feature, $Features{$feature}{comment});
        }
        elsif( $opt_c )
        {
           print "$feature`$Features{$feature}{comment}\n";
        }
        else
        {
           print "$feature\n";
        }
      }
   }
   print "</td></tr></table>\n";
   print "</body></html>\n";
}

1;

