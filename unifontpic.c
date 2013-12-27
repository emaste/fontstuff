/*
   unifontpic.c - see the "Big Picture": the entire Unifont in one BMP bitmap.

   Author: Paul Hardy, 2013

   Copyright (C) 2013 Paul Hardy

   LICENSE:

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 2 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSTRING 256

#define HEADER_STRING "GNU Unifont 6.3" /* to be printed as chart title */


int main(int argc, char **argv) {

   /* long and dpi are set from command-line options */
   int wide=1; /* =1 for a 256x256 grid, =0 for a 16x4096 grid */
   int dpi=96; /* change for 256x256 grid to fit paper if desired */
   int tinynum=0; /* whether to use tiny labels for 256x256 grid */

   int i; /* loop variable */

   int bitarray[0x10000][16]; /* 16 pixel rows for each of 65,536 glyphs */

   void gethex();
   void genlongbmp();
   void genwidebmp();

   memset((void *)bitarray, 0, 0x10000 * 16 * sizeof(int));

   gethex(bitarray); /* read .hex input file and fill bitarray with glyph data */

   if (argc > 1) {
      for (i = 1; i < argc; i++) {
         if (strncmp(argv[i],"-l",2) == 0) { /* long display */
           wide = 0;
         }
         else if (strncmp(argv[i],"-d",2) == 0) {
            dpi = atoi(&argv[i][2]); /* dots/inch specified on command line */
         }
         else if (strncmp(argv[i],"-t",2) == 0) {
            tinynum = 1;
         }
      }
   }

   if (wide) {
      genwidebmp(bitarray, dpi, tinynum); /* write bitarray glyph data to BMP file */
   }
   else {
      genlongbmp(bitarray, dpi, tinynum);
   }

   exit(EXIT_SUCCESS);
}


void output4(int thisword) {

   putchar( thisword        & 0xFF);
   putchar((thisword >>  8) & 0xFF);
   putchar((thisword >> 16) & 0xFF);
   putchar((thisword >> 24) & 0xFF);

   return;
}


void output2(int thisword) {

   putchar( thisword       & 0xFF);
   putchar((thisword >> 8) & 0xFF);

   return;
}


/*
   gethex() reads a Unifont .hex-format input file from stdin.
*/
void gethex(int bitarray[0x10000][16]) {

   char instring[MAXSTRING]; /* input buffer for a code point */
   char *bitstring;          /* pointer into instring for glyph bitmap */

   int i;
   int codept; /* the Unicode code point of the current glyph */

   /*
      Read each input line and place its glyph into the bit array.
   */
   while (fgets(instring, MAXSTRING, stdin) != NULL) {
      sscanf(instring, "%X", &codept);
      for (i = 0; (instring[i] != ':') && (i < 9); i++); /* find the colon separator */
      i++; /* position past it */
      bitstring = &instring[i];
      /*
         If this glyph is only 8 pixels wide, expand so right half of glyph is 0s.
      */
      if (strlen(bitstring) <= 33) { /* count terminating newline */
         for (i = 60; i >= 0; i -= 4) {
            bitstring[i + 3] = '0';
            bitstring[i + 2] = '0';
            bitstring[i + 1] = bitstring[(i >> 1) + 1];
            bitstring[i    ] = bitstring[ i >> 1     ];
         }
      }
      bitstring[64] = '\0'; /* truncate string, overwriting newline */

      for (i = 0; i < 16; i++) {
         sscanf(bitstring, "%4X", &bitarray[codept][i]);
         bitstring += 4;
      }
   }

   return;
}


/*
   genlongbmp() generates the BMP output file from a bitmap parameter.
   This is a long bitmap, 16 glyphs wide by 4,096 glyphs tall.
*/
void genlongbmp(int bitarray[0x10000][16], int dpi, int tinynum) {

   char header_string[17];
   int header[16][16]; /* header row, for chart title */
   int hdrlen;         /* length of HEADER_STRING */
   int startcol;       /* column to start printing header, for centering */

   unsigned leftcol[0x1000][16]; /* code point legend on left side of chart */
   int d1, d2, d3, d4;          /* digits for filling leftcol[][] legend   */
   int codept;                  /* current starting code point for legend  */
   int thisrow;                 /* glyph row currently being rendered */

   unsigned toprow[16][16];     /* code point legend on top of chart */

   /*
      hexdigit contains 4x5 pixel arrays of tiny digits for legend.
      See unihexgen.c for more detailed description in comments.
   */
   char hexdigit[16][5] = {
      {0x6,0x9,0x9,0x9,0x6},  /* 0x0 */
      {0x2,0x6,0x2,0x2,0x7},  /* 0x1 */
      {0xF,0x1,0xF,0x8,0xF},  /* 0x2 */
      {0xE,0x1,0x7,0x1,0xE},  /* 0x3 */
      {0x9,0x9,0xF,0x1,0x1},  /* 0x4 */
      {0xF,0x8,0xF,0x1,0xF},  /* 0x5 */
      {0x6,0x8,0xE,0x9,0x6},  /* 0x6 */
      {0xF,0x1,0x2,0x4,0x4},  /* 0x7 */
      {0x6,0x9,0x6,0x9,0x6},  /* 0x8 */
      {0x6,0x9,0x7,0x1,0x6},  /* 0x9 */
      {0xF,0x9,0xF,0x9,0x9},  /* 0xA */
      {0xE,0x9,0xE,0x9,0xE},  /* 0xB */
      {0x7,0x8,0x8,0x8,0x7},  /* 0xC */
      {0xE,0x9,0x9,0x9,0xE},  /* 0xD */
      {0xF,0x8,0xE,0x8,0xF},  /* 0xE */
      {0xF,0x8,0xE,0x8,0x8}   /* 0xF */
   };
   int digitrow;  /* row we're in (0..4) for the above hexdigit digits */

   /*
      DataOffset = BMP Header bytes + InfoHeader bytes + ColorTable bytes.
   */
   int DataOffset = 14 + 40 + 8; /* fixed size for monochrome BMP */
   int ImageSize;
   int FileSize;
   int Width, Height; /* bitmap image width and height in pixels */
   int ppm;     /* integer pixels per meter */

   int i, j, k;

   unsigned bytesout;

   void output4(int), output2(int);

   /*
      Image width and height, in pixels.

         N.B.: Width must be an even multiple of 32 pixels, or 4 bytes.
   */
   Width  =   18 * 16;  /* (2 legend +   16 glyphs) * 16 pixels/glyph */
   Height = 4099 * 16;  /* (1 header + 4096 glyphs) * 16 rows/glyph   */

   ImageSize = Height * (Width / 8); /* in bytes, calculated from pixels */

   FileSize = DataOffset + ImageSize;

   /* convert dots/inch to pixels/meter */
   if (dpi == 0) dpi = 96;
   ppm = (int)((double)dpi * 100.0 / 2.54 + 0.5);

   /*
      Generate the BMP Header
   */
   putchar('B');
   putchar('M');

   /*
      Calculate file size:

         BMP Header + InfoHeader + Color Table + Raster Data
   */
   output4(FileSize);  /* FileSize */
   output4(0x0000); /* reserved */

   /* Calculate DataOffset */
   output4(DataOffset);

   /*
      InfoHeader
   */
   output4(40);         /* Size of InfoHeader                       */
   output4(Width);      /* Width of bitmap in pixels                */
   output4(Height);     /* Height of bitmap in pixels               */
   output2(1);          /* Planes (1 plane)                         */
   output2(1);          /* BitCount (1 = monochrome)                */
   output4(0);          /* Compression (0 = none)                   */
   output4(ImageSize);  /* ImageSize, in bytes                      */
   output4(ppm);        /* XpixelsPerM (96 dpi = 3780 pixels/meter) */
   output4(ppm);        /* YpixelsPerM (96 dpi = 3780 pixels/meter) */
   output4(2);          /* ColorsUsed (= 2)                         */
   output4(2);          /* ColorsImportant (= 2)                    */
   output4(0x00000000); /* black (reserved, B, G, R)                */
   output4(0x00FFFFFF); /* white (reserved, B, G, R)                */

   /*
      Create header row bits.
   */
   memset((void *)header, 0, 16 * 16 * sizeof(int)); /* fill with white */
   memset((void *)header_string, ' ', 16 * sizeof(char)); /* 16 spaces */
   header_string[16] = '\0';  /* null-terminated */

   hdrlen = strlen(HEADER_STRING);
   if (hdrlen > 16) hdrlen = 16;        /* only 16 columns to print header */
   startcol = 8 - ((hdrlen + 1) >> 1);                 /* to center header */
   strncpy(&header_string[startcol], HEADER_STRING, hdrlen); /* center up to 16 chars */

   /* Copy each letter's bitmap from the bitarray[][] we constructed. */
   for (j = 0; j < 16; j++) {
      for (i = 0; i < 16; i++) {
         header[i][j] = bitarray[(unsigned)header_string[j]][i];
      }
   }

   /*
      Create the left column legend.
   */
   memset((void *)leftcol, 0, 4096 * 16 * sizeof(unsigned));

   for (codept = 0x0000; codept < 0x10000; codept += 0x10) {
      d1 = (codept >> 12) & 0xF; /* most significant hex digit */
      d2 = (codept >>  8) & 0xF;
      d3 = (codept >>  4) & 0xF;
//    d4 =  codept        & 0xF; /* least significant hex digit */

      thisrow = codept >> 4; /* rows of 16 glyphs */

      /* fill in first and second digits */
      for (digitrow = 0; digitrow < 5; digitrow++) {
         leftcol[thisrow][2 + digitrow] =
            (hexdigit[d1][digitrow] << 10) |
            (hexdigit[d2][digitrow] <<  4);
      }

      /* fill in third digit */
      for (digitrow = 0; digitrow < 5; digitrow++) {
         leftcol[thisrow][9 + digitrow] = hexdigit[d3][digitrow] << 10;
      }
      leftcol[thisrow][9 + 4] |= 0xF << 4; /* underscore as 4th digit */

      for (i = 0; i < 15; i ++) {
         leftcol[thisrow][i] |= 0x00000002;      /* right border */
      }

      leftcol[thisrow][15] = 0x0000FFFE;        /* bottom border */

      if (d3 == 0xF) {                     /* 256-point boundary */
         leftcol[thisrow][15] |= 0x00FF0000;  /* longer tic mark */
      }

      if ((thisrow % 0x40) == 0x3F) {    /* 1024-point boundary */
         leftcol[thisrow][15] |= 0xFFFF0000; /* longest tic mark */
      }
   }

   /*
      Create the top row legend.
   */
   memset((void *)toprow, 0, 16 * 16 * sizeof(unsigned));

   for (codept = 0x0; codept <= 0xF; codept++) {
      d1 = (codept >> 12) & 0xF; /* most significant hex digit */
      d2 = (codept >>  8) & 0xF;
      d3 = (codept >>  4) & 0xF;
      d4 =  codept        & 0xF; /* least significant hex digit */

      /* fill in last digit */
      for (digitrow = 0; digitrow < 5; digitrow++) {
         toprow[6 + digitrow][codept] = hexdigit[d4][digitrow] << 6;
      }
   }

   for (j = 0; j < 16; j++) {
      /* force bottom pixel row to be white, for separation from glyphs */
      toprow[15][j] = 0x0000;
   }

   /* 1 pixel row with left-hand legend line */
   for (j = 0; j < 16; j++) {
      toprow[14][j] |= 0xFFFF;
   }

   /* 14 rows with line on left to fill out this character row */
   for (i = 13; i >= 0; i--) {
      for (j = 0; j < 16; j++) {
         toprow[i][j] |= 0x0001;
      }
   }

   /*
      Now write the raster image.

      XOR each byte with 0xFF because black = 0, white = 1 in BMP.
   */

   /* Write the glyphs, bottom-up, left-to-right, in rows of 16 (i.e., 0x10) */
   for (i = 0xFFF0; i >= 0; i -= 0x10) {
      thisrow = i >> 4; /* 16 glyphs per row */
      for (j = 15; j >= 0; j--) {
         /* left-hand legend */
         putchar((~leftcol[thisrow][j] >> 24) & 0xFF);
         putchar((~leftcol[thisrow][j] >> 16) & 0xFF);
         putchar((~leftcol[thisrow][j] >>  8) & 0xFF);
         putchar( ~leftcol[thisrow][j]        & 0xFF);
         /* Unifont glyph */
         for (k = 0; k < 16; k++) {
            bytesout = ~bitarray[i+k][j] & 0xFFFF;
            putchar((bytesout >> 8) & 0xFF);
            putchar( bytesout       & 0xFF);
         }
      }
   }

   /*
      Write the top legend.
   */
   /* i == 15: bottom pixel row of header is output here */
   /* left-hand legend: solid black line except for right-most pixel */
   putchar(0x00);
   putchar(0x00);
   putchar(0x00);
   putchar(0x01);
   for (j = 0; j < 16; j++) {
      putchar((~toprow[15][j] >> 8) & 0xFF);
      putchar( ~toprow[15][j]       & 0xFF);
   }

   putchar(0xFF);
   putchar(0xFF);
   putchar(0xFF);
   putchar(0xFC);
   for (j = 0; j < 16; j++) {
      putchar((~toprow[14][j] >> 8) & 0xFF);
      putchar( ~toprow[14][j]       & 0xFF);
   }

   for (i = 13; i >= 0; i--) {
      putchar(0xFF);
      putchar(0xFF);
      putchar(0xFF);
      putchar(0xFD);
      for (j = 0; j < 16; j++) {
         putchar((~toprow[i][j] >> 8) & 0xFF);
         putchar( ~toprow[i][j]       & 0xFF);
      }
   }

   /*
      Write the header.
   */

   /* 7 completely white rows */
   for (i = 7; i >= 0; i--) {
      for (j = 0; j < 18; j++) {
         putchar(0xFF);
         putchar(0xFF);
      }
   }

   for (i = 15; i >= 0; i--) {
      /* left-hand legend */
      putchar(0xFF);
      putchar(0xFF);
      putchar(0xFF);
      putchar(0xFF);
      /* header glyph */
      for (j = 0; j < 16; j++) {
         bytesout = ~header[i][j] & 0xFFFF;
         putchar((bytesout >> 8) & 0xFF);
         putchar( bytesout       & 0xFF);
      }
   }

   /* 8 completely white rows at very top */
   for (i = 7; i >= 0; i--) {
      for (j = 0; j < 18; j++) {
      putchar(0xFF);
      putchar(0xFF);
      }
   }

   return;
}



/*
   genwidebmp() generates the BMP output file from a bitmap parameter.
   This is a wide bitmap, 256 glyphs wide by 256 glyphs tall.
*/
void genwidebmp(int bitarray[0x10000][16], int dpi, int tinynum) {

   char header_string[257];
   int header[16][256]; /* header row, for chart title */
   int hdrlen;         /* length of HEADER_STRING */
   int startcol;       /* column to start printing header, for centering */

   unsigned leftcol[0x100][16]; /* code point legend on left side of chart */
   int d1, d2, d3, d4;          /* digits for filling leftcol[][] legend   */
   int codept;                  /* current starting code point for legend  */
   int thisrow;                 /* glyph row currently being rendered */

   unsigned toprow[32][256];     /* code point legend on top of chart */

   /*
      hexdigit contains 4x5 pixel arrays of tiny digits for legend.
      See unihexgen.c for more detailed description in comments.
   */
   char hexdigit[16][5] = {
      {0x6,0x9,0x9,0x9,0x6},  /* 0x0 */
      {0x2,0x6,0x2,0x2,0x7},  /* 0x1 */
      {0xF,0x1,0xF,0x8,0xF},  /* 0x2 */
      {0xE,0x1,0x7,0x1,0xE},  /* 0x3 */
      {0x9,0x9,0xF,0x1,0x1},  /* 0x4 */
      {0xF,0x8,0xF,0x1,0xF},  /* 0x5 */
      {0x8,0x8,0xF,0x9,0xF},  /* 0x6 */
      {0xF,0x1,0x2,0x4,0x4},  /* 0x7 */
      {0x6,0x9,0x6,0x9,0x6},  /* 0x8 */
      {0xF,0x9,0xF,0x1,0x1},  /* 0x9 */
      {0xF,0x9,0xF,0x9,0x9},  /* 0xA */
      {0xE,0x9,0xE,0x9,0xE},  /* 0xB */
      {0x7,0x8,0x8,0x8,0x7},  /* 0xC */
      {0xE,0x9,0x9,0x9,0xE},  /* 0xD */
      {0xF,0x8,0xE,0x8,0xF},  /* 0xE */
      {0xF,0x8,0xE,0x8,0x8}   /* 0xF */
   };
   int digitrow;  /* row we're in (0..4) for the above hexdigit digits */
   int hexalpha1, hexalpha2; /* to convert hex digits to ASCII */

   /*
      DataOffset = BMP Header bytes + InfoHeader bytes + ColorTable bytes.
   */
   int DataOffset = 14 + 40 + 8; /* fixed size for monochrome BMP */
   int ImageSize;
   int FileSize;
   int Width, Height; /* bitmap image width and height in pixels */
   int ppm;     /* integer pixels per meter */

   int i, j, k;

   unsigned bytesout;

   void output4(int), output2(int);

   /*
      Image width and height, in pixels.

         N.B.: Width must be an even multiple of 32 pixels, or 4 bytes.
   */
   Width  = 258 * 16;  /* (           2 legend + 256 glyphs) * 16 pixels/glyph */
   Height = 260 * 16;  /* (2 header + 2 legend + 256 glyphs) * 16 rows/glyph   */

   ImageSize = Height * (Width / 8); /* in bytes, calculated from pixels */

   FileSize = DataOffset + ImageSize;

   /* convert dots/inch to pixels/meter */
   if (dpi == 0) dpi = 96;
   ppm = (int)((double)dpi * 100.0 / 2.54 + 0.5);

   /*
      Generate the BMP Header
   */
   putchar('B');
   putchar('M');
   /*
      Calculate file size:

         BMP Header + InfoHeader + Color Table + Raster Data
   */
   output4(FileSize);  /* FileSize */
   output4(0x0000); /* reserved */
   /* Calculate DataOffset */
   output4(DataOffset);

   /*
      InfoHeader
   */
   output4(40);         /* Size of InfoHeader                       */
   output4(Width);      /* Width of bitmap in pixels                */
   output4(Height);     /* Height of bitmap in pixels               */
   output2(1);          /* Planes (1 plane)                         */
   output2(1);          /* BitCount (1 = monochrome)                */
   output4(0);          /* Compression (0 = none)                   */
   output4(ImageSize);  /* ImageSize, in bytes                      */
   output4(ppm);        /* XpixelsPerM (96 dpi = 3780 pixels/meter) */
   output4(ppm);        /* YpixelsPerM (96 dpi = 3780 pixels/meter) */
   output4(2);          /* ColorsUsed (= 2)                         */
   output4(2);          /* ColorsImportant (= 2)                    */
   output4(0x00000000); /* black (reserved, B, G, R)                */
   output4(0x00FFFFFF); /* white (reserved, B, G, R)                */

   /*
      Create header row bits.
   */
   memset((void *)header, 0, 256 * 16 * sizeof(int)); /* fill with white */
   memset((void *)header_string, ' ', 256 * sizeof(char)); /* 256 spaces */
   header_string[256] = '\0';  /* null-terminated */

   hdrlen = strlen(HEADER_STRING);
   if (hdrlen > 256) hdrlen = 256;        /* only 256 columns to print header */
   startcol = 128 - ((hdrlen + 1) >> 1);                 /* to center header */
   strncpy(&header_string[startcol], HEADER_STRING, hdrlen); /* center up to 16 chars */

   /* Copy each letter's bitmap from the bitarray[][] we constructed. */
   for (j = 0; j < 256; j++) {
      for (i = 0; i < 16; i++) {
         header[i][j] = bitarray[(unsigned)header_string[j]][i];
      }
   }

   /*
      Create the left column legend.
   */
   memset((void *)leftcol, 0, 256 * 16 * sizeof(unsigned));

   for (codept = 0x0000; codept < 0x10000; codept += 0x100) {
      d1 = (codept >> 12) & 0xF; /* most significant hex digit */
      d2 = (codept >>  8) & 0xF;
  //  d3 = (codept >>  4) & 0xF;
  //  d4 =  codept        & 0xF; /* least significant hex digit */

      thisrow = codept >> 8; /* rows of 256 glyphs */

      /* fill in first and second digits */

      if (tinynum) { /* use 4x5 pixel glyphs */
         for (digitrow = 0; digitrow < 5; digitrow++) {
            leftcol[thisrow][6 + digitrow] =
               (hexdigit[d1][digitrow] << 10) |
               (hexdigit[d2][digitrow] <<  4);
         }
      }
      else { /* bigger numbers -- use glyphs from Unifont itself */
         /* convert hexadecimal digits to ASCII equivalent */
         hexalpha1 = d1 < 0xA ? '0' + d1 : 'A' + d1 - 0xA;
         hexalpha2 = d2 < 0xA ? '0' + d2 : 'A' + d2 - 0xA;

         for (i = 0 ; i < 16; i++) {
            leftcol[thisrow][i] =
               (bitarray[hexalpha1][i] << 2) |
               (bitarray[hexalpha2][i] >> 6);
         }
      }

      for (i = 0; i < 15; i ++) {
         leftcol[thisrow][i] |= 0x00000002;      /* right border */
      }

      leftcol[thisrow][15] = 0x0000FFFE;        /* bottom border */

      if (d2 == 0xF) {                     /* 4096-point boundary */
         leftcol[thisrow][15] |= 0x00FF0000;  /* longer tic mark */
      }

      if ((thisrow % 0x40) == 0x3F) {    /* 16,384-point boundary */
         leftcol[thisrow][15] |= 0xFFFF0000; /* longest tic mark */
      }
   }

   /*
      Create the top row legend.
   */
   memset((void *)toprow, 0, 32 * 256 * sizeof(unsigned));

   for (codept = 0x00; codept <= 0xFF; codept++) {
//    d1 = (codept >> 12) & 0xF; /* most significant hex digit */
//    d2 = (codept >>  8) & 0xF;
      d3 = (codept >>  4) & 0xF;
      d4 =  codept        & 0xF; /* least significant hex digit */

      if (tinynum) {
         for (digitrow = 0; digitrow < 5; digitrow++) {
            toprow[16 + 6 + digitrow][codept] =
               (hexdigit[d3][digitrow] << 10) |
               (hexdigit[d4][digitrow] <<  4);
         }
      }
      else {
         /* convert hexadecimal digits to ASCII equivalent */
         hexalpha1 = d3 < 0xA ? '0' + d3 : 'A' + d3 - 0xA;
         hexalpha2 = d4 < 0xA ? '0' + d4 : 'A' + d4 - 0xA;
         for (i = 0 ; i < 16; i++) {
            toprow[14 + i][codept] =
               (bitarray[hexalpha1][i]     ) |
               (bitarray[hexalpha2][i] >> 7);
         }
      }
   }

   for (j = 0; j < 256; j++) {
      /* force bottom pixel row to be white, for separation from glyphs */
      toprow[16 + 15][j] = 0x0000;
   }

   /* 1 pixel row with left-hand legend line */
   for (j = 0; j < 256; j++) {
      toprow[16 + 14][j] |= 0xFFFF;
   }

   /* 14 rows with line on left to fill out this character row */
   for (i = 13; i >= 0; i--) {
      for (j = 0; j < 256; j++) {
         toprow[16 + i][j] |= 0x0001;
      }
   }

   /* Form the longer tic marks in top legend */
   for (i = 8; i < 16; i++) {
      for (j = 0x0F; j < 0x100; j += 0x10) {
         toprow[i][j] |= 0x0001;
      }
   }

   /*
      Now write the raster image.

      XOR each byte with 0xFF because black = 0, white = 1 in BMP.
   */

   /* Write the glyphs, bottom-up, left-to-right, in rows of 16 (i.e., 0x10) */
   for (i = 0xFF00; i >= 0; i -= 0x100) {
      thisrow = i >> 8; /* 256 glyphs per row */
      for (j = 15; j >= 0; j--) {
         /* left-hand legend */
         putchar((~leftcol[thisrow][j] >> 24) & 0xFF);
         putchar((~leftcol[thisrow][j] >> 16) & 0xFF);
         putchar((~leftcol[thisrow][j] >>  8) & 0xFF);
         putchar( ~leftcol[thisrow][j]        & 0xFF);
         /* Unifont glyph */
         for (k = 0x00; k < 0x100; k++) {
            bytesout = ~bitarray[i+k][j] & 0xFFFF;
            putchar((bytesout >> 8) & 0xFF);
            putchar( bytesout       & 0xFF);
         }
      }
   }

   /*
      Write the top legend.
   */
   /* i == 15: bottom pixel row of header is output here */
   /* left-hand legend: solid black line except for right-most pixel */
   putchar(0x00);
   putchar(0x00);
   putchar(0x00);
   putchar(0x01);
   for (j = 0; j < 256; j++) {
      putchar((~toprow[16 + 15][j] >> 8) & 0xFF);
      putchar( ~toprow[16 + 15][j]       & 0xFF);
   }

   putchar(0xFF);
   putchar(0xFF);
   putchar(0xFF);
   putchar(0xFC);
   for (j = 0; j < 256; j++) {
      putchar((~toprow[16 + 14][j] >> 8) & 0xFF);
      putchar( ~toprow[16 + 14][j]       & 0xFF);
   }

   for (i = 16 + 13; i >= 0; i--) {
      if (i >= 8) { /* make vertical stroke on right */
         putchar(0xFF);
         putchar(0xFF);
         putchar(0xFF);
         putchar(0xFD);
      }
      else { /* all white */
         putchar(0xFF);
         putchar(0xFF);
         putchar(0xFF);
         putchar(0xFF);
      }
      for (j = 0; j < 256; j++) {
         putchar((~toprow[i][j] >> 8) & 0xFF);
         putchar( ~toprow[i][j]       & 0xFF);
      }
   }

   /*
      Write the header.
   */

   /* 8 completely white rows */
   for (i = 7; i >= 0; i--) {
      for (j = 0; j < 258; j++) {
         putchar(0xFF);
         putchar(0xFF);
      }
   }

   for (i = 15; i >= 0; i--) {
      /* left-hand legend */
      putchar(0xFF);
      putchar(0xFF);
      putchar(0xFF);
      putchar(0xFF);
      /* header glyph */
      for (j = 0; j < 256; j++) {
         bytesout = ~header[i][j] & 0xFFFF;
         putchar((bytesout >> 8) & 0xFF);
         putchar( bytesout       & 0xFF);
      }
   }

   /* 8 completely white rows at very top */
   for (i = 7; i >= 0; i--) {
      for (j = 0; j < 258; j++) {
      putchar(0xFF);
      putchar(0xFF);
      }
   }

   return;
}


