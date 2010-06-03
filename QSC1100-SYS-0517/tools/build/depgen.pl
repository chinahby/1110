#!/usr/bin/perl -w
# ------------------------------------------------------------------------
# AMSS dependency generator
# Copyright (c) 2003,2004 QUALCOMM Inc. All Rights Reserved.
# $Header: //depot/asic/qsc1100/tools/build/depgen.pl#1 $
#
# - Find's #include statements and generates dependency list
# - Checks for the existence of the file first 
#
# Does NOT handle this:
#    #include<string.h>    // no space between 'e' and '<'
#    #include voc_core.h   // this appears in a comment
#    #include “uim.h”      // doesn't understand delimiters other than <>"
#    Circular single refs  // e.g. srch_hw.h : srch_hw.h
#
# ------------------------------------------------------------------------
use POSIX qw(uname);

use File::Find;
use File::Basename;

# $| = 1;

# ------------------------------------------------------------------------
# BuildDir
#   User specified directory for targets
# ------------------------------------------------------------------------
my $BuildDir = "\$(TARGETDIR)";

my %IncludePaths = ();
my %ExcludePaths = ();
my %RecursiveExcludePaths = ();

my %Defines = ();

my $opt_f = "";
my $opt_d = "";

# ------------------------------------------------------------------------
# SuffixList
#   List of file types that will be searched during the directory
#   traversal.  Does NOT correspond to the files for which dependencies
#   will be calculated.  Should contain suffixes for all dependent types
# ------------------------------------------------------------------------
my   @SuffixList = ();
push @SuffixList, "\\.h";
push @SuffixList, "\\.c";
push @SuffixList, "\\.cpp";
push @SuffixList, "\\.s";
push @SuffixList, "\\.smf";
push @SuffixList, "\\.bar";
push @SuffixList, "\\.bid";
push @SuffixList, "\\.ipc";

push @SuffixList, "\\.oov";
push @SuffixList, "\\.smh";
push @SuffixList, "\\.txt";
push @SuffixList, "\\.rff";

my $CustBIDName = "";

# ------------------------------------------------------------------------
# Build Platform
#    "LINUX" when on Linux via uname call
#    "WIN32" otherwise
# ------------------------------------------------------------------------
my $BuildPlatform = "WIN32";

# ------------------------------------------------------------------------
# StartDir
#   From where the directory tree list creation will be started
# ------------------------------------------------------------------------
my $StartDir = "";

my $GlobalIncludeFile = "";

# ------------------------------------------------------------------------
# Include Dependencies
#    Hash of file name with files that are included by it
#       Depends{$normalized_filename}{$dependency}
#
#       Files{$file}{name}
#                   {path}
#                   {suffix}
#
#       FileRef{$normalized_filename} = filename
# ------------------------------------------------------------------------
my %Depends     = ();
my %Files       = ();
my %FileRef     = ();
my %Resolved    = ();

# ------------------------------------------------------------------------
# Patterns
# ------------------------------------------------------------------------
my $DispLine      = "-----------------------------------------------------------------\n";

my $IncPattern = '(\s*?\#{1}?\s*?include)(\s+)(\w*|\(.+?|\".+?|\<.+?)\s*(\/.*$|$)';

$opt_tr         = 0;
$opt_incpath    = 0;
my @CommandArgs = ();

# ------------------------------------------------------------------------
# SUBROUTINE: GetCommandLine
#
# ------------------------------------------------------------------------
sub GetCommandLine
{
   my ($uname_s, $uname_r)  = (POSIX::uname())[0,2];
 
   if ($uname_s eq "Linux")
   {
      $BuildPlatform = "LINUX";
   }

   my $i = 0;
   for( $i=0; $i<$#ARGV+1; $i++)
   {
      my $arg = $ARGV[$i];
      if( $arg =~ /^\@/ )
      {
         $arg =~ s/\@//g;
         open( RSPFILE, "<$arg" ) or die "Couldn't open command file $arg\n";
         while( <RSPFILE> )
         {
            chop;
            if( /^\#/ )
            {
               next;
            }
            @rsp_args = split / /;
            push @CommandArgs, @rsp_args;
         }
         close( RSPFILE );
      }
      else
      {
         push @CommandArgs, $arg;
      }
   }
   reverse @CommandArgs;
}

# ------------------------------------------------------------------------
# SUBROUTINE: ProcessCommandLine
#
# ------------------------------------------------------------------------
sub ProcessCommandLine
{
   my $i   = 0;
   my $arg = 0;
   for( $i=0; $i<$#CommandArgs+1; $i++)
   {
      my $arg = $CommandArgs[$i];

      # Help
      #
      if ( $arg eq "-h" )
      {
         Usage();
         die;
      }
   
      # Spec the start dir, recurse from there
      #
      elsif ( $arg eq "-d" )
      {
         $i++;
         $opt_d    = $CommandArgs[$i];
         $StartDir = $opt_d;
      }
   
      # Spec the name of the generated dependency file
      #
      elsif ( $arg eq "-f" )
      {
         $i++;
         $opt_f = $CommandArgs[$i];
      }
   
      # Spec the name of the build directory, that is, where
      #   objects, intermediate files, etc are written
      #
      elsif ( $arg eq "-b" )
      {
         $i++;
         $BuildDir = $CommandArgs[$i];
      }
     
      # Spec a build ID in the form ABCD
      #
      elsif ( $arg eq "-bid" )
      {
         $i++;
         $BuildID = $CommandArgs[$i];
      }
   
      # Spec additional include paths, paths that will not be
      #   generated by this tool but are known to the build
      #   environment as needed for the include file search
      #   by the compiler.  Same as the -I option to the compiler
      # 
      elsif ( $arg eq "-I" )
      {
         $i++;
         $IncludePaths{$CommandArgs[$i]} = $CommandArgs[$i];
      }
   
      # Spec exclude paths, paths that will not be considered
      #   in the search for dependencies, not recursive
      #
      elsif ( $arg eq "-X" )
      {
         $i++;
         $ExcludePaths{$CommandArgs[$i]} = $CommandArgs[$i];
      }
   
      # Spec exclude paths, paths that will not be considered
      #   in the search for dependencies, recursive
      #
      elsif ( $arg eq "-XS" )
      {
         $i++;
         $RecursiveExcludePaths{$CommandArgs[$i]} = $CommandArgs[$i];
      }
   
      # Spec defines that reference dependencies.  e.g. to handle this case"
      #   include FOO_H
      # user passed to this tool:
      #   -D FOO_H=<relative path>/<file>
      # e.g.:
      #   -DFOO_H=../../drivers/foo/foo.h
      #
      elsif ( $arg eq "-D" )
      {
         $i++;
         $define_arg = $CommandArgs[$i];
         $define_arg =~ s/\r//g;
         ($define,$value) = split /=/, $define_arg;
         chomp $value;
         if( ! -e $value )
         {
            print "$define=$value refers to non existant file $value\n";
         }
         $Defines{$define} = $value;
      }

      # Write a target build rule for the each target based on the suffix.
      # 
      elsif ( $arg eq "-tr" )
      {
         $opt_tr = 1;
      }

      # Write a file with the global include paths
      #
      elsif ( $arg eq "-ginc" )
      {
         $i++;
         $GlobalIncludeFile = $CommandArgs[$i];
      }

      # Write target specific include paths for each target
      # 
      elsif ( $arg eq "-incpath" )
      {
         $opt_incpath = 1;
      }

   } # END handle command line args

   if( $opt_d eq "" )
   {
      print "ERROR: Must specify a directory with the -d option\n";
      Usage();
      die;
   }
   
   if( $opt_f eq "" )
   {
      print "ERROR: Must specify an output file with the -f option\n";
      Usage();
      die;
   }
} # END ProcessCommandLine

# ------------------------------------------------------------------------
# SUBROUTINE: AddDependency
#
# ------------------------------------------------------------------------
sub AddDependency
{
   my( $target, $dep ) = @_;

   if ( ( $dep ne "" ) && ( $target ne "" ) )
   {
      $Depends{ lc( $target ) }{ lc( $dep ) } = lc( $dep );
   }

   return;
}


# ------------------------------------------------------------------------
# SUBROUTINE: ParseForIncludes
#
# ------------------------------------------------------------------------
sub ParseForIncludes
{
   my( $filename, $filepath ) = @_;

   local $CURFILE;

   my $inc_file = "";

   open( CURFILE, "<$filepath\/$filename" ) or die "Couldn't open target file $filepath\/$filename\n";
   while( <CURFILE> )
   {
      chomp;

      if( /^(#|\s*)*\#include/ )
      {
         /$IncPattern/;
 
         if( defined $3 )
         {
            $inc_file = $3;

            # include FOO_H
            #
            if( !($inc_file =~ /\"|\</) )
            {
               if( exists $Defines{$inc_file} )
               {
                  ( $name, $path, $suffix ) = fileparse( $Defines{$inc_file}, @SuffixList );

                  chop $path;
                  $full_name = "$name$suffix";

                  if( !exists $Files{ $full_name } )
                  {
                     $Files{ $full_name }{ 'name' }    = $full_name;
                     $Files{ $full_name }{ 'path'    } = $path;
                     $Files{ $full_name }{ 'suffix'  } = $suffix;
                     $FileRef{ lc( $full_name ) }      = $full_name;
                  }
                  AddDependency( $filename, $full_name );
               }
               else
               {
                  $inc_file = "\$($inc_file)";
                  if( !exists $Files{ $inc_file } )
                  {
                     $Files{ $inc_file }{ 'path'    } = "";
                     $Files{ $inc_file }{ 'suffix'  } = "makedep";
                     $Files{ $inc_file }{ 'name' } = $inc_file;
                     $FileRef{ lc( $inc_file ) }   = $inc_file;
                  }
                  $Resolved{ lc( $filename ) }{ lc( $inc_file ) } = $inc_file;
                  AddDependency( $filename, $inc_file );
               }  
            }
         
            # include "foo.h"
            # include <foo.h>
            #
            else
            {
               $inc_file =~ s/\"//g;
               $inc_file =~ s/\<|\>//g;

               if( $filename ne $inc_file )
               {
                  AddDependency( $filename, $inc_file );
               }
            } # if FOO_H, else "foo.h", <foo.h>
         } # if $3
      } # if include
   } # while <CURFILE>
   close( CURFILE ); 
   return;
} # ParseForIncludes


# ------------------------------------------------------------------------
# SUBROUTINE: Wanted
#
# ------------------------------------------------------------------------
sub Wanted
{
   my( $name, $path, $suffix, $relpath );

   if( ! -d $_ )
   {  
      $relpath = $File::Find::dir;
      $relpath =~ s/\\/\//g;
      $relpath =~ s/\/\//\//g;

      # Skip specific paths 
      #
      if( exists $ExcludePaths{ $relpath } )
      {
         return;
      }

      # Skip specific paths and their subdirectories
      #    if a path in RecursiveExcludePaths is a subset of relpath
      #      skip it
      #
      foreach $testpath (%RecursiveExcludePaths)
      {
          if( $testpath eq substr( $relpath, 0, length( $testpath ) ) )
          {
             return;
          }
      }

      # Parse the file name against the suffix list
      #
      $suffix = "";
      ( $name, $path, $suffix ) = fileparse( $_, @SuffixList );

      if( $suffix ne "" )
      {
         $relpath = $File::Find::dir;
         $relpath =~ s/\\/\//g;
         $relpath =~ s/\/\//\//g;

         $name = $_;

         $Files{ $name }{ 'name'   }      = $name;
         $FileRef{ lc( $name ) }          = $name;
         $Files{ $name }{ 'path'   }      = $relpath;
         $Files{ $name }{ 'suffix' }      = $suffix;

         # Special Cases

         # SMF
         #    $(TARGETDIR)/%.smt: %.smf
         #
         #    Generate smt files, so add the smt file
         #    and locate it in the build directory
         #
         if( $suffix eq ".smf" )
         {
            my $smt_name = $name;
            $smt_name =~ s/\.smf/\.smt/g;
            $Files{ $smt_name }{ 'name'   }          = $smt_name;
            $FileRef{ lc( $smt_name ) }              = $smt_name;
            $Files{ $smt_name }{ 'path'   }          = $BuildDir;
            $Files{ $smt_name }{ 'suffix' }          = ".smt";

            AddDependency( $smt_name, $name );
            $Resolved{ lc( $smt_name ) }{ lc( $name ) } = $name;
         }

         # BAR 
         #    $(UI)/%.txt: %.bar
         #
         #    Generate txt files, so add the txt file
         #    and locate it in the build directory
         #
         if( $suffix eq ".bar" )
         {
            my $txt_name = $name;
            $txt_name =~ s/\.bar/\_res.txt/g;
            $Files{ $txt_name }{ 'name'    }         = $txt_name;
            $FileRef{ lc( $txt_name ) }              = $txt_name;
            $Files{ $txt_name }{ 'path'    }         = "\$(UI)";
            $Files{ $txt_name }{ 'suffix'  }         = ".txt";

            AddDependency( $txt_name, $name );
            $Resolved{ lc( $txt_name ) }{ lc( $name ) } = $name;
         }

         # CUSTOMER.H
         #    $(TARGETDIR)/%.o : customer.h
         #       should also get
         #    $(TARGETDIR)/%.o : $(CUSTFILE)
         #    $(TARGETDIR)/%.o : includes from $(CUSTFILE)
         #
         #    Add the cust<build_id>.h file 
         #
         if( $name eq "customer.h" && $BuildID ne "" && $CustBIDName eq "" )
         {  
            if( $BuildPlatform eq "LINUX" )
            {
               my $custbid_name = "cust" . uc( $BuildID ) . ".h";
            }

            else
            {
               my $custbid_name = "cust" . lc( $BuildID ) . ".h";
            }

            $CustBIDName = $custbid_name;
            $Files{ $custbid_name }{ 'name'    }             = $custbid_name;
            $FileRef{ lc( $custbid_name ) }                  = $custbid_name;
            $Files{ $custbid_name }{ 'path'    }             = "../../build/ms";
            $Files{ $custbid_name }{ 'suffix'  }             = ".h";

            AddDependency( $name, $custbid_name ); 
            $Resolved{ lc( $name ) }{ lc( $custbid_name ) } = $custbid_name;
         }
      } # if this has a suffix we want
   } # if this is a file name
   return;
} # Wanted


# ------------------------------------------------------------------------
# SUBROUTINE: GetDependencies
#
# ------------------------------------------------------------------------
sub GetDependencies
{
   my $name = "";

   foreach $name ( keys %Files )
   {
      $suffix = $Files{ $name }{ 'suffix' };

      if( $suffix ne ".txt" && $suffix ne ".smt" )
      {
         ParseForIncludes( $name, $Files{ $name }{ 'path' } );
      }
   }

   return;
} # GetDependencies


# ------------------------------------------------------------------------
# SUBROUTINE: Usage 
#
# ------------------------------------------------------------------------
sub Usage 
{
   print "$0 - Copyright (c) 2003 QUALCOMM Inc.\n";
   print "   % perl $0 -d <dir> -f <outfile>\n";
   print "   % perl $0 -d <dir> -f <outfile> -I <additional path>\n";
   print "   % perl $0 -d <dir> -f <outfile> -X <exclude this path>\n";
   print "   % perl $0 -d <dir> -f <outfile> -bid <build id>\n";
   print "         e.g. perl $0 -d ..\\..\\ -f build.dep -bid FNTA\n";
   print "   % perl $0 -d <dir> -f <outfile> -b <build dir>\n";
   print "         e.g. perl $0 -d ..\\..\\ -f build.dep -b FXTA\n";
   return;
} # Usage

# ------------------------------------------------------------------------
# SUBROUTINE: DetermineFileName 
#
# ------------------------------------------------------------------------
sub DetermineFileName
{
   my ($file) = @_;
   my $path   = "";

   # Return only if we know something about it
   #   if not, do some research
   if( !exists $Files{$file} )
   {
      if( !exists $FileRef{ lc( $file ) } )
      {
         return "","";
      }
      else
      {
         my $lc_file = lc( $file );
         $path = $Files{ $FileRef{ $lc_file } }{ 'path' };
         $file = $Files{ $FileRef{ $lc_file } }{ 'name' };
      }
   }
   else
   {
      $path = $Files{$file}{'path'};
   }
   
   return $file,$path;
} # DetermineFileName

# ------------------------------------------------------------------------
# SUBROUTINE: Resolve
#
# ------------------------------------------------------------------------
sub Resolve
{
   my( $tlf, $lhv ) = @_;
   my $rhv = "";

   foreach $rhv ( keys %{ $Depends{$lhv} } )
   {
      if( !exists $Resolved{$tlf}{$rhv} )
      {
         if( ( $rhv ne ""   ) &&
             ( $rhv ne $lhv )    )
         {
            $Resolved{$tlf}{$rhv} = $rhv;
         }
         if( ( $tlf ne $rhv ) &&
             ( $rhv ne ""   )    )
         {
            Resolve( $tlf, $rhv );
         }
      }
      if( ( $tlf ne $rhv ) &&
          ( $rhv ne ""   )    )
      {
         $Resolved{$tlf}{$rhv} = $rhv;
      }
   }

   return;
}

# ------------------------------------------------------------------------
# SUBROUTINE: ResolveDependencies
#
# ------------------------------------------------------------------------
sub ResolveDependencies
{
   my $file = "";

   foreach $file ( sort keys %Files )
   {
      Resolve( lc( $file ), lc( $file ) );
   }

   return;
}

# ------------------------------------------------------------------------
# SUBROUTINE: WriteDependencies
#
# ------------------------------------------------------------------------
sub WriteDependencies
{
   my $file = "";

   my %all_inc = ();

   open( DEPFILE, ">>$opt_f" ) or die "Couldn't open dependency file $opt_f\n";

   # Scan through list of files searching for targets and write
   # the associated dependencies
   #
   foreach $file (sort keys %Files)
   {  
      if( ( $Files{$file}{'suffix'} eq ".h" )      ||
          ( $Files{$file}{'suffix'} eq "makedep" )    )
      {
         next;
      }

      # Assemble LHV (target)
      #
      # (.h) $Files{$file}{'path'}/$file
      #    or
      # (.c, cpp, .s, etc.) $builddir$file
      #
      # where $file for .c, .s, and .cpp is converted to .o
      #
      my $lhv        = "";
      my $lhv_src    = "";
      my $lhv_suffix = $Files{$file}{'suffix'};

      if( ( $Files{$file}{'suffix'} eq ".smt" ) ||
          ( $Files{$file}{'suffix'} eq ".txt" ) ||
          ( $Files{$file}{'suffix'} eq ".smh" ) ||
          ( $Files{$file}{'suffix'} eq ".smf" )    )
      {
         $lhv     = "$Files{$file}{'path'}/$file";
      }
      else
      {
         $lhv     = "$BuildDir/$file";
         $lhv     =~ s/(\.cpp)|(\.c)|(\.s)/\.o/g;
      }

      $all_inc{ $Files{$file}{'path'} } = $Files{$file}{'path'};
      $lhv_src = "$Files{$file}{'path'}/$file";

      # Assemble and write RHV's (dependencies)
      #
      my $rhv = "";

      # Collect the include paths
      #
      my %inc = ();

      foreach $rhv ( sort keys %{ $Resolved{ lc( $file ) } } )
      {
         # Get the RHV
         #

         my ($rhv,$path) = DetermineFileName( $rhv );

         if( $rhv ne "" )
         {  
            if( $path ne "" )
            {
               $path =~ s/\/\//\//g;
               $inc{ $path } = $path;
               $all_inc{ $path } = $path;
               $rhv = "$path/$rhv";
            }
         }

         # Write the LHV : RHV to the dependency file
         #
         if( $rhv ne "" )
         {
            print DEPFILE "$lhv: $rhv\n";
         }
      }

      # Write the include paths for this LHV
      #
      if( $opt_incpath eq 1 )
      {
         if( $Files{$file}{'suffix'} ne ".h" )
         {
            my $includes = "";
            foreach $ipath (keys %inc)
            {
               $includes = $includes . "-I" . $ipath . " ";
            }
            foreach $ipath (keys %IncludePaths)
            {
               $includes = $includes . "-I" . $ipath . " ";
            }
            print DEPFILE "$lhv: ALL_INCLUDES = $includes \$(ADDED_BUILD_INCLUDES)\n";
         }
      }

      # Write the build rule for this LHV
      #
      if( $opt_tr eq 1 )
      {
         my %SuffixRules = ();
         $SuffixRules{ ".c" }   = "\$(c_compile)";
         $SuffixRules{ ".s" }   = "\$(s_compile)";
         $SuffixRules{ ".cpp" } = "\$(cxx_compile)";

         if( exists $SuffixRules{ $lhv_suffix } )
         {
            print DEPFILE "$lhv : $lhv_src\n";
            print DEPFILE "\t$SuffixRules{ $lhv_suffix }\n";
         }
      }
   }

   if( $GlobalIncludeFile ne "" )
   {
      open( INCFILE, ">>$GlobalIncludeFile" ) or die "Couldn't open global include file $GlobalIncludeFile\n";

      foreach $path (%all_inc)
      {
         $include = $include . "-I" . $path . " ";
      }
      print INCFILE "GLOBAL_INC_PATHS = $include\n";

      close( INCFILE );
   }

   close( DEPFILE );
   return;
}

# ------------------------------------------------------------------------
# MAIN SCRIPT
#
# ------------------------------------------------------------------------

GetCommandLine();
ProcessCommandLine();

# print "Scanning directory tree...\n";

find( \&Wanted, $StartDir );

GetDependencies();

if( -e $opt_f )
{
   unlink $opt_f;
}

if( $CustBIDName ne "" )
{
   my $custname = $Files{ "$CustBIDName" }{ 'path' } . "/" . $CustBIDName;
   ParseForIncludes( $custname, $Files{ "$CustBIDName" }{ 'path' } );
}

# print "Resolving...\n";
ResolveDependencies();

# print "Writing dependency file...\n";
WriteDependencies();

# open( RESFILE, ">>resolved.txt" ) or die "Couldn't open resolve file resolved.txt\n";

# foreach $file ( sort keys %Resolved )
# {
#   print RESFILE "$file\n";
#   foreach $inc ( sort keys %{ $Resolved{$file} } )
#   {
#      print RESFILE "   $inc\n";
#   } 
#}

#close RESFILE;

1;
