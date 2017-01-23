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
ret_t check_tag(ctiff_t * ctif, tag_t tag) {
  //printf("check if tag %u (%s) exists\n", tag, TIFFTagName(tif, tag));
  tifp_check( ctif);
  tif_rules_tag(tag, "exists");
    return check_tag_quiet( ctif, tag);
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
