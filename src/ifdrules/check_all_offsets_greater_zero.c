/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015-2017
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <unistd.h>


/* check if offsets are greater zero */
ret_t check_all_offsets_are_greater_zero(ctiff_t * ctif) {
  GET_EMPTY_RET(ret)
  tifp_check( ctif);
  int count = get_ifd0_count( ctif);
  int tagidx;
  for (tagidx = 0; tagidx< count; tagidx++) {
    ifd_entry_t ifd_entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
    uint32 tag = TIFFGetRawTagListEntry( ctif, tagidx);
    uint32 offset=0;
    if (ifd_entry.value_or_offset==is_offset || tag == TIFFTAG_EXIFIFD) {
      if (ifd_entry.value_or_offset==is_offset) offset = ifd_entry.data32offset;
      else if (tag == TIFFTAG_EXIFIFD) {
        if (ifd_entry.count > 1) {
          ret = set_value_found_ret(&ret, int2str(ifd_entry.count));
          ret.returncode = tagerror_expected_count_isgreaterone;
          return ret;
        }
        switch (ifd_entry.datatype) {
          case TIFF_LONG: { /*  correct type */
                            offset = ifd_entry.data32;
                            break;
                          }
          default: { /*  incorrect type for EXIF IFD */
                     ret = set_value_found_ret(&ret, TIFFTypeName(ifd_entry.datatype));
                     ret.returncode = tagerror_unexpected_type_found;
                     return ret;
                     break;
                   };

        }
      } /*  end else if tag == TIFFTAG_EXIFIFD */
      if ( 0 == offset) {
        // FIXME: tif_fails?
        char array[TIFFAILSTRLEN];
        snprintf(array, sizeof(array), "tag %i pointing to 0x%08x", tag, offset);
        ret = set_value_found_ret (&ret, array);
        ret.returncode = tagerror_offset_is_zero;
        return ret;
      }
    }
  }
  ret.returncode=is_valid;
  return ret;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
