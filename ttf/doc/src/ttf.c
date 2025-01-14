

/** @addtogroup libttf
 * @{
 */
/**
 * @file Start 
 */


#include <stdlib.h>
#include <str.h>
#include "ttf.h"


char* read_file(char *file_name, int* file_size) { 
	if(str_length(file_name) > 0) {
		FILE* file = fopen(file_name, "rb");
		if(file) {
			fseek(file, 0, SEEK_END);
			int size = ftell(file);
			fseek(file, 0, SEEK_SET);

			if(file_size) { *file_size = size; }
			char *file_content = (char*)malloc(size+1);
			int read_amount = fread(file_content, size, 1, file);
			file_content[size] = '\0';
			if(read_amount) {
				fclose(file);
				return file_content;
			}
			free(file_content);
			fclose(file);
			return NULL;
		}
	}
	return NULL;
}

void read_offset_subtable(char** mem, offset_subtable* off_sub) {
	char* m = *mem;
	off_sub->scaler_type = READ_BE32_MOVE(m);
	off_sub->numTables = READ_BE16_MOVE(m);
	off_sub->searchRange = READ_BE16_MOVE(m);
	off_sub->entrySelector = READ_BE16_MOVE(m);
	off_sub->rangeShift = READ_BE16_MOVE(m);

	*mem = m;
}


void read_cmap(char* mem, cmap* c) {
	char *m = mem;
	c->version = READ_BE16_MOVE(m);
	c->numberSubtables = READ_BE16_MOVE(m);

	c->subtables = (cmap_encoding_subtable*) calloc(1, sizeof(cmap_encoding_subtable)*c->numberSubtables);
	for(int i = 0; i < c->numberSubtables; ++i) {
		cmap_encoding_subtable* est = c->subtables + i;
		est->platformID = READ_BE16_MOVE(m);
		est->platformSpecificID = READ_BE16_MOVE(m);
		est->offset = READ_BE32_MOVE(m);
	}

}

void print_cmap(cmap* c ) {
	printf("#)\tpId\tpsID\toffset\ttype\n");
	for(int i = 0; i < c->numberSubtables; ++i) {
		cmap_encoding_subtable* cet = c->subtables + i;
		printf("%d)\t%d\t%d\t%d\t", i+1, cet->platformID, cet->platformSpecificID, cet->offset);
		switch(cet->platformID) {
			case 0: printf("Unicode"); break;
			case 1: printf("Mac"); break;
			case 2: printf("Not Supported"); break;
			case 3: printf("Microsoft"); break;
		}
		printf("\n");
	}
}

void read_format4(char* mem, format4** format) {
	char* m = mem;

	u16 length = READ_BE16(m + 2);

	format4* f = NULL;

	f = (format4*) calloc(1, length + sizeof(u16*)*5);
	f->format = READ_BE16_MOVE(m);
	f->length = READ_BE16_MOVE(m);
	f->language = READ_BE16_MOVE(m);
	f->segCountX2 = READ_BE16_MOVE(m);
	f->searchRange = READ_BE16_MOVE(m);
	f->entrySelector = READ_BE16_MOVE(m);
	f->rangeShift = READ_BE16_MOVE(m);

	f->endCode = (u16*) ((u8*)f  + sizeof(format4));
	f->startCode = f->endCode + f->segCountX2/2;
	f->idDelta = f->startCode + f->segCountX2/2;
	f->idRangeOffset = f->idDelta + f->segCountX2/2;
	f->glyphIdArray = f->idRangeOffset + f->segCountX2/2;

	char* start_code_start = m + f->segCountX2 + 2;
	char* id_delta_start = m + f->segCountX2*2 + 2;
	char* id_range_start = m + f->segCountX2*3 + 2;

	for(int i = 0; i < f->segCountX2/2; ++i) {
		f->endCode[i] = READ_BE16(m + i*2);
		f->startCode[i] = READ_BE16(start_code_start + i*2);
		f->idDelta[i] = READ_BE16(id_delta_start + i*2);
		f->idRangeOffset[i] = READ_BE16(id_range_start + i*2);
	}

	P_MOVE(m, f->segCountX2*4 + 2);	

	int remaining_bytes = f->length - (m - mem);
	for(int i = 0; i < remaining_bytes/2; ++i) {
		f->glyphIdArray[i] = READ_BE16_MOVE(m);
	}

	*format = f;
}

void print_format4(format4 *f4) {
	printf("Format: %d, Length: %d, Language: %d, Segment Count: %d\n", f4->format, f4->length, f4->language, f4->segCountX2/2);
	printf("Search Params: (searchRange: %d, entrySelector: %d, rangeShift: %d)\n",
			f4->searchRange, f4->entrySelector, f4->rangeShift);
	printf("Segment Ranges:\tstartCode\tendCode\tidDelta\tidRangeOffset\n");
	for(int i = 0; i < f4->segCountX2/2; ++i) {
		printf("--------------:\t% 9d\t% 7d\t% 7d\t% 12d\n", f4->startCode[i], f4->endCode[i], f4->idDelta[i], f4->idRangeOffset[i]);
	}
}


void read_table_directory(char* file_start, char** mem, font_directory* ft) {
	char* m = *mem;
	ft->tbl_dir = (table_directory*)calloc(1, sizeof(table_directory)*ft->off_sub.numTables);

	for(int i = 0; i < ft->off_sub.numTables; ++i) {
		table_directory* t = ft->tbl_dir + i;
		t->tag = READ_BE32_MOVE(m);
		t->checkSum = READ_BE32_MOVE(m);
		t->offset = READ_BE32_MOVE(m);
		t->length = READ_BE32_MOVE(m);

		switch(t->tag) {
			case GLYF_TAG: ft->glyf = t->offset + file_start; break;
			case LOCA_TAG: ft->loca = t->offset + file_start; break;
			case HEAD_TAG: ft->head = t->offset + file_start; break;
			case CMAP_TAG: {
				ft->cmap = (cmap*) calloc(1, sizeof(cmap));
				read_cmap(file_start + t->offset, ft->cmap);
				read_format4(file_start + t->offset + ft->cmap->subtables[0].offset, &ft->f4);
			} break;
		}
	}

	*mem = m;
}

void print_table_directory(table_directory* tbl_dir, int tbl_size) {
	printf("#)\ttag\tlen\toffset\n");
	for(int i = 0; i < tbl_size; ++i) {
		table_directory* t = tbl_dir + i;
		printf("%d)\t%c%c%c%c\t%d\t%d\n", i+1,
				t->tag_c[3], t->tag_c[2],
				t->tag_c[1], t->tag_c[0],
				t->length, t->offset);
	}
}

void read_font_directory(char* file_start, char** mem, font_directory* ft) {
	read_offset_subtable(mem, &ft->off_sub); 
	read_table_directory(file_start, mem, ft);
}


int get_glyph_index(font_directory* ft, u16 code_point) {
	format4 *f = ft->f4;
	int index = -1;
	u16 *ptr = NULL;
	for(int i = 0; i < f->segCountX2/2; i++) {
		if(f->endCode[i] > code_point) {index = i; break;};
	}
	
	if(index == -1) return 0;

	if(f->startCode[index] < code_point) {
		if(f->idRangeOffset[index] != 0) {
			ptr = f->idRangeOffset + index + f->idRangeOffset[index]/2;
			ptr += code_point - f->startCode[index];
			if(*ptr == 0) return 0;
			return *ptr + f->idDelta[index];
		} else {
			return code_point + f->idDelta[index];
		}
	}

	return 0;
}

int read_loca_type(font_directory* ft) {
	return READ_BE16(ft->head + 50);
}

u32 get_glyph_offset(font_directory *ft, u32 glyph_index) {
	u32 offset = 0;
	if(read_loca_type(ft)) {
		//32 bit
		offset = READ_BE32((u32*)ft->loca + glyph_index);
	} else {
		offset =  READ_BE16((u16*)ft->loca + glyph_index)*2;
	}
	return offset;
}


glyph_outline get_glyph_outline(font_directory *ft, u32 glyph_index) {
	
	u32 offset = get_glyph_offset(ft, glyph_index);
	unsigned char* glyph_start = (unsigned char* ) ft->glyf + offset;
	glyph_outline outline = {0};
	outline.numberOfContours = READ_BE16_MOVE(glyph_start);
	outline.xMin = READ_BE16_MOVE(glyph_start);
	outline.yMin = READ_BE16_MOVE(glyph_start);
	outline.xMax = READ_BE16_MOVE(glyph_start);
	outline.yMax = READ_BE16_MOVE(glyph_start);

	outline.endPtsOfContours = (u16*) calloc(1, outline.numberOfContours*sizeof(u16));
	for(int i = 0; i < outline.numberOfContours; ++i) {
		outline.endPtsOfContours[i] = READ_BE16_MOVE(glyph_start);
	}

	outline.instructionLength = READ_BE16_MOVE(glyph_start);
	outline.instructions = (u8*)calloc(1, outline.instructionLength);
	memcpy(outline.instructions, glyph_start, outline.instructionLength);
	P_MOVE(glyph_start, outline.instructionLength);

	int last_index = outline.endPtsOfContours[outline.numberOfContours-1];
	outline.flags = (glyph_flag*) calloc(1, last_index + 1);

	for(int i = 0; i < (last_index + 1); ++i) {
		outline.flags[i].flag = *glyph_start;
		glyph_start++;
		if(outline.flags[i].repeat) {
			u8 repeat_count = *glyph_start;
			while(repeat_count-- > 0) {
				i++;
				outline.flags[i] = outline.flags[i-1];
			}
			glyph_start++;
		}
	}


	outline.xCoordinates = (i16*) calloc(1, (last_index+1)*2);
	i16 prev_coordinate = 0;
	i16 current_coordinate = 0;
	for(int i = 0; i < (last_index+1); ++i) {
		int flag_combined = outline.flags[i].x_short << 1 | outline.flags[i].x_short_pos;
		switch(flag_combined) {
			case 0: {
				current_coordinate = READ_BE16_MOVE(glyph_start);
			} break;
			case 1: { current_coordinate = 0; }break;
			case 2: { current_coordinate = (*glyph_start++)*-1; }break;
			case 3: { current_coordinate = (*glyph_start++); } break;
		}

		outline.xCoordinates[i] = current_coordinate + prev_coordinate;
		prev_coordinate = outline.xCoordinates[i];
	}

	outline.yCoordinates = (i16*) calloc(1, (last_index+1)*2);
	current_coordinate = 0;
	prev_coordinate = 0;
	for(int i = 0; i < (last_index+1); ++i) {
		int flag_combined = outline.flags[i].y_short << 1 | outline.flags[i].y_short_pos;
		switch(flag_combined) {
			case 0: {
				current_coordinate = READ_BE16_MOVE(glyph_start);
			} break;
			case 1: { current_coordinate = 0; }break;
			case 2: { current_coordinate = (*glyph_start++)*-1; }break;
			case 3: { current_coordinate = (*glyph_start++); } break;
		}

		outline.yCoordinates[i] = current_coordinate + prev_coordinate;
		prev_coordinate = outline.yCoordinates[i];
	}

	return outline;
}

void print_glyph_outline(glyph_outline *outline) {
 FILE *fptr;
fptr = fopen("../FontTest.txt", "w");
  // write to file
    fprintf(fptr, "#contours\t(xMin,yMin)\t(xMax,yMax)\tinst_length\n");
	fprintf(fptr, "%9d\t(%d,%d)\t\t(%d,%d)\t%d\n", outline->numberOfContours,
			outline->xMin, outline->yMin,
			outline->xMax, outline->yMax,
			outline->instructionLength);
	// write to screen
	printf("%9d\t(%d,%d)\t\t(%d,%d)\t%d\n", outline->numberOfContours,
			outline->xMin, outline->yMin,
			outline->xMax, outline->yMax,
			outline->instructionLength);

         // Write head text to the file
        fprintf(fptr, "    \t(  x  ,  y  )\n");
		// Write head text to the screen
		printf("#)\t(  x  ,  y  )\n"); // print to screen

	int last_index = outline->endPtsOfContours[outline->numberOfContours-1];
	for(int i = 0; i <= last_index; ++i) {

      // Write all text to the file
       fprintf(fptr, "%d)\t(%5d,%5d)\n", i, outline->xCoordinates[i], outline->yCoordinates[i]);
      // write first 20 lines to the screen , text is too big
	  if (i<20)
	  printf("%d)\t(%5d,%5d)\n", i, outline->xCoordinates[i], outline->yCoordinates[i]);
	}
	
// close file fptr
fclose(fptr);
 
}




/** @}
 */
