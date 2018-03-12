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

ret_t check_tag_has_valuelist(ctiff_t * ctif, tag_t tag, int count, const unsigned int * values) {
  GET_EMPTY_RET(ret)

  tifp_check( ctif);
  ret=check_tag_quiet(ctif, tag);
  if (ret.returncode != is_valid) return ret;

  if (count < 0) {
    if (snprintf(ret.value_found, VALUESTRLEN, "count=%i", count) > 0) {
      ret.returncode = calling_error_count_size;
      return ret;
    } else {
      ret.returncode = could_not_print;
      return ret;
    }
  }
  unsigned int v[count];
  for (int i=0; i< count; i++) {
    v[i] = *values;
    values++;
  }
  char value[VALUESTRLEN];
  memset(value, '\0', VALUESTRLEN);
  ifd_entry_t ifd_entry = TIFFGetRawIFDEntry(ctif, tag);
  if ((uint32) count != ifd_entry.count) {
    snprintf(value, sizeof(value), "has %u values", ifd_entry.count);
    ret = set_value_found_ret (&ret, value);
    ret.returncode = tagerror_expected_count_differs;
    return ret;
  }
  switch (ifd_entry.datatype) {
    case TIFF_LONG: {
                      /*  value */
                      if (ifd_entry.value_or_offset == is_value) {
                        for (int i=0; i< count; i++) {
                          if (v[i] != ifd_entry.data32) {
                            snprintf(value, sizeof(value), "at [%i]=%u", i,  ifd_entry.data32);
                            ret = set_value_found_ret (&ret, value);
                            ret.returncode = tagerror_value_differs;
                            return ret;
                          }
                        }
                      }
                      /*  offset */
                      if (ifd_entry.value_or_offset == is_offset) {
                        offset_t offset;
                        ret = read_offsetdata(ctif, ifd_entry.data32offset, count, ifd_entry.datatype, &offset, &ret);
                        uint32 * p = offset.data32p;
                        for (int i=0; i< count; i++) {
                          uint32 pval = *p;
#ifdef DEBUG
                          printf("OFFSET: v[%i]=%u p[%i]=%u\n", i,v[i],i,pval);
#endif
                          if (v[i] != *p) {
                            snprintf(value, sizeof(value), "at [%i]=%u", i,  pval);
                            ret = set_value_found_ret (&ret, value);
                            ret.returncode = tagerror_value_differs;
                            return ret;
                          }
                          p++;
                        }
                      }
                      break;
                    }
    case TIFF_SHORT: {
                       /*  value */
                       if (ifd_entry.value_or_offset == is_value) {
                         for (int i=0; i< count; i++) {
                           int c = (v[i]) == (ifd_entry.data16[i]);
                           if (!c) {
                             snprintf(value, sizeof(value), "at [%i]=%u", i,  ifd_entry.data16[i]);
                             ret = set_value_found_ret (&ret, value);
                             ret.returncode = tagerror_value_differs;
                             return ret;
                           }
                         }
                       }
                       /*  offset */
                       if (ifd_entry.value_or_offset == is_offset) {
                         offset_t offset;
                         ret = read_offsetdata(ctif, ifd_entry.data32offset, count, ifd_entry.datatype, &offset, &ret);
                         assert( NULL == ret.value_found);
                         uint16 * p = offset.data16p;
                         for (int i=0; i< count; i++) {
                           uint16 pval = *p;
#ifdef DEBUG
                           printf("SHORTOFFSET (tag=%i): v[%i]=%u p[%i]=0x%04x\n", tag, i,v[i],i,pval);
#endif
                           if (v[i] != pval) {
                             snprintf(value, sizeof(value), "at [%i]=%u", i,  pval);
                             ret = set_value_found_ret (&ret, value);
                             ret.returncode = tagerror_value_differs;
                             return ret;
                           }
                           p++;
                         }
                       }
                       break;
                     }
    default: /*  none */
                      {
                        snprintf(value, sizeof(value), "type:%s", TIFFTypeName(ifd_entry.datatype));
                        ret = set_value_found_ret(&ret, value);
                        ret.returncode = tagerror_unexpected_type_found;
                        return ret;
                      }

  }
  ret.returncode=is_valid;
  return ret;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
