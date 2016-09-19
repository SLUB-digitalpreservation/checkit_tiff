#!/usr/bin/perl -w
use strict;
use warnings;
use File::Path;
use File::Slurp;
use Testcall;
use Test::More tests => 6;

my $testdir=prepare();

my $cfgfile="../example_configs/cit_tiff_baseline_minimal.cfg";
my $tiffdir="../tiffs_should_pass";
my $baselinetiff="$tiffdir/minimal_valid_baseline.tiff";
my $smalltiff="$tiffdir/minimal_valid.tiff";


ok ( call_checkit_tiff( $cfgfile, $baselinetiff), "call ($cfgfile, $baselinetiff), TIFF6 baseline" );

## all mandatory
my $cfg=<<ALL_MANDATORY;
# simple any test for minimal_valid_baseline.tiff
254; mandatory; any
256; mandatory; any
257; mandatory; any
258; mandatory; any
259; mandatory; any
262; mandatory; any
273; mandatory; any
274; mandatory; any
277; mandatory; any
278; mandatory; any
279; mandatory; any
282; mandatory; any
283; mandatory; any
284; mandatory; any
296; mandatory; any
ALL_MANDATORY
write_file("$testdir/test.cfg1", $cfg) || die "could not write $testdir/test.cfg1, $!\n";
ok ( call_checkit_tiff( "$testdir/test.cfg1", $baselinetiff), "call ($testdir/test.cfg1, $baselinetiff), mandatory any" );


$cfg=<<LOGICAL_FIRST_MATCH;
# simple any test for minimal_valid_baseline.tiff
254; mandatory; any
256; mandatory; logical_or(20,10)
257; mandatory; any
258; mandatory; any
259; mandatory; any
262; mandatory; any
273; mandatory; any
274; mandatory; any
277; mandatory; any
278; mandatory; any
279; mandatory; any
282; mandatory; any
283; mandatory; any
284; mandatory; any
296; mandatory; any
LOGICAL_FIRST_MATCH
write_file("$testdir/test.cfg2", $cfg) || die "could not write $testdir/test.cfg2, $!\n";
ok ( call_checkit_tiff( "$testdir/test.cfg2", $baselinetiff), "call ($testdir/test.cfg2, $baselinetiff), logical first match (20,15,10)" );

$cfg=<<LOGICAL_LAST_MATCH;
# simple any test for minimal_valid_baseline.tiff
254; mandatory; any
256; mandatory; logical_or(10,20)
257; mandatory; any
258; mandatory; any
259; mandatory; any
262; mandatory; any
273; mandatory; any
274; mandatory; any
277; mandatory; any
278; mandatory; any
279; mandatory; any
282; mandatory; any
283; mandatory; any
284; mandatory; any
296; mandatory; any
LOGICAL_LAST_MATCH
write_file("$testdir/test.cfg3", $cfg) || die "could not write $testdir/test.cfg3, $!\n";
ok ( call_checkit_tiff( "$testdir/test.cfg3", $baselinetiff), "call ($testdir/test.cfg3, $baselinetiff), logical last match (10,15,20)" );

my $cfg=<<'REGEX_TEST';
# simple any test for minimal_valid.tiff
254; mandatory; any
256; mandatory; any
257; mandatory; any
258; mandatory; any
259; mandatory; any
262; mandatory; any
269; mandatory; regex("^/home/romeyke/fixit_tiff/checkit/tiffs_should_pass/minimal_valid2.tiff$")
273; mandatory; any
274; mandatory; any
277; mandatory; any
278; mandatory; any
279; mandatory; any
282; mandatory; any
283; mandatory; any
284; mandatory; any
296; mandatory; any
REGEX_TEST
write_file("$testdir/test.cfg4", $cfg) || die "could not write $testdir/test.cfg4, $!\n";
ok ( call_checkit_tiff( "$testdir/test.cfg4", $smalltiff), "call ($testdir/test.cfg4, $smalltiff), regex true" );

my $cfg=<<'REGEX2_TEST';
# simple any test for minimal_valid.tiff
254; mandatory; any
256; mandatory; any
257; mandatory; any
258; mandatory; any
259; mandatory; any
262; mandatory; any
269; mandatory; regex("^foo$")
273; mandatory; any
274; mandatory; any
277; mandatory; any
278; mandatory; any
279; mandatory; any
282; mandatory; any
283; mandatory; any
284; mandatory; any
296; mandatory; any
REGEX2_TEST
write_file("$testdir/test.cfg5", $cfg) || die "could not write $testdir/test.cfg5, $!\n";
ok (! call_checkit_tiff( "$testdir/test.cfg5", $smalltiff), "call ($testdir/test.cfg5, $smalltiff), regex false" );




cleanup();
