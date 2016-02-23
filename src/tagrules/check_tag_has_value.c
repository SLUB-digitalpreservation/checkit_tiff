/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <pcre.h>
#include <string.h>

/*
#define DEBUG
*/


ret_t check_tag_has_value(TIFF* tif, tag_t tag, unsigned int value) {
  // printf("check if tag %u (%s) has value %u\n", tag, TIFFTagName(tif, tag), value);
  tifp_check( tif);
  tif_rules_tag(tag, "has value");
  ret_t ret;
  ret = check_tag_has_valid_type( tif, tag);
  if (ret.returncode == 0) {
    ret =  check_tag_has_value_quiet( tif, tag, value);
    if (ret.returncode == 0) {
      return ret;
    }
  }
  return tif_fails_by_returns( ret );
}



