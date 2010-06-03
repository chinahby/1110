to make these png files, use Imagemagick:

mogrify -transparent #ff00ff -transparent #fe00fe -transparent #fd00fd -transparent #fc00fc -transparent #fb00fb -transparent #fa00fa -transparent #f900f9 -transparent #f800f8 -transparent #f700f7 -transparent #f600f6 -transparent #f500f5 -transparent #f400f4 -transparent #f300f3 -transparent #f200f2 -transparent #f100f1 -transparent #f000f0 -transparent #ff01ff -transparent #ff02ff -transparent #fe01fe -transparent #fe02fe -quality 90 -format png8 *.bmp

BREW can't support both transparency and alpha blending.  The Set Default menu 
look function in coreapp assumes transparency.  So, Unless (or until) the entire 
UI moves to Alpha blended bitmaps, we need to use PNG8 with transparency. 
(look up AEE_RO_BLEND vs. AEE_RO_TRANSPARENT) 

The other option would be to refactor menus so that all images in the menu use
Alpha blending, and then use a different menu look function for those.

You can get ImageMagick from www.imagemagick.org
