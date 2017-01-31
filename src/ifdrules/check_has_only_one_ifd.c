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
  ret_t ret;
  ret.value_found = malloc(VALUESTRLEN);
  if (NULL == ret.value_found) {
    ret.returncode=could_not_allocate_memory;
    return ret;
  }
  tifp_check( ctif);
  /* next commented lines, because TIFFNumberOfDirectories are in endless loop,
   * if the TIFF file from https://github.com/EasyinnovaSL/DPFManager/blob/develop/src/test/resources/IFD%20struct/Circular%20E.tif
   */
  /* seek the image file directory (bytes 4-7) */
  uint32 offset = get_ifd0_pos(ctif );
  uint32 IFDn = get_next_ifd_pos( ctif, offset );
  if (0 == IFDn) {
    ret.returncode=is_valid;
    return ret;
  } else {
    // FIXME: tif_fails?
      char array[TIFFAILSTRLEN];
      snprintf(array, sizeof(array), "baseline TIFF should have only one IFD, but IFD0 at 0x%08x has pointer to IFDn 0x%08x", offset, IFDn );
      ret.value_found = strncpy(ret.value_found, array, VALUESTRLEN);
      ret.returncode = ifderror_multiple_ifd_detected;
      return ret;
  }
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
