/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015/2016
 * licensed under conditions of libtiff 
 * (see file LICENSE)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <unistd.h>
#include <assert.h>


void add_mem_entry(mem_map_t * memmap_p, uint32 offset, uint32 count, memtype_t type) {
  mem_map_t memmap = *( memmap_p );
  assert(NULL != memmap.base_p);
  assert(0 < memmap.max_entries);
  assert(memmap.count < memmap.max_entries);
  assert(type < mt_END_marker);
  mem_map_entry_t * memmap_it_p = memmap.base_p;
  memmap_it_p = memmap.base_p + memmap.count;
  memmap_it_p->offset=offset;
  memmap_it_p->count=count;
  memmap_it_p->mem_type=type;
  memmap.count++;
  assert( memmap.max_entries > memmap.count);
  *( memmap_p) = memmap;
}

int compare_memmap(void const * lhs, void const * rhs) {
	mem_map_entry_t left = *((mem_map_entry_t *) lhs);
	mem_map_entry_t right = *((mem_map_entry_t *) rhs);

	return ((left.offset) - (right.offset));
}

/* TODO: add support for StripOffsets and StripByteCounts */
mem_map_t * scan_mem_map(ctiff_t * ctif) {
  assert( NULL != ctif);
  static mem_map_t memmap;
  memmap.count = 0;
  memmap.base_p = NULL;
  memmap.max_entries = 2048;
  memmap.base_p = malloc (sizeof(mem_map_entry_t) * memmap.max_entries);
  if (NULL == memmap.base_p) {
	  perror ("could not allocate mem for memmap, abort");
  }
  /* size of tiff file in bytes */
  memmap.max_len = ctif->streamlen;
  /* header */
  add_mem_entry( &memmap, 0, 4, mt_constant);
  /* IFD0 Offset */
  add_mem_entry( &memmap, 4, 4, mt_offset_to_ifd0);
  /* IFDO */
  uint32 ifd = get_ifd0_pos( ctif );
  uint16 count = get_ifd0_count( ctif);

  add_mem_entry( &memmap, ifd, 2, mt_ifd); /* count of tags in ifd */
  int ifdbase=2+ifd;
  /* iterate through IFD0 entries */
  uint16 tagidx;
  ifd_entry_t stripoffset_entry; 

  for (tagidx = 0; tagidx< count; tagidx++) {
	  add_mem_entry( &memmap, ifdbase+(tagidx*12), 8, mt_ifd); /* tagid, field type, count */
	  ifd_entry_t ifd_entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
	  uint32 tag = TIFFGetRawTagListEntry( ctif, tagidx);
	  TIFFDataType datatype = TIFFGetRawTagType( ctif, tag);
	  int datasize;
	  switch (datatype) {
		  case TIFF_ASCII: datasize = 1; break;
		  case TIFF_LONG: datasize =  4; break;
		  case TIFF_SHORT: datasize =  2; break;
		  case TIFF_BYTE: datasize =  1; break;
		  case TIFF_UNDEFINED: datasize =  1; break;
		  case TIFF_RATIONAL: datasize =  8; break;
		  case TIFF_SSHORT: datasize =  2; break;
		  case TIFF_SBYTE: datasize =  1; break;
		  case TIFF_SLONG: datasize =  4; break;
		  case TIFF_SRATIONAL: datasize =  8; break;
		  case TIFF_FLOAT: datasize =  4; break;
		  case TIFF_DOUBLE: datasize =  8; break;
                  default: fprintf(stderr, "unknown datatype %i, possible a program error", datatype); exit(EXIT_FAILURE);
	  }
          uint32 offset = ifd_entry.data32offset;
          uint16 count = ifd_entry.count;

          if (tag == TIFFTAG_STRIPOFFSETS) {
                  if (ifd_entry.value_or_offset == is_offset) {
                  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8,4,mt_ifd_offset_to_standardized_value ); 
                  add_mem_entry( &memmap, offset, ((uint32) count)*datasize, mt_ifd_offset_to_stripoffsets );
                  } else if (ifd_entry.value_or_offset==is_value) {
                        add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_embedded_standardized_value );
                  }
                  stripoffset_entry=ifd_entry;
          } else if (ifd_entry.value_or_offset==is_offset) { /* offset */
		  if (tag < 32768) { /* standard tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_offset_to_standardized_value ); 
			  add_mem_entry( &memmap, offset, ((uint32) count)*datasize, mt_standardized_value );
		  } else if (tag < 65000) { /* registered tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_offset_to_registered_value ); 
			  add_mem_entry( &memmap, offset, ((uint32) count)*datasize, mt_registered_value );
		  } else { /* private tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_offset_to_private_value ); 
			  add_mem_entry( &memmap, offset, ((uint32) count)*datasize, mt_private_value );
		  }
	  } else if (ifd_entry.value_or_offset==is_value) { /* embedded value */
		  if (tag < 32768) { /* standard tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_embedded_standardized_value ); 
		  } else if (tag < 65000) { /* registered tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_embedded_registered_value ); 
		  } else { /* private tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_embedded_private_value ); 
		  }

	  }
  }
 
  /* check next IFD mark */
  // uint32 offset = get_ifd0_pos(ctif );
  // uint32 IFDn = get_next_ifd_pos( ctif, offset );
  // printf("IFD: offset=%i, IFD0=%i IFDn=%i ifd+count=%i\n", offset, ifd, IFDn, ifdbase+12*count);
  add_mem_entry( &memmap, ifdbase+12*count, 4, mt_offset_to_ifd);

  /* handle stripoffset data */
  //printf("ifd_count=%i, stripoffset_count=%i\n", stripoffset_entry.count, stripoffset_count);
  uint32 stripoffset_values[stripoffset_entry.count];
  switch (stripoffset_entry.datatype) {
    case TIFF_LONG: {
                      /*  value */
                      if (stripoffset_entry.value_or_offset == is_value) {
                        for (uint32 i=0; i< stripoffset_entry.count; i++) {
                          stripoffset_values[i] = stripoffset_entry.data32;
                        }
                      }
                      /*  offset */
                      if (stripoffset_entry.value_or_offset == is_offset) {
                        offset_t offset;
                        ret_t ret = read_offsetdata(ctif, stripoffset_entry.data32offset, stripoffset_entry.count, stripoffset_entry.datatype, &offset, &ret);
                        uint32 * p = offset.data32p;
                        if ((uint64) stripoffset_entry.count*sizeof(uint32) > 0xffffffff) {
                          fprintf(stderr, "stripoffset_entry.count has size %u, resulting address %zu, but only offset to %lu is possible\n", stripoffset_entry.count, ((uint64) stripoffset_entry.count*sizeof(uint32)), (uint64) 0xffffffff);
                          exit(EXIT_FAILURE);
                        }
                        for (uint32 i=0; i< stripoffset_entry.count; i++) {
                          uint32 pval = *p;
                          if (is_byteswapped(ctif)) {
                            TIFFSwabLong(&pval);
                          }
                          stripoffset_values[i]=pval;
                          p++;
                        }
                      }
                      break;
                    }
    case TIFF_SHORT: {
                       /*  value */
                       if (stripoffset_entry.value_or_offset == is_value) {
                         for (uint32 i=0; i< stripoffset_entry.count; i++) {
                           stripoffset_values[i]= stripoffset_entry.data16[i];
                         }
                       }
                       /*  offset */
                       if (stripoffset_entry.value_or_offset == is_offset) {
                         offset_t offset;
                         ret_t ret = read_offsetdata(ctif, stripoffset_entry.data32offset, stripoffset_entry.count, stripoffset_entry.datatype, &offset, &ret);
                         uint16 * p = offset.data16p;
                         if ((uint64) stripoffset_entry.count*sizeof(uint16) > 0xffffffff) {
                           fprintf(stderr, "stripoffset_entry.count has size %u, resulting address %zu, but only offset to %lu is possible\n", stripoffset_entry.count, ((uint64) stripoffset_entry.count*sizeof(uint16)), (uint64) 0xffffffff);
                           exit(EXIT_FAILURE);
                         }
                         for (uint32 i=0; i< count; i++) {
                           uint16 pval = *p;
                           if (is_byteswapped(ctif)) {
                             TIFFSwabShort(&pval);
                           }
                           stripoffset_values[i]=pval;
                           p++;
                         }
                       }
                       break;
                     }
    default: {
               perror("stripoffset_entry.datatype has an unexpected value, possible a program error\n");
               exit(EXIT_FAILURE);
             }
  }
  //printf("count=%i\n", stripoffset_entry.count);
/*TODO:
  for (int i=0; i< stripoffset_entry.count; i++) {
    uint32 rawstriplen = TIFFRawStripSize(ctif->tif, i);
    //printf("OFFSET: p[%i]=%u len=%i\n", i,stripoffset_values[i], rawstriplen);
    add_mem_entry( &memmap, stripoffset_values[i], rawstriplen, mt_stripoffset_value);
  }
*/
  /* sort entries by offset */
  qsort(memmap.base_p, memmap.count, sizeof( mem_map_entry_t), compare_memmap);
  /*
  printf("memmap before HOLE detection\n");
  print_mem_map( &memmap );
  printf("----------------------------\n");
  */

  /* add all unused areas */
  uint32 memmap_orig_count = memmap.count;
  for (uint32 j=1; j< memmap_orig_count; j++) {
    mem_map_entry_t * prev=memmap.base_p+j-1;
    mem_map_entry_t * act =memmap.base_p+j;
    uint32 estimated_offset = (prev->offset + prev->count);
    if (estimated_offset < act->offset) { /*  found a hole */
      printf("HOLE FOUND at %u\n", estimated_offset);
      
      printf("\tprev->offset=%u prev->count=%u estimated=%u\n", prev->offset, prev->count, estimated_offset);
      printf("\tact->offset=%u act->count=%u\n", act->offset, act->count);
      
      add_mem_entry( &memmap, estimated_offset, (act->offset -estimated_offset), mt_unused);
    }
  }
  /* sort entries by offset again */
  qsort(memmap.base_p, memmap.count, sizeof( mem_map_entry_t), compare_memmap);
  /*  add unused area at end */
  mem_map_entry_t * last = memmap.base_p + memmap.count-1;
  uint32 estimated_offset = (last->offset + last->count);
  if (memmap.max_len > estimated_offset) {
      printf("HOLE (at end) FOUND at %u\n", estimated_offset);
      add_mem_entry( &memmap, estimated_offset, (memmap.max_len -estimated_offset), mt_unused);
  }
  /* sort entries by offset again */
  qsort(memmap.base_p, memmap.count, sizeof( mem_map_entry_t), compare_memmap);
  return &memmap;
}



void print_mem_map( mem_map_t * memmap_p ) {
	assert(NULL != memmap_p);
	mem_map_t memmap = *( memmap_p );
	printf("size of file is %u bytes\n", memmap.max_len);
	for (int j=0; j< memmap.count; j++) {
		mem_map_entry_t * i=memmap.base_p+j;
		printf ("entry=%03i, offset_start=%10u, count=%10u, offset_next=%10u, [%02i], type=%s\n", j,i->offset, i->count, (i->offset + i->count), i->mem_type, memtype_string[i->mem_type]);
	}
}

void print_mem_stats( mem_map_t * memmap_p) {
	uint32 size = memmap_p->max_len;
	uint32 stat[ mt_END_marker ];
	uint32 counted=0;
	for (int i=0; i< mt_END_marker; i++) {
		stat[i]=0;
	}

	for (int j=0; j< memmap_p->count; j++) {
		mem_map_entry_t * i=memmap_p->base_p+j;
		stat[i->mem_type]+= i->count;
		counted+=i->count;
	}
	for (int i=0; i<mt_END_marker; i++) {
		float ratio = ((float) (stat[i])) / ((float) size);
		printf("[%02i], type=%32s, bytes=%10u, ratio=%0.5f\n", i, memtype_string[i], stat[i], ratio);
	}
	printf("counted: %u bytes, size: %u bytes\n", counted, size);
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
