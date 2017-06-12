#!/usr/bin/perl -w
use strict;
use warnings;
use File::Path;
use File::Slurp;
use Testcall;
use Test::More tests => 36;
use POSIX;

my $testdir=prepare();
# testcases should use a cfg, which test only the *special* aspect of broken
# tiffs

my $checkit_tiff="../build/checkit_tiff";
my $tiffdir="../tiffs_should_fail";
my $cfg_without_modes=<<CFG;
# tiff
254; optional; only(0)
256; mandatory; any
257; mandatory; any
258; depends(262.0); logical_or(1,4,8)
258; depends(262.1); logical_or(1,4,8)
258; depends(320.any); logical_or(4,8)
258; depends(262.2); logical_or(ntupel(8,8,8),ntupel(16,16,16))
259; depends(262.0); logical_or(1,2,32773)
259; depends(262.1); logical_or(1,2,32773)
259; depends(258.4); logical_or(1,32773)
259; depends(258.8); logical_or(1,32773)
259; depends(262.3); logical_or(1,32773)
259; depends(262.2); logical_or(1,32773)
262; mandatory; logical_or(0,1,2)
262; depends(320.any); only(3)
266; optional; only(1)
269; optional; printascii
273; mandatory; any
274; optional; only(1)
277; depends(262.2); only(3)
277; depends(262.3); only(1)
277; depends(262.1); only(1)
277; depends(262.0); only(1)
278; mandatory; any
279; mandatory; any
282; mandatory; any
283; mandatory; any
284; optdepends(277.3); only(1)
284; optdepends(277.1); range(1,2)
296; optional; logical_or(1,2,3)
297; optional; ntupel(0, 1)
305; optional; printascii
306; optional; datetime
315; optional; printascii
320; optional; any
34377; optional; any
CFG
write_file("$testdir/test.cfg_without_modes", $cfg_without_modes) || die "could not write $testdir/test.cfg_without_modes, $!\n";

my $cfg_with_modes=<<CFG;
# tiff
mode(baseline)
mode(enable_type_checks)
mode(enable_offset_checks)
mode(enable_ifd_checks)
$cfg_without_modes
CFG
write_file("$testdir/test.cfg_with_modes", $cfg_with_modes) || die "could not write $testdir/test.cfg_with_modes, $!\n";

ok (!call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/invalid_date.tiff"), "call ($testdir/test.cfg_without_modes, $testdir/invalid_date.tif)" );
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes", "$tiffdir/invalid_date.tiff"), "call ($testdir/test.cfg_with_modes, $testdir/invalid_date.tif) w modes" );
# pause();

my $cfg=<<CFG;
# tiff
mode(baseline)
mode(enable_type_checks)
mode(enable_offset_checks)
mode(enable_ifd_checks)
CFG
write_file("$testdir/test.cfg3", $cfg) || die "could not write $testdir/test.cfg3, $!\n";
ok (!call_checkit_tiff( "$testdir/test.cfg3", "$tiffdir/minimal_empty_IFD.tif"), "call ($testdir/test.cfg3, $testdir/minimal_empty_IFD.tif) w modes" );
# pause();

# TODO: error message test
#write_file("$testdir/test.cfg4", $cfg) || die "could not write $testdir/test.cfg4, $!\n";
#like(call_checkit_tiff( "$testdir/test.cfg4", "$tiffdir/minimal_empty_IFD.tif"), qr/pointer to IFD0 must be greater than 8/, "call ($testdir/test.cfg4, $testdir/minimal_empty_IFD.tif) err msg" );

ok ( call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_IFD_not_word_aligned.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_IFD_not_word_aligned.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_IFD_not_word_aligned.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_IFD_not_word_aligned.tif) w modes" );
# pause();

ok (!call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_IFD_points_outside_filesize.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_IFD_points_outside_filesize.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_IFD_points_outside_filesize.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_IFD_points_outside_filesize.tif) w modes" );
# pause();
# TODO: error message test

ok ( call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_IFD_with_endless_loop.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_IFD_with_endless_loop.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_IFD_with_endless_loop.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_IFD_with_endless_loop.tif) w modes" );
# pause();
# TODO: error message test

ok ( call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_invalid_type_imagewidth.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_invalid_type_imagewidth.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_invalid_type_imagewidth.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_invalid_type_imagewidth.tif) w modes" );
# pause();
# TODO: error message test

ok ( call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_invalid_type_imagewidth.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_invalid_type_imagewidth.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_invalid_type_imagewidth.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_invalid_type_imagewidth.tif) w modes" );
# pause();
# TODO: error message test

ok ( call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_offsets_multiple_used.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_offsets_multiple_used.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_offsets_multiple_used.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_offsets_multiple_used.tif) w modes" );
# pause();
# TODO: error message test

ok ( call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_offsets_not_word_aligned.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_offsets_not_word_aligned.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_offsets_not_word_aligned.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_offsets_not_word_aligned.tif) w modes" );
# pause();
# TODO: error message test

ok (!call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_tag_is_doubled.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_tag_is_doubled.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_tag_is_doubled.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_tag_is_doubled.tif) w modes" );
# pause();
# TODO: error message test

ok (!call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_tag_points_outside_filesize.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_tag_points_outside_filesize.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_tag_points_outside_filesize.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_tag_points_outside_filesize.tif) w modes" );
# pause();
# TODO: error message test

ok (!call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_tags_in_wrong_order.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_tags_in_wrong_order.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_tags_in_wrong_order.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_tags_in_wrong_order.tif) w modes" );
# pause();
# TODO: error message test

ok (!call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_tag_with_contradictory_photometricinterpretation_and_colortag.tiff"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_tag_with_contradictory_photometricinterpretation_and_colortag.tiff)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_tag_with_contradictory_photometricinterpretation_and_colortag.tiff"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_tag_with_contradictory_photometricinterpretation_and_colortag.tiff) w modes" );
# pause();
# TODO: error message test

ok (!call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_tag_with_count_equal_null.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_tag_with_count_equal_null.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_tag_with_count_equal_null.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_tag_with_count_equal_null.tif) w modes" );
# pause();
# TODO: error message test

ok (!call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_without_IFD.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_without_IFD.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_without_IFD.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_without_IFD.tif) w modes" );
# pause();
# TODO: error message test

ok (!call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/minimal_wrong_exififd_type.tif"), "call ($testdir/test.cfg_without_modes, $testdir/minimal_wrong_exififd_type.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/minimal_wrong_exififd_type.tif"), "call ($testdir/test.cfg_with_modes, $testdir/minimal_wrong_exififd_type.tif) w modes" );
# pause();
# TODO: error message test

ok (!call_checkit_tiff( "$testdir/test.cfg_without_modes", "$tiffdir/wrong_subfile_type__wrong_xresolution_alignment.tif"), "call ($testdir/test.cfg_without_modes, $testdir/wrong_subfile_type__wrong_xresolution_alignment.tif)" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/wrong_subfile_type__wrong_xresolution_alignment.tif"), "call ($testdir/test.cfg_with_modes, $testdir/wrong_subfile_type__wrong_xresolution_alignment.tif) w modes" );
# pause();
# TODO: error message test

######### no tiff tests
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/no_tiff/minimal.jxr"), "call ($testdir/test.cfg_with_modes, $testdir/no_tiff/minimal.jxr) w modes" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/no_tiff/minimal.tiff64"), "call ($testdir/test.cfg_with_modes, $testdir/no_tiff/minimal.tiff64) w modes" );
# pause();
ok (!call_checkit_tiff( "$testdir/test.cfg_with_modes"   , "$tiffdir/no_tiff/minimal.mdi"), "call ($testdir/test.cfg_with_modes, $testdir/no_tiff/minimal.mdi) w modes" );

cleanup();
