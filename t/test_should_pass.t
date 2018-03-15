#!/usr/bin/perl -w
use strict;
use warnings;
use File::Path;
use File::Slurp;
use Testcall;
use Test::More tests => 12;
use POSIX;

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
ok ( call_checkit_tiff( "$testdir/test.cfg2", $baselinetiff), "call ($testdir/test.cfg2, $baselinetiff), logical first match (20,10)" );

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
ok ( call_checkit_tiff( "$testdir/test.cfg3", $baselinetiff), "call ($testdir/test.cfg3, $baselinetiff), logical last match (10,20)" );

$cfg=<<'REGEX_TEST';
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

$cfg=<<'REGEX2_TEST';
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
is (call_checkit_tiff( "$testdir/test.cfg5", $smalltiff),  "", "call ($testdir/test.cfg5, $smalltiff), regex false" );

$cfg=<<'TAILORED_TEST';
# tailored test for minimal_valid.tiff
254; mandatory; only(0)
256; mandatory; only(20)
257; mandatory; only(10)
258; mandatory; only(1)
259; mandatory; only(1)
262; mandatory; only(0)
269; mandatory; printascii
273; mandatory; any
274; mandatory; only(1)
277; mandatory; only(1)
278; mandatory; any
279; mandatory; only(30)
282; mandatory; range(300,400)
283; mandatory; range(300,400)
284; mandatory; only(1)
296; mandatory; only(2)
TAILORED_TEST
write_file("$testdir/test.cfg6", $cfg) || die "could not write $testdir/test.cfg6, $!\n";
ok ( call_checkit_tiff( "$testdir/test.cfg6", $smalltiff), "call ($testdir/test.cfg6, $smalltiff), tailored" );

$cfg=<<'TAILORED2_TEST';
# tailored test for minimal_valid.tiff
# with modes enabled
mode(baseline)
mode(enable_type_checks)
mode(enable_offset_checks)
mode(enable_ifd_checks)
254; mandatory; only(0)
256; mandatory; only(20)
257; mandatory; only(10)
258; mandatory; only(1)
259; mandatory; only(1)
262; mandatory; only(0)
269; mandatory; printascii
273; mandatory; any
274; mandatory; only(1)
277; mandatory; only(1)
278; mandatory; any
279; mandatory; only(30)
282; mandatory; range(300,400)
283; mandatory; range(300,400)
284; mandatory; only(1)
296; mandatory; only(2)
TAILORED2_TEST
write_file("$testdir/test.cfg7", $cfg) || die "could not write $testdir/test.cfg7, $!\n";
ok ( call_checkit_tiff( "$testdir/test.cfg7", $smalltiff), "call ($testdir/test.cfg7, $smalltiff), tailored w modes" );

$cfg=<<'TAILORED_TEST';
# tailored test for minimal_valid_12bit.tiff
# with modes enabled
mode(baseline)
mode(enable_type_checks)
mode(enable_offset_checks)
mode(enable_ifd_checks)
#254; mandatory; only(0)
256; mandatory; only(20)
257; mandatory; only(10)
258; mandatory; only(12)
259; mandatory; only(1)
262; mandatory; only(1)
266; mandatory; only(1)
273; mandatory; any
274; mandatory; only(1)
277; mandatory; only(1)
278; mandatory; any
279; mandatory; only(300)
282; mandatory; range(300,400)
283; mandatory; range(300,400)
284; mandatory; only(1)
296; mandatory; only(2)
297; mandatory; ntupel(0,1)
TAILORED_TEST
write_file("$testdir/test.cfg8", $cfg) || die "could not write $testdir/test.cfg8, $!\n";
ok ( call_checkit_tiff( "$testdir/test.cfg8", "$tiffdir/minimal_valid_12bit.tiff"), "call ($testdir/test.cfg8,  $tiffdir/minimal_valid_12bit.tiff), tailored w modes" );

$cfg=<<'TAILORED_TEST';
# tailored test for minimal_valid_with_icc
# with modes enabled
mode(baseline)
mode(enable_type_checks)
mode(enable_offset_checks)
mode(enable_ifd_checks)
#254; mandatory; only(0)
256; mandatory; only(20)
257; mandatory; only(10)
258; mandatory; only(1)
259; mandatory; only(1)
262; mandatory; only(1)
266; mandatory; only(1)
273; mandatory; any
274; mandatory; only(1)
277; mandatory; only(1)
278; mandatory; any
279; mandatory; only(30)
282; mandatory; range(300,400)
283; mandatory; range(300,400)
284; mandatory; only(1)
296; mandatory; only(2)
297; mandatory; ntupel(0,1)
34675; mandatory; iccprofile
TAILORED_TEST
write_file("$testdir/test.cfg9", $cfg) || die "could not write $testdir/test.cfg9, $!\n";
ok ( call_checkit_tiff( "$testdir/test.cfg9", "$tiffdir/minimal_valid_with_icc.tiff"), "call ($testdir/test.cfg9,  $tiffdir/minimal_valid_with_icc.tiff), tailored w modes" );

$cfg=<<'TAILORED_TEST';
# tailored test for bigendian/minimal_valid_baseline_BigEndian.tiff
# with modes enabled
mode(baseline)
mode(enable_type_checks)
mode(enable_offset_checks)
mode(enable_ifd_checks)
254; mandatory; only(0)
256; mandatory; only(20)
257; mandatory; only(10)
258; mandatory; only(1)
259; mandatory; only(1)
262; mandatory; only(0)
273; mandatory; any
274; mandatory; only(1)
277; mandatory; only(1)
278; mandatory; any
279; mandatory; only(30)
282; mandatory; range(300,400)
283; mandatory; range(300,400)
284; mandatory; only(1)
296; mandatory; only(2)
TAILORED_TEST
write_file("$testdir/test.cfg10", $cfg) || die "could not write $testdir/test.cfg10, $!\n";
ok ( call_checkit_tiff( "$testdir/test.cfg10", "$tiffdir/bigendian/minimal_valid_baseline_BigEndian.tiff"), "call ($testdir/test.cfg10,  $tiffdir/bigendian/minimal_valid_baseline_BigEndian.tiff), tailored w modes" );


$cfg=<<'TAILORED_TEST';
# regression test for minimal_valid_baseline.tiff
254; mandatory; any
256; mandatory; any
257; mandatory; any
258; mandatory; any
273; mandatory; any
274; mandatory; any
277; mandatory; any
278; mandatory; any
279; mandatory; any
282; mandatory; any
283; mandatory; any
284; mandatory; any
296; mandatory; any
259; depends(262.0); logical_or(1,2,32773)
262; mandatory; logical_or(0,2)
TAILORED_TEST
write_file("$testdir/test.cfg11", $cfg) || die "could not write $testdir/test.cfg11, $!\n";
ok ( call_checkit_tiff( "$testdir/test.cfg11", "$tiffdir/minimal_valid_baseline.tiff"), "call ($testdir/test.cfg11,  $tiffdir/minimal_valid_baseline.tiff), tailored regression" );
############
cleanup();
