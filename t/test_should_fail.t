#!/usr/bin/perl -w
use strict;
use warnings;
use File::Path;
use File::Slurp;
use Testcall;
use Test::More tests => 14;

my $testdir=prepare();


my $checkit_check_config="../build/checkit_check_config";
my $checkit_tiff="../build/checkit_tiff";
my $cfg="$testdir/cit_tiff.cfg";
my $tiffdir="../tiffs_should_fail";
# config base is: ../example_configs/cit_tiff_baseline_minimal.cfg
# but with tag 269 optionally allowed
my $cfgstring=<<CFG;
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
269; optional; any
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
296; optional; logical_or(1,2,3)
320; optional; any
CFG
write_file($cfg, $cfgstring);
opendir (my $dirh, $tiffdir) || die "could not open dir '$tiffdir', $!\n";
while(readdir $dirh) {
    if ($_ !~ /^\./ && $_ =~ /\.tiff?$/ && -f "$tiffdir/$_") {
        my $tiff = "$tiffdir/$_";
        ok (!
            call_checkit_tiff( $cfg, $tiff),
            "call ($cfg, $tiff)"
        );
    }
}
cleanup();
