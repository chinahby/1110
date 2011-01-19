use Math::Trig;

my $Angle      = 0;
my $AngleMax   = 360;
my $StepMax    = 360;
my $Factor     = 1024;

if(4 == @ARGV)
{
    $Angle      = $ARGV[0];
    $AngleMax   = $ARGV[1];
    $StepMax    = $ARGV[2];
    $Factor     = $ARGV[3];
}

# Inter Paramater
$Step       = $AngleMax/$StepMax;
$Count      = 0;

while($Angle<$AngleMax)
{
    my $result = sin(pi*$Angle/180)*$Factor;
    my $rounded = sprintf("%6.0f", $result);
    print $rounded.",";
    $Angle += $Step;
    $Count++;
    if($Count % 15 == 0)
    {
        print "\n";
    }
}
print "\n";
