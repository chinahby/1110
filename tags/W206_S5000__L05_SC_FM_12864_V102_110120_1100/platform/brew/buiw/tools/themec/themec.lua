WidgetProps = {
   borderwidth                =  0x100; -- both active and inactive widths
   active_borderwidth         =  0x101; -- border width when widget is active
   inactive_borderwidth       =  0x102; -- border width when widget is inactive
                              
   bordercolor                =  0x110; -- all border colors to same value
   active_bordercolor         =  0x111; -- border color when widget is active
   inactive_bordercolor       =  0x112; -- border color when widget is inactive
   sactive_bordercolor        =  0x113; -- border color when widget is selected/active
   sinactive_bordercolor      =  0x114; -- border color when widget is selected/inactive
   selected_bordercolor       =  0x115; -- border color for selected (active&inactive)
                              
   shadowoffset               =  0x203; -- set (only) shadow offset x and y to same value (negative value == shadow on top,left)
   shadowoffsetx              =  0x204; -- shadow offset in x direction (negative value == shadow on left) 
   shadowoffsety              =  0x205; -- shadow offset in y direction (negative value == shadow on top)
   shadowcolor                =  0x206; -- shadow color 

   selected_shadowoffset      =  0x250; -- set (only) shadow ofset (selected, all)
   selected_shadowcolor       =  0x251; -- set (only) shadow (selected, all)
   selected_shadowoffsetx     =  0x252; -- set (only) shadow x offset (selected, all)
   selected_shadowoffsety     =  0x253; -- set (only) shadow x offset (selected, all)

   -- bg colors
   bgcolor                    =  0x130; -- all bgcolors to same value
   active_bgcolor             =  0x131; -- bgcolor when widget is active
   inactive_bgcolor           =  0x132; -- bgcolor when widget is inactive 
   sactive_bgcolor            =  0x133; -- bgcolor when widget is selected/active  
   sinactive_bgcolor          =  0x134; -- bgcolor when widget is selected/inactive   
   selected_bgcolor           =  0x135; -- bgcolor for selected (active&inactive)
   text_sactive_bgcolor       =  0x136; -- bgcolor for selected text (active)
   text_sinactive_bgcolor     =  0x137; -- bgcolor for selected text (inactive)
   text_selected_bgcolor      =  0x138; -- bgcolor for selected text (active&inactive)
                             
   -- gradient                
   gradient_style             =  0x210; -- gradient fill style
   gradient                   =  0x238; -- background gradient end color (all)
   selected_gradient          =  0x239; -- background gradient end color (selected, all)
   active_gradient            =  0x240; -- background gradient end color (active)
   inactive_gradient          =  0x241; -- background gradient end color (inactive) 
   sactive_gradient           =  0x242; -- background gradient end color (selected,active)  
   sinactive_gradient         =  0x243; -- background gradient end color (selected,inactive)   
                              
   -- fg colors               
   fgcolor                    =  0x140; -- all fgcolors to same value
   active_fgcolor             =  0x141; -- fgcolor when widget is active           
   inactive_fgcolor           =  0x142; -- fgcolor when widget is inactive         
   sactive_fgcolor            =  0x143; -- fgcolor when widget is selected/active  
   sinactive_fgcolor          =  0x144; -- fgcolor when widget is selected/inactive
   selected_fgcolor           =  0x145; -- fgcolor for selected (active&inactive)
                              
   padding                    =  0x120; -- all pads to same value
   left_pad                   =  0x121; -- left padding just inside the border
   right_pad                  =  0x122; -- right ...
   top_pad                    =  0x123; -- top ...
   bottom_pad                 =  0x124; -- bottom ...
                              
   font                       =  0x315; -- font for widget to use
   flags                      =  0x153; -- widget specific flags

   -- scrollbars
   scrollhandlewidth          =  0x180; -- width of scrollbar handle
   scrollpad                  =  0x181; -- space between scrollbar/indicator and child widget
   scrollstyle                =  0x182; -- scroll style
   active_scrollcolor         =  0x184; -- active color of scrollbar/indicator
   inactive_scrollcolor       =  0x185; -- inactive color of scrollbar/indicator
   scrollcolor                =  0x186; -- set (only) active & inactive color to same value
   active_scrollhandlecolor   =  0x187; -- active color of scrollbar handle
   inactive_scrollhandlecolor =  0x188; -- inactive color of scrollbar handle
   scrollhandlecolor          =  0x189; -- set (only) active & inactive handle color to same value
   scrollfillcolor            =  0x190; -- fill (inside, non-handle) color of scroll bar
                                 
   image_animate              =  0x216; -- image animate on (1) or off (0)
};


-- PropsOrder defines the order in which properties are stored in the theme file 
--  (and therefore read out and applied to widgets)

-- Properties in this table will be stored in order listed, 
--  There is no order gaurantee for properties not listed here.

PropsOrder = {

  -- Foreground and backround / gradient precendence

   WidgetProps.fgcolor,
   WidgetProps.selected_fgcolor,
   WidgetProps.inactive_fgcolor,
   WidgetProps.active_fgcolor,
   WidgetProps.sinactive_fgcolor,
   WidgetProps.sactive_fgcolor,

   WidgetProps.bgcolor,
   WidgetProps.selected_bgcolor,
   WidgetProps.inactive_bgcolor,
   WidgetProps.active_bgcolor,
   WidgetProps.sinactive_bgcolor,
   WidgetProps.sactive_bgcolor,

   WidgetProps.gradient,
   WidgetProps.selected_gradient,
   WidgetProps.inactive_fgcolor,
   WidgetProps.active_fgcolor,
   WidgetProps.sinactive_fgcolor,
   WidgetProps.sactive_fgcolor,

  -- Shadow precedence

   WidgetProps.shadowoffset,
   WidgetProps.selected_shadowoffset,

   WidgetProps.shadowoffsetx,
   WidgetProps.selected_shadowoffsetx,

   WidgetProps.shadowoffsety,
   WidgetProps.selected_shadowoffsety,

   WidgetProps.shadowcolor,
   WidgetProps.selected_shadowcolor,

  -- Scrollbar precendence
   
   WidgetProps.scrollcolor,
   WidgetProps.inactive_scrollcolor,
   WidgetProps.active_scrollcolor,

   WidgetProps.scrollhandlecolor,
   WidgetProps.inactive_scrollhandlecolor,
   WidgetProps.active_scrollhandlecolor,

  -- Border and padding precedence

   WidgetProps.padding,

   WidgetProps.borderwidth,
   WidgetProps.active_borderwidth,
   WidgetProps.inactive_borderwidth,

   WidgetProps.bordercolor,
   WidgetProps.selected_bordercolor,
   WidgetProps.inactive_bordercolor,
   WidgetProps.active_bordercolor,
   WidgetProps.sinactive_bordercolor,
   WidgetProps.sactive_bordercolor,

};


Color = createtable();

Color.rgba = function(r, g, b, a)
		local function check(v) return v and 0 <= v and v <= 255 end;
		assert(check(r) and check(g) and check(b) and check(a),
		       "Invalid color value");
		return bit.lshift(b, 24) +  bit.lshift(g, 16) + bit.lshift(r, 8) + a;
	     end;
Color.rgb = function(r, g, b)
	       return Color.rgba(r, g, b, 0xFF);
	    end;

Color.rgbp = function(r, g, b, p)
		return Color.rgba(r, g, b, math.abs(255 * (100 - p) / 100));
	     end;

Color.none           = 0xFFFFFF00;
Color.black          = Color.rgb(0, 0, 0);
Color.white          = Color.rgb(255, 255, 255);
Color.red            = Color.rgb(255, 0, 0);
Color.green          = Color.rgb(0, 255, 0);
Color.blue           = Color.rgb(0, 0, 255);

-- This is used for combining different flags
Combine = function(...)           
	   return bit.bor(unpack(arg));
	   end;

Font = {
   normal            = 0x01012786;
   large             = 0x01012787;
   bold              = 0x01012788;
   italic            = 0x0101402c;
   bolditalic        = 0x0101402d;
   largeitalic       = 0x0101402e;
};

Align = {
   none              = 0x00000000;
   left              = 0x00000010;
   center            = 0x00000020;
   right             = 0x00000040;
                     
   top               = 0x00000100;
   middle            = 0x00000200;
   bottom            = 0x00000400;
};

Align.centered       = bit.bor(Align.middle, Align.center);
Align.northeast      = bit.bor(Align.top,    Align.right);
Align.northwest      = bit.bor(Align.top,    Align.left);
Align.southeast      = bit.bor(Align.bottom, Align.right);
Align.southwest      = bit.bor(Align.bottom, Align.left);

Text = {
   wrapped           = 0x00100000;     -- wrap text to fit in widget rect
   noshorten         = 0x00200000;     -- don't shorten text that doesn't fit
   nobreak           = 0x00400000;     -- wrap text but don't break on white space
};


Gradient = {
   vert        =   0; -- gradient fill vertically (top to bottom)
   horz        =   1; -- gradient fill horizontally (left to right)
   centervert  =   2; -- gradient fill from top & bottom vertically to center
   centerhorz  =   3; -- gradient fill from left & right to center 
   none        =   4; -- gradient fill none
};

Image = createtable();
function Image.from(fname, type)
   return loadimage(fname, type);
end;

initialize(Color, Font, Gradient, Text, Align, WidgetProps, PropsOrder, Image);
