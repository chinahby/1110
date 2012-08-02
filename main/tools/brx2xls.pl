use Cwd;
use File::Find; 
use File::Basename;
use XML::Simple;  
use Data::Dumper;  
use Encode;
use Spreadsheet::WriteExcel;
use utf8;

if($#ARGV!=0)
{
  die "Usage brx3xls.pl DIRECTORY OUTPUTFILE\n";
}

$output_file = @ARGV[0];

my $xls = Spreadsheet::WriteExcel->new($output_file)|| die "Cannot open output file $output_file\n";

my   @SuffixList = ();
push @SuffixList, "\\.brx";

sub OutputBRX
{
    my ( $file, $FileName, $FileSuffex) = @_; 
    my $xml = XML::Simple->new();
    my $doc = $xml->XMLin($file);
    Dumper($doc);
    my $xlsContent = $xls->add_worksheet($FileName);
    my $contentStyle = $xls->add_format();
    my $contentStyle2 = $xls->add_format();
    $contentStyle2->set_text_wrap();
    $contentStyle2->set_bold();
    $contentStyle2->set_size(10);
    $xlsContent->set_column(2,2,64);
    $xlsContent->set_column(3,3,64);
    $row = 0;
    
    if(ref($doc->{Strings}->{String}) eq "ARRAY")
    {
        for my $item1 ( @{ $doc->{Strings}->{String} } ) {
          print "$item1->{Id}-$item1->{Name}\n";
          $xlsContent->write_string($row, 0, $item1->{Id},   $contentStyle);
          $xlsContent->write_string($row, 1, $item1->{Name}, $contentStyle);
          $xlsContent->write_string($row, 2, $item1->{Text}, $contentStyle2);
          $row += 1;
        }
    }
    
    if(ref($doc->{Strings}->{STRING}) eq "ARRAY")
    {
        for my $item2 ( @{ $doc->{Strings}->{STRING} } ) {
          print "$item2->{Id}-$item2->{Name}\n";
          $xlsContent->write_string($row, 0, $item2->{Id},   $contentStyle);
          $xlsContent->write_string($row, 1, $item2->{Name}, $contentStyle);
          $xlsContent->write_string($row, 2, $item2->{Text}, $contentStyle2);
          $row += 1;
        }
    }
}

sub Wanted
{   
    my( $name, $path, $suffix, $file );

    if( ! -d $_ )
    {
        $suffix = "";
        ( $name, $path, $suffix ) = fileparse( $_, @SuffixList );
        
        if( $suffix ne "" )
        {
            $suffix = substr($suffix,1);
            $file = $File::Find::name;
            $file =~ s/\//\\/g;
            print "Proccessing...$file \n";
			OutputBRX($file, $name, $suffix);
        }
    }
}

find(\&Wanted, ".");
$xls->close();
