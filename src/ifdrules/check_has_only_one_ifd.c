/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <unistd.h>

/*  check if only one IFD exists */
ret_t check_has_only_one_ifd(ctiff_t * ctif) {
  //printf("check if only one IFD exists\n");
  tif_rules("only one IFD exists");
  /* next commented lines, because TIFFNumberOfDirectories are in endless loop,
   * if the TIFF file from https://github.com/EasyinnovaSL/DPFManager/blob/develop/src/test/resources/IFD%20struct/Circular%20E.tif
   */
  /* seek the image file directory (bytes 4-7) */
  uint32 offset = get_ifd0_pos(ctif );
  uint32 IFDn = get_next_ifd_pos( ctif, offset );
  if (0 == IFDn) {
    ret_t res;
    res.returnmsg=NULL;
    res.returncode=0;
    return res;
  } else {
    // FIXME: tif_fails?
      char array[160];
      snprintf(array, sizeof(array), "baseline TIFF should have only one IFD, but IFD0 at 0x%08x has pointer to IFDn 0x%08x", offset, IFDn );
      return tif_fails( array);
  }
}
