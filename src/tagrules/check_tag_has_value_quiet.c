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


ret_t check_tag_has_value_quiet(ctiff_t * ctif, tag_t tag, unsigned int expected_value) {
  GET_EMPTY_RET(ret)
  tifp_check( ctif);
  ret=check_tag_quiet(ctif, tag);
  if (ret.returncode != is_valid) return ret;


  ifd_entry_t ifd_entry = TIFFGetRawIFDEntry(ctif, tag);
  if (ifd_entry.count > 1) {
     ret = set_value_found_ret(&ret, int2str(ifd_entry.count));
     ret.returncode = tagerror_expected_count_isgreaterone;
     return ret;
  }
  switch (ifd_entry.datatype) {
    case TIFF_LONG: {
                      if (expected_value != ifd_entry.data32) {
                        ret = set_value_found_ret(&ret, int2str(ifd_entry.data32));
                        ret.returncode = tagerror_value_differs;
                        return ret;
                      }
                      break;
                    }
    case TIFF_SHORT: {
                       if (expected_value != ifd_entry.data16[0])  {
                         ret = set_value_found_ret(&ret, int2str(ifd_entry.data16[0]));
                         ret.returncode = tagerror_value_differs;
                         return ret;
                       }
                       break;
                     }
    case TIFF_RATIONAL: {
                          if (0 == ifd_entry.data16[1]) {
                            char msg[VALUESTRLEN];
                            snprintf(msg, VALUESTRLEN, "%i/%i", ifd_entry.data16[0], ifd_entry.data16[1]);
                            ret = set_value_found_ret(&ret, msg);
                            ret.returncode = tagerror_denominator_is_zero_in_fract;
                            return ret;
                          } else if (expected_value - (ifd_entry.data16[0] / ifd_entry.data16[1]) > 1) {
                            ret = set_value_found_ret(&ret, frac2str( ifd_entry.data16[0], ifd_entry.data16[1]));
                            ret.returncode = tagerror_value_differs;
                            return ret;
                          }
                          break;
                        };
    default: { /*  none */
               ret = set_value_found_ret(&ret, TIFFTypeName(ifd_entry.datatype));
               ret.returncode = tagerror_unexpected_type_found;
               return ret;
               /* break; */

             };
  };
  ret.returncode=is_valid;
  return ret;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
