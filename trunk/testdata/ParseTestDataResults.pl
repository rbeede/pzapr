#
#
# Rodney Beede
#
# 2011-04-25
#
# Tested with Perl 5.12
#


use strict;
use warnings "all";;



use File::Find;
use Time::Local;
use List::Util;
use Cwd;


# Global vars

my %results;


main();  # for scoping

sub main {
	my $baseDir = Cwd::abs_path($ARGV[0]);

	unless(defined($baseDir) && -d $baseDir) {
		die("Directory $baseDir doesn't exist!\n\n");
	}

	print STDERR "Searching $baseDir\n";

	File::Find::find(\&processLog, ($baseDir));
	
	
	outputResults();
}


sub processLog {
	my $fullPathname = $File::Find::name;

	if((! (-f $fullPathname)) || $fullPathname !~ /\.log$/i) {
		return;  # not a log to parse
	}

	#print "Parsing $fullPathname\n";
	
	open(FH,"<",$fullPathname) or die("Unable to open $fullPathname\n\tPossible Cause:\t$!\n");
	my @lines = <FH> ;
	close(FH);
	
	
	# First line has total number of processes
	$lines[0] =~ m/rank (\d+) of (\d+)/;
	my $thisProcessRank = $1;
	my $numberProcesses = $2;
	
	# Line 4 has the test data info and position (first, middle, last, never)
	$lines[3] =~ m/argv\[2\] == .\/Test_Data_([^_]*)_.*.zip/;
	my $testPosition = $1;
	$testPosition = uc($testPosition);
	if(index($testPosition, "FIRST") != -1) {
		$testPosition = "FIRST";
	} elsif(index($testPosition, "MIDDLE") != -1) {
		$testPosition = "MIDDLE";
	} elsif(index($testPosition, "LAST") != -1) {
		$testPosition = "LAST";
	} elsif(index($testPosition, "NEVER") != -1) {
		$testPosition = "NEVER";
	} else {
		$testPosition = "UNKNOWN - " . $testPosition;
	}
	
	# BRUTE or DICTIONARY
	$lines[4] =~ m/argv\[3\] == (.*)/;
	my $testType = $1;
	
	
	my $startEpoch = -1;
	my $endEpoch = -1;
	my $totalAttempts = -1;
	my $processFinished = "false";
	for(my $i = 5; $i < $#lines; $i++) {
		if($lines[$i] =~ m/Starting attempts/) {
			$startEpoch = datestampToEpoch($lines[$i]);
		} elsif($lines[$i] =~ m/(\d+) attempts made by process/) {
			$endEpoch = datestampToEpoch($lines[$i]);
			$totalAttempts = $1;
			$processFinished = "true";
			last;  # all done
		} elsif($lines[$i] =~ m/Rank \d+ has made (\d+) attempts with the last password tried/) {
			# Didn't actually finish, so record partial result
			$endEpoch = datestampToEpoch($lines[$i]);
			$totalAttempts = $1;
		}
	}
	
	
	# Record results
	if(-1 == $startEpoch || -1 == $endEpoch) {
		die("Invalid log $fullPathname!  No valid start/end epoch\n");
	}
	
	
	my $attemptsPerSecond = $totalAttempts / List::Util::max(($endEpoch - $startEpoch), 1);  # 1 second is minimum
	
	
	if(!defined($results{$numberProcesses}{$testType}{$testPosition}{MAX})) {
		$results{$numberProcesses}{$testType}{$testPosition}{MAX} = 0;
	}
	if(!defined($results{$numberProcesses}{$testType}{$testPosition}{MIN})) {
		$results{$numberProcesses}{$testType}{$testPosition}{MIN} = 0;
	}
	if(!defined($results{$numberProcesses}{$testType}{$testPosition}{TOTAL_ATTEMPTS_ALL_NODES})) {
		$results{$numberProcesses}{$testType}{$testPosition}{TOTAL_ATTEMPTS_ALL_NODES} = 0;
	}
	if(!defined($results{$numberProcesses}{$testType}{$testPosition}{ALL_PROCESSES_FINISHED})) {
		$results{$numberProcesses}{$testType}{$testPosition}{ALL_PROCESSES_FINISHED} = "true";
	}
	if(!defined($results{$numberProcesses}{$testType}{$testPosition}{TOTAL_ATTEMPTS_PER_SECOND_ALL_NODES})) {
		$results{$numberProcesses}{$testType}{$testPosition}{TOTAL_ATTEMPTS_PER_SECOND_ALL_NODES} = 0;
	}
	
	if($results{$numberProcesses}{$testType}{$testPosition}{MAX} < $attemptsPerSecond) {
		$results{$numberProcesses}{$testType}{$testPosition}{MAX} = $attemptsPerSecond;
	}
	if($results{$numberProcesses}{$testType}{$testPosition}{MIN} > $attemptsPerSecond) {
		$results{$numberProcesses}{$testType}{$testPosition}{MIN} = $attemptsPerSecond;
	}
	
	# We can calculate the average later by dividing this by the number of nodes
	$results{$numberProcesses}{$testType}{$testPosition}{TOTAL_ATTEMPTS_PER_SECOND_ALL_NODES} += $attemptsPerSecond;
	
	if($results{$numberProcesses}{$testType}{$testPosition}{ALL_PROCESSES_FINISHED} eq "true" && $processFinished eq "false") {
		$results{$numberProcesses}{$testType}{$testPosition}{ALL_PROCESSES_FINISHED} = $processFinished;
	} else {
		$results{$numberProcesses}{$testType}{$testPosition}{ALL_PROCESSES_FINISHED} = $processFinished;
	}
	
	$results{$numberProcesses}{$testType}{$testPosition}{TOTAL_ATTEMPTS_COMBINED_ALL_NODES} += $totalAttempts;
}


sub datestampToEpoch {
	my $logLine = shift;
	
	# we already assume time is in UTC so no timezone conversion needed
	
	$logLine =~ m/^(\d\d\d\d)\-(\d\d)\-(\d\d)_(\d\d)\:(\d\d)\:(\d\d)/;

	return timegm($6,$5,$4,$3,$2 - 1,$1);
}


sub outputResults {
	print "Number Processes" . "," . "Test Type" . "," . "Test Position" . "," . "Maximum Attempts Per Second for 1 Process" . "," . "Minimum Attempts Per Second for 1 Process" . "," . "Average Attempts Per Second for 1 Node" . "," . "All Processes Finished" . "," . "Total Number Attempts for All Nodes Combined" . "\n";
	
	foreach my $numProcesses (sort keys %results) {
		foreach my $testType (sort keys %{$results{$numProcesses}}) {
			foreach my $testPosition (sort keys %{$results{$numProcesses}{$testType}}) {
				print $numProcesses;
				print ",";
				print $testType;
				print ",";
				print $testPosition;
				print ",";
				
				print $results{$numProcesses}{$testType}{$testPosition}{MAX};
				print ",";
				print $results{$numProcesses}{$testType}{$testPosition}{MIN};
				print ",";
				print $results{$numProcesses}{$testType}{$testPosition}{TOTAL_ATTEMPTS_PER_SECOND_ALL_NODES} / $numProcesses;
				print ",";
				print $results{$numProcesses}{$testType}{$testPosition}{ALL_PROCESSES_FINISHED};
				print ",";
				print $results{$numProcesses}{$testType}{$testPosition}{TOTAL_ATTEMPTS_COMBINED_ALL_NODES};

				print "\n";
			}
		}
	}
}
