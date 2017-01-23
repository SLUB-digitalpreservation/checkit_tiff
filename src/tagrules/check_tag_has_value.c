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


ret_t check_tag_has_value(ctiff_t * ctif, tag_t tag, unsigned int value) {
  // printf("check if tag %u (%s) has value %u\n", tag, TIFFTagName( tag), value);
  tifp_check( ctif);
  // tif_rules_tag(tag, "has value");
  ret_t ret;
    ret =  check_tag_has_value_quiet( ctif, tag, value);
    if (ret.returncode == 0) {
      return ret;
    }
}



/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
