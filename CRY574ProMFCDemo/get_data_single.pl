#!/usr/bin/perl
#
use warnings;
use strict;
use Spreadsheet::WriteExcel;

my @files = <*.txt>;

my @cali_data;
my $index;

sub get_one_file;
sub numerically;
sub delete_same_item;
sub array_equal;

my %calibrate_data;

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime();
$year += 2000;
my $excel_name = "data_" . $year . "-" . $mon . "-" . $mday . "-" . $hour . "-" . $min . "-" . $sec . ".xls";

my $workbook  = Spreadsheet::WriteExcel->new($excel_name); 
#$workbook->compatibility_mode();
my $worksheet1 = $workbook->add_worksheet();



# the data struct of calibrated data.
# bt addr => [{ "left" => {"inear" => value, "outear" => value}, "right" => {"inear" => value, "outear" => value} } ]
# 

for (@files)
{
	get_one_file $_;
}

my @inear_values;
my @outear_values;
my @delta;
my @rdata;
my $no = 0;
my %same_values;
my $row = 0;
my $col = 0;

$worksheet1->write($row, $col++, "bt addr");
$worksheet1->write($row, $col++, "side");
$worksheet1->write($row, $col++, "base");
$worksheet1->write($row, $col++, "gray");
$worksheet1->write($row, $col++, "rawdata");
$worksheet1->write($row, $col++, "status");
$worksheet1->write($row, $col++, "threshold");

$row++;
$col = 0;

for (keys %calibrate_data)
{
	my $bda = $_;
	my $aryref = $calibrate_data{$_};
	print "\nbt device($no): $_ => [\n";
	$col = 0;
	$worksheet1->write($row, $col, $_);
	$col++;
	$no++;
	for (my $i = 0; $i < @$aryref; $i++)
	{
		my $hashref = $aryref->[$i];

		for my $k (keys %$hashref) {
			my $side = $k;
			$col = 1;
			$worksheet1->write($row, $col, $side);
			$col++;
			print "$k\t=>\t";
			my $valref = $hashref->{$k};
			
			print "{";
			my $near;
			my $far;
			my $rdata;

			for my $v (sort keys %$valref) {
				my $ear_state = $v;
				my $uniq_key = $bda . $side . $ear_state;

				next if defined($same_values{$uniq_key});


				$same_values{$uniq_key}++;

				print "$v\t=>\t". $valref->{$v} . ",\t";	
				$worksheet1->write($row, $col, $valref->{$v});
				$col++;

				if ($v eq 'inear' or $v eq 'gray')
				{
					$near = $valref->{$v};
					push @inear_values, $valref->{$v};
				}
				elsif ($v eq 'outear' or $v eq 'base')
				{
					$far = $valref->{$v};
					push @outear_values, $valref->{$v};
				}
				elsif ($v eq 'rawdata')
				{
					$rdata = $valref->{$v};
					push @rdata, $rdata;
				}

				if (defined($near) && defined($far))
				{
					$near =~ /0[xX]([a-f0-9A-F]+)/;
					my $near_val = hex($1);
					$far =~ /0[xX]([a-f0-9A-F]+)/;
					my $far_val = hex($1);
					#bda(e0:9d:fa:e0:02:0f),psensor left data OK: cali_base=1829(0x725), cali_gray=2248(0x8c8), raw_data=1760(0x6e0), low_threhold=2457(0x999)
					push @delta, ($near_val - $far_val);
				}
			}
			$row++;
			print "},\n";
		}
	}
	print "]\n";
}

$, = ",";
print "inear values:\n";
my @tmp = map { /0[xX]([0-9a-fA-F]+)/} @inear_values;
print map { hex } @tmp;
print "\n";
print "outear values:\n";
@tmp = map { /0[xX]([0-9a-fA-F]+)/} @outear_values;
print map {hex} @tmp;
print "\n";
print "delta values:\n";
print @delta;
print "\n";
@tmp = map {/0[xX]([0-9a-fA-F]+)/} @rdata;
print "\n";
print "rawdata:\n";
print map { hex } @tmp;

$workbook->close();

sub numerically {
       my $va = $a->[1];
       my $vb = $b->[1];

       $va =~ s/0x//;
       $vb =~ s/0x//;

       print $va . "," . $vb . "\n";

       hex($va) <=> hex($vb);
}

sub get_one_file {
	my $file = shift;

	if (!$file)
	{
		print "no file!\n";
		return;
	}

	open F , "<$file" or die $!;

	my $device;
	while (<F>)
	{
		chomp;

		if (/bt device mac:\s*([0-9a-fA-F:]+)\s*/)
		{ 
		#	print "bt device:$1, ";
			$device = $1;
		}

		#		 bda(e0:9d:fa:e0:02:0f),psensor left data OK: cali_base=1829(0x725), cali_gray=2248(0x8c8), raw_data=1760(0x6e0), low_threhold=2457(0x999)
		next unless /(left|right)\s*data\s*(ok|error).*base=[0-9]+\((0x[a-f0-9A-F]+)\).*gray=[0-9]+\((0x[a-f0-9A-F]+)\).*raw_data=[0-9]+\((0x[a-fA-F0-9]+)\).*hold=[0-9]+\((0x[0-9a-fA-F]+)\)/i;
		my %item;

		$item{$1} = {"result" => $2, "base" => $3, "gray" => $4, "rawdata" => $5, "threshold" => $6};
		my $aryref = $calibrate_data{$device};
		if (!defined($aryref))
		{
			 $calibrate_data{$device} = [];
		}

		$aryref = $calibrate_data{$device};
		push @$aryref, \%item;
	}

	close F;
}

sub array_equal {
	my $first = shift;
	my $second = shift;
	my $i;

	return 0 if not defined($first) or not defined($second);
	return 0 if @$first != @$second;

	for ($i = 0; $i < @$first; $i++)
	{
		if ($first->[$i] ne $second->[$i])
		{
			return 0;
		}
	}

	return 1;
}

sub delete_same_item {
	my $aryref = shift;
	my $i;
	my @ret;
	my $j = 0;


	for ($i = 0; $i < @$aryref - 1; $i++)
	{
		my $ref = $aryref->[$i];
		my $nref = $ret[$j];

		if (not array_equal($ref, $nref))
		{
			$j++;
			$ret[$j]= $ref;
		}
	}

	return @ret;
}
