#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator
    #elif TARGET_OS_IPHONE
        // iOS device
    #elif TARGET_OS_MAC
        // Other kinds of Mac OS
        #define HAVE_STRLCAT
        #define HAVE_STRLCPY
    #else
    #   error "Unknown Apple platform"
    #endif
#elif __linux__
    // linux
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif

#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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
#include <semaphore.h>
#include <dirent.h>
#include <ifaddrs.h> //getinterfaces

#include <luajit.h>
#include <lauxlib.h>
#include <lualib.h>

#include <curl/curl.h>

#include <openssl/md5.h>
#include <openssl/hmac.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <json-c/json.h>

#include <microhttpd.h>
#include <sqlite3.h>
//#include <tensorflow/c/c_api.h>
#include <tensorflow/lite/c/c_api.h>

#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

//#include <tensorflow/c/c_api.h>

//#include <enet/enet.h>
#define ENET_IMPLEMENTATION
#define ENET_DEBUG
#include "enet.h"
#include <ejdb2/ejdb2.h>

#include <zmq.h>

#ifndef HAVE_STRLCAT
/*
 * '_cups_strlcat()' - Safely concatenate two strings.
 */

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

#define POSTBUFFERSIZE  512
#define GET             1
#define POST            2
#define PUT             3
#define DELETE          4

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif

#define PROTEO_MAX_LENGTH 200
#define PROTEO_MAX_ID 50

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MIN
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define lua_ref(L,lock) ((lock) ? luaL_ref(L, LUA_REGISTRYINDEX) : \
      (lua_pushstring(L, "unlocked references are obsolete"), lua_error(L), 0))

#define lua_unref(L,ref)        luaL_unref(L, LUA_REGISTRYINDEX, (ref))

#define lua_getref(L,ref)       lua_rawgeti(L, LUA_REGISTRYINDEX, ref)

int run;
int paused;
int toreboot=-1;
int verbose=FALSE;
int debug=FALSE;
char basedir[50];

#define PROTEOCOMPONENT "ProteoComponent"
#define PROTEOTIMER "ProteoTimer"
//#define PROTEO_APP_KEY "}~?d1BE+\"d5?TZ(j`{+n`pfK&*2U(WPy"


void init(lua_State *state,const char* app) ;
int initLUA();
void closeLUA();

const char *replaceWords(int n,...);
char *replaceWord(const char *s, const char *oldW, const char *newW);
char* concat(const char *s1, const char *s2);
char* concat3(const char *s1, const char *s2, const char *s3);

char* rand_string(char *str, size_t size);
char* loadfile(char *filename);
int writefile(char* filename,char* data);
void mkpath(const char* dir);
const char* app_call(int type,const char* url,char* data,char* permissions,char* username,const char* token);

void hexDump (const char * desc, const void * addr, const int len);

void addFunction_proteo(lua_State *state,const char *lib,const char *fname,lua_CFunction f);
void addTable_proteo(lua_State *state,const char *lib,const char *tname);
void addEnum_proteo(lua_State *state,const char *lib,const char *enumname,const char *vname,int value);

//==============================================================================
//   SQLITE
//==============================================================================

int sqlite_addCloudPlugin(const char* app,const char* url,int value);

int sqlite_create_auth_user_db(const char* path);
int sqlite_addUser(char* user,char* password);
int sqlite_getUserID(char* user,int* userID);
int sqlite_getPassword(const char* user,char** password);
int sqlite_getPermission(const char* user,const char* app,char** permission);
int sqlite_setPassword(char* user,char* password);
int sqlite_setPermission(char* user,char* app,char* permission);
int sqlite_deleteUser(char* user,int delete);
int sqlite_deletePermission(char* user,char* app,int delete);

int getTicket(const char* app,char** url);

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
