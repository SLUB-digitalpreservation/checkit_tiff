use strict;
use warnings;
package Testcall;
use IPC::Open3;
use File::Spec;
use Exporter 'import';

our @EXPORT = qw(
    $testdir
    call_checkit_tiff
    call_checkit_check_config
    call2_checkit_check_config
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
    #print "------------------------\n";
    #system("$checkit_tiff", "-q", $cfg, $tiff);
    #print "========================\n";

    # code from perlfaq8
    my $in = '';
    use IO::File;
    local *CATCHERR = IO::File->new_tmpfile;
    my $pid = open3($in, \*CATCHOUT, ">&CATCHERR", "$checkit_tiff -q $cfg $tiff");
    my @out;
    while( <CATCHOUT> ) { push @out, $_; }
    waitpid($pid, 0);
    seek CATCHERR, 0, 0;
    while( <CATCHERR> ) {}

    #
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
        #printf "child exited with value %d\n", $ret;
        if ($ret != 0) {
            print "----------->\n";
            print join ("\n", @out);
            print "<-----------\n\n";
        }
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

sub call2_checkit_check_config {
    my $grammar = shift;
    my @out;
    my ($rh, $wh, $eh);
    use Symbol 'gensym'; $eh = gensym;
    my $pid = open3($wh, $rh, $eh, "$checkit_check_config");
    print $wh $grammar;
    close $wh;
    while (<$rh>) {
        push @out, $_;
    }
    use Data::Printer;
    while (<$eh>) {
        p( $_ );
    }
    waitpid($pid, 0);
    if ($? == -1) {
        die "failed to execute: $!\n";
    }
    elsif ($? & 127) {
        printf "child died with signal %d, %s coredump\n",
        ($? & 127),  ($? & 128) ? 'with' : 'without';
        return $?;
    }
    else {
        #printf "child exited with value %d\n", $? >> 8;
    }

    return join("", @out);
}

sub prepare {
    mkdir "$testdir";
    return $testdir;
}

sub cleanup {
    rmdir "$testdir";
}

1;
