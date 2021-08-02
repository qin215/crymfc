#!/usr/bin/perl
#
use warnings;
use strict;

my @files = <*.txt>;

my @cali_data;
my $index;

sub get_one_file;
sub numerically;
sub delete_same_item;
sub array_equal;

my %calibrate_data;


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
my $no;
my %same_values;

for (keys %calibrate_data)
{
	my $bda = $_;
	my $aryref = $calibrate_data{$_};
	print "\nbt device($no): $_ => [\n";
	$no++;
	for (my $i = 0; $i < @$aryref; $i++)
	{
		my $hashref = $aryref->[$i];

		for my $k (keys %$hashref) {
			my $side = $k;
			print "$k\t=>\t";
			my $valref = $hashref->{$k};
			
			print "{";
			my $near;
			my $far;
			for my $v (sort keys %$valref) {
				my $ear_state = $v;
				my $uniq_key = $bda . $side . $ear_state;

				next if defined($same_values{$uniq_key});


				$same_values{$uniq_key}++;

				print "$v\t=>\t". $valref->{$v} . ",\t";	
				if ($v eq 'inear')
				{
					$near = $valref->{$v};
					push @inear_values, $valref->{$v};
				}
				elsif ($v eq 'outear')
				{
					$far = $valref->{$v};
					push @outear_values, $valref->{$v};
				}

				if (defined($near) && defined($far))
				{
					$near =~ /0[xX]([a-f0-9A-F]+)/;
					my $near_val = hex($1);
					$far =~ /0[xX]([a-f0-9A-F]+)/;
					my $far_val = hex($1);

					push @delta, ($near_val - $far_val);
				}
			}
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

		next unless /(left|right)\s*earphone\s*in\s*ear\s*value=(0x[a-f0-9A-F]{4}).*value=(0x[a-f0-9A-F]{4})/;
		my %item;

		$item{$1} = {"inear" => $2, "outear" => $3};
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
