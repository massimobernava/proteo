
//#define strlcpy strncpy

#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <libgen.h>
#include <sys/stat.h>
#include <time.h>
#include <strings.h>
#include <stdint.h>
#include <stddef.h>
#include <dirent.h>

//#include "lua.h"
//#include "lauxlib.h"
//#include "lualib.h"

#ifdef __EMSCRIPTEN__
#include <lua.h>
#elif defined(_WIN32)
#include <lua.h>
#else
#include <luajit.h>
#endif //emscripten

#include <lauxlib.h>
#include <lualib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/fetch.h>
#else

    #include <curl/curl.h>

    #include <openssl/md5.h>
    #include <openssl/hmac.h>
    #include <openssl/pem.h>

    //#include <json-c/json.h>

#ifdef PROTEO_FFMPEG
    #include <libavutil/imgutils.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
#endif //ffmpeg

#endif //emscripten

#ifdef PROTEO_ZMQ
#include <zmq.h>
#endif // PROTEO_ZMQ

#ifdef PROTEO_USE_INTERNALMD5
#include "md5.c"
#define MD5_DIGEST_LENGTH   16
unsigned char *MD5(const unsigned char *d, size_t n, unsigned char *md)
{
	 MD5_CTX c;
	 static unsigned char m[MD5_DIGEST_LENGTH];
	 if (md == NULL) md=m;
	 MD5_Init(&c);
	 MD5_Update(&c,d,n);
	 MD5_Final(md,&c);
	 return(md);
}
#endif

#ifdef PROTEO_USE_TINYJSON
#include "tiny-json.c"
#else
#include <json-c/json.h>
#endif

#define ENET_IMPLEMENTATION
#define ENET_DEBUG
#ifdef PROTEO_ENET
#if TARGET_OS_IPHONE
#include <enet/enet.h>
#else
#include "enet.h"
#endif
#endif

#define NANOSVG_ALL_COLOR_KEYWORDS    // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION        // Expands implementation
#include "nanosvg.h"

#ifndef HAVE_STRLCAT
size_t strlcat(char* dst,const char *src, size_t size)
{
  size_t    srclen;
  size_t    dstlen;

  dstlen = strlen(dst);
  size   -= dstlen + 1;

  if (!size)
    return (dstlen);

  srclen = strlen(src);

  if (srclen > size)
    srclen = size;

  memcpy(dst + dstlen, src, srclen);
  dst[dstlen + srclen] = '\0';

  return (dstlen + srclen);
}
#endif /* !HAVE_STRLCAT */

#ifndef HAVE_STRLCPY

size_t strlcpy(char       *dst, const char *src, size_t      size)
{
  size_t    srclen;

  size --;
  srclen = strlen(src);

  if (srclen > size)
    srclen = size;

  memcpy(dst, src, srclen);
  dst[srclen] = '\0';

  return (srclen);
}
#endif /* !HAVE_STRLCPY */

#ifndef FALSE
#define FALSE false
#endif
#ifndef TRUE
#define TRUE  true
#endif

#if SDL_MAJOR_VERSION == 2 && SDL_MINOR_VERSION == 0 && SDL_PATCHLEVEL < 10
typedef struct SDL_FPoint
{
    float x;
    float y;
} SDL_FPoint;
#endif

#define PROTEO_VERSION "2.0.0"
#define PROTEO_MAJOR_VERSION 2
#define PROTEO_MINOR_VERSION 0

#define PROTEO_MAX_LENGTH 200
#define PROTEO_MAX_ID 50
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define luaL_checkbool(L, i) (lua_isboolean(L, i) ? lua_toboolean(L, i) : (int)luaL_checkinteger(L, i))

#define lua_ref(L,lock) ((lock) ? luaL_ref(L, LUA_REGISTRYINDEX) : \
      (lua_pushstring(L, "unlocked references are obsolete"), lua_error(L), 0))

#define lua_unref(L,ref)        luaL_unref(L, LUA_REGISTRYINDEX, (ref))

#ifndef lua_getref
#define lua_getref(L,ref)       lua_rawgeti(L, LUA_REGISTRYINDEX, ref)
#endif

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const float RATIO = (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT;

int run;

int verbose=FALSE;
int debug=FALSE;
int opt_fullscreen=FALSE;
int opt_remoteconsole=FALSE;

char app_path[PATH_MAX];

#define LIGHTPROTEOCOMPONENT
#define LIGHTPROTEOSOUND

#define PROTEOCOMPONENT "ProteoComponent"
#define PROTEOSOUND "ProteoSound"
#define PROTEOTIMER "ProteoTimer"
#define PROTEOSTREAM "ProteoStream"
#define PROTEOSKELETON "ProteoSkeleton"

#define PROTEO2

typedef struct ProteoColor
{
  Uint8 a;
  Uint8 r;
  Uint8 g;
  Uint8 b;
} ProteoColor;

typedef struct color_name
{
	char * name;
	ProteoColor color;
} color_name;

typedef struct font_name
{
	char * name;
	char* path;
} font_name;

font_name nfonts[]=
	{
		{"ColaborateThin","ColabThi.otf"},
		{"ColaborateBold","ColabBol.otf"},
		{"ColaborateLight","ColabLig.otf"},
		{"ColaborateMedium","ColabMed.otf"},
		{"Colaborate","ColabReg.otf"},
		{"Helvetica","Helvetica 400.ttf"},
		{"OpenSans","OpenSans-Regular.ttf"}

	};

typedef struct icon_name
{
    char * name;
    char* path;
} icon_name;

icon_name nicons[]=
{
    {"arrow-12","icons/arrow-12-512.png"},
    {"arrow-112","icons/arrow-112-512.png"},
    {"arrow-225","icons/arrow-225-512.png"},
    {"loop-circular","icons/loop-circular-512.png"},
    {"add_user","icons/add-user-2-512.png"},
    {"power","icons/power-512.png"}
};

/* TODO

    "honeydew":"#f0fff0","hotpink":"#ff69b4",
    "indianred ":"#cd5c5c","indigo":"#4b0082","ivory":"#fffff0","khaki":"#f0e68c",
    "lavender":"#e6e6fa","lavenderblush":"#fff0f5","lawngreen":"#7cfc00","lemonchiffon":"#fffacd","lightblue":"#add8e6","lightcoral":"#f08080","lightcyan":"#e0ffff","lightgoldenrodyellow":"#fafad2",
    "lightgrey":"#d3d3d3","lightgreen":"#90ee90","lightpink":"#ffb6c1","lightsalmon":"#ffa07a","lightseagreen":"#20b2aa","lightskyblue":"#87cefa","lightslategray":"#778899","lightsteelblue":"#b0c4de",
    "lightyellow":"#ffffe0","lime":"#00ff00","limegreen":"#32cd32","linen":"#faf0e6",
    "magenta":"#ff00ff","maroon":"#800000","mediumaquamarine":"#66cdaa","mediumblue":"#0000cd","mediumorchid":"#ba55d3","mediumpurple":"#9370d8","mediumseagreen":"#3cb371","mediumslateblue":"#7b68ee",
    "mediumspringgreen":"#00fa9a","mediumturquoise":"#48d1cc","mediumvioletred":"#c71585","midnightblue":"#191970","mintcream":"#f5fffa","mistyrose":"#ffe4e1","moccasin":"#ffe4b5",
    "navajowhite":"#ffdead","navy":"#000080",
    "oldlace":"#fdf5e6","olive":"#808000","olivedrab":"#6b8e23","orange":"#ffa500","orangered":"#ff4500","orchid":"#da70d6",
    "palegoldenrod":"#eee8aa","palegreen":"#98fb98","paleturquoise":"#afeeee","palevioletred":"#d87093","papayawhip":"#ffefd5","peachpuff":"#ffdab9","peru":"#cd853f","pink":"#ffc0cb","plum":"#dda0dd","powderblue":"#b0e0e6","purple":"#800080",
    "rebeccapurple":"#663399","rosybrown":"#bc8f8f","royalblue":"#4169e1",
    "saddlebrown":"#8b4513","salmon":"#fa8072","sandybrown":"#f4a460","seagreen":"#2e8b57","seashell":"#fff5ee","sienna":"#a0522d","silver":"#c0c0c0","skyblue":"#87ceeb","slateblue":"#6a5acd","slategray":"#708090","snow":"#fffafa","springgreen":"#00ff7f","steelblue":"#4682b4",
    "tan":"#d2b48c","teal":"#008080","thistle":"#d8bfd8","tomato":"#ff6347","turquoise":"#40e0d0",
    "violet":"#ee82ee",
    "wheat":"#f5deb3","whitesmoke":"#f5f5f5",
    "yellow":"#ffff00","yellowgreen":"#9acd32"
    */

color_name colors[]={
	{"clear",{0,0,0,0}},
	{"black",{0xff,0,0,0}},
	{"white",{0xff,0xff,0xff,0xff}},
	{"red",{0xff,0xff,0,0}},
	{"green",{0xff,0,0x80,0}},
	{"blue",{0xff,0,0,0xff}},
	{"darkgrey",{0xff,0xa9,0xa9,0xa9}},

	{"aliceblue",{0xff,0xf0,0xf8,0xff}},
	{"antiquewhite",{0xff,0xfa,0xeb,0xd7}},
	{"aqua",{0xff,0,0xff,0xff}},
	{"aquamarine",{0xff,0x7f,0xff,0xd4}},
	{"azure",{0xff,0xf0,0xff,0xff}},

	{"beige",{0xff,0xf5,0xf5,0xdc}},
	{"bisque",{0xff,0xff,0xe4,0xc4}},
	{"blanchedalmond",{0xff,0xff,0xeb,0xcd}},
	{"blueviolet",{0xff,0x8a,0x2b,0xe2}},
	{"brown",{0xff,0xa5,0x2a,0x2a}},
	{"burlywood",{0xff,0xde,0xb8,0x87}},

	{"cadetblue",{0xff,0x5f,0x9e,0xa0}},
    {"cadetgrey",{0xff,0x93,0xa8,0xac}},
	{"chartreuse",{0xff,0x7f,0xff,0x00}},
	{"chocolate",{0xff,0xd2,0x69,0x1e}},
	{"coral",{0xff,0xff,0x7f,0x50}},
	{"cornflowerblue",{0xff,0x64,0x95,0xed}},
	{"cornsilk",{0xff,0xff,0xf8,0xdc}},
	{"crimson",{0xff,0xdc,0x14,0x3c}},
	{"cyan",{0xff,0x00,0xff,0xff}},

	{"darkblue",{0xff,0x00,0x00,0x8b}},
	{"darkcyan",{0xff,0x00,0x8b,0x8b}},
	{"darkgoldenrod",{0xff,0,0,0}},//"#b8860b"
	{"darkgreen",{0xff,0,0,0}},//"#006400"
	{"darkkhaki",{0xff,0,0,0}}, //"#bdb76b"
	{"darkmagenta",{0xff,0x8b,0x00,0x8b}},
	{"darkolivegreen",{0xff,0,0,0}},//"#556b2f"
	{"darkorange",{0xff,0xff,0x8c,0x00}},
	{"darkorchid",{0xff,0,0,0}}, //"#9932cc"
	{"darkred",{0xff,0,0,0}}, //"#8b0000"
	{"darksalmon",{0xff,0,0,0}}, //"#e9967a"
	{"darkseagreen",{0xff,0,0,0}}, //"#8fbc8f"
	{"darkslateblue",{0xff,0x48,0x3d,0x8b}},
	{"darkslategray",{0xff,0x2f,0x4f,0x4f}},
	{"darkturquoise",{0xff,0,0,0}}, //"#00ced1"
    {"darkviolet",{0xff,0,0,0}}, //"#9400d3"
    {"deeppink",{0xff,0,0,0}}, //"#ff1493"
    {"deepskyblue",{0xff,0,0xbf,0xff}},
    {"dimgray",{0xff,0x69,0x69,0x69}},
    {"dodgerblue",{0xff,0,0,0}}, //"#1e90ff"

    {"firebrick",{0xff,0,0,0}}, //"#b22222"
    {"floralwhite",{0xff,0,0,0}}, //"#fffaf0"
    {"forestgreen",{0xff,0,0,0}}, //"#228b22"
    {"fuchsia",{0xff,0,0,0}}, //"#ff00ff"

    {"gainsboro",{0xff,0xdc,0xdc,0xdc}},
    {"ghostwhite",{0xff,0xf8,0xf8,0xff}},
    {"gold",{0xff,0xff,0xd7,0x00}},
    {"goldenrod",{0xff,0xda,0xa5,0x20}},
    {"gray",{0xff,0x80,0x80,0x80}},
    {"greenyellow",{0xff,0xad,0xff,0x2f}},

    {"lightblue",{0xff,0xad,0xd8,0xe6}},
    {"lightgreen",{0xff,0x90,0xee,0x90}},
    {"lightgrey",{0xff,0xd3,0xd3,0xd3}},

    {"purple",{0xff,0x80,0x00,0x80}},

    {"yellow",{0xff,0xff,0xff,0x00}},

    {"orange",{0xff,0xff,0xa5,0x00}},
    {"jet",{0xff,0x33,0x33,0x33}},
    {"grullo",{0xff,0xa5,0x9e,0x8c}},
    {"turquoise",{0xff,0x40,0xe0,0xd0}}
};

const char server_form[] = "local label_server=nil \n"
                            "local label_info=nil \n"
                            "local text_server=nil \n"
                            "local label_appkey=nil \n"
                            "local text_appkey=nil \n"
                            "local btn_save=nil \n"
                            "--local json=require \"json\" \n"
                            "local config=nil \n"
                            "function init() \n"
                              "collectgarbage('stop')\n"
                              "proteo.graphics.setBackground('dimgray') \n"
                              "config=proteo.system.loadConfig() \n"
                              "label_info=proteo.gui.newLabel('label_info',\"Impossibile accedere al server. Inserire l'indirizzo corretto e riprovare.\",'OpenSans',18,'black','Clear',proteo.gui.LabelAlignment.Left,MIN_X +30,MIN_Y + 170,400,30) \n"

                              "label_server=proteo.gui.newLabel('label_server','Server:','OpenSans',18,'black','Clear',proteo.gui.LabelAlignment.Left,MIN_X +80,MIN_Y + 250,150,30) \n"
                              "text_server=proteo.gui.newTextField('text_server',config.server,'OpenSans',18,'Black','White',MIN_X +200,MIN_Y + 250,380,30,'') \n"
                                
                              "label_appkey=proteo.gui.newLabel('label_appkey','AppKey:','OpenSans',18,'black','Clear',proteo.gui.LabelAlignment.Left,MIN_X +80,MIN_Y + 300,150,30) \n"
                              "text_appkey=proteo.gui.newTextField('text_appkey',config.appkey,'OpenSans',18,'Black','White',MIN_X +200,MIN_Y + 300,380,30,'') \n"

                              "btn_save=proteo.gui.newButton('btn_save','Save','OpenSans',20,'black','cornsilk',1,'crimson',true,MID_X - 100,MAX_Y - 100,100,50,\"save\") \n"
                            "end \n"
                            "function update(dt) end \n"
                            "function save(sender) \n"
                              "--data={} \n"
                              "config[\"server\"]=proteo.gui.getText(text_server) \n"
                              "config[\"appkey\"]=proteo.gui.getText(text_appkey) \n"
                              "config[\"script\"]=\"\" \n"
                              "config[\"version\"]=\"" PROTEO_VERSION "\"\n"
                              "proteo.system.saveConfig(config) \n"
                              "proteo.system.login(\"\",\"\",\"login\") \n"
                              "--hide \n"
                              "--proteo.gui.setHidden(label_info,true) \n"
                              "proteo.gui.setHidden(label_server,true) \n"
                              "proteo.gui.setHidden(text_server,true) \n"
                              "proteo.gui.setHidden(label_appkey,true) \n"
                              "proteo.gui.setHidden(text_appkey,true) \n"
                              "proteo.gui.setHidden(btn_save,true) \n"
                              "--and set nil \n"
                              "collectgarbage('restart')\n"
                              "collectgarbage('collect')\n"
                            "end";

typedef struct ProteoTexture
{
    char id[MD5_DIGEST_LENGTH];
    SDL_Texture* texture;

    //List
    struct ProteoTexture* next;
} ProteoTexture;

ProteoTexture* textures=NULL;

typedef struct ProteoFont
{
    char id[MD5_DIGEST_LENGTH];
    TTF_Font* font;
    int size;
    char path[256];

    //List
    struct ProteoFont* next;
} ProteoFont;

ProteoFont* fonts=NULL;

typedef struct ProteoLib
{
    char name[PROTEO_MAX_ID];
    char* lib;

    //List
    struct ProteoLib* next;
} ProteoLib;

ProteoLib* libs=NULL;

typedef struct ProteoIcon
{
    char id[MD5_DIGEST_LENGTH];
    SDL_Texture* texture;
    ProteoColor color;
    char path[256];

    //List
    struct ProteoIcon* next;
} ProteoIcon;

ProteoIcon* icons=NULL;

//==============================================================================
//   COMPONENT
//==============================================================================

enum ProteoType
{
  Deleted,
  Rect,
  Ellips,
  Image,
  Button,
  Label,
  TextField,
  List,
  ListItem,
  Checkbox,
  Sprite,
  DropDown,
  DropDownItem,
  Form,
  Icon,
  Container,
  Shape,
  Polyg
  //Bezier
};

typedef struct
{
    ProteoColor font_color;
    ProteoColor back_color;

    //TODO multiline

    int align;
} _label;

typedef struct
{
    ProteoColor font_color;
    ProteoColor back_color;
    ProteoColor border_color;

    int border_size;
    int rounded;
} _button;

typedef struct
{
    ProteoColor font_color;

    //TODO multiline

    int mode;
    int cursor_position;

} _textfield;

typedef struct
{
    ProteoColor font_color;

    int nchilds;
    struct ProteoComponent** childs;

    struct ProteoComponent* selected_item;

    int scroll_y;

} _list;

typedef struct
{
    ProteoColor font_color;

    //struct ProteoComponent* child_next;

} _listitem;

typedef struct
{
    ProteoColor font_color;

    int nchilds;
    struct ProteoComponent** childs;

    struct ProteoComponent* selected_child;

    int state;
    int type;
} _dropdown;

typedef struct
{
    ProteoColor font_color;

    //struct ProteoComponent* child_next;

} _dropdownitem;

typedef struct
{
    ProteoColor mark_color;
    struct ProteoComponent* prev_link;
    struct ProteoComponent* next_link;

    int state;
    int type;
} _checkbox;

typedef struct
{
    ProteoColor font_color;
    ProteoColor back_color;
    ProteoColor bar_color;

    int nchilds;
    struct ProteoComponent** childs;

    int state;
    int mode;
    int bar_size;
} _form;

typedef struct
{
    ProteoColor back_color;

    int nchilds;
    struct ProteoComponent** childs;

    int scroll;
} _container;

typedef struct
{
    ProteoTexture* image;
    SDL_Rect framesSource[50];//TODO "SDL_Rect* frames"
    SDL_Point framesDestination[50];
    int nFrame;
    int currentFrame;
    SDL_RendererFlip flip;
} _sprite;

typedef struct
{
    ProteoTexture* image;
} _image;

typedef struct
{
    ProteoColor color;
} _ellipse;

typedef struct
{
    ProteoColor color;
} _rect;

/*typedef struct
{
    Sint16 x0;
    Sint16 y0;
    Sint16 x1;
    Sint16 y1;
    Sint16 x2;
    Sint16 y2;
    Sint16 x3;
    Sint16 y3;

    ProteoColor color;
    int npoints;
    Sint16 *vx;
    Sint16 *vy;
    Sint16 *tvx;
    Sint16 *tvy;

} _bezier;*/

#ifdef BEZIER
typedef struct
{
    float step;

    SDL_FPoint P0;
    SDL_FPoint P1;
    SDL_FPoint P2;
    SDL_FPoint P3;

    /*float m;
    float q;

    float d1;
    float d2;*/

    //int S1;
    //int S2;

    SDL_FPoint T0;
    SDL_FPoint T1;
    SDL_FPoint T2;
    SDL_FPoint T3;

} _bezier;
#endif

typedef struct
{
    ProteoColor color;
    int npoints;

    Sint16 *vx;
    Sint16 *vy;
    Sint16 *tvx;
    Sint16 *tvy;
#ifdef BEZIER
    int nbeziers;
    _bezier* beziers;
#endif
} _polygon;

typedef struct
{
    int nchilds;
    struct ProteoComponent** childs;
    int hasSkeleton;

} _shape;

typedef struct
{

    ProteoIcon* icon;
} _icon;

typedef union
{
    _label label;
    _button button;
    _textfield textfield;
    _checkbox checkbox;
    _dropdown dropdown;
    _dropdownitem dropdownitem;
    _list list;
    _listitem listitem;
    _form form;
    _container container;

    _sprite sprite;
    _image image;
    _ellipse ellipse;
    _rect rect;
    _polygon polygon;
    //_bezier bezier;
    _shape shape;
    _icon icon;
} _component;

typedef struct ProteoComponent
{
    char id[PROTEO_MAX_ID];
    enum ProteoType type;
    SDL_Rect rect;
    //SDL_Point anchor; //TODO
    Uint8 hidden;
    Uint8 enabled;
    int layer;

    _component component;

    //TODO altri campi da spostare dentro _component

    ProteoFont* font;
    char *txt;//[PROTEO_MAX_LENGTH];

    SDL_Surface* surface;
    SDL_Texture* texture;
    SDL_Rect texture_rect;

    //Sprite
    SDL_Rect framesSource[50];//TODO "SDL_Rect* frames"
    SDL_Point framesDestination[50];
    int nFrame;
    int currentFrame;

    char* callback;
    int ref_callback;//TODO usa ovunque (già usato in Button)
    //ProteoColor maincolor;
    ProteoColor color; //TODO da rimuovere
    ProteoColor colorB; //TODO da rimuovere

    struct ProteoComponent* parent;

    //List
    struct ProteoComponent* next;
#ifdef DLINK_LIST_COMPONENT
    struct ProteoComponent* prev;
#endif
} ProteoComponent;

ProteoComponent* components=NULL;
ProteoComponent* selected=NULL;
int blinkCursor=0;
int nComponents=0;

static ProteoComponent *checkProteoComponent (lua_State *L, int index);
static ProteoComponent *pushProteoComponent (lua_State *L);
static ProteoComponent *toProteoComponent (lua_State *L, int index);

void addComponent(ProteoComponent * component,ProteoComponent** list);

//==============================================================================
//   UTILITY
//==============================================================================

char* concat(const char *s1, const char *s2);
char* concat3(const char *s1, const char *s2, const char *s3);
int c_quote(const char* src, char* dest, int maxlen);

static ProteoFont* newFont(const char* path,int size);

void addFunction_proteo(lua_State *state,const char *lib,const char *fname,lua_CFunction f);
void addTable_proteo(lua_State *state,const char *lib,const char *tname);
void addEnum_proteo(lua_State *state,const char *lib,const char *enumname,const char *vname,int value);

ProteoColor brightness(ProteoColor color,float bright);
ProteoColor saturation(ProteoColor color,float sat);
void mkpath(const char* dir);

int writefile(char* filename,char* data);
int writedatafile(char* filename,char* data,int sizeData);

void hexDump (const char * desc, const void * addr, const int len);

SDL_Point affineTrasformation(SDL_Point point,float affine[3][3]);
void laderman_mul(const float a[3][3],const float b[3][3],float c[3][3]);
float distance(SDL_FPoint p0,SDL_FPoint p1);
SDL_FPoint normalize(SDL_FPoint p);
SDL_FPoint subtract(SDL_FPoint p0, SDL_FPoint p1);
float dot(SDL_FPoint p0, SDL_FPoint p1);
SDL_FPoint sum(SDL_FPoint p0, SDL_FPoint p1);
SDL_FPoint multiply(SDL_FPoint p, float f);
float magnitude(SDL_FPoint p);

SDL_FPoint I2FPoint(SDL_Point p);
SDL_Point F2IPoint(SDL_FPoint p);

SDL_FPoint getClosestPointOnSegment(SDL_FPoint p0, SDL_FPoint p1, SDL_FPoint p);

int currentline(lua_State *L);

char* icon2path(const char* icon);
char* font2path(const char* font);
ProteoColor hex2color(const char* input);

char* loadfile(char *filename);

void freecomponents(void);
void freetextures(void);
void freesounds(void);
void freefonts(void);
void freestreams(void);
void freeicons(void);
void freeskeletons(void);

static size_t writeMemory_callback(void *contents, size_t size, size_t nmemb, void *userp);
void bifrost_get(const char* res,const char* appkey, const char* token,const char* callback);
void bifrost_post(const char* res,const char* appkey, const char* token,const char* post_data, const char* callback);

void proteo_get_callback(const char* res,const char* data,const char* callback);
void proteo_post_callback(const char* res,const char* data,const char* callback,const int ref_callback);

int Base64encode(char *encoded, const char *string, int len);
int Base64encode_len(int len);
int Base64decode(char *bufplain, const char *bufcoded);
int Base64decode_len(const char *bufcoded);

//==============================================================================
//   GUI
//==============================================================================

static int gui_emptyList (lua_State *state);
static int gui_setHidden (lua_State *state);
static int gui_setColor (lua_State *state);
static int gui_setEnabled (lua_State *state);
static int gui_getText (lua_State *state);
static int gui_setText (lua_State *state);
static int gui_linkCheckbox (lua_State *state);

static int gui_eventButton (lua_State *state,ProteoComponent* button,SDL_Event e,SDL_Renderer* renderer);
static int gui_drawButton (SDL_Renderer* renderer,ProteoComponent* button);
static int gui_newButton (lua_State *state);

static int gui_eventTextField (lua_State *state,ProteoComponent* textfield,SDL_Event e,SDL_Renderer* renderer);
static int gui_drawTextField  (SDL_Renderer* renderer,ProteoComponent* textfield);
static int gui_newTextField  (lua_State *state);

static int gui_eventLabel (lua_State *state,ProteoComponent* label,SDL_Event e,SDL_Renderer* renderer);
static int gui_drawLabel  (SDL_Renderer* renderer,ProteoComponent* label);
static int gui_newLabel (lua_State *state);

static int gui_eventCheckbox (lua_State *state,ProteoComponent* checkbox,SDL_Event e,SDL_Renderer* renderer);
static int gui_drawCheckbox (SDL_Renderer* renderer,ProteoComponent* checkbox);
static int gui_newCheckbox (lua_State *state);

static int gui_eventDropDown (lua_State *state,ProteoComponent* dropdown,SDL_Event e,SDL_Renderer* renderer);
static int gui_drawDropDown (SDL_Renderer* renderer,ProteoComponent* dropdown);
static int gui_newDropDown (lua_State *state);

static int gui_eventForm (lua_State *state,ProteoComponent* form,SDL_Event e,SDL_Renderer* renderer);
static int gui_drawForm (SDL_Renderer* renderer,ProteoComponent* form);
static int gui_newForm (lua_State *state);

static int gui_eventContainer (lua_State *state,ProteoComponent* container,SDL_Event e,SDL_Renderer* renderer);
static int gui_drawContainer (SDL_Renderer* renderer,ProteoComponent* container);
static int gui_newContainer (lua_State *state);

static int gui_infoComponent (lua_State *state);
static int gui_setPosition (lua_State *state);

//==============================================================================
//   GRAPHICS
//==============================================================================

typedef struct ProteoJoint
{
    char id[PROTEO_MAX_ID];//MD5_DIGEST_LENGTH];

    SDL_FPoint pos;

#ifdef AFFINE
    float affine[3][3];
#else
    SDL_FPoint move;
#endif
    SDL_FPoint tpos;
/* OLD
#ifndef BEZIER
    int nweights;
    float** weights;
#endif
#ifdef BEZIER
    int nbez_wei;
    float** bez_wei;
#endif*/
} ProteoJoint;

typedef struct ProteoBone
{
    char id[PROTEO_MAX_ID];//MD5_DIGEST_LENGTH];

    ProteoJoint *a;
    ProteoJoint *b;

    int nweights;
    float** weights;
    SDL_FPoint** transformPoints;
    float** anchorPercs;

    int group_id;

} ProteoBone;

typedef struct ProteoSkeleton
{
    char id[PROTEO_MAX_ID];//MD5_DIGEST_LENGTH];

    ProteoJoint *joints;
    ProteoBone *bones;

    int njoints;
    int nbones;

    //SDL_Point pos;
    ProteoComponent* shape;

    Uint8 hidden;
    //List
    struct ProteoSkeleton* next;
} ProteoSkeleton;


ProteoSkeleton* skeletons=NULL;

static ProteoSkeleton *checkProteoSkeleton (lua_State *L, int index);
static ProteoSkeleton *pushProteoSkeleton (lua_State *L);
static ProteoSkeleton *toProteoSkeleton (lua_State *L, int index);

static int graphics_setbackground (lua_State *state);
static int graphics_setLayer (lua_State *state);
static int graphics_changeImage (lua_State *state);
static int graphics_isInside (lua_State *state);
static int graphics_setHidden (lua_State *state);
static int graphics_setPosition (lua_State *state);
static int graphics_getPosition (lua_State *state);
//static int graphics_eventEllipse (lua_State *state,ProteoComponent* ellipse,SDL_Event e,SDL_Renderer* renderer);
static int graphics_drawEllipse (SDL_Renderer* renderer,ProteoComponent* ellipse);
static int graphics_newEllipse (lua_State *state);
//static int graphics_eventRect (lua_State *state,ProteoComponent* rect,SDL_Event e,SDL_Renderer* renderer);
static int graphics_drawRect (SDL_Renderer* renderer,ProteoComponent* rect);
static int graphics_newRect (lua_State *state);
//static int graphics_eventImage (lua_State *state,ProteoComponent* image,SDL_Event e,SDL_Renderer* renderer);
static int graphics_drawImage (SDL_Renderer* renderer,ProteoComponent* image);
static int graphics_newImage (lua_State *state);
static int graphics_drawSprite (SDL_Renderer* renderer,ProteoComponent* sprite);
static int graphics_newSprite (lua_State *state);
static int graphics_drawIcon (SDL_Renderer* renderer,ProteoComponent* icon);
static int graphics_newIcon (lua_State *state);
static int graphics_drawPolygon (SDL_Renderer* renderer,ProteoComponent* polygon);
static int graphics_drawSkeleton(SDL_Renderer* renderer,ProteoSkeleton* skeleton);

//==============================================================================
//   NETWORK
//==============================================================================
static int network_bifrost_get(lua_State *state);
static int network_bifrost_post(lua_State *state);

//==============================================================================
//   SYSTEM
//==============================================================================

typedef struct ProteoTimer
{
    double time;
    double last;
    int state;
    char* callback;
    int ref_callback;

    //List
    struct ProteoTimer* next;
} ProteoTimer;


ProteoTimer* timers=NULL;

static ProteoTimer *checkProteoTimer (lua_State *L, int index);
static ProteoTimer *pushProteoTimer (lua_State *L);
static ProteoTimer *toProteoTimer (lua_State *L, int index);

void add_system_proteo(lua_State* state);
static int system_console (lua_State *state);
static int system_clock (lua_State *state);
static int system_fileexist (lua_State *state);
static int system_document (lua_State *state);
static int system_readFile (lua_State *state);
static int system_getNode (lua_State *state);
static int system_loadConfig (lua_State *state);
static int system_saveConfig (lua_State *state);
static int system_setConfig (lua_State *state);

//void closeLUA();


//==============================================================================
//   OPENCV
//==============================================================================
#ifdef PROTEO_OPENCV
typedef struct OCVImage
{
    int  channels;
    unsigned long size;
    int  depth;
    int  width;
    int  height;

    int type;
    
    unsigned char *data;
    unsigned long  step;

} OCVImage;

//void imread_c(const char* path);
int opencv_imwrite(lua_State* state);
int opencv_imread(lua_State* state);
int opencv_videocapture(lua_State* state);
int opencv_videowriter(lua_State* state);
int opencv_write(lua_State* state);
int opencv_frame(lua_State* state);
int opencv_readnet(lua_State* state);
int opencv_forward(lua_State* state);
int opencv_forwardTable(lua_State* state);
//int opencv_getpoints(lua_State* state);
int opencv_circle(lua_State* state);
int opencv_rectangle(lua_State* state);
int opencv_sliceImg(lua_State* state);
int opencv_minMaxLoc(lua_State* state);
int opencv_img(lua_State *state);
int opencv_line(lua_State* state);
int opencv_putText(lua_State* state);
int opencv_size(lua_State* state);
int opencv_cropImg(lua_State* state);
int opencv_infoNet(lua_State* state);
int opencv_infoImg(lua_State* state);
int opencv_setImg(lua_State* state);
int opencv_setSize(lua_State* state);
int opencv_fill(lua_State* state);
int opencv_resize(lua_State* state);
int opencv_copy(lua_State* state);
int opencv_mul(lua_State* state);
int opencv_add(lua_State* state);
int opencv_addWeighted(lua_State* state);
int opencv_flip(lua_State* state);
int opencv_threshold(lua_State* state);
int opencv_print(lua_State* state);
int opencv_getFrameSize(lua_State* state);
int opencv_setFrameSize(lua_State* state);
int opencv_setBufferSize(lua_State* state);
int opencv_imencode(lua_State* state);
int opencv_imdecode(lua_State* state);
int opencv_convert(lua_State* state);
int opencv_getaffinetransform(lua_State* state);
int opencv_warpaffine(lua_State* state);
int opencv_totable(lua_State* state);

OCVImage getImage(void* mat);
void toRGB(void* mat);
void toBGR(void* mat);
#endif
//==============================================================================
//   AUDIO
//==============================================================================

#define MAX_POLYPHONY  50
#define MAX_REC 9

typedef struct ProteoSound
{
    char id[MD5_DIGEST_LENGTH];

    Uint8 *buffer;
    Uint32 size;
    SDL_AudioSpec spec;

    //List
    struct ProteoSound* next;
} ProteoSound;

typedef struct ProteoStream
{
    SDL_AudioStream *stream;
    SDL_AudioSpec in,out;
    int size;
    int state;
    SDL_AudioDeviceID device;

    //List
    struct ProteoStream* next;

} ProteoStream;

typedef struct PSound
{
    ProteoSound *sound;
    int pos;
    int volume;
    SDL_AudioDeviceID device;
    int callback;
} PSound;

typedef struct RSound
{
    ProteoSound *sound;
    int pos;
    int state;
    SDL_AudioDeviceID device;
} RSound;

ProteoSound* sounds=NULL;
PSound playingsounds[MAX_POLYPHONY]; //TODO [MAX_DEV][MAX_POLYPHONY]
int playIndex=0;
RSound recordingsounds[MAX_REC];
int recIndex=0;

ProteoStream* streams=NULL;

static ProteoSound *checkProteoSound (lua_State *L, int index);
static ProteoSound *pushProteoSound (lua_State *L);
static ProteoSound *toProteoSound (lua_State *L, int index);
static ProteoStream *checkProteoStream (lua_State *L, int index);
static ProteoStream *pushProteoStream (lua_State *state, long in_freq, long in_format, long in_channels,
                                       long out_freq, long out_format, long out_channels);
static ProteoStream *toProteoStream (lua_State *L, int index);

int audio_infoDrivers(lua_State* state);
int audio_init(lua_State* state);
int audio_quit(lua_State* state);
int audio_infoDevices(lua_State* state);
int audio_infoAudio(lua_State* state);
int audio_convertAudio(lua_State* state);
int audio_getAudioStatus(lua_State* state);
int audio_openDevice(lua_State* state);
int audio_queueAudio(lua_State* state);
int audio_loadWav(lua_State* state);
int audio_playAudio(lua_State* state);
int audio_stopAudio(lua_State* state);
int audio_pauseDevice(lua_State* state);
int audio_newStream(lua_State* state);
int audio_playStream(lua_State* state);
int audio_recordStream(lua_State* state);
int audio_stopStream(lua_State* state);
int audio_getStream(lua_State* state);
int audio_putStream(lua_State* state);
int audio_createBuffer(lua_State* state);
int audio_recordAudio(lua_State* state);
