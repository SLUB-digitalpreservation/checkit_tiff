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
                       break;
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
        tag_t last = TIFFGetRawTagListEntry(ctif, 0);
        for (int i= 1; i < get_ifd0_count( ctif ); i++) {
         tag_t current = TIFFGetRawTagListEntry( ctif, i );
          if (last >= current) {
            ctif->tagorder=has_unsorted_tags;
            break;
          }
          last = current;
        };
        if (has_sorted_tags == ctif->tagorder) {
          for (int i= 0; i < get_ifd0_count( ctif ); i++) {
            tag_t tag = TIFFGetRawTagListEntry( ctif, i );
            ctif->tag_cache[tag] = i;
          }
          return ctif->tag_cache[tag];
        }
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
  ret_t ret;
  ret.value_found = malloc(VALUESTRLEN);
  if (NULL == ret.value_found) {
    ret.returncode=could_not_allocate_memory;
    return ret;
  }

  tifp_check( ctif);

  float * valp = NULL;
  float val;
  int found=TIFFGetFieldRATIONAL(ctif, tag, &valp);
  if (1 == found) {
    val = * valp;
    if ( fabs(val - value) < 0.01 ) {
      ret.returncode=is_valid;
      return ret;
    } else {
      ret.value_found = strncpy(ret.value_found, float2str(val), VALUESTRLEN) ;
      ret.returncode = tagerror_value_differs;
    }
  } else {
    ret.returncode = tagerror_value_not_found;
    return ret;
  }
  ret.returncode = should_not_occure;
  return ret;
}

//------------------------------------------------------------------------------
ret_t check_tag_has_u16value(ctiff_t * ctif, tag_t tag, uint16 value) {
  ret_t ret;
  ret.value_found = malloc(VALUESTRLEN);
  if (NULL == ret.value_found) {
    ret.returncode=could_not_allocate_memory;
    return ret;
  }

  tifp_check( ctif);

  uint16 * valp = NULL;
  uint16 val;
  int found=TIFFGetFieldSHORT(ctif, tag, &valp);
  if (1 == found) {
    val = *valp;
    if ( val == value ) {
      ret.returncode=is_valid;
      return ret;
    } else {
      ret.value_found = strncpy(ret.value_found, int2str(val), VALUESTRLEN);
      ret.returncode = tagerror_value_differs;
      return ret;
    }

  } else {
    ret.returncode = tagerror_value_not_found;
    return ret;
  }
  ret.returncode = should_not_occure;
  return ret;
}


//------------------------------------------------------------------------------
ret_t check_tag_has_u32value(ctiff_t * ctif, tag_t tag, uint32 value) {
  ret_t ret;
  ret.value_found = malloc(VALUESTRLEN);
  if (NULL == ret.value_found) {
    ret.returncode=could_not_allocate_memory;
    return ret;
  }

  tifp_check( ctif);

  uint32 * valp = NULL;
  uint32 val;
  int found=TIFFGetFieldLONG(ctif, tag, &valp);
  if (1 == found) {
    val = *valp;
    if ( val == value )  {
      ret.returncode=is_valid;
      return ret;
    } else {
      ret.value_found = strncpy(ret.value_found, int2str(val), VALUESTRLEN);
      ret.returncode = tagerror_value_differs;
      return ret;
    }

  } else {
    ret.returncode = tagerror_value_not_found;
    return ret;
  }
  ret.returncode = should_not_occure;
  return ret;

}


ret_t parse_header_and_endianess(ctiff_t * ctif) {
  ret_t ret;
  ret.value_found = malloc(VALUESTRLEN);
  if (NULL == ret.value_found) {
    ret.returncode=could_not_allocate_memory;
    return ret;
  }

   /* seek the image file directory (bytes 4-7) */
  //ct_seek(fd, (off_t) 0, SEEK_SET);
  if ( ct_seek(ctif, 0, SEEK_SET) != 0)  {
	  strncpy(ret.value_found, "TIFF header ct_seek error to 0", VALUESTRLEN);
	  ret.returncode=tiff_seek_error_header;
	  return ret;
  }
  uint16 header;
  uint16 magic;
  int endianess;
  if ( ct_read( ctif, &header, 2) != 2 ) {
	  strncpy(ret.value_found, "TIFF Header ct_read error to magic byte header (first 2 bytes)", VALUESTRLEN);
	  ret.returncode=tiff_read_error_header;
	  return ret;
  }
  if (header == 0x4949) endianess = 0; /* little endian */
  else if (header == 0x4d4d) endianess = 1; /*  big endian */
  else {
    char errmsg[VALUESTRLEN]="";
    snprintf (errmsg, VALUESTRLEN, "TIFF Header error, not Byte Order Bytes for TIFF: 0x%04x", header);
    if (header == 0x4550) {
	    strncat(errmsg, ", could be a Microsoft Document Image file (little endian), if header begins with by 0x45 0x50 0x2a 0x00", VALUESTRLEN);
    }
    strncpy(ret.value_found, errmsg, VALUESTRLEN);
    ret.returncode = tiff_byteorder_error; 
    return ret;
  }
  ctif->isbyteswapped = endianess;
  if ( ct_read( ctif, &magic, 2) != 2 ) {
	  strncpy(ret.value_found, "TIFF Header ct_read error to magic byte header (second 2 bytes == 42)", VALUESTRLEN);
	  ret.returncode=tiff_read_error_header;
	  return ret;
  }

  uint16 magic2 = magic;
  if (endianess) TIFFSwabShort( &magic2 ); /*  big endian */
  if (magic2 == 42) { ret.returncode=is_valid; return ret; }
  else {
	  char errmsg[VALUESTRLEN]="";
	  snprintf (errmsg, VALUESTRLEN, "TIFF Header error, not a MAGIC BYTE for TIFF: 0x%04x\n", magic);
	  if (magic2==0x002b) strncat(errmsg, ", but could be a BigTIFF, see http://www.awaresystems.be/imaging/tiff/bigtiff.html", VALUESTRLEN);
	  if (magic2==0x0055) strncat(errmsg, ", but could be a Panasonic Raw/RW2, see http://libopenraw.freedesktop.org/wiki/Panasonic_RAW/", VALUESTRLEN);
	  if (magic2==0x01bc) strncat(errmsg, ", but could be a JPEG XR, see http://www.itu.int/rec/T-REC-T.832", VALUESTRLEN);
	  if (magic2==0x314e) strncat(errmsg, ", but could be a Navy Image FileFormat, see http://www.navsea.navy.mil/nswc/carderock/tecinfsys/cal-std/doc/28002c.pdf", VALUESTRLEN);
	  if (magic2==0x4352) strncat(errmsg, ", but could be a DNG camera profile, see http://wwwimages.adobe.com/www.adobe.com/content/dam/Adobe/en/products/photoshop/pdfs/dng_spec_1.4.0.0.pdf", VALUESTRLEN);
	  if (magic2==0x4f52) strncat(errmsg, ", but could be an Olympus ORF, see http://libopenraw.freedesktop.org/wiki/Olympus_ORF/", VALUESTRLEN);
	  if (magic2==0x5352) strncat(errmsg, ", but could be an Olympus ORF, see http://libopenraw.freedesktop.org/wiki/Olympus_ORF/", VALUESTRLEN);
	  strncpy(ret.value_found, errmsg, VALUESTRLEN);
	  ret.returncode = tiff_byteorder_error; 
	  return ret;
  }
  ret.returncode = should_not_occure;
  return ret;
}

ret_t get_first_IFD(ctiff_t * ctif, uint32 * ifd) {
	ret_t ret;
	ret.value_found = malloc(VALUESTRLEN);
	if (NULL == ret.value_found) {
		ret.returncode=could_not_allocate_memory;
		return ret;
	}
	int isByteSwapped = ctif->isbyteswapped;
	/* seek the image file directory (bytes 4-7) */
	if (ct_seek(ctif, 4, SEEK_SET) != 4 ) {
		strncpy(ret.value_found, "TIFF Header seak error, seek set to byte 4", VALUESTRLEN);
		ret.returncode=tiff_seek_error_header;
		return ret;
	}
	uint32 offset;
	if ( ct_read( ctif, &offset, 4) != 4 ) {
		strncpy(ret.value_found, "TIFF Header ct_read error, reading 4 bytes from 4", VALUESTRLEN);
		ret.returncode=tiff_read_error_header;
		return ret;
	}
	if (isByteSwapped) {
		TIFFSwabLong (&offset);
	}
	if (offset <= 7) {
		snprintf(ret.value_found, VALUESTRLEN, "pointer to IFD0 is %i", offset);
                ret.returncode=tiff_ifd0_offset_must_be_greater_than_eight;
		return ret;
	}
	ctif->ifd0pos=offset;
	if (ct_seek(ctif, offset, SEEK_SET) != offset ) {
		snprintf(ret.value_found, VALUESTRLEN, "TIFF Header seek error, seek set to byte %i", offset);
		ret.returncode=tiff_seek_error_header;
		return ret;
	}
	uint16 count;
	if ( ct_read( ctif, &count, 2) != 2 ) {
		strncpy(ret.value_found, "TIFF Header ct_read error2, reading ifd0 count (2 bytes)", VALUESTRLEN);
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

/* scans first IDF and returns count of tags
 * Hint: sideeffect, if succeed the seek points to beginning of the first
 * IFD-entry */
int TIFFGetRawTagListCount (ctiff_t * ctif, uint32 ifdpos) {
	/* parse TIF */
	/* seek the image file directory (bytes 4-7) */
	uint32 offset = ifdpos;

	// printf("diroffset to %i (0x%04lx)\n", offset, offset);
	//printf("byte swapped? %s\n", (TIFFIsByteSwapped(tif)?"true":"false"));
	/* ct_read and seek to IFD address */
	ct_seek(ctif, offset, SEEK_SET);

	uint16 count;
	if ( ct_read( ctif, &count, 2) != 2 ) {
		perror ("TIFF Header ct_read error2");
		exit( EXIT_FAILURE );
	}

	if (is_byteswapped(ctif))
		TIFFSwabShort(&count);
	return count;
}

/* scans first IDF and returns the n-th tag */
tag_t TIFFGetRawTagListEntry( ctiff_t * ctif, int tagidx ) {
	int byteswapped = is_byteswapped(ctif);
	int count = get_ifd0_count( ctif);
	/* ct_read count of tags (2 Bytes) */
	/* replace i/o operatrions with in-memory-operations */
	uint8 * ifdentries = NULL;
	ifdentries = malloc ( sizeof(uint8) * 12 * count);
	ct_seek(ctif, ctif->ifd0pos+2, SEEK_SET); /* IFD0 plus 2byte to get IFD-entries */
	if ( ct_read( ctif, ifdentries, 12 * count) != 12*count ) {
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

#define offset_malloc(fd, of, os, count ) {\
	of = NULL; of = malloc ( sizeof(os) * count);\
	  if ( ct_read( ctif, of, sizeof(os) * count) != sizeof(os) *count ) {\
      fprintf(stderr, "TIFF Offset ct_read error, try to read from offset count=%lu bytes\n", sizeof(os) * count);\
		  exit( EXIT_FAILURE );\
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
offset_t read_offsetdata( ctiff_t * ctif, uint32 address, uint32 count, uint16 datatype) {
  //int fd = TIFFFileno( tif);
  /* ct_read and seek to IFD address */
  ct_seek(ctif, address, SEEK_SET);


#ifdef DEBUG
  printf("read_offsetdata(tif, adress=%u, count=%u, datatype=%u)\n", address, count, datatype);
#endif

  offset_t offset;
  offset.count = count;
  offset.datatype = datatype;
  switch (datatype) {
    case 1: /* 8-bit unsigned integer */
    case 7: /* !8-bit untyped data */
      /*
      offset.data8p = NULL;
      offset.data8p = malloc ( sizeof(uint8) * count);
      if (ct_read(fd, offset.data8p,  sizeof(uint8) * count) != sizeof(uint8) *count)
        perror ("TIFF Offset ct_read error");
      */
      offset_malloc(fd, offset.data8p, uint8, count)
      break;
    case 2: /* 8-bit bytes w/ last byte null */
      assert( sizeof(char) == sizeof(uint8));
      offset_malloc(fd, offset.datacharp, char, count)
      break;
    case 6: /* !8-bit signed integer */
      offset_malloc(fd, offset.datas8p, int8, count)
      break;
    case 3: /* 16-bit unsigned integer */
      offset_malloc(fd, offset.data16p, uint16, count)
      offset_swabshort(ctif, offset.data16p, count);
      break;
    case 8: /* !16-bit signed integer */
      offset_malloc(fd, offset.datas16p, int16, count)
      offset_swabshort(ctif, (uint16 *) offset.datas16p, count);
      break;
    case 4: /* 32-bit unsigned integer */
    case 13: /* %32-bit unsigned integer (offset) */
      offset_malloc(fd, offset.data32p, uint32, count)
      offset_swablong(ctif, offset.data32p, count);
      break;
    case 9: /* !32-bit signed integer */
      offset_malloc(fd, offset.datas32p, uint32, count)
      offset_swablong(ctif, (uint32 *) offset.data32p, count);
      break;
    case 5: /* 64-bit unsigned fraction */
      offset_malloc(fd, offset.data32p, uint32, 2*count) /* because numerator + denominator */
      offset_swablong(ctif, offset.data32p, 2*count);
      break;
    case 10: /* !64-bit signed fraction */
      fprintf(stderr, "offsetdata datatype=%i not supported yet", datatype);
      exit(EXIT_FAILURE);
    case 11: /* !32-bit IEEE floating point */
      assert( sizeof(float) == 4);
      offset_malloc(fd, offset.datafloatp, float, count)
      break;
    case 12: /* !64-bit IEEE floating point */
      assert( sizeof(double) == 8);
      offset_malloc(fd, offset.datadoublep, double, count)
      break;
    case 16: /* BigTIFF 64-bit unsigned integer */
    case 18: /* BigTIFF 64-bit unsigned integer (offset) */
      assert( sizeof(double) == 8);
      offset_malloc(fd, offset.data64p, uint64, count)
      break;
    case 17: /* BigTIFF 64-bit signed integer */
      assert( sizeof(double) == 8);
      offset_malloc(fd, offset.datas64p, int64, count)
        break;
    default: /*  should not occure */
      fprintf(stderr, "offsetdata datatype=%i not supported yet", datatype);
      exit(EXIT_FAILURE);
  };
  return offset;
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
  int i;
  ifd_entry_t ifd_entry;
  ifd_entry.value_or_offset = is_error;
  /* replace i/o operatrions with in-memory-operations */
  uint8 * ifdentries = NULL;
  ifdentries = malloc ( sizeof(uint8) * 12 * tagcount);
  ct_seek(ctif, ctif->ifd0pos+2, SEEK_SET); /* IFD0 plus 2byte to get IFD-entries */

  if ( ct_read( ctif, ifdentries, 12 * tagcount) != 12*tagcount ) {
	  perror ("TIFF Header ct_read error4");
	  exit( EXIT_FAILURE );
  }
  uint8 * e = ifdentries;
  for (i = 0; i<tagcount; i++) {
    uint8 lo = *e;
    e++;
    uint8 hi = *e;
    uint16 tagid = (hi << 8) + lo;
    e++;
    if (byteswapped)
      TIFFSwabShort(&tagid);
    if (i == tagidx) {
      // tag type check
      lo = *e; e++;
      hi = *e; e++;
      uint16 tagtype = (hi << 8) + lo;
      if (byteswapped)
        TIFFSwabShort(&tagtype);

      uint32 count = (*(e++));
      count += (*(e++) << 8);
      count += (*(e++) << 16);
      count += (*(e++) << 24);
      if (byteswapped)
        TIFFSwabLong( &count);
#ifdef DEBUG
printf("\ncount=%0x\n\n", count);
#endif

      /*  is value or offset? */
      /*  TODO */
      ifd_entry.count=count;
      ifd_entry.datatype=tagtype;
      uint8 data[4];
      data[0] = *(e++);
      data[1] = *(e++);
      data[2] = *(e++);
      data[3] = *(e++);
      uint32 value_or_offset = (data[0]);
      value_or_offset += (data[1] << 8);
      value_or_offset += (data[2] << 16);
      value_or_offset += (data[3] << 24);
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
            ifd_entry.data8[0] = data[0];
            ifd_entry.data8[1] = data[1];
            ifd_entry.data8[2] = data[2];
            ifd_entry.data8[3] = data[3];
#ifdef DEBUG
            printf("data8[0]=%u\n", data[0]);
            printf("data8[1]=%u\n", data[1]);
            printf("data8[2]=%u\n", data[2]);
            printf("data8[3]=%u\n", data[3]);
#endif
          }; break;
        case 3: /* 16-bit unsigned integer */
        case 8: /* !16-bit signed integer */
          if (count > 2) { /* offset */
            ifd_entry.value_or_offset=is_offset;
            ifd_entry.data32offset=value_or_offset;
          } else { /*  values */
            ifd_entry.value_or_offset=is_value;
            uint16 w0 = (data[0]) + (data[1]<<8);
            uint16 w1 = (data[2]) + (data[3]<<8);
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
      printf("tag idx=%i, tag=%u (0x%04x) tagtype=0x%04x is_offset=%s count=%d value_or_offset=0x%08x\n", i, tagid, tagid, tagtype, (ifd_entry.value_or_offset==is_offset ? "true" : "false"), count, value_or_offset);
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
	ctif = NULL;
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
		perror ("TIFF Header ct_read error2");
		exit( EXIT_FAILURE );
	}

	if (ctif->isbyteswapped)
		TIFFSwabShort(&count);
	ct_seek(ctif, 12 * count, SEEK_CUR);
	uint32 offset;
	if ( ct_read( ctif, &offset, 4) != 4 ) {
		perror ("TIFF Header ct_read error3");
		exit( EXIT_FAILURE );
	}
	if (ctif->isbyteswapped)
		TIFFSwabLong(&offset);
	return offset;
}

int TIFFGetFieldASCII(ctiff_t * ctif, tag_t tag, char** string_pp) {
	assert( *string_pp == NULL);
	int tagidx = TIFFGetRawTagListIndex(ctif, tag);
	  if (tagidx >= 0) { /* there exists the tag */
	    ifd_entry_t entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
	    *(string_pp) = malloc( sizeof(char) * entry.count);
	    if (NULL == (* string_pp)) {
	    	perror ("could not allocate memory for string_p");
	    	exit( EXIT_FAILURE);
	    }

	    assert (entry.datatype == TIFF_ASCII);
	    if (entry.value_or_offset == is_value) {
	    	assert (entry.count >= 0 && entry.count <= 4);
	    	for (int i=0; i<entry.count; i++) {
	    	  (*string_pp)[i]=entry.data8[i];
	    	}
	    	return (entry.count);
	    } else if (entry.value_or_offset == is_offset) {
	    	uint32 data32offset = entry.data32offset;
	    	offset_t offset = read_offsetdata( ctif, data32offset, entry.count, entry.datatype);
	    	char * p = offset.datacharp;
	    	char * s = *(string_pp);
        /* DEBUG: */
        //printf("tag=%i entry.count=%i offset.count=%i\n", tag, entry.count, offset.count);
        for (int i=0; i<entry.count; i++) {
	    		//printf("P[%i]=%c\n", i, *p);
	    	  *(s++) = *(p++);
	    	}
	    	//printf("ASCII='%s'\n", *(string_pp));
		free(offset.datacharp);
	    	return (entry.count);
	    }
	  }
	  return -1;
}

int TIFFGetFieldLONG(ctiff_t * ctif, tag_t tag, uint32 ** long_pp) {
	assert( *long_pp == NULL);
	int tagidx = TIFFGetRawTagListIndex(ctif, tag);
	  if (tagidx >= 0) { /* there exists the tag */
	    ifd_entry_t entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
	    *(long_pp) = malloc( sizeof(uint32) * entry.count);
	    if (NULL == (*long_pp)) {
	    	perror ("could not allocate memory for long_p");
	    	exit( EXIT_FAILURE);
	    }
	    assert (entry.datatype == TIFF_LONG);
	    if (entry.value_or_offset == is_value) {
	    	assert (entry.count >= 0 && entry.count <= 1);
        //printf("LONG (direct)=%lu\n", entry.data32);
	    	memcpy((void *) (*long_pp), (void *) &entry.data32, (sizeof(uint32)*entry.count));
	    	return (entry.count);
	    } else if (entry.value_or_offset == is_offset) {
	    	uint32 data32offset = entry.data32offset;
	    	offset_t offset = read_offsetdata( ctif, data32offset, entry.count, entry.datatype);
       //printf("LONG (offset)=%lu\n", *offset.datacharp);
	    	memcpy((void *) (*long_pp), (void *) offset.datacharp, (sizeof(uint32)*offset.count));
		free(offset.datacharp);
	    	return (entry.count);
	    }
	  }
	  return -1;
}

int TIFFGetFieldSHORT(ctiff_t * ctif, tag_t tag, uint16 ** short_pp) {
	assert( *short_pp == NULL);
	int tagidx = TIFFGetRawTagListIndex(ctif, tag);
	  if (tagidx >= 0) { /* there exists the tag */
	    ifd_entry_t entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
	    *(short_pp) = malloc( sizeof(uint16) * entry.count);
	    if (NULL == *(short_pp)) {
	    	perror ("could not allocate memory for short_p");
	    	exit( EXIT_FAILURE);
	    }
	    assert (entry.datatype == TIFF_SHORT);
	    if (entry.value_or_offset == is_value) {
	    	assert (entry.count >= 0 && entry.count <= 2);
	    	memcpy((void *) (*short_pp), (void *) &entry.data16, (sizeof(uint16)*entry.count));
        //printf("SHORT (direct)=%u %u\n", entry.data32, **short_pp);
	    	return (entry.count);
	    } else if (entry.value_or_offset == is_offset) {
	    	uint32 data32offset = entry.data32offset;
	    	offset_t offset = read_offsetdata( ctif, data32offset, entry.count, entry.datatype);
       //printf("SHORT (offset)=%u\n", *offset.datacharp);
	    	memcpy((void *) (*short_pp), (void *)offset.datacharp, (sizeof(uint16)*offset.count));
		free( offset.datacharp );
		offset.datacharp=NULL;

	    	return (entry.count);
	    }
	  }
	  return -1;
}
int TIFFGetFieldRATIONAL(ctiff_t * ctif, tag_t tag, float ** float_pp) {
	assert( *float_pp == NULL);
  int tagidx = TIFFGetRawTagListIndex(ctif, tag);
  if (tagidx >= 0) { /* there exists the tag */
    ifd_entry_t entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
    // printf("entry.count=%i\n", entry.count);
    *(float_pp) = malloc( sizeof(float) * (entry.count));
    if (NULL == *(float_pp)) {
      perror ("could not allocate memory for float_p");
      exit( EXIT_FAILURE);
    }
    assert (entry.datatype == TIFF_RATIONAL);
    /*  rational is defined as 2x32bits */
    if (entry.value_or_offset == is_value) {
      perror ("entry.value_or_offset must be offset");
      exit(EXIT_FAILURE);
    } else if (entry.value_or_offset == is_offset) {
      uint32 data32offset = entry.data32offset;
      offset_t offset = read_offsetdata( ctif, data32offset, entry.count, entry.datatype);
      /* copy to a float */
      uint32 numerator = 0;
      uint32 denominator = 0;
      uint32 * orig_data32p = offset.data32p;
      for (int i = 0; i< entry.count; i++) {
          numerator = *(orig_data32p++);
          denominator = *(orig_data32p++);
          //printf("DEBUG: numerator=%i denumeator=%i\n", numerator, denominator);
          float v;
          if (denominator == 0) {
            v=NAN;
          } else {
            v = (float) numerator / (float) denominator;
          }
          // printf("DEBUG2: *float_pp[%i]=%f (%u / %u)\n", i, v, numerator, denominator);
          (*(float_pp))[i]=v;
      }
      free( offset.data32p );
      offset.data32p=NULL;

      return (entry.count);
    }
  }
  return -1;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
