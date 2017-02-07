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
#include <assert.h>


/* check if IFDs are word aligned */
ret_t check_all_IFDs_are_word_aligned(ctiff_t * ctif) {
  ret_t ret = get_empty_ret();
  tifp_check( ctif);

  uint32 ifd = get_ifd0_pos( ctif ); /*  TODO: check all other IFDs, too */
  if ( 0 != (ifd & 1)) {
    // FIXME: tif_fails?
    char array[VALUESTRLEN];
    snprintf(array, sizeof(array), "offset of first IFD points to 0x%08x and is not word-aligned", ifd);
    ret = set_value_found_ret (&ret, array);
    ret.returncode = ifderror_offset_not_word_aligned;
    return ret;
  }
  ret.returncode=is_valid;
  return ret;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
