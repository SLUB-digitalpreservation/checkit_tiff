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

ret_t check_tag_has_valuelist(ctiff_t * ctif, tag_t tag, int count, unsigned int * values) {
  ret_t ret;
  ret.value_found = malloc(VALUESTRLEN);
  if (NULL == ret.value_found) {
    ret.returncode=could_not_allocate_memory;
    return ret;
  }

  tifp_check( ctif);

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
  ifd_entry_t ifd_entry = TIFFGetRawIFDEntry(ctif, tag);
  if (count != ifd_entry.count) {
    char expected[EXPECTSTRLEN];
    snprintf(expected, sizeof(expected), "list has %u values", count);
    char value[VALUESTRLEN];
    snprintf(value, sizeof(value), "has %u values", ifd_entry.count);
    ret.value_found = strncpy(ret.value_found, value, VALUESTRLEN);
    ret.returncode = tagerror_expected_count_differs;
    return ret;
  }
  switch (ifd_entry.datatype) {
    case TIFF_LONG: {
                      /*  value */
                      if (ifd_entry.value_or_offset == is_value) {
                        for (int i=0; i< count; i++) {
                          if (v[i] != ifd_entry.data32) {
                            char value[VALUESTRLEN];
                            snprintf(value, sizeof(value), "at [%u]=%u", i,  ifd_entry.data32);
                            ret.value_found = strncpy(ret.value_found, value, VALUESTRLEN);
                            ret.returncode = tagerror_value_differs;
                            return ret;
                          }
                        }
                      }
                      /*  offset */
                      if (ifd_entry.value_or_offset == is_offset) {
                        offset_t offset = read_offsetdata(ctif, ifd_entry.data32offset, count, ifd_entry.datatype);
                        uint32 * p = offset.data32p;
                        for (int i=0; i< count; i++) {
                          uint32 pval = *p;
#ifdef DEBUG
                          printf("OFFSET: v[%i]=%u p[%i]=%u\n", i,v[i],i,pval);
#endif
                          if (v[i] != *p) {
                            char value[VALUESTRLEN];
                            snprintf(value, sizeof(value), "at [%u]=%u", i,  pval);
                            ret.value_found = strncpy(ret.value_found, value, VALUESTRLEN);
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
                             char value[VALUESTRLEN];
                             snprintf(value, sizeof(value), "at [%u]=%u", i,  ifd_entry.data16[i]);
                             ret.value_found = strncpy(ret.value_found, value, VALUESTRLEN);
                             ret.returncode = tagerror_value_differs;
                             return ret;
                           }
                         }
                       }
                       /*  offset */
                       if (ifd_entry.value_or_offset == is_offset) {
                         offset_t offset = read_offsetdata(ctif, ifd_entry.data32offset, count, ifd_entry.datatype);
                         uint16 * p = offset.data16p;
                         for (int i=0; i< count; i++) {
                           uint16 pval = *p;
#ifdef DEBUG
                           printf("SHORTOFFSET (tag=%i): v[%i]=%u p[%i]=0x%04x\n", tag, i,v[i],i,pval);
#endif
                           if (v[i] != pval) {
                             char value[VALUESTRLEN];
                             snprintf(value, sizeof(value), "at [%u]=%u", i,  pval);
                             ret.value_found = strncpy(ret.value_found, value, VALUESTRLEN);
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
                        char array[VALUESTRLEN];
                        snprintf(array, sizeof(array), "type:%s", TIFFTypeName(ifd_entry.datatype));
                        ret.value_found = strncpy( ret.value_found, array, VALUESTRLEN);
                        ret.returncode = tagerror_unexpected_type_found;
                        return ret;
                      }

  }
  ret.returncode=should_not_occure;
  return ret;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
