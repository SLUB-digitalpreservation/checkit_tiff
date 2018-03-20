#!/usr/bin/perl -w
use strict;
use warnings;
use File::Path;
use File::Slurp;
use Testcall;
use Test::More tests => 8;

my $testdir=prepare();

my $cfgdir="../example_configs/";

opendir (my $dirh, $cfgdir) || die "could not open dir '$cfgdir', $!\n";
while(readdir $dirh) {
    if ($_ !~ /^\./ && $_ =~ /\.cfg$/ && -f "$cfgdir/$_") {
        my $cfg = "$cfgdir/$_";
        my $cfgstring=read_file($cfg);
        ok (
            call_checkit_check_config( $cfgstring),
            "call (<$cfg)"
        );
    }
}
cleanup();
