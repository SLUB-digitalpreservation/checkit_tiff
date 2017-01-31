/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
/* #define DEBUG */


/* checks if TIF has a specified tag */
returncode_t check_tag_quiet(ctiff_t * ctif, tag_t tag) {
  tifp_check( ctif);
  if (-1 < TIFFGetRawTagListIndex(ctif, tag)) {
      return tag_exist;
  }
  return tag_does_not_exist;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
