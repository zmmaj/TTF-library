

/** @addtogroup libttf
 * @{
 */
/**
 * @file Start 
 */

#ifndef _TTF_TTF_H
#define _TTF_TTF_H

#include <stdio.h>
#include <str.h>
#define READ_BE16(mem) ((((u8*)(mem))[0] << 8) | (((u8*)(mem))[1]))
#define READ_BE32(mem) ((((u8*)(mem))[0] << 24) | (((u8*)(mem))[1] << 16) | (((u8*)(mem))[2] << 8) | (((u8*)(mem))[3]))
#define P_MOVE(mem, a) ((mem) += (a))

#define READ_BE16_MOVE(mem) (READ_BE16((mem))); (P_MOVE((mem), 2))
#define READ_BE32_MOVE(mem) (READ_BE32((mem))); (P_MOVE((mem), 4))

#define FONT_TAG(a, b, c, d) (a<<24|b<<16|c<<8|d)
#define GLYF_TAG FONT_TAG('g', 'l', 'y', 'f')
#define LOCA_TAG FONT_TAG('l', 'o', 'c', 'a')
#define HEAD_TAG FONT_TAG('h', 'e', 'a', 'd')
#define CMAP_TAG FONT_TAG('c', 'm', 'a', 'p')


typedef unsigned char u8;
typedef char i8;

typedef unsigned short u16;
typedef short i16;

typedef unsigned int u32;
typedef int i32;

typedef struct {
	u32	scaler_type;
	u16	numTables;
	u16	searchRange;
	u16	entrySelector;
	u16	rangeShift;
} offset_subtable;


typedef struct {
	u16 platformID;
	u16 platformSpecificID;
	u32 offset;
} cmap_encoding_subtable;


typedef struct {
	u16 version;
	u16 numberSubtables;
	cmap_encoding_subtable* subtables;
} cmap;

typedef struct {
	u16  format;
 	u16  length;
 	u16  language;
 	u16  segCountX2;
 	u16  searchRange;
 	u16  entrySelector;
 	u16  rangeShift;
	u16  reservedPad;
	u16  *endCode;
	u16  *startCode;
	u16  *idDelta;
	u16  *idRangeOffset;
	u16  *glyphIdArray;
} format4;

typedef struct {
	union { 
		char tag_c[4];
		u32	tag;
	};
	u32	checkSum;
	u32	offset;
	u32	length;
} table_directory;

typedef struct  {
	offset_subtable off_sub;
	table_directory* tbl_dir;
	format4* f4;
	cmap* cmap;
	char* glyf;
	char* loca;
	char* head;
} font_directory; 

typedef union {
	  struct {
		u8 on_curve: 1;
		
		u8 x_short: 1;
		u8 y_short: 1;
		
		u8 repeat: 1;
		
		u8 x_short_pos: 1;
		u8 y_short_pos: 1;
		
		u8 reserved1: 1;
		u8 reserved2: 1;
	};
	u8 flag;
} glyph_flag;


typedef struct {
	u16 numberOfContours;
	i16 xMin;
	i16 yMin;
	i16 xMax;
	i16 yMax;
	u16 instructionLength;
	u8* instructions;
	glyph_flag* flags;
	i16* xCoordinates;
	i16* yCoordinates;
	u16* endPtsOfContours;
} glyph_outline;

extern char* read_file(char *file_name, int* file_size);
extern void read_offset_subtable(char** mem, offset_subtable* off_sub);
extern void read_cmap(char* mem, cmap* c);
extern void print_cmap(cmap* c);
extern void read_format4(char* mem, format4** format);
extern void print_format4(format4 *f4);
extern void read_table_directory(char* file_start, char** mem, font_directory* ft);
extern void print_table_directory(table_directory* tbl_dir, int tbl_size);
extern void read_font_directory(char* file_start, char** mem, font_directory* ft);
extern int get_glyph_index(font_directory* ft, u16 code_point);
extern int read_loca_type(font_directory* ft);
extern u32 get_glyph_offset(font_directory *ft, u32 glyph_index);
extern glyph_outline get_glyph_outline(font_directory* ft, u32 glyph_index);
extern void print_glyph_outline(glyph_outline *outline);

#endif

/** @}
 */
