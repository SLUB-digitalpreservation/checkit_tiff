/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015-2017
 * licensed under conditions of libtiff
 * (see file LICENSE)
 *
 */

#define _GNU_SOURCE
#include <math.h>
#include "check.h"
#include "check_helper.h"
#include "msg_tiffparse.h"
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#ifdef _HAVE__MMAP
#include <sys/mman.h>
#endif

#include <errno.h>
/*
#define DEBUG
*/

off_t ct_seek(ctiff_t * ctif, off_t pos, int whence) {
#ifdef _HAVE__MMAP
  switch (ctif->ioflag) {
    case is_filep: 
      assert(ctif->fd >= 0);
      // TODO: add checks if seek will be outside of file!!!
      return lseek(ctif->fd, pos, whence);
      break;

    case  is_memmap:
      assert( ctif->streamp != NULL);
      assert( ctif->actual_streamp != NULL);
      switch (whence) {
        case SEEK_SET: 
          ctif->actual_streamp = ctif->streamp + pos;
          break;
        case SEEK_CUR:
          ctif->actual_streamp+=pos;
          break;
        case SEEK_END:
          ctif->actual_streamp = ctif->streamp + ctif->streamlen + pos;
          break;
      }
      int testpos = ctif->actual_streamp - ctif->streamp;
      if ( testpos > ctif->streamlen) {
        /*
         * fprintf(stderr, "seek offset outside of file on new pos=%i (filesize=%i)\n", testpos, ctif->streamlen);
         * exit(EXIT_FAILURE);
         */
        return -1;
      }
      if ( testpos < 0 ) {
        /*
         * fprintf(stderr, "seek offset outside of file on new pos=%i (filesize=%i)\n", testpos, ctif->streamlen);
         * exit(EXIT_FAILURE);
         */
        return -1;
      }

      return ctif->actual_streamp - ctif->streamp;
      break;
  }
#else
  assert(ctif->fd >= 0);
  // TODO: add checks if seek will be outside of file!!!
  return lseek(ctif->fd, pos, whence);
#endif
}

ssize_t ct_read(ctiff_t * ctif, void *buf, size_t count) {
#ifdef _HAVE__MMAP
  switch (ctif->ioflag) {
    case is_filep: 
      // TODO: add checks if seek will be outside of file!!!
      assert(ctif->fd >= 0);
      return read(ctif->fd, buf, count);
      break;
    case  is_memmap: {
                       assert( ctif->streamp != NULL);
                       assert( ctif->actual_streamp != NULL);

                       int testpos = (ctif->actual_streamp+count) - (ctif->streamp);
                       if ( testpos > ctif->streamlen) {
                         /*
                          * fprintf(stderr, "read offset outside of file on new pos=%i (filesize=%i)\n", testpos, ctif->streamlen);
                          * exit(EXIT_FAILURE);
                          */
                         return -1;
                       }
                       if ( testpos < 0 ) {
                         /*
                          * fprintf(stderr, "read offset outside of file on new pos=%i (filesize=%i)\n", testpos, ctif->streamlen);
                          * exit(EXIT_FAILURE);
                          */
                         return -1;
                       }
                       memcpy(buf, ctif->actual_streamp, count);
                       ctif->actual_streamp+=count;
                       return count;
                       /* break; */
                     }
  }
#else
  // TODO: add checks if seek will be outside of file!!!
  assert(ctif->fd >= 0);
  return read(ctif->fd, buf, count);
#endif

}


int TIFFGetRawTagListIndex(ctiff_t * ctif, tag_t tag) { /* find n-th entry in IFD for given tag, return -1 if not found */
  /*
     for (int i= 0; i < get_ifd0_count( ctif ); i++) {
     if (tag > 253 && tag == TIFFGetRawTagListEntry( ctif, i )) {
     return i;
     };
     }
     */

  switch (ctif->tagorder) {
    case unknown_tag_order:
      {
        ctif->tagorder=has_sorted_tags;
        int count = get_ifd0_count( ctif);
        if (0 == count) { return -1; }
        tag_t last = TIFFGetRawTagListEntry(ctif, 0);
        for (int i= 1; i < get_ifd0_count( ctif ); i++) {
         tag_t current = TIFFGetRawTagListEntry( ctif, i );
          if (last >= current) {
            ctif->tagorder=has_unsorted_tags;
            break;
          }
          last = current;
        };
        /* if (has_sorted_tags == ctif->tagorder) { */
          for (int i= 0; i < get_ifd0_count( ctif ); i++) {
            tag_t tag = TIFFGetRawTagListEntry( ctif, i );
            ctif->tag_cache[tag] = i;
          }
          return ctif->tag_cache[tag];
        /* } */
      }
      break;
    case has_unsorted_tags:
    case has_sorted_tags:
      return ctif->tag_cache[tag];
  }
  return -1;
}


//------------------------------------------------------------------------------
ret_t check_tag_has_fvalue(ctiff_t * ctif, tag_t tag, float value) {
  GET_EMPTY_RET(ret)
  tifp_check( ctif);

  float * valp = NULL;
  uint32 found;
  ret =TIFFGetFieldRATIONAL(ctif, tag, &valp, &found);
  if (1 == found) {
    float val = * valp;
    if ( fabs(val - value) < 0.01 ) {
      ret.returncode=is_valid;
      return ret;
    } else {
      ret = set_value_found_ret(&ret, float2str(val)) ;
      ret.returncode = tagerror_value_differs;
    }
  }
  return ret;
}

//------------------------------------------------------------------------------
ret_t check_tag_has_u16value(ctiff_t * ctif, tag_t tag, uint16 value) {
  GET_EMPTY_RET(ret)
  tifp_check( ctif);

  uint16 * valp = NULL;
  uint32 found;
  ret=TIFFGetFieldSHORT(ctif, tag, &valp, &found);
  if (1 == found) {
    uint16 val  = *valp;
    if ( val == value ) {
      ret.returncode=is_valid;
      return ret;
    } else {
      ret = set_value_found_ret(&ret, int2str(val));
      ret.returncode = tagerror_value_differs;
      return ret;
    }

  }
  return ret;
}


//------------------------------------------------------------------------------
ret_t check_tag_has_u32value(ctiff_t * ctif, tag_t tag, uint32 value) {
  GET_EMPTY_RET(ret)

  tifp_check( ctif);

  uint32 * valp = NULL;
  uint32 found;
  ret=TIFFGetFieldLONG(ctif, tag, &valp, &found);
  if (1 == found) {
    uint32 val = *valp;
    if ( val == value )  {
      ret.returncode=is_valid;
      return ret;
    } else {
      ret = set_value_found_ret(&ret, int2str(val));
      ret.returncode = tagerror_value_differs;
      return ret;
    }
  }
  return ret;
}


ret_t parse_header_and_endianess(ctiff_t * ctif) {
  GET_EMPTY_RET(ret)

   /* seek the image file directory (bytes 4-7) */
  //ct_seek(fd, (off_t) 0, SEEK_SET);
  if ( ct_seek(ctif, 0, SEEK_SET) != 0)  {
    ret = set_value_found_ret(&ret, "TIFF header ct_seek error to 0");
    ret.returncode=tiff_seek_error_header;
    return ret;
  }
  uint16 header;
  uint16 magic;
  int endianness;
  if ( ct_read( ctif, &header, 2) != 2 ) {
    ret = set_value_found_ret(&ret, "TIFF Header ct_read error to magic byte header (first 2 bytes)");
    ret.returncode=tiff_read_error_header;
    return ret;
  }
  if (header == 0x4949) endianness = 0; /* little endian */
  else if (header == 0x4d4d) endianness = 1; /*  big endian */
  else {
    char errmsg[VALUESTRLEN]="";
    snprintf (errmsg, VALUESTRLEN, "TIFF Header error, not Byte Order Bytes for TIFF: 0x%04x", header);
    if (header == 0x4550) {
      strcat(errmsg, ", could be a Microsoft Document Image file (little endian), if header begins with by 0x45 0x50 0x2a 0x00");
    }
    ret=set_value_found_ret(&ret, errmsg);
    ret.returncode = tiff_byteorder_error;
    return ret;
  }
  ctif->isbyteswapped = endianness;
  if ( ct_read( ctif, &magic, 2) != 2 ) {
    ret = set_value_found_ret(&ret, "TIFF Header ct_read error to magic byte header (second 2 bytes == 42)");
    ret.returncode=tiff_read_error_header;
    return ret;
  }

  uint16 magic2 = magic;
  if (endianness) TIFFSwabShort( &magic2 ); /*  big endian */
  if (magic2 == 42) { ret.returncode=is_valid; return ret; }
  else {
    char errmsg[VALUESTRLEN]="";
    snprintf (errmsg, VALUESTRLEN, "TIFF Header error, not a MAGIC BYTE for TIFF: 0x%04x\n", magic);
    if (magic2==0x002b) strcat(errmsg, ", but could be a BigTIFF, see http://www.awaresystems.be/imaging/tiff/bigtiff.html");
    if (magic2==0x0055) strcat(errmsg, ", but could be a Panasonic Raw/RW2, see http://libopenraw.freedesktop.org/wiki/Panasonic_RAW/");
    if (magic2==0x01bc) strcat(errmsg, ", but could be a JPEG XR, see http://www.itu.int/rec/T-REC-T.832");
    if (magic2==0x314e) strcat(errmsg, ", but could be a Navy Image FileFormat, see http://www.navsea.navy.mil/nswc/carderock/tecinfsys/cal-std/doc/28002c.pdf");
    if (magic2==0x4352) strcat(errmsg, ", but could be a DNG camera profile, see http://wwwimages.adobe.com/www.adobe.com/content/dam/Adobe/en/products/photoshop/pdfs/dng_spec_1.4.0.0.pdf");
    if (magic2==0x4f52) strcat(errmsg, ", but could be an Olympus ORF, see http://libopenraw.freedesktop.org/wiki/Olympus_ORF/");
    if (magic2==0x5352) strcat(errmsg, ", but could be an Olympus ORF, see http://libopenraw.freedesktop.org/wiki/Olympus_ORF/");
    ret = set_value_found_ret(&ret, errmsg);
    ret.returncode = tiff_byteorder_error;
    return ret;
  }
  ret.returncode = should_not_occur;
  assert( ret.returncode != should_not_occur);
  return ret;
}

ret_t get_first_IFD(ctiff_t * ctif, uint32 * ifd) {
  GET_EMPTY_RET(ret)
  int isByteSwapped = ctif->isbyteswapped;
  /* seek the image file directory (bytes 4-7) */
  if (ct_seek(ctif, 4, SEEK_SET) != 4 ) {
    ret = set_value_found_ret(&ret, "TIFF Header seak error, seek set to byte 4");
    ret.returncode=tiff_seek_error_header;
    return ret;
  }
  uint32 offset;
  if ( ct_read( ctif, &offset, 4) != 4 ) {
    ret = set_value_found_ret(&ret, "TIFF Header ct_read error, reading 4 bytes from 4");
    ret.returncode=tiff_read_error_header;
    return ret;
  }
  if (isByteSwapped) {
    TIFFSwabLong (&offset);
  }
  if (offset <= 7) {
    char msg[VALUESTRLEN];
    snprintf(msg, VALUESTRLEN, "pointer to IFD0 is %u", offset);
    ret=set_value_found_ret(&ret, msg);
    ret.returncode=tiff_ifd0_offset_must_be_greater_than_eight;
    return ret;
  }
  ctif->ifd0pos=offset;
  if (ct_seek(ctif, offset, SEEK_SET) != offset ) {
    char msg[VALUESTRLEN];
    snprintf(msg, VALUESTRLEN, "TIFF Header seek error, seek set to byte %u", offset);
    ret=set_value_found_ret(&ret, msg);
    ret.returncode=tiff_seek_error_header;
    return ret;
  }
  uint16 count;
  if ( ct_read( ctif, &count, 2) != 2 ) {
    ret=set_value_found_ret(&ret, "TIFF Header ct_read error2, reading ifd0 count (2 bytes)");
    ret.returncode=tiff_read_error_header;
    return ret;
  }

  if (is_byteswapped(ctif))
    TIFFSwabShort(&count);
  ctif->ifd0c = count;
  *ifd = offset;
  ret.returncode=is_valid;
  return ret;
}


/* scans first IDF and returns the n-th tag */
tag_t TIFFGetRawTagListEntry( ctiff_t * ctif, int tagidx ) {
  int byteswapped = is_byteswapped(ctif);
  int count = get_ifd0_count( ctif);
  assert( count > 0);
  /* ct_read count of tags (2 Bytes) */
  /* replace i/o operatrions with in-memory-operations */
  uint8 * ifdentries = NULL;
  ifdentries = malloc ( sizeof(uint8) * 12 * count);
  ct_seek(ctif, ctif->ifd0pos+2, SEEK_SET); /* IFD0 plus 2byte to get IFD-entries */
  if ( ct_read( ctif, ifdentries, 12 * count) != 12*count ) {
    /*  FIXME: replace perror/exit with own error handling routine */
    perror ("TIFF Header ct_read error3");
    exit( EXIT_FAILURE );
  }

  uint8 * e = ifdentries;
  uint16 ret = 0;
  /*
     for (i = 0; i<count; i++) {
     uint8 lo = *e;
     e++;
     uint8 hi = *e;
     e++;
     if (i == tagidx) {
     uint16 tagid = (hi << 8) + lo;
     if (byteswapped)
     TIFFSwabShort(&tagid);
  // printf("tag idx=%i, tag=%u (0x%04x) (0x%02x) (0x%02x)\n", i, tagid, tagid, hi, lo);
  ret = tagid;
  goto LABEL1;
  }
  e+=10;
  }
  */
  assert(tagidx <= count);
  assert(tagidx >= 0);
  e+=(12*tagidx);
  uint8 lo = *e;
  e++;
  uint8 hi = *e;
  e++;
  uint16 tagid = (hi << 8) + lo;
  if (byteswapped)
    TIFFSwabShort(&tagid);
  //printf("tag idx=%i, tag=%u (0x%04x) (0x%02x) (0x%02x)\n", i, tagid, tagid, hi, lo);
  ret = tagid;
  // LABEL1:
  /* loop each tag until end or given tag found */
  free( ifdentries );
  return ret;
}

#define OFFSET_MALLOC(ctif_p, offsetdata, offset_type, count ) {\
  if(ctif_p->streamlen < sizeof(offset_type) * count) {\
    char msg[VALUESTRLEN]; \
    snprintf(msg, VALUESTRLEN,  "TIFF Offset ct_read error, try to read from offset count=%lu bytes, but file has size=%u\n", sizeof(offset_type) * count, ctif_p->streamlen); \
    *ret_p  = set_value_found_ret( ret_p, msg); \
    ret_p->returncode = tiff_seek_error_offset;\
    return * ret_p;\
  }\
  offsetdata = NULL; offsetdata = malloc ( sizeof(offset_type) * count);\
  if (NULL == offsetdata) {\
    fprintf( stderr, "could not allocate memory for offset_t\n");\
    exit (EXIT_FAILURE);\
  }\
  if ( ct_read( ctif_p, offsetdata, sizeof(offset_type) * count) != (sizeof(offset_type) *count) ) {\
       /*  fprintf(stderr, "TIFF Offset ct_read error, try to read from offset count=%lu bytes\n", sizeof(offset_type) * count); */\
       /*  exit( EXIT_FAILURE ); */\
    char msg[VALUESTRLEN]; \
    snprintf(msg, VALUESTRLEN,  "TIFF Offset ct_read error, try to read from offset count=%lu bytes\n", sizeof(offset_type) * count); \
    *ret_p  = set_value_found_ret( ret_p, msg); \
    ret_p->returncode = tiff_read_error_offset; \
    return *ret_p; \
  }\
}

void offset_swabshort(ctiff_t * ctif, uint16 * address, uint16 count) {
  if (is_byteswapped(ctif))
    for (int i=0; i<count; i++, address++) {
      TIFFSwabShort( address );
    }
}

void offset_swablong(ctiff_t * ctif, uint32 * address, uint16 count) {
  if (is_byteswapped(ctif))
    for (int i=0; i<count; i++, address++) {
      TIFFSwabLong( address );
    }
}
/*  get count-data datastream from offset-address */
ret_t read_offsetdata(ctiff_t * ctif, const uint32 address, const uint32 count, const uint16 datatype, offset_t * offset_p, ret_t * ret_p) {
  assert(NULL !=  offset_p);
  assert(NULL != ret_p);
  offset_p->count = count;
  offset_p->datatype = datatype;
  ret_p->returncode = is_valid;
  
  /* ct_read and seek to IFD address */
  if (ct_seek(ctif, address, SEEK_SET) != address) {
    offset_p->count = -1;
    ret_p->returncode = tiff_seek_error_offset;
    return * ret_p;
  }
#ifdef DEBUG
  printf("read_offsetdata(tif, address=%u, count=%u, datatype=%u)\n", address, count, datatype);
#endif
  switch (datatype) {
    case 1: /* 8-bit unsigned integer */
    case 7: /* !8-bit untyped data */
      /*
      offset_p->data8p = NULL;
      offset_p->data8p = malloc ( sizeof(uint8) * count);
      if (ct_read(fd, offset_p->data8p,  sizeof(uint8) * count) != sizeof(uint8) *count)
        perror ("TIFF Offset ct_read error");
      */
      OFFSET_MALLOC(ctif, offset_p->data8p, uint8, count)
      break;
    case 2: /* 8-bit bytes w/ last byte null */
      assert( sizeof(char) == sizeof(uint8));
      OFFSET_MALLOC(ctif, offset_p->datacharp, char, count)
      break;
    case 6: /* !8-bit signed integer */
      OFFSET_MALLOC(ctif, offset_p->datas8p, int8, count)
      break;
    case 3: /* 16-bit unsigned integer */
      OFFSET_MALLOC(ctif, offset_p->data16p, uint16, count)
      offset_swabshort(ctif, offset_p->data16p, count);
      break;
    case 8: /* !16-bit signed integer */
      OFFSET_MALLOC(ctif, offset_p->datas16p, int16, count)
      offset_swabshort(ctif, (uint16 *) offset_p->datas16p, count);
      break;
    case 4: /* 32-bit unsigned integer */
    case 13: /* %32-bit unsigned integer (offset) */
      OFFSET_MALLOC(ctif, offset_p->data32p, uint32, count)
      offset_swablong(ctif, offset_p->data32p, count);
      break;
    case 9: /* !32-bit signed integer */
      OFFSET_MALLOC(ctif, offset_p->datas32p, uint32, count)
      offset_swablong(ctif, (uint32 *) offset_p->data32p, count);
      break;
    case 5: /* 64-bit unsigned fraction */
      OFFSET_MALLOC(ctif, offset_p->data32p, uint32, 2*count) /* because numerator + denominator */
      offset_swablong(ctif, offset_p->data32p, 2*count);
      break;
    case 10: /* !64-bit signed fraction */
      fprintf(stderr, "offsetdata datatype=%i not supported yet", datatype);
      exit(EXIT_FAILURE);
    case 11: /* !32-bit IEEE floating point */
      assert( sizeof(float) == 4);
      OFFSET_MALLOC(ctif, offset_p->datafloatp, float, count)
      break;
    case 12: /* !64-bit IEEE floating point */
      assert( sizeof(double) == 8);
      OFFSET_MALLOC(ctif, offset_p->datadoublep, double, count)
      break;
    case 16: /* BigTIFF 64-bit unsigned integer */
    case 18: /* BigTIFF 64-bit unsigned integer (offset) */
      assert( sizeof(double) == 8);
      OFFSET_MALLOC(ctif, offset_p->data64p, uint64, count)
      break;
    case 17: /* BigTIFF 64-bit signed integer */
      assert( sizeof(double) == 8);
      OFFSET_MALLOC(ctif, offset_p->datas64p, int64, count)
        break;
    default: /*  should not occur */
      {
        char msg[VALUESTRLEN];
        snprintf(msg, VALUESTRLEN, "offsetdata datatype=%i not supported yet", datatype);
        *ret_p = set_value_found_ret(ret_p, msg);
        ret_p->returncode = should_not_occur;
        assert( ret_p->returncode != should_not_occur);
        return *ret_p;
      }
  };
#ifdef DEBUG
  printf ("is valid offset\n");
  printf ("RET=%s\n", get_parser_error_description(ret_p->returncode));
#endif
  assert( NULL == ret_p->value_found);
  return *ret_p;
}

/* scans first IDF and returns the type of the n-th tag */
ifd_entry_t TIFFGetRawTagIFDListEntry( ctiff_t * ctif, int tagidx ) {
  int tagcount = get_ifd0_count( ctif);
  assert( tagcount > 0);
  int byteswapped = is_byteswapped(ctif);
#ifdef DEBUG
  printf(" count of tags = %i\n", tagcount);
#endif
  // int fd = TIFFFileno( tif);
  //printf("count %i\n", count);
  /* ct_read count of tags (2 Bytes) */
  ifd_entry_t ifd_entry;
  ifd_entry.value_or_offset = is_error;
  /* replace i/o operatrions with in-memory-operations */
  uint8 * ifdentries = NULL;
  ifdentries = malloc ( sizeof(uint8) * 12 * tagcount);
  ct_seek(ctif, ctif->ifd0pos+2, SEEK_SET); /* IFD0 plus 2byte to get IFD-entries */

  if ( ct_read( ctif, ifdentries, 12 * tagcount) != 12*tagcount ) {
    /*  FIXME: replace perror/exit with own error handling routine */
	  perror ("TIFF Header ct_read error4");
	  exit( EXIT_FAILURE );
  }
  uint8 * e = ifdentries;
  
  for (int i = 0; i<tagcount; i++) {
    uint8 * unloop_e = e;
    uint8 * unloop_f = e+1;
    uint8 * unloop_g = e+2;
    uint8 * unloop_h = e+3;
    uint8 lo = *unloop_e;
    uint8 hi = *unloop_f;
    uint16 tagid = (hi << 8) + lo;
    e+=2;
    if (byteswapped)
      TIFFSwabShort(&tagid);
    if (i == tagidx) {
      // tag type check
      uint8 * unloop_e = e;
      uint8 * unloop_f = e+1;
      lo = *unloop_e; 
      hi = *unloop_f; 
      e+=2;
      uint16 tagtype = (hi << 8) + lo;
      if (byteswapped)
        TIFFSwabShort(&tagtype);
      unloop_e=e;
      unloop_f=e+1;
      unloop_g=e+2;
      unloop_h=e+3;
      uint8 data0 = *unloop_e;
      uint8 data1 = *unloop_f;
      uint8 data2 = *unloop_g;
      uint8 data3 = *unloop_h;
      uint32 count = data0;
      count += (data1 << 8);
      count += (data2 << 16);
      count += (data3 << 24);
      e+=4;
      if (byteswapped)
        TIFFSwabLong( &count);
#ifdef DEBUG
printf("\ncount=%0x\n\n", count);
#endif

      /*  is value or offset? */
      /*  TODO */
      ifd_entry.count=count;
      ifd_entry.datatype=tagtype;
      unloop_e = e;
      unloop_f=e+1;
      unloop_g=e+2;
      unloop_h=e+3;


      data0 = *unloop_e;
      data1 = *unloop_f;
      data2 = *unloop_g;
      data3 = *unloop_h;
      uint32 value_or_offset = (data0);
      value_or_offset += (data1 << 8);
      value_or_offset += (data2 << 16);
      value_or_offset += (data3 << 24);
      e+=4;
      if (byteswapped)
        TIFFSwabLong( &value_or_offset);
      switch( tagtype) {
        case 1: /* 8-bit unsigned integer */
        case 2: /* 8-bit bytes w/ last byte null */
        case 6: /* !8-bit signed integer */
        case 7: /* !8-bit untyped data */
          if (count > 4) { /* offset */
            ifd_entry.value_or_offset=is_offset;
            ifd_entry.data32offset=value_or_offset;
          } else { /*  values */
            ifd_entry.value_or_offset=is_value;
            ifd_entry.data8[0] = data0;
            ifd_entry.data8[1] = data1;
            ifd_entry.data8[2] = data2;
            ifd_entry.data8[3] = data3;
#ifdef DEBUG
            printf("data8[0]=%u\n", data0);
            printf("data8[1]=%u\n", data1);
            printf("data8[2]=%u\n", data2);
            printf("data8[3]=%u\n", data3);
#endif
          }; break;
        case 3: /* 16-bit unsigned integer */
        case 8: /* !16-bit signed integer */
          if (count > 2) { /* offset */
            ifd_entry.value_or_offset=is_offset;
            ifd_entry.data32offset=value_or_offset;
          } else { /*  values */
            ifd_entry.value_or_offset=is_value;
            uint16 w0 = (data0) + (data1<<8);
            uint16 w1 = (data2) + (data3<<8);
            if (byteswapped) {
              TIFFSwabShort( &w0 );
              TIFFSwabShort( &w1 );
            }
            ifd_entry.data16[0] = w0;
            ifd_entry.data16[1] = w1;
#ifdef DEBUG
            printf("data16[0]=%u\n", w0);
            printf("data16[1]=%u\n", w1);
#endif
          }; break;
        case 4: /* 32-bit unsigned integer */
        case 9: /* !32-bit signed integer */
          if (count > 1) { /* offset */
            ifd_entry.value_or_offset=is_offset;
            ifd_entry.data32offset=value_or_offset;
          } else { /*  values */
            ifd_entry.value_or_offset=is_value;
            ifd_entry.data32=value_or_offset;
#ifdef DEBUG
            printf("data32[0]=%u\n", value_or_offset);
#endif
          }; break;
        case 5: /* 64-bit unsigned fraction */
        case 10: /* !64-bit signed fraction */
        case 11: /* !32-bit IEEE floating point */
        case 12: /* !64-bit IEEE floating point */
        case 13: /* %32-bit unsigned integer (offset) */
        case 16: /* BigTIFF 64-bit unsigned integer */
        case 17: /* BigTIFF 64-bit signed integer */
        case 18: /* BigTIFF 64-bit unsigned integer (offset) */
          ifd_entry.value_or_offset=is_offset;
          ifd_entry.data32offset=value_or_offset;

      }
      free( ifdentries );
#ifdef DEBUG
      printf("tag idx=%i, tag=%u (0x%04x) tagtype=0x%04x is_offset=%s count=%u value_or_offset=0x%08x\n", i, tagid, tagid, tagtype, (ifd_entry.value_or_offset==is_offset ? "true" : "false"), count, value_or_offset);
#endif
      return ifd_entry;
    }
    e+=10;
  }
  /* loop each tag until end or given tag found */
  free( ifdentries );
  return ifd_entry;
}

/*  TODO */
ifd_entry_t TIFFGetRawIFDEntry( ctiff_t * ctif, tag_t tag) {
  int tagidx = -1;
  for (int i= 0; i < get_ifd0_count( ctif ); i++) {
    if (tag > 253 && tag == TIFFGetRawTagListEntry( ctif, i )) {
      tagidx= i;
      break;
    };
  };
  ifd_entry_t ifd_entry;
  if (tagidx >= 0) {
    ifd_entry =  TIFFGetRawTagIFDListEntry( ctif, tagidx );
  } else { /* tag not defined */
	  printf("\ttag %u (%s) was not found, but requested because defined\n", tag, TIFFTagName(tag));
	  ifd_entry.value_or_offset = is_error;
	  ifd_entry.count = 0;
  }
  return ifd_entry;
}
/* scans first IDF and returns the type of the n-th tag */
uint32 TIFFGetRawTagTypeListEntry( ctiff_t * ctif, int tagidx ) {
if (tagidx >= 0) {
    ifd_entry_t ifd_entry;
    ifd_entry =  TIFFGetRawTagIFDListEntry( ctif, tagidx );
    return ifd_entry.datatype;
  } else { /* tag not defined */
    fprintf(stderr, "tagidx should be greater equal 0");
    exit(EXIT_FAILURE);
  }
}

/* ct_reads the datatype of given tag on specified TIFF,
 * because FieldType of libtiff does not return the true value (because it maps
 * its own datastructure), we need to use this function instead
 * @param tif pointer to TIFF structure
 * @param tag tag
 * @return datatype of given tag
 * if tag does not exists the function aborts with an error
 */
TIFFDataType TIFFGetRawTagType(ctiff_t * ctif, tag_t tag) {
  int tagidx = -1;
  for (int i= 0; i < get_ifd0_count( ctif ); i++) {
    if (tag > 253 && tag == TIFFGetRawTagListEntry( ctif, i )) {
      tagidx= i;
      break;
    };
  };
  if (tagidx >= 0) {
    TIFFDataType datatype =  TIFFGetRawTagTypeListEntry( ctif, tagidx );
#ifdef DEBUG
    printf("### datatype=%i \n", datatype);
#endif
    return datatype;
  } else { /* tag not defined */
	  printf("\ttag %u (%s) was not found, but requested because defined\n", tag, TIFFTagName(tag));
	  return -1;
  }
}

ctiff_t * initialize_ctif(const char * tiff_file, ct_ioflag_t ioflag) {
  ctiff_t * ctif = malloc ( sizeof( ctiff_t) );
  if (NULL == ctif) {
    fprintf( stderr, "could not allocate memory for ctiff_t\n");
    exit (EXIT_FAILURE);
  }
  /* load tiff file */
#ifdef __WIN32__
  int tif = open(tiff_file, O_RDONLY | O_BINARY);
#else
  int tif = open(tiff_file, O_RDONLY);
#endif
  if (-1 == tif) {
	  fprintf( stderr, "file '%s' could not be opened\n", tiff_file);
	  exit (EXIT_FAILURE);
  };
  ctif->fd = tif;
  ctif->streamlen = fsize(tif);
  ctif->streamp = NULL;
  ctif->actual_streamp = NULL;
  ctif->tagorder=unknown_tag_order;
  for (int i= 0; i < 65536; i++) {
    ctif->tag_cache[i]= -1;
  }
#ifdef _HAVE__MMAP
  switch (ioflag) {
	  case is_filep: {
				 /* streamlen */
				 break;
			 }
	  case  is_memmap: {
				   void * tifmap = mmap( NULL, ctif->streamlen, PROT_READ, MAP_PRIVATE, tif, 0 );
				   if (MAP_FAILED == tifmap) {
					   fprintf( stderr, "file '%s' could not be mem-mapped, %s\n", tiff_file, strerror(errno));
					   exit (EXIT_FAILURE);
				   };
				   ctif->streamp=tifmap;
				   ctif->actual_streamp=tifmap;
				  break;
			   }
  }
#endif
  ctif->ioflag = ioflag;
  ctif->filename = strdup(tiff_file);
  ctif->ifd0pos= 0;
  ctif->ifd0c= 0;
  return ctif;
}

void free_ctif( ctiff_t * ctif) {
	assert( NULL != ctif);
	if (NULL != ctif->filename) free(ctif->filename);
	ctif->filename=NULL;
	 switch (ctif->ioflag) {
	  case is_filep: {
				 break;
			 }
	  case is_memmap: {

				  /* TODO */
				  break;
			  }
	 }
	 close(ctif->fd);
	 ctif->fd = -1;
	free (ctif);
	/* ctif = NULL; */
}

uint32 get_ifd0_pos( ctiff_t * ctif ) {
	assert( NULL != ctif);
	return ctif->ifd0pos;
}

uint16 get_ifd0_count( ctiff_t * ctif ) {
	assert( NULL != ctif);
	return ctif->ifd0c;
}

char is_byteswapped( ctiff_t * ctif ) {
	assert( NULL != ctif);
	return ctif->isbyteswapped;
}

uint32 get_next_ifd_pos(ctiff_t * ctif, uint32 actual_pos) {
	assert( NULL != ctif);

	ct_seek(ctif, actual_pos, SEEK_SET);
	uint16 count;
	if ( ct_read( ctif, &count, 2) != 2 ) {
    /*  FIXME: replace perror/exit with own error handling routine */
		perror ("TIFF Header ct_read error2");
		exit( EXIT_FAILURE );
	}

	if (ctif->isbyteswapped)
		TIFFSwabShort(&count);
	ct_seek(ctif, 12 * count, SEEK_CUR);
	uint32 offset;
	if ( ct_read( ctif, &offset, 4) != 4 ) {
    /*  FIXME: replace perror/exit with own error handling routine */
		perror ("TIFF Header ct_read error3");
		exit( EXIT_FAILURE );
	}
	if (ctif->isbyteswapped)
		TIFFSwabLong(&offset);
	return offset;
}

ret_t TIFFGetFieldASCII(ctiff_t * ctif, const tag_t tag, char** string_pp, uint32 * countp ) {
  assert( *string_pp == NULL);
  assert( countp != NULL);
  GET_EMPTY_RET(ret)
  int tagidx = TIFFGetRawTagListIndex(ctif, tag);
  if (tagidx >= 0) { /* there exists the tag */
    ifd_entry_t entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
    /* assert (entry.datatype == TIFF_ASCII); */
    if (entry.datatype != TIFF_ASCII) {
      char msg[200];
      snprintf(msg, 200, "data of type ASCII expected, but got type %s", TIFFTypeName(entry.datatype));
      ret = set_value_found_ret(&ret, msg);
      ret.returncode = tagerror_unexpected_type_found;
      return ret;
    }
    *countp = entry.count;
/*
    *(string_pp) = malloc( sizeof(char) * entry.count +1);
    if (NULL == (* string_pp)) {
      ret.returncode=could_not_allocate_memory;
      return ret;
    }
    memset(*string_pp, '\0', entry.count+1);
*/
    *(string_pp) = calloc(entry.count+1, sizeof(char) );
    if (NULL == (* string_pp)) {
      ret.returncode=could_not_allocate_memory;
      return ret;
    }
    if (entry.value_or_offset == is_value) {
      assert (entry.count >= 0 && entry.count <= 4);
      for (uint32 i=0; i<entry.count; i++) {
        (*string_pp)[i]=entry.data8[i];
      }
      ret.returncode=is_valid;
      return ret;
    } else if (entry.value_or_offset == is_offset) {
      uint32 data32offset = entry.data32offset;
      offset_t offset;
      ret = read_offsetdata( ctif, data32offset, entry.count, entry.datatype, &offset, &ret);
      if (ret.returncode != is_valid) {
        /*  FIXME: free(offset.datacharp); */
        return ret;
      }
      char * p = offset.datacharp;
      char * s = *(string_pp);
#ifdef DEBUG
      /* DEBUG: */
      printf("offset.count=%u, offset.datacharp=%p\n", offset.count, offset.datacharp);
      printf("tag=%i entry.count=%u offset.count=%u\n", tag, entry.count, offset.count);
#endif
      for (uint32 i=0; i<entry.count; i++) {
#ifdef DEBUG
        printf("P[%u]=%c\n", i, *p);
#endif
        *(s++) = *(p++);
      }
      //printf("ASCII='%s'\n", *(string_pp));
      free(offset.datacharp);
      ret.returncode=is_valid;
      return ret;
    }
  }
  ret.returncode = tag_does_not_exist;
  return ret;
}

ret_t TIFFGetFieldLONG(ctiff_t * ctif, const tag_t tag, uint32 ** long_pp, uint32 * countp) {
  assert( *long_pp == NULL);
  assert( countp != NULL);
  *countp = -1; /*  init */
  GET_EMPTY_RET(ret)
  int tagidx = TIFFGetRawTagListIndex(ctif, tag);
  if (tagidx >= 0) { /* there exists the tag */
    ifd_entry_t entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
    uint32 overflow =  (0xffffffff / (sizeof(uint32)));
    if (entry.count > overflow) {
      char msg[200];
      snprintf(msg, 200, "count=%u overflow boundary=%u", entry.count, overflow);
      ret = set_value_found_ret(&ret, msg);
      ret.returncode = tagerror_count_results_in_offsets_overflow;
      return ret;
    }

    *countp = entry.count;
    *(long_pp) = malloc( sizeof(uint32) * entry.count);
    if (NULL == (*long_pp)) {
      ret.returncode=could_not_allocate_memory;
      return ret;
    }
    /* assert (entry.datatype == TIFF_LONG); */
    if (entry.datatype != TIFF_LONG) {
      char msg[200];
      snprintf(msg, 200, "data of type LONG expected, but got type %s", TIFFTypeName(entry.datatype));
      ret = set_value_found_ret(&ret, msg);
      ret.returncode = tagerror_unexpected_type_found;
      return ret;
    }

    if (entry.value_or_offset == is_value) {
      assert (entry.count >= 0 && entry.count <= 1);
      //printf("LONG (direct)=%lu\n", entry.data32);
      memcpy((void *) (*long_pp), (void *) &entry.data32, (sizeof(uint32)*entry.count));
      ret.returncode=is_valid;
      return ret;
    } else if (entry.value_or_offset == is_offset) {
      uint32 data32offset = entry.data32offset;
      offset_t offset;
      ret = read_offsetdata( ctif, data32offset, entry.count, entry.datatype, &offset, &ret);
      if (ret.returncode != is_valid) {
        /*  FIXME: free(offset.datacharp); */
        return ret;
      }
      //printf("LONG (offset)=%lu\n", *offset.datacharp);
      memcpy((void *) (*long_pp), (void *) offset.datacharp, (sizeof(uint32)*offset.count));
      free(offset.datacharp);
      ret.returncode=is_valid;
      return ret;
    }
  }
  ret.returncode = tag_does_not_exist;
  return ret;
}

ret_t TIFFGetFieldSHORT(ctiff_t * ctif, const tag_t tag, uint16 ** short_pp, uint32 * countp) {
  assert( *short_pp == NULL);
  assert( countp != NULL);
  *countp = -1; /*  init */
  GET_EMPTY_RET(ret)
  int tagidx = TIFFGetRawTagListIndex(ctif, tag);
  if (tagidx >= 0) { /* there exists the tag */
    ifd_entry_t entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
    uint32 overflow =  (0xffffffff / (sizeof(uint16)));
    if (entry.count > overflow) {
      char msg[200];
      snprintf(msg, 200, "count=%u overflow boundary=%u", entry.count, overflow);
      ret = set_value_found_ret(&ret, msg);
      ret.returncode = tagerror_count_results_in_offsets_overflow;
      return ret;
    }

    *countp = entry.count;
    *(short_pp) = malloc( sizeof(uint16) * entry.count);
    if (NULL == *(short_pp)) {
      ret.returncode=could_not_allocate_memory;
      return ret;
    }
    /* assert (entry.datatype == TIFF_SHORT); */
    if (entry.datatype != TIFF_SHORT) {
      char msg[200];
      snprintf(msg, 200, "data of type SHORT expected, but got type %s", TIFFTypeName(entry.datatype));
      ret = set_value_found_ret(&ret, msg);
      ret.returncode = tagerror_unexpected_type_found;
      return ret;
    }

    if (entry.value_or_offset == is_value) {
      assert (entry.count >= 0 && entry.count <= 2);
      memcpy((void *) (*short_pp), (void *) &entry.data16, (sizeof(uint16)*entry.count));
      //printf("SHORT (direct)=%u %u\n", entry.data32, **short_pp);
      ret.returncode=is_valid;
      return ret;
    } else if (entry.value_or_offset == is_offset) {
      uint32 data32offset = entry.data32offset;
      offset_t offset;
      ret = read_offsetdata( ctif, data32offset, entry.count, entry.datatype, &offset, &ret);
      if (ret.returncode != is_valid) {
        /*  FIXME: free(offset.datacharp); */
        return ret;
      }
      //printf("SHORT (offset)=%u\n", *offset.datacharp);
      memcpy((void *) (*short_pp), (void *)offset.datacharp, (sizeof(uint16)*offset.count));
      free( offset.datacharp );
      offset.datacharp=NULL;
      ret.returncode=is_valid;
      return ret;
    }
  }
  ret.returncode = tag_does_not_exist;
  return ret;
}

ret_t TIFFGetFieldRATIONAL(ctiff_t * ctif, const tag_t tag, float ** float_pp, uint32 * countp) {
  assert( *float_pp == NULL);
  assert( countp != NULL);
  *countp = 0; /*  init */
  GET_EMPTY_RET(ret)
  int tagidx = TIFFGetRawTagListIndex(ctif, tag);
  if (tagidx >= 0) { /* there exists the tag */
    ifd_entry_t entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
    uint32 overflow =  (0xffffffff / (2*sizeof(uint32)));
    if (entry.count > overflow) {
      char msg[200];
      snprintf(msg, 200, "count=%u overflow boundary=%u", entry.count, overflow);
      ret = set_value_found_ret(&ret, msg);
      ret.returncode = tagerror_count_results_in_offsets_overflow;
      return ret;
    }
    //printf("entry.count=%i\n", entry.count);
    *countp = entry.count;
    *(float_pp) = malloc( sizeof(float) * (entry.count));
    if (NULL == *(float_pp)) {
      ret.returncode=could_not_allocate_memory;
      return ret;
    }
    /* assert (entry.datatype == TIFF_RATIONAL); */
    if (entry.datatype != TIFF_RATIONAL) {
      char msg[200];
      snprintf(msg, 200, "data of type RATIONAL expected, but got type %s", TIFFTypeName(entry.datatype));
      ret = set_value_found_ret(&ret, msg);
      ret.returncode = tagerror_unexpected_type_found;
      return ret;
    }

    /*  rational is defined as 2x32bits */
    if (entry.value_or_offset == is_value) {
      ret.returncode = tagerror_expected_offsetdata;
      return ret;
    } else if (entry.value_or_offset == is_offset) {
      uint32 data32offset = entry.data32offset;
      offset_t offset;
      //printf("data32offset=%u count=%i\n", data32offset, entry.count);
      ret = read_offsetdata( ctif, data32offset, entry.count, entry.datatype, &offset, &ret);
      if (ret.returncode != is_valid) {
        /*  FIXME: free(offset.datacharp); */
        return ret;
      }
      /* copy to a float */
      uint32 * orig_data32p = offset.data32p;
      for (uint32 i = 0; i< entry.count; i++, orig_data32p+=2) {
        uint32 numerator = *(orig_data32p);
        uint32 denominator = *(orig_data32p+1);
        //printf("DEBUG: numerator=%u denumeator=%u\n", numerator, denominator);
        float v;
        if (denominator == 0) {
          v=NAN;
        } else {
          v = (float) numerator / (float) denominator;
        }
        //printf("DEBUG2: *float_pp[%i]=%f (%u / %u)\n", i, v, numerator, denominator);
        (*(float_pp))[i]=v;
      }
      free( offset.data32p );
      offset.data32p=NULL;
      ret.returncode=is_valid;
      return ret;
    }
  }
  ret.returncode = tag_does_not_exist;
  return ret;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
