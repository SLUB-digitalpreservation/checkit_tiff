use strict;
use warnings;
package Testcall;
use Exporter 'import';
our @EXPORT = qw(
    $testdir
    call_checkit_tiff
    call_checkit_check_config
    prepare
    cleanup
);

my $testdir = "./testdir/";
my $checkit_check_config="../build/checkit_check_config";
my $checkit_tiff="../build/checkit_tiff";

sub call_checkit_tiff {
    my $cfg=shift;
    my $tiff = shift;
    #print "DEBUG: $checkit_tiff -c $tiff $cfg\n";
    print "------------------------\n";
    system("$checkit_tiff", "-c", $tiff, $cfg);
    print "========================\n";
    my $ret=0;
    if ($? == -1) {
        die "failed to execute: $!\n";
    }
    elsif ($? & 127) {
        printf "child died with signal %d, %s coredump\n",
        ($? & 127),  ($? & 128) ? 'with' : 'without';
        return $?;
    }
    else {
        $ret=$?>>8;
        #printf "child exited with value %d\n", $? >> 8;
    }
    return ($ret == 0)
}

sub call_checkit_check_config {
    my $grammar = shift;
    open(my $FOO, "|$checkit_check_config >/dev/null") or die "check failed: $!\n";
    print $FOO $grammar;
    close $FOO;
    #print "test '$?'\n";
    my $ret=0;
    if ($? == -1) {
        die "failed to execute: $!\n";
    }
    elsif ($? & 127) {
        printf "child died with signal %d, %s coredump\n",
        ($? & 127),  ($? & 128) ? 'with' : 'without';
        return $?;
    }
    else {
        $ret=$?>>8;
        #printf "child exited with value %d\n", $? >> 8;
    }
    return ($ret == 0)
}

sub prepare {
    mkdir "$testdir";
    return $testdir;
}

sub cleanup {
    rmdir "$testdir";
}

1;
