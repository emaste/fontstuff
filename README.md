Newcons Unicode Font Stuff
==========================

This repository contains a bits and pieces related to fixed-size bitmap fonts.
The goal is to produce fonts for the FreeBSD Newcons console update project.

A number of tools here are from the GNU Unifont project,
http://czyborra.com/unifont/.  Roman Czyborra came up with a simple ASCII hex
format for font data which is perfectly suited to this application.

To try out one of the fonts in an xterm, use:
```sh
./hex2bdf newcons-default-normal.hex > font.bdf
mkfontdir
xset fp+ $(pwd)
xset fp rehash
xterm -fn -freebsd-misc-medium-r-normal--16-160-75-75-c-80-iso10646-1
```

iso-8x16.hex
------------
Source: FreeBSD share/syscons/fonts/iso-8x16.fnt  

k16-1990.hex
------------
Source: http://web.mit.edu/ghudson/dev/nokrb/third/intlfonts/Japanese/k16-1990.bdf  
License: public domain  
Notable content: CJK Symbols and Punctuation, Hiragana, Katakana, CJK Unified Ideographs  

newcons-default-{normal,bold}.hex
---------------------------------
Source: FreeBSD sys/dev/vt/font/vt_font_default.c  
License: 2-clause BSD  
