#!/bin/bash

# call with:
# compare_stable_with_current_checkit_tiff.sh stable_checkit_tiff stable_cfg current_checkit_tiff current_cfg tiff-dir/

# example:
# bash checkit_tiff/t/compare_stable_with_current_checkit_tiff.sh checkit_tiff__stable/build/checkit_tiff checkit_tiff__stable/example_configs/cit_tiff6_baseline_SLUB.cfg checkit_tiff/build/checkit_tiff checkit_tiff/example_configs/cit_tiff6_baseline_SLUB.cfg checkit_tiff/tiffs_should_pass/

STABLE=$1
CONFIGSTABLE=$2
UNSTABLE=$3
CONFIGUNSTABLE=$4
TIFFDIR=$5

echo STABLE  =$STABLE CONFIG=$CONFIGSTABLE
echo UNSTABLE=$UNSTABLE CONFIG=$CONFIGUNSTABLE
echo TIFFDIR=$TIFFDIR

# diff -i -w -B -y "$RES_STABLE" "$RES_UNSTABLE"
function resultcompare {
RES_STABLE=/tmp/res_stable
RES_UNSTABLE=/tmp/res_unstable
$STABLE "$1" "$CONFIGSTABLE" 2>&1 | fgrep -v "version:" | fgrep -v "licensed under"| sed -e "" > "$RES_STABLE"
$UNSTABLE "$CONFIGUNSTABLE" "$1" 2>&1 | fgrep -v "(./)" | fgrep -v "version:" | fgrep -v "licensed under" |sed -e ""  > "$RES_UNSTABLE"
diff  -i -w -B -y  "$RES_STABLE" "$RES_UNSTABLE"

rm -f $RES_STABLE
rm -f $RES_UNSTABLE
}

IFS=$'\n'

for TIFF in $(find $TIFFDIR -name "*.tif*" -print); do
  echo TIFF=$TIFF
  echo "-------- DIFF --------"
  resultcompare $TIFF 
  echo "----------------------"
  echo
done


