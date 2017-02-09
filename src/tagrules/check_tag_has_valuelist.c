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
  ret_t res;
  res.returnmsg=NULL;
  res.returncode=0;
  tifp_check( ctif);
  if (count < 0) {
    perror ("count should be positive!");
    exit( EXIT_FAILURE );
  }
  char msg[EXPECTSTRLEN];
  snprintf(msg, sizeof(msg), "has these %i-values: ", count);
  unsigned int * p = values;
  for (int i=0; i< count; i++) {
    if (0 < i) { secstrcat (msg, ", ", EXPECTSTRLEN);
    }
    secstrcat (msg, int2str(*p), EXPECTSTRLEN);
    p++;
  }
  tif_rules_tag(tag, strdup(msg));
  res = check_tag_has_valid_type( ctif, tag);
  if (res.returncode == 0) {

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
      return tif_fails_tag( tag, strdup(expected), strdup(value));
    }
    switch (ifd_entry.datatype) {
      case TIFF_LONG: {
                        /*  value */
                        if (ifd_entry.value_or_offset == is_value) {
                          for (int i=0; i< count; i++) {
                            if (v[i] != ifd_entry.data32) {
                              char expected[EXPECTSTRLEN];
                              snprintf(expected, sizeof(expected), "at [%u]=%u", i, v[i]);
                              char value[VALUESTRLEN];
                              snprintf(value, sizeof(value), "at [%u]=%u", i,  ifd_entry.data32);
                              return tif_fails_tag( tag, strdup(expected), strdup(value));
                              //tif_fails_tag( tag2str(tif, tag), "tag %u (%s), tagvalue[%i]=%u differs from value=%u (long)\n",  tag, TIFFTagName(tif, tag), i, ifd_entry.data32, v[i]);
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
                              char expected[EXPECTSTRLEN];
                              snprintf(expected, sizeof(expected), "at [%u]=%u", i, v[i]);
                              char value[VALUESTRLEN];
                              snprintf(value, sizeof(value), "at [%u]=%u", i,  pval);
                              return tif_fails_tag( tag, strdup(expected), strdup(value));
                            }
                            p++;
                          }
                        }

                        return res;
                        break;
                      }
      case TIFF_SHORT: {
                         /*  value */
                         if (ifd_entry.value_or_offset == is_value) {
                           for (int i=0; i< count; i++) {
                             int c = (v[i]) == (ifd_entry.data16[i]);
                             if (!c) {
                               char expected[EXPECTSTRLEN];
                               snprintf(expected, sizeof(expected), "at [%u]=%u", i, v[i]);
                               char value[VALUESTRLEN];
                               snprintf(value, sizeof(value), "at [%u]=%u", i,  ifd_entry.data16[i]);
                               return tif_fails_tag( tag, strdup(expected), strdup(value));
                               //tif_fails("tag %u (%s), tagvalue[%i]=%u differs from value[%i]=%u (short)\n",  tag, TIFFTagName(tif, tag), i, ifd_entry.data16[i], i, v[i]);
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
                               char expected[EXPECTSTRLEN];
                               snprintf(expected, sizeof(expected), "at [%u]=%u", i, v[i]);
                               char value[VALUESTRLEN];
                               snprintf(value, sizeof(value), "at [%u]=%u", i,  pval);
                               return tif_fails_tag( tag, strdup(expected), strdup(value));
                               // tif_fails("tag %u (%s), tagvalue[%i]=%u differs from value=%u (short offset)\n",  tag, TIFFTagName(tif, tag), i, pval, v[i]);
                             }
                             p++;
                           }
                         }

                         return res;
                         break;
                       }
      default: /*  none */
                       {
                         char array[VALUESTRLEN];
                         snprintf(array, sizeof(array), "type:%i", ifd_entry.datatype);
                         return tif_fails_tag( tag, "of type long, short or float", array);
                       }

    }
  }
  return tif_fails_by_returns( res );
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
