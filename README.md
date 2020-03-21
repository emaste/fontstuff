Newcons Unicode Font Stuff
==========================

This repository contains a bits and pieces related to fixed-size bitmap fonts.
The goal is to produce fonts for the FreeBSD Newcons console update project.

Some tools tools here are obtained from or similar to ones from Roman
Czyborra's GNU Unifont project.  Roman came up with a simple ASCII hex format
for bitmap font data which is perfectly suited to this application.

To try out one of the fonts in an xterm, use:
```sh
./hex2bdf vt-default-normal.hex > vt-default-normal.bdf
mkfontdir
xset fp+ $(pwd)
xset fp rehash
xterm -fn -freebsd-misc-medium-r-normal--16-160-75-75-c-80-iso10646-1
```

To produce a .bmp image showing each glyph in a .hex font:
```sh
cc -o fontpic gpl/fontpic.c
./fontpic < vt-default-normal.hex > vt-default-normal.bmp
```

Links
=====
* http://czyborra.com/unifont/ (Original GNU Unifont home)
* http://unifoundry.com/unifont.html (Unifont after becoming a GNU project)
* http://www.unicode.org/faq/han_cjk.html (Unicode CJK background)

Fonts in this Repository
========================

b16.hex
-------
Source: b16.bdf in http://openlab.ring.gr.jp/efont/dist/unicode-bdf/efont-unicode-bdf-0.4.2.tar.bz2  
License: 3-clause BSD license  

bold8x16.hex
------------
Source: http://ftp.netbsd.org/pub/NetBSD/NetBSD-current/src/sys/dev/wsfont/bold8x16.h  
License: Public domain  

gallant12x22.hex
----------------
Source: http://ftp.netbsd.org/pub/NetBSD/NetBSD-current/src/sys/dev/wsfont/gallant12x22.h  
License: 3-caluse BSD license  

iso-8x16.hex
------------
Source: FreeBSD share/syscons/fonts/iso-8x16.fnt  

k16-1990.hex
------------
Source: http://web.mit.edu/ghudson/dev/nokrb/third/intlfonts/Japanese/k16-1990.bdf  
License: public domain  
Notable content: CJK Symbols and Punctuation, Hiragana, Katakana, CJK Unified Ideographs  

vt-default-{normal,bold}.hex
----------------------------
Source: FreeBSD sys/dev/vt/font/vt_font_default.c  
License: 2-clause BSD  

syscons-fonts/
--------------
Source: FreeBSD /usr/share/syscons/fonts  
