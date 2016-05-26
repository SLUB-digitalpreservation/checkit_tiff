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
  tifp_check( ctif);
    ifd_entry_t ifd_entry = TIFFGetRawIFDEntry(ctif, tag);
  if (ifd_entry.count > 1) {
    char array[VALUESTRLEN];
     snprintf(array, sizeof(array), "but has type:%i and count:%i",ifd_entry.datatype, ifd_entry.count);
    return tif_returns( tag, "of type long, short or float", strdup(array));
  }
  switch (ifd_entry.datatype) {
    case TIFF_LONG: {
                      if (expected_value != ifd_entry.data32) {
                        return tif_returns( tag,  int2str(expected_value),int2str(ifd_entry.data32));
                      }
                      break;
                    }
    case TIFF_SHORT: {
                       if (expected_value != ifd_entry.data16[0])  {
                         return tif_returns(  tag,  int2str(expected_value),int2str(ifd_entry.data16[0]));
                       }
                       break;
                     }
    case TIFF_RATIONAL: {
                          if (0 == ifd_entry.data16[1]) {
                            return tif_returns( tag,  float2str(expected_value),frac2str( ifd_entry.data16[0], ifd_entry.data16[1]));
                            break;
                          } else if (expected_value - (ifd_entry.data16[0] / ifd_entry.data16[1]) > 1) {
                            return tif_returns( tag,  float2str(expected_value), frac2str( ifd_entry.data16[0], ifd_entry.data16[1]));
                            break;
                          }
                        };
    default: { /*  none */
               char array[VALUESTRLEN];
               snprintf(array, sizeof(array), " but was datatype:%u",  ifd_entry.datatype);
               return tif_returns( tag, "of type long, short or float", array );
               break;

             };
  };
  ret_t ret;
  ret.returncode=0; ret.returnmsg=NULL; return ret;
}
