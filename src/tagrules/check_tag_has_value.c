/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "../check.h"
#include "../check_helper.h"
#include <pcre.h>
#include <string.h>

/*
#define DEBUG
*/


ret_t check_tag_has_value_quiet(TIFF* tif, tag_t tag, unsigned int value) {
  tifp_check( tif);
    ifd_entry_t ifd_entry = TIFFGetRawIFDEntry(tif, tag);
  if (ifd_entry.count > 1) {
    char array[40];
     snprintf(array, sizeof(array), "but has type:%i and count:%i",ifd_entry.datatype, ifd_entry.count);
    return tif_returns( tag2str(tif, tag), "should have single value of type long, short or float", strdup(array));
  }
  switch (ifd_entry.datatype) {
    case TIFF_LONG: {
                      if (value != ifd_entry.data32) {
                        return tif_returns( tag2str(tif, tag), int2str(ifd_entry.data32), int2str(value)); 
                      }
                      break;
                    }
    case TIFF_SHORT: {
                       if (value != ifd_entry.data16[0])  {
                         return tif_returns(  tag2str(tif, tag), int2str(ifd_entry.data16[0]), int2str(value)); 
                       }
                       break;
                     }
    case TIFF_RATIONAL: {
                          if (0 == ifd_entry.data16[1]) {
                            return tif_returns( tag2str(tif, tag), frac2str( ifd_entry.data16[0], ifd_entry.data16[1]), float2str(value));
                            break;
                          } else if (value - (ifd_entry.data16[0] / ifd_entry.data16[1]) > 1) {
                            return tif_returns( tag2str(tif, tag),  frac2str( ifd_entry.data16[0], ifd_entry.data16[1]), float2str(value));
                            break;
                          }
                        };
    default: { /*  none */
               char array[40];
               snprintf(array, sizeof(array), " but was datatype:%u",  ifd_entry.datatype);
               return tif_returns( tag2str(tif, tag), "should have single value of type long, short or float", array );
               break;

             };
  };
  ret_t ret;
  ret.returncode=0; ret.returnmsg=NULL; return ret;
}

ret_t check_tag_has_value(TIFF* tif, tag_t tag, unsigned int value) {
  printf("check if tag %u (%s) has value %u\n", tag, TIFFTagName(tif, tag), value);
  ret_t ret = check_tag_has_value_quiet( tif, tag, value);
  if (ret.returncode == 0) {
    return ret;
  } else {
    return tif_fails_by_returns( ret );
  }
}

ret_t check_tag_has_value_matching_regex(TIFF* tif, tag_t tag, const char * regex_string) {
  printf("check if tag %u (%s) has value matching regex '%s'\n", tag, TIFFTagName(tif, tag), regex_string);
  tifp_check( tif);
    TIFFDataType datatype =  TIFFGetRawTagType( tif, tag );
  switch (datatype) {
    case TIFF_ASCII: {
                       char * val;
                       uint32 count;
                       int found=TIFFGetField(tif, tag, &val, &count);
                       if (1 == found) {
                         count = strlen( val);
#define OVECCOUNT 30    /* should be a multiple of 3 */
                         pcre *re;
                         int erroffset;
                         const char * errorcode;
                         re = pcre_compile(
                             regex_string,                /* the pattern */
                             0,                      /* default options */
                             &errorcode,             /* for error code */
                             &erroffset,             /* for error offset */
                             NULL);                  /* no compile context */
                         if (NULL != re) {

                           int ovector[OVECCOUNT];
                           /*  PCRE_NOTEMPTY: An empty string is not a valid match */
                           int rc = pcre_exec( re, NULL, val, count, 0,PCRE_NOTEMPTY , ovector, OVECCOUNT);
#ifdef DEBUG
                           printf("tag %s with count=%d and value='%s' -> rc=%d\n", TIFFTagName(tif, tag), count, val, rc);
#endif
                           pcre_free( re );
                           if (rc >= 0 ) {
                             ret_t res;
                             res.returnmsg=NULL;
                             res.returncode=0;
                             return res;
                           } else {
                             switch(rc) {
                               case PCRE_ERROR_NOMATCH: 
                                 return tif_fails_tag( tag2str(tif, tag), strdup(regex_string), val);
                                 break;
                                 /*
                                    Handle other special cases if you like
                                    */
                               default: 
                                 return tif_fails_tag( tag2str(tif, tag), strdup(regex_string), val);
                                 // tif_fails("tag %u with value '%s' called regex '%s' with matching error %d\n", tag, val, regex_string, rc); 
                                 break;
                             }
                         }
                         } else {
                            char array[80];
                            snprintf(array, sizeof(array), "regex '%s' compile error: %s at offset: %i\n",regex_string, errorcode, erroffset);
                           return tif_fails(array);
                         }
                       } else {
                         return tif_fails_tag( tag2str(tif, tag), "should exist, because defined", "");
                       }
                     }
    default:  /*  none */
                     {
                     char array[10];
                     snprintf(array, sizeof(array), "but was:%i\n", datatype);
                     return tif_fails_tag( tag2str(tif, tag), "should have value of type ASCII", array);
                     }
  }
}

ret_t check_tag_has_valuelist(TIFF* tif, tag_t tag, int count, unsigned int * values) {
  printf("check if tag %u (%s) has these %i-values", tag, TIFFTagName(tif, tag), count);
  tifp_check( tif);
  int i;
  unsigned int v[count];
  for (i=0; i< count; i++) {
    v[i] = *values;
    printf (", %i", v[i]);
    values++;
  }
  printf("\n");
  ifd_entry_t ifd_entry = TIFFGetRawIFDEntry(tif, tag);
  if (count != ifd_entry.count) {
    char expected[10];
    snprintf(expected, sizeof(expected), "list has %u values", count);
    char value[10];
    snprintf(value, sizeof(value), "has %u values", ifd_entry.count);
    return tif_fails_tag( tag2str(tif, tag), strdup(expected), strdup(value));
  }
  ret_t res;
  res.returncode=0;
  res.returnmsg=NULL;
  switch (ifd_entry.datatype) {
    case TIFF_LONG: {
                      /*  value */
                      if (ifd_entry.value_or_offset == is_value) {
                        for (i=0; i< count; i++) {
                          if (v[i] != ifd_entry.data32) {
                            char expected[10];
                            snprintf(expected, sizeof(expected), "value[%u]=%u", i, v[i]);
                            char value[10];
                            snprintf(value, sizeof(value), "has value[%u]=%u", i,  ifd_entry.data32);
                            return tif_fails_tag( tag2str(tif, tag), strdup(expected), strdup(value));
                            //tif_fails_tag( tag2str(tif, tag), "tag %u (%s), tagvalue[%i]=%u differs from value=%u (long)\n",  tag, TIFFTagName(tif, tag), i, ifd_entry.data32, v[i]); 
                          }
                        }
                      }
                      /*  offset */
                      if (ifd_entry.value_or_offset == is_offset) {
                        offset_t offset = read_offsetdata(tif, ifd_entry.data32offset, count, ifd_entry.datatype);
                        uint32 * p = offset.data32p;
                        for (i=0; i< count; i++) {
                          uint32 pval = *p;
                          if (TIFFIsByteSwapped(tif))
                            TIFFSwabLong(&pval);
#ifdef DEBUG
                          printf("OFFSET: v[%i]=%u p[%i]=%u\n", i,v[i],i,pval);
#endif
                          if (v[i] != *p) {
                            char expected[10];
                            snprintf(expected, sizeof(expected), "value[%u]=%u", i, v[i]);
                            char value[10];
                            snprintf(value, sizeof(value), "has value[%u]=%u", i,  pval);
                            return tif_fails_tag( tag2str(tif, tag), strdup(expected), strdup(value));
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
                         for (i=0; i< count; i++) {
                           int c = (v[i]) == (ifd_entry.data16[i]);
                           if (!c) {
                             char expected[10];
                             snprintf(expected, sizeof(expected), "value[%u]=%u", i, v[i]);
                             char value[10];
                             snprintf(value, sizeof(value), "has value[%u]=%u", i,  ifd_entry.data16[i]);
                             return tif_fails_tag( tag2str(tif, tag), strdup(expected), strdup(value));
                             //tif_fails("tag %u (%s), tagvalue[%i]=%u differs from value[%i]=%u (short)\n",  tag, TIFFTagName(tif, tag), i, ifd_entry.data16[i], i, v[i]); 
                           }
                         }
                       }
                       /*  offset */
                       if (ifd_entry.value_or_offset == is_offset) {
                         offset_t offset = read_offsetdata(tif, ifd_entry.data32offset, count, ifd_entry.datatype);
                         uint16 * p = offset.data16p;
                         for (i=0; i< count; i++) {
                           uint16 pval = *p;
                           if (TIFFIsByteSwapped(tif))
                             TIFFSwabShort(&pval);
#ifdef DEBUG
                           printf("OFFSET: v[%i]=%u p[%i]=%u\n", i,v[i],i,pval);
#endif
                           if (v[i] != pval) {
                             char expected[10];
                             snprintf(expected, sizeof(expected), "value[%u]=%u", i, v[i]);
                             char value[10];
                             snprintf(value, sizeof(value), "has value[%u]=%u", i,  pval);
                             return tif_fails_tag( tag2str(tif, tag), strdup(expected), strdup(value));
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
                        char array[10];
                        snprintf(array, sizeof(array), "but was:%i\n", ifd_entry.datatype);
                        return tif_fails_tag( tag2str(tif, tag), "should have values of type long, short or float", array);
                      }

  }

}

