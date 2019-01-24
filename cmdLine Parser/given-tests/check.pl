#!/usr/bin/perl

#
# A little script to run the project and check it against the
# expected result trace.
#

use strict;

my $create;
my $display_errors ;
my $binary = "./cmdline";

###########################################
# Parse Command Line Arguments
# [CMD Line] [EXPECTED FILE]
###########################################
if( scalar(@ARGV) < 2 ) {
    print "ERROR: Not enough arguments\n";
    exit -1;
}

my $cmd_line_args = $ARGV[0];
my $exp_file      = $ARGV[1];
my $cmd;

if( !(-e $exp_file) && !defined($create) ) {
    print "Error: Expected file does not exist ($exp_file)\n";
    exit -2;
}


###########################################
# Diagnostics
###########################################
print "-"x70 . "\n";
print "Running the command:\n";
$cmd = $binary . " " . $cmd_line_args;
print "\tshell\$ $cmd\n";

if( defined($create) ) {
    system( "$cmd 2>&1 >$exp_file" );
    exit 0;
}

###########################################
# Run the command and gather all output
###########################################
my @output_exp;
my @output_actual;

@output_exp = `cat $exp_file`;
@output_actual = `$cmd 2>&1`;

my $total_errors = 0;

###########################################
# Compare the expected to the actual (for necessary output)
###########################################
my $tmp_exp;
my $tmp_actual;
my $found = 0;
foreach my $line_exp (@output_exp) {
    $found = 0;

    if($line_exp =~ /^#(-)*$/ ) {
        next;
    }

    $tmp_exp = $line_exp;
    $tmp_exp =~ s/\h+/ /g; # Many spaces to 1 space

    foreach my $line_actual (@output_actual) {
        if($line_actual =~ /^#(-)*$/ ) {
            next;
        }

        $tmp_actual = $line_actual;
        $tmp_actual =~ s/\h+/ /g; # Many spaces to 1 space

        if($tmp_exp =~ /^Error/ && $tmp_actual =~ /^Error/ ) {
            if( defined($display_errors) ) {
                print "Compare Error Messages:\n";
                print "EXP: $line_exp";
                print "ACT: $line_actual";
            }
            $found = 1;
            last;
        }
        elsif( $tmp_exp eq $tmp_actual ) {
            $found = 1;
            last;
        }
    }
    if($found == 0 ) {
        $total_errors += 1;
    }
}

###########################################
# Compare the actual to the expected (for extra output)
###########################################
$found = 0;
foreach my $line_actual (@output_actual) {
    $found = 0;

    if($line_actual =~ /^#(-)*$/ ) {
        next;
    }

    $tmp_actual = $line_actual;
    $tmp_actual =~ s/\h+/ /g; # Many spaces to 1 space

    foreach my $line_exp (@output_exp) {
        if($line_exp =~ /^#(-)*$/ ) {
            next;
        }

        $tmp_exp = $line_exp;
        $tmp_exp =~ s/\h+/ /g; # Many spaces to 1 space

        if($tmp_exp =~ /^Error/ && $tmp_actual =~ /^Error/ ) {
            $found = 1;
            last;
        }
        elsif( $tmp_exp eq $tmp_actual ) {
            $found = 1;
            last;
        }
    }

    if($found == 0 ) {
        print "** Extra line in actual output:\n";
        print "-"x35 . "\n";
        print "> " .$line_actual;
        print "-"x35 . "\n";
        $total_errors += 1;
    }
}

###########################################
# Determine pass/fail
###########################################
display_result($total_errors);
if( $total_errors != 0 ) {
    print "*"x35 . "\n";
    print "-"x10 . " Actual Output " . "-"x10 . "\n";
    foreach my $line (@output_actual) {
        print "> " .$line;
    }
    print "-"x10 . " Expected Output " . "-"x8 . "\n";
    foreach my $line (@output_exp) {
        print "< " .$line;
    }
    print "*"x35 . "\n";
}

print "\n";
exit 0;

#
# Display the result of the test
#
sub display_result() {
    my $num_errors = shift(@_);

    if( 0 == $num_errors ) {
        print "Passed!\n";
    }
    else {
        print "*"x5 . " Failed test on ".$num_errors." lines! " . "*"x5 . "\n";
    }

    return $num_errors;
}
