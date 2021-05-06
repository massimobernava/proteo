//==============================================================================
//=>                         Proteo v2.0
//=>
//=>
//=>                         CC BY-NC-SA 3.0
//=>
//=> Massimo Bernava
//=> massimo.bernava@gmail.com
//=> 2021-05-06
//==============================================================================

//#if defined(__MINGW32__) || defined(__MINGW64__)
//    #define _WIN32
//#endif

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
    //"Unknown compiler"
#endif

#ifdef __EMSCRIPTEN__
    //#include <emscripten.h>
    //#include <emscripten/fetch.h>

    #define PROTEO_USE_TINYJSON
    #define PROTEO_USE_INTERNALMD5
#elif defined(_WIN32)
    #define PROTEO_USE_TINYJSON
    
#endif

#define PROTEO_ZMQ
#define PROTEO_OPENCV
#define PROTEO_FFMPEG

//TODO
//#define DLINK_LIST_COMPONENT

#define PROTEO_ENET

#define BEZIER
//#define DEBUG_BEZIER

#include "proteo.h"

#include "proteo_b64.c"
#include "proteo_config.c"
#include "proteo_sdl.c"
#include "proteo_gui.c"
#include "proteo_lua.c"
#include "proteo_network.c"
#ifndef __EMSCRIPTEN__
#include "proteo_bifrost.c"
#endif
#include "proteo_graphics.c"
#include "proteo_system.c"
#include "proteo_audio.c"
#ifdef PROTEO_ZMQ
#include "proteo_zmq.c"
#endif
#ifdef PROTEO_ENET
#include "proteo_enet.c"
#endif
#ifdef __EMSCRIPTEN__
    #ifdef PROTEO_OPENCV
    #include "proteo_opencv.c"
    #endif
#endif
#ifdef PROTEO_FFMPEG
#include "proteo_ffmpeg.c"
#endif
//==============================================================================
//   TODO
//==============================================================================

//
//==============================================================================
//   UTILITY
//==============================================================================

ProteoColor brightness(ProteoColor color,float bright)
{
	ProteoColor col={255,
		MIN(255,color.r*bright),
		MIN(255,color.g*bright),
		MIN(255,color.b*bright)};

		return col;
}

ProteoColor saturation(ProteoColor color, float sat)
{
	const float Pr=0.299;
	const float Pg=0.587;
	const float Pb=0.114;

	float P=sqrt(color.r*color.r*Pr+color.g*color.g*Pg+color.b*color.b*Pb);

	ProteoColor col={255,
		P+(color.r-P)*sat,
		P+(color.g-P)*sat,
		P+(color.b-P)*sat};

		return col;
}

char font_path[256];
char* font2path(const char* font)
{
	//#ifdef __EMSCRIPTEN__
	//strcpy(font_path,"");
	//#else
	strcpy(font_path,config.basedir);
	//#endif

	size_t n = sizeof(nfonts)/sizeof(nfonts[0]);
	for(int i=0;i<n;i++)
	{
		if(strcasecmp(font,nfonts[i].name)==0)
		{
			return strcat(font_path,nfonts[i].path);
		}
	}
	return NULL;
}

char icon_path[256];
char* icon2path(const char* icon)
{
    strcpy(icon_path,config.basedir);
    //#endif

    size_t n = sizeof(nicons)/sizeof(nicons[0]);
    for(int i=0;i<n;i++)
    {
        if(strcasecmp(icon,nicons[i].name)==0)
        {
            return strcat(icon_path,nicons[i].path);
        }
    }
    return NULL;
}

ProteoColor hex2color(const char* input) {

	size_t n = sizeof(colors)/sizeof(colors[0]);
	for(int i=0;i<n;i++)
	{
		if(strcasecmp(input,colors[i].name)==0) return colors[i].color;
	}

    ProteoColor col;
	char color[8];

    if(verbose) printf("hex2color: %s\n",input);
	if (input[0] == '#')
	{
		strcpy(color,input);
		color[0]='0';
	}
	else
	{
		return col;
	}

    unsigned long value = strtoul(color, NULL, 16);

    col.a = 255;//(value >> 24) & 0xff;
    col.r = (value >> 16) & 0xff;
    col.g = (value >> 8) & 0xff;
    col.b = (value >> 0) & 0xff;
    //return color;

    return col;
}

void mkpath(const char* dir) {
        char tmp[256];
        char *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp),"%s",dir);
        dirname(tmp);
        len = strlen(tmp);
        if(tmp[len - 1] == '/')
                tmp[len - 1] = 0;
        for(p = tmp + 1; *p; p++)
                if(*p == '/') {
                        *p = 0;
                        #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__MINGW64__)
                        _mkdir(tmp);
                        #else
                        mkdir(tmp, S_IRWXU);
                        #endif
                        *p = '/';
                }
        #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__MINGW64__)
        _mkdir(tmp);
        #else
        mkdir(tmp, S_IRWXU);
        #endif
}

int writefile(char* filename,char* data)
{
    FILE *handler=fopen(filename,"w");
    if (handler)
    {
        fprintf(handler,"%s",data);
        fclose(handler);
        return 0;
    }

    return 1;
}

int writedatafile(char* filename,char* data,int data_size)
{
	int write_size;
	FILE *handler=fopen(filename,"wb");
    if (handler)
    {
        write_size = fwrite(data, sizeof(char), data_size, handler);
        fclose(handler);
        if(debug) printf("Write data file: %s,%d,%d\n",filename,data_size,write_size);
        return write_size;
    }
    else printf("Write data %s error: %s\n",filename,strerror(errno));

    return 0;
}

char* loadfile(char *filename)
{
   char *buffer = NULL;
   int string_size, read_size;
   FILE *handler = fopen(filename, "r");

   if (handler)
   {
       fseek(handler, 0, SEEK_END);
       string_size = ftell(handler);
       rewind(handler);

       buffer = (char*) malloc(sizeof(char) * (string_size + 1) );
       if(buffer!=NULL)
       {
           read_size = fread(buffer, sizeof(char), string_size, handler);
           buffer[string_size] = '\0';

           if (string_size != read_size)
           {
               free(buffer);
               buffer = NULL;
           }
       }

      fclose(handler);
    }
    else printf("Load file %s error: %s\n",filename,strerror(errno));

    return buffer;
}

int loaddatafile(char *filename,char **buffer)
{
  	int data_size, read_size;
	FILE *handler = fopen(filename, "r");

	if (handler)
	{
    	fseek(handler, 0, SEEK_END);
    	data_size = ftell(handler);
    	rewind(handler);

    	*buffer = (char*) malloc(sizeof(char) * (data_size) );
    	if(buffer!=NULL)
    	{
    		read_size = fread(*buffer, sizeof(char), data_size, handler);

    		if (data_size != read_size)
    		{
           		free(*buffer);
           		*buffer = NULL;
           		return 0;
       		}
			//if(debug) printf("Load data file: %s,%d,%d\n",filename,data_size,read_size);
            if(debug) printf("Load data file: %s\n",filename);
		}
      fclose(handler);
    }
    else printf("Load data %s error: %s\n",filename,strerror(errno));

    return read_size;
}

void hexDump (const char * desc, const void * addr, const int len) {
    int i;
    unsigned char buff[17];
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.

    if (desc != NULL)
        printf ("%s:\n", desc);

    // Length checks.

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    else if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.

    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Don't print ASCII buffer for the "zeroth" line.

            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.

            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.

    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII buffer.

    printf ("  %s\n", buff);
}

char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1);

    if(result!=NULL)
    {
        memcpy(result, s1, len1);
        memcpy(result + len1, s2, len2 + 1);
    }
    result[len1 + len2]=0;

    return result;
}

char* concat3(const char *s1, const char *s2, const char *s3)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    const size_t len3 = strlen(s3);

    char *result = malloc(len1 + len2 + len3 + 1);

    if(result!=NULL)
    {
        memcpy(result, s1, len1);
        memcpy(result + len1, s2, len2);
        memcpy(result + len1 + len2, s3, len3 + 1); // +1 to copy the null-terminator
    }
    result[len1 + len2 + len3]=0;

    return result;
}

//simple encrypt-decrypt function
void encryptDecrypt(char inpString[])
{
    char xorKey[] = "Key";
    int len = strlen(inpString);

    for (int i = 0; i < len; i++)
    {
        inpString[i] = inpString[i] ^ xorKey[i%strlen(xorKey)];
        printf("%c",inpString[i]);
    }
}

#ifdef AFFINE
SDL_Point affineTrasformation(SDL_Point point,float affine[3][3])
{
    SDL_Point ret;

    ret.x=point.x*affine[0][0]+point.y*affine[1][0]+affine[2][0];
    ret.y=point.x*affine[0][1]+point.y*affine[1][1]+affine[2][1];

    return ret;
}
#endif

void mult_matrices(float a[3][3], float b[3][3], float result[3][3])
{
    int i, j, k;
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            for(k = 0; k < 3; k++)
            {
                result[i][j] +=  a[i][k] *  b[k][j];
            }
        }
    }
}


void laderman_mul(const float a[3][3],const float b[3][3],float c[3][3]) {

   float m[24]; // not off by one, just wanted to match the index from the paper

   m[1 ]= (a[0][0]+a[0][1]+a[0][2]-a[1][0]-a[1][1]-a[2][1]-a[2][2])*b[1][1];
   m[2 ]= (a[0][0]-a[1][0])*(-b[0][1]+b[1][1]);
   m[3 ]= a[1][1]*(-b[0][0]+b[0][1]+b[1][0]-b[1][1]-b[1][2]-b[2][0]+b[2][2]);
   m[4 ]= (-a[0][0]+a[1][0]+a[1][1])*(b[0][0]-b[0][1]+b[1][1]);
   m[5 ]= (a[1][0]+a[1][1])*(-b[0][0]+b[0][1]);
   m[6 ]= a[0][0]*b[0][0];
   m[7 ]= (-a[0][0]+a[2][0]+a[2][1])*(b[0][0]-b[0][2]+b[1][2]);
   m[8 ]= (-a[0][0]+a[2][0])*(b[0][2]-b[1][2]);
   m[9 ]= (a[2][0]+a[2][1])*(-b[0][0]+b[0][2]);
   m[10]= (a[0][0]+a[0][1]+a[0][2]-a[1][1]-a[1][2]-a[2][0]-a[2][1])*b[1][2];
   m[11]= a[2][1]*(-b[0][0]+b[0][2]+b[1][0]-b[1][1]-b[1][2]-b[2][0]+b[2][1]);
   m[12]= (-a[0][2]+a[2][1]+a[2][2])*(b[1][1]+b[2][0]-b[2][1]);
   m[13]= (a[0][2]-a[2][2])*(b[1][1]-b[2][1]);
   m[14]= a[0][2]*b[2][0];
   m[15]= (a[2][1]+a[2][2])*(-b[2][0]+b[2][1]);
   m[16]= (-a[0][2]+a[1][1]+a[1][2])*(b[1][2]+b[2][0]-b[2][2]);
   m[17]= (a[0][2]-a[1][2])*(b[1][2]-b[2][2]);
   m[18]= (a[1][1]+a[1][2])*(-b[2][0]+b[2][2]);
   m[19]= a[0][1]*b[1][0];
   m[20]= a[1][2]*b[2][1];
   m[21]= a[1][0]*b[0][2];
   m[22]= a[2][0]*b[0][1];
   m[23]= a[2][2]*b[2][2];

  c[0][0] = m[6]+m[14]+m[19];
  c[0][1] = m[1]+m[4]+m[5]+m[6]+m[12]+m[14]+m[15];
  c[0][2] = m[6]+m[7]+m[9]+m[10]+m[14]+m[16]+m[18];
  c[1][0] = m[2]+m[3]+m[4]+m[6]+m[14]+m[16]+m[17];
  c[1][1] = m[2]+m[4]+m[5]+m[6]+m[20];
  c[1][2] = m[14]+m[16]+m[17]+m[18]+m[21];
  c[2][0] = m[6]+m[7]+m[8]+m[11]+m[12]+m[13]+m[14];
  c[2][1] = m[12]+m[13]+m[14]+m[15]+m[22];
  c[2][2] = m[6]+m[7]+m[8]+m[9]+m[23];
}

SDL_FPoint I2FPoint(SDL_Point p)
{
    SDL_FPoint ret={p.x,p.y};
    return ret;
}
SDL_Point F2IPoint(SDL_FPoint p)
{
    SDL_Point ret={p.x,p.y};
    return ret;
}
SDL_FPoint subtract(SDL_FPoint p0, SDL_FPoint p1)
{
    SDL_FPoint ret={p0.x-p1.x,p0.y-p1.y};

    return ret;
}

SDL_FPoint sum(SDL_FPoint p0, SDL_FPoint p1)
{
    SDL_FPoint ret={p0.x+p1.x,p0.y+p1.y};

    return ret;
}

SDL_FPoint multiply(SDL_FPoint p, float f)
{
    SDL_FPoint ret={p.x*f,p.y*f};

    return ret;
}

float dot(SDL_FPoint p0, SDL_FPoint p1)
{
    float ret=(p0.x*p1.x)+(p0.y*p1.y);

    return ret;
}

float distance(SDL_FPoint p0,SDL_FPoint p1)
{
    return hypotf(p0.x-p1.x,p0.y-p1.y);
}

SDL_FPoint normalize(SDL_FPoint p)
{
    float mag=hypotf(p.x,p.y);
    SDL_FPoint ret={p.x/mag,p.y/mag};

    //problema di normalizzazione? vettore spesso 0
    return ret;
}

float magnitude(SDL_FPoint p)
{
    float len=hypotf(p.x,p.y);

    return len;
}

SDL_FPoint getClosestPointOnSegment(SDL_FPoint p0, SDL_FPoint p1, SDL_FPoint p)
{
    SDL_FPoint d=subtract(p1,p0);
    float c=dot(subtract(p,p0),d)/dot(d,d);
    if(c>=1) return p1;
    else if(c<=0) return p0;
    else return sum(p0,multiply(d,c));
}

/*static getClosestPointOnSegment(p0, p1, p) {
    let d = p1.subtract(p0);
    let c = p.subtract(p0).dot(d) / (d.dot(d));
    if (c >= 1) {
        return p1.clone();
    } else if (c <= 0) {
        return p0.clone();
    } else {
        return p0.add(d.multiply(c));
    }
}
*/
//==============================================================================
//   PROTEO
//==============================================================================
static int require (lua_State *state) {

	const char* lib = luaL_checkstring(state,1);
	if(verbose) printf("require %s\n",lib);

    ProteoLib* tmp=findLib(lib);
    if(tmp==NULL)
    {
	#ifdef __EMSCRIPTEN__
        //TODO ?
        lua_pushnil(L);
	#else
        char get_lib[50];

        strcpy(get_lib,"/proteo/lib/");
        strcat(get_lib,lib);
        bifrost_get(get_lib,PROTEO_APP_KEY,Token,"");
	#endif
    }
    else
		luaL_dostring(L,tmp->lib);

	return 1;
}

//==============================================================================
//   MEDIA
//==============================================================================

static int media_bhrDiscover(lua_State *state) {
  printf("bhrDiscover: %s\n",luaL_checkstring(state,1));
  return 0;
}

//==============================================================================
//   THREAD
//==============================================================================
/*void* thread_update(void* arg)
{
    while(run)
    {
        lua_pushcfunction(L, traceback);
        lua_getglobal(L,"update");
        lua_pushinteger(L, 0); //dt
        int error = lua_pcall(L, 1, 0, -2);
        if (error) {
            fprintf(stderr, "ERROR pcall(update): %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
	return NULL;
}
*/

//==============================================================================
//   EXIT_CLI
//==============================================================================
void exit_cli(int sig)
{
	run = FALSE;
	printf("\rproteo closing down...\n\n");
}

//==============================================================================
//   PRINTHELP
//==============================================================================
void printhelp()
{
	printf("Usage: proteo [glvhru:p:ds:b:a:c:] \n");
	printf("Options:\n");
    printf("\tg: graphic login mode\n");
	printf("\tv: verbose mode\n");
    printf("\td: debug mode\n");
	printf("\th: show this help\n");
	printf("\tl: local script\n");
	printf("\tr: remote remote\n");
	printf("\tu: username\n");
	printf("\tp: password\n");
    printf("\ts: script name\n");
    printf("\tb: base directory\n");
    printf("\ta: remote server\n");
	printf("\n\n");
}

//==============================================================================
//   PROTEO_LIB
//==============================================================================
LUALIB_API int luaopen_proteo (lua_State *state) {

	lua_newtable(state);

	lua_newtable(state);
	lua_setfield(state, -2, "system");

	lua_newtable(state);
	lua_setfield(state, -2, "gui");

	lua_newtable(state);
	lua_setfield(state, -2, "graphics");

	lua_newtable(state);
	lua_setfield(state, -2, "network");

	lua_newtable(state);
	lua_setfield(state, -2, "media");

	lua_newtable(state);
	lua_setfield(state, -2, "enet");

    lua_newtable(state);
    lua_setfield(state, -2, "zmq");

    lua_newtable(state);
    lua_setfield(state, -2, "opencv");

    lua_newtable(state);
    lua_setfield(state, -2, "audio");

	lua_newtable(state);
	lua_setfield(state, -2, "debug");

    lua_newtable(state);
    lua_setfield(state, -2, "ffmpeg");
    
	lua_setglobal(state,"proteo");

	lua_pushinteger(state, PROTEO_MAJOR_VERSION);
	lua_setglobal(state, "MAJOR_VERSION");

    lua_pushinteger(state, PROTEO_MINOR_VERSION);
    lua_setglobal(state, "MINOR_VERSION");

    #if TARGET_OS_IPHONE
    lua_pushboolean(state, TRUE);
    #else
    lua_pushboolean(state, FALSE);
    #endif
    lua_setglobal(state,  "IPHONE");

    #if __linux__
    lua_pushboolean(state, TRUE);
    #else
    lua_pushboolean(state, FALSE);
    #endif
    lua_setglobal(state,  "LINUX");

    add_system_proteo(state);
    add_gui_proteo(state);
    add_network_proteo(state);
    add_graphics_proteo(state);
    add_audio_proteo(state);
    #ifdef PROTEO_ZMQ
    add_zmq_proteo(state);
    #endif

    #ifdef PROTEO_FFMPEG
    add_ffmpeg_proteo(state);
    #endif
    
  return 1;
}

void addFunction_proteo(lua_State *state,const char *lib,const char *fname,lua_CFunction f)
{
	lua_getglobal(state,"proteo");
	lua_getfield(state,-1,lib);
	lua_pushcfunction(state,f);
	lua_setfield(state, -2, fname);
	lua_pop(state,2);
}

void addTable_proteo(lua_State *state,const char *lib,const char *tname)
{
	lua_getglobal(state,"proteo");
	lua_getfield(state,-1,lib);
	lua_newtable(state);
	lua_setfield(state, -2, tname);
	lua_pop(state,2);
}

void addEnum_proteo(lua_State *state,const char *lib,const char *enumname,const char *vname,int value)
{
	lua_getglobal(state,"proteo");
	lua_getfield(state,-1,lib);
	lua_getfield(state,-1,enumname);
	lua_pushinteger(state,value);
	lua_setfield(state, -2, vname);
	lua_pop(state,3);
}

//==============================================================================
//   INITLUA
//==============================================================================
int initLUA()
{
    L = luaL_newstate();//lua_open();

    luaL_openlibs(L);

    lua_getglobal(L, "require");
    lua_setglobal(L, "local_require");
    lua_pushcfunction(L,require);
	lua_setglobal(L, "require");
	//lua_pop(L,2);

	luaopen_proteo(L);

    //TODO devo decidere se mantenere "media"
    // potrei inserire tutte le funzioni che fanno riferimento ad altre librerie semplificandone l'uso:
    // newCamera, recordCamera, newVideo, playVideo ecc... usando OpenCv
    // proteo.media.pose usando sempre OpenCV
    // newSound, playSound ecc... usando proteo.audio

    addFunction_proteo(L,"media","bhrDiscover",media_bhrDiscover);

	addFunction_proteo(L,"debug","traceback",traceback);

	//printf("GetTop %d\n",lua_gettop(L));
#ifdef PROTEO_OPENCV
    addFunction_proteo(L,"opencv","imread",opencv_imread);
    addFunction_proteo(L,"opencv","imwrite",opencv_imwrite);
    addFunction_proteo(L,"opencv","videocapture",opencv_videocapture);
    addFunction_proteo(L,"opencv","videowriter",opencv_videowriter);
    addFunction_proteo(L,"opencv","write",opencv_write);
    addFunction_proteo(L,"opencv","frame",opencv_frame);
    #ifdef OPENCVNET
    addFunction_proteo(L,"opencv","readnet",opencv_readnet);
    addFunction_proteo(L,"opencv","forward",opencv_forward);
    addFunction_proteo(L,"opencv","forwardTable",opencv_forwardTable);
    addFunction_proteo(L,"opencv","infoNet",opencv_infoNet);
    #endif
    //addFunction_proteo(L,"opencv","getpoints",opencv_getpoints);
    addFunction_proteo(L,"opencv","circle",opencv_circle);
    addFunction_proteo(L,"opencv","rectangle",opencv_rectangle);
    addFunction_proteo(L,"opencv","sliceImg",opencv_sliceImg);
    addFunction_proteo(L,"opencv","minMaxLoc",opencv_minMaxLoc);
    addFunction_proteo(L,"opencv","img",opencv_img);
    addFunction_proteo(L,"opencv","line",opencv_line);
    addFunction_proteo(L,"opencv","putText",opencv_putText);
    addFunction_proteo(L,"opencv","size",opencv_size);
    addFunction_proteo(L,"opencv","cropImg",opencv_cropImg);
    addFunction_proteo(L,"opencv","infoImg",opencv_infoImg);

    addFunction_proteo(L,"opencv","setImg",opencv_setImg);
    addFunction_proteo(L,"opencv","setSize",opencv_setSize);
    addFunction_proteo(L,"opencv","fill",opencv_fill);
    addFunction_proteo(L,"opencv","resize",opencv_resize);
    addFunction_proteo(L,"opencv","mul",opencv_mul);
    addFunction_proteo(L,"opencv","add",opencv_add);
    addFunction_proteo(L,"opencv","addWeighted",opencv_addWeighted);
    addFunction_proteo(L,"opencv","flip",opencv_flip);
    addFunction_proteo(L,"opencv","threshold",opencv_threshold);
    addFunction_proteo(L,"opencv","print",opencv_print);
    addFunction_proteo(L,"opencv","setFrameSize",opencv_setFrameSize);
    addFunction_proteo(L,"opencv","getFrameSize",opencv_getFrameSize);
    addFunction_proteo(L,"opencv","setBufferSize",opencv_setBufferSize);
    addFunction_proteo(L,"opencv","imencode",opencv_imencode);
    addFunction_proteo(L,"opencv","imdecode",opencv_imdecode);
    addFunction_proteo(L,"opencv","convert",opencv_convert);
    addFunction_proteo(L,"opencv","getAffineTransform",opencv_getaffinetransform);
    addFunction_proteo(L,"opencv","warpAffine",opencv_warpaffine);
    addFunction_proteo(L,"opencv","toTable",opencv_totable);
    
    addTable_proteo(L,"opencv","matType");
    addEnum_proteo(L,"opencv","matType","CV_8U",0);
    addEnum_proteo(L,"opencv","matType","CV_8S",1);
    addEnum_proteo(L,"opencv","matType","CV_16U",2);
    addEnum_proteo(L,"opencv","matType","CV_16S",3);
    addEnum_proteo(L,"opencv","matType","CV_32S",4);
    addEnum_proteo(L,"opencv","matType","CV_32F",5);
    addEnum_proteo(L,"opencv","matType","CV_64F",6);
    addEnum_proteo(L,"opencv","matType","CV_16F",7);

    addTable_proteo(L,"opencv","threshType");
    addEnum_proteo(L,"opencv","threshType","THRESH_BINARY",0);
    addEnum_proteo(L,"opencv","threshType","THRESH_BINARY_INV",1);
    addEnum_proteo(L,"opencv","threshType","THRESH_TRUNC",2);
    addEnum_proteo(L,"opencv","threshType","THRESH_TOZERO",3);
    addEnum_proteo(L,"opencv","threshType","THRESH_TOZERO_INV",4);

    addTable_proteo(L,"opencv","fourcc");
    addEnum_proteo(L,"opencv","fourcc","MJPG",0);

#endif

#ifdef PROTEO_ENET
	addFunction_proteo(L,"enet","linked_version",linked_version);
	addFunction_proteo(L,"enet","host_create",host_create);

	// create metatables
	luaL_newmetatable(L, "enet_host");
	lua_newtable(L); // index
	luaL_register(L, NULL, enet_host_funcs);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, host_gc);
	lua_setfield(L, -2, "__gc");
	lua_pop(L,1);

	luaL_newmetatable(L, "enet_peer");
	lua_newtable(L);
	luaL_register(L, NULL, enet_peer_funcs);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, peer_tostring);
	lua_setfield(L, -2, "__tostring");
	lua_pop(L,1);

	// set up peer table
	lua_newtable(L);

	lua_newtable(L); // metatable
	lua_pushstring(L, "v");
	lua_setfield(L, -2, "__mode");
	lua_setmetatable(L, -2);

	lua_setfield(L, LUA_REGISTRYINDEX, "enet_peers");
#endif

	if(verbose) printf("GetTop %d\n",lua_gettop(L));

	return 1;
}



//==============================================================================
//   MAIN
//==============================================================================

SDL_Event e;
unsigned int frameStart,frameTime;//,frameDelay=16;

void eventloop(ProteoComponent* list)
{
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT) {
            run = FALSE;
        }
        else if (e.type == SDL_WINDOWEVENT) {
            //if(verbose) printf("SDL_WINDOWEVENT\n");
            return;
        }
        else if (e.type == SDL_MOUSEMOTION) {
            //if(verbose) printf("SDL_MOUSEMOTION\n");
        }
        else if(e.type == SDL_MOUSEBUTTONUP)
        {
            //if(verbose) printf("SDL_MOUSEBUTTONUP\n");

            lua_getglobal(L,"release");
            if(lua_isfunction(L, -1) )
            {
                lua_pushinteger(L, e.button.x/gScale);
                lua_pushinteger(L, e.button.y/gScale);
                int error = lua_pcall(L, 2, 0, 0);

                if (error) {
                    fprintf(stderr, "ERROR pcall(touch): %s\n", lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
            }
        }
        else if(e.type == SDL_MOUSEWHEEL)
        {
            //if(verbose) printf("SDL_MOUSEWHEEL\n");
        }
        else if(e.type == SDL_MOUSEBUTTONDOWN)
        {
            //printf("SDL_MOUSEBUTTONDOWN x:%d y:%d \n",e.button.x,e.button.y);

            lua_getglobal(L,"touch");
            if(lua_isfunction(L, -1) )
            {
                lua_pushinteger(L, e.button.x/gScale);
                lua_pushinteger(L, e.button.y/gScale);
                int error = lua_pcall(L, 2, 0, 0);

                if (error) {
                    fprintf(stderr, "ERROR pcall(touch): %s\n", lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
            }
        }
        else if(e.type == SDL_KEYDOWN)
        {
            lua_getglobal(L,"keydown");
            if(lua_isfunction(L, -1) )
            {
                lua_pushinteger(L,e.key.keysym.sym);
                int error = lua_pcall(L, 1, 0, 0);

                if (error) {
                    fprintf(stderr, "ERROR pcall(touch): %s\n", lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
            }
        }
        else if(e.type == SDL_KEYUP)
        {
            lua_getglobal(L,"keyup");
            if(lua_isfunction(L, -1) )
            {
                lua_pushinteger(L,e.key.keysym.sym);
                int error = lua_pcall(L, 1, 0, 0);

                if (error) {
                    fprintf(stderr, "ERROR pcall(touch): %s\n", lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
            }

        }
        else if(e.type == SDL_FINGERDOWN)
        {
            if(verbose) printf("SDL_FINGERDOWN\n");
            
            lua_getglobal(L,"touch");
            if(lua_isfunction(L, -1) )
            {
                lua_pushinteger(L, e.tfinger.x*SCREEN_WIDTH);
                lua_pushinteger(L, e.tfinger.y*SCREEN_HEIGHT);
                int error = lua_pcall(L, 2, 0, 0);

                if (error) {
                    fprintf(stderr, "ERROR pcall(touch): %s\n", lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
            }
        }
        else if(e.type == SDL_FINGERUP)
        {
            if(verbose) printf("SDL_FINGERUP\n");
            
            lua_getglobal(L,"release");
            if(lua_isfunction(L, -1) )
            {
                lua_pushinteger(L, e.tfinger.x*SCREEN_WIDTH);
                lua_pushinteger(L, e.tfinger.y*SCREEN_HEIGHT);
                int error = lua_pcall(L, 2, 0, 0);

                if (error) {
                    fprintf(stderr, "ERROR pcall(touch): %s\n", lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
            }
        }
        else if(e.type == SDL_FINGERMOTION)
        {
            if(verbose) printf("SDL_FINGERMOTION\n");
        }
        else if(e.type == SDL_MULTIGESTURE)
        {
            if(verbose) printf("SDL_MULTIGESTURE\n");
        }
        else if(debug) printf("SDL_EVENT type: %d\n",e.type);

        //TODO mentre vanno disegnati dal fondo in avanti, gli eventi dovrebbero "fermarsi" sui componenti più in vista, quindi sarebbe più utile procedere al contrario in questo ciclo
        ProteoComponent* current=list;
        while(current!=NULL)
        {
            if(current->type!=Deleted && current->hidden==FALSE && current->enabled==TRUE  && current->parent==NULL)
            {
                if(current->type==Button) { if(gui_eventButton (L,current,e,gRenderer)) break; }
                else if(current->type==TextField) { if(gui_eventTextField (L,current,e,gRenderer)) break; }
                else if(current->type==List) { if(gui_eventList (L,current,e,gRenderer)) break; }
                //else if(current->type==Image) { if(graphics_eventImage (L,current,e,gRenderer)) break; }
                else if(current->type==Checkbox) { if(gui_eventCheckbox (L,current,e,gRenderer)) break; }
                else if(current->type==DropDown) { if(gui_eventDropDown(L,current,e,gRenderer)) break; }
                else if(current->type==Form) { if(gui_eventForm(L,current,e,gRenderer)) break; }
                else if(current->type==Container) { if(gui_eventContainer(L,current,e,gRenderer)) break; }

                //Con questa modifica tutti i componenti impilati si attivano ma è meglio attivare solo quelli in alto
                /*if(current->type==Button) gui_eventButton (L,current,e,gRenderer);
                else if(current->type==TextField) gui_eventTextField (L,current,e,gRenderer);
                else if(current->type==List) gui_eventList (L,current,e,gRenderer);
                //else if(current->type==Image) { if(graphics_eventImage (L,current,e,gRenderer)) break; }
                else if(current->type==Checkbox) gui_eventCheckbox (L,current,e,gRenderer);
                else if(current->type==DropDown) gui_eventDropDown(L,current,e,gRenderer);
                else if(current->type==Form) gui_eventForm(L,current,e,gRenderer);*/
            }
            #ifdef DLINK_LIST_COMPONENT
            current=current->prev;
            #else
            current=current->next;
            #endif
        }
    }
}
void mainloop()
{
	if(components==NULL) sleep(1);

    	//SDL_RenderClear(gRenderer);
        //SDL_RenderCopy(renderer, texture, NULL, NULL);
        //SDL_RenderCopy(renderer, spriteTexture, &rcSprite, &gdSprite);
		SDL_SetRenderDrawColor( gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255 );

		//frameStart = SDL_GetTicks();
    	// Clear window
    	SDL_RenderClear( gRenderer );

		ProteoComponent* current=components;
#ifdef DLINK_LIST_COMPONENT
        ProteoComponent* last=NULL;
#endif
    	while(current!=NULL)
		{
            if(current->type!=Deleted && current->hidden==FALSE && current->parent==NULL)
			{
                SDL_SetRenderDrawBlendMode(gRenderer,SDL_BLENDMODE_BLEND);
                
				if(current->type==Rect) graphics_drawRect (gRenderer,current);
				else if(current->type==Ellips) graphics_drawEllipse (gRenderer,current);
				else if(current->type==Label)  gui_drawLabel (gRenderer,current);
				else if(current->type==Image) graphics_drawImage (gRenderer,current);
                else if(current->type==Icon) graphics_drawIcon (gRenderer,current);
				else if(current->type==Button) gui_drawButton (gRenderer,current);
				else if(current->type==TextField) gui_drawTextField (gRenderer,current);
				else if(current->type==List) gui_drawList (gRenderer,current);
				else if(current->type==Checkbox) gui_drawCheckbox (gRenderer,current);
                else if(current->type==Sprite) graphics_drawSprite (gRenderer,current);
                else if(current->type==Polyg) graphics_drawPolygon (gRenderer,current);
                //else if(current->type==Bezier) graphics_drawBezier (gRenderer,current);
                else if(current->type==Shape) graphics_drawShape (gRenderer,current);
                else if(current->type==DropDown) gui_drawDropDown (gRenderer,current);
                else if(current->type==Form) gui_drawForm (gRenderer,current);
                else if(current->type==Container) gui_drawContainer (gRenderer,current);
			}
#ifdef DLINK_LIST_COMPONENT
            last=current;
#endif
            if(current->next!=NULL && current->next->type==Deleted) //TODO non cancella il primo elemento
                freecomponent(current,current->next);
			current=current->next;
		}

        ProteoSkeleton* current_sk=skeletons;
        while(current_sk!=NULL)
        {
            if(current_sk->hidden==FALSE)
            {
                graphics_drawSkeleton (gRenderer,current_sk);
            }

            current_sk=current_sk->next;
        }

        SDL_RenderPresent(gRenderer);
#ifdef DLINK_LIST_COMPONENT
    eventloop(last);
#else
    eventloop(components);
#endif
        frameTime = SDL_GetTicks() - frameStart;
        frameStart = SDL_GetTicks();

        lua_pushcfunction(L, traceback);
		lua_getglobal(L,"update");
        if(lua_isfunction(L, -1) )
        {
            lua_pushinteger(L, frameTime); //dt
            int error = lua_pcall(L, 1, 0, -2);
            if (error) {
                fprintf(stderr, "ERROR pcall(update): %s\n", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }

        struct timespec now;
		clock_gettime(CLOCK_REALTIME, &now);
		double n=now.tv_sec*1000;
		n=n+(now.tv_nsec/1000000.0);

        ProteoTimer* timer=timers;
        double min=25; 
		while(timer!=NULL)
		{
            if(timer->state==1)
            {
                if(timer->time<=(n-timer->last))
                {
                    if(timer->callback!=NULL)
                        lua_getglobal(L,timer->callback);
                    else if(timer->ref_callback!=-1)
                        lua_getref(L,timer->ref_callback);

                    if(lua_isfunction(L, -1) )
                    {
                        lua_pushinteger(L, n-timer->last);
                        int error = lua_trace_pcall(L,1,0);//lua_pcall(L, 1, 0, 0);

                        if (error) {
                            //printf("ERROR pcall(timer->callback,%d): %s\n",currentline(L),lua_tostring(L, -1));
                            printf("ERROR pcall(timer->callback): %s\n",lua_tostring(L, -1));
                            //lua_pop(L, 1);
                            int top=lua_gettop(L);
                            if(top>0)
                                lua_pop(L,top);
                            if(debug) printf("app_call GetTop %d\n",top);
                        }
                    }
                    timer->last=n;
                }
                else
                {
                    double next=timer->time -(n-timer->last);
                    if(next<min) min=next;
                }
            }
			timer=timer->next;
		}
        if(min>0) SDL_Delay((unsigned int)min);

		/*lua_getglobal(L,"proteo_network_roomCheck");
		if(lua_isfunction(L, -1) )
		{
			lua_pcall(L,0,0,0);
		}*/
}

int main(int argc,char **argv)
{
    //imread_c( "lena.jpg");

	pthread_t t_update;

	printf("Proteo\n");

	//char luafile[50];
	char username[50];
	char password[50];

	int opt_username=FALSE;
	int opt_password=FALSE;
	int opt_script=FALSE;
    int opt_gmode=FALSE;
   
	//int opt_appkey=FALSE;
    config=load_config();
    if(config.load_error!=0)
    {
        config=default_config();

        //TODO da salvare ma qui o no?
        //forse qui perchè non esiste, e poi altrove per eventuali modifiche
        //save_config(config);

        /*
         luaL_dostring(L,server_form);

         //lua_pushcfunction(L, traceback);
         lua_getglobal(L,"init");
         //int error = lua_pcall(L, 0, 0, -2);
         int error = lua_trace_pcall(L,0,0);

         if (error) {
             fprintf(stderr, "ERROR pcall(init): %s\n", lua_tostring(L, -1));
             lua_pop(L, 1);
         }
         */

        //try
        //strcpy(config.server,"http://localhost:8888");

    }

#if TARGET_OS_IPHONE

    /*opt_username=TRUE;
    opt_password=TRUE;
    opt_script=TRUE;

    remote_lua=TRUE;
    local_lua=FALSE;

    strcpy(username,"");
    strcpy(password,"");
    strcpy(script,"login");*/

    opt_gmode=TRUE;

    debug=TRUE;
    verbose=TRUE;

    char *home = getenv("HOME");
    strcpy(config.basedir,home);
    strcat(config.basedir,"/Documents/");

    //strcpy(basedir,"./");

    //printf("HOME: %s\n",home);
#elif __EMSCRIPTEN__
	opt_gmode=TRUE;

    debug=FALSE;//TRUE;
    verbose=FALSE;//TRUE;

	EM_ASM(
        // Make a directory other than '/'
        FS.mkdir('/proteo');
        // Then mount with IDBFS type
        FS.mount(IDBFS, {}, '/proteo');

        // Then sync
        FS.syncfs(true, function (err) {
            console.log("Mount proteo dir error: "+err);
        });
    );

    strcpy(config.basedir,"/proteo/");

#else
    int c;
	while ((c = getopt (argc, argv, "efglvhru:p:ds:b:a:c:")) != -1)
    	switch (c)
		{
            case 'g': //Graphic login
                opt_gmode=TRUE;
            break;
            case 'f': //Fullscreen
                opt_fullscreen=TRUE;
            break;
            case 'e': //REmote Console
                opt_remoteconsole=TRUE;
            break;
            case 'c': //loadconfig

            break;
			case 'b': //Basedir
                strcpy(config.basedir,optarg);
    			//strcat(basedir,"//");
    			printf("basedir: %s\n",config.basedir);
			break;
			case 'a':
                strcpy(config.server,optarg);
    			//strcat(basedir,"//");
    			printf("baseURL: %s\n",config.server);
			break;
			case 'l': //local script optarg=file

				local_lua=TRUE;

			break;

			case 'v': //verbose mode
				verbose=TRUE;
			break;

			case 'h': //print help
				printhelp();
				return 0;
			break;

			case 'r': //remote script

				//strcpy(basedir,optarg);
				remote_lua=TRUE;
			break;

			case 'u'://user
				strcpy(username,optarg);
				opt_username=TRUE;
			break;

			case 'p'://password
				//printf("%s\n",optarg);
				strcpy(password,optarg);
				opt_password=TRUE;
			break;

			case 's'://app
				//printf("%s\n",optarg);
				strcpy(script,optarg);
				opt_script=TRUE;
			break;

			/*case 'k':
				Appkey=loadfile(optarg);
				opt_appkey=TRUE;
				//printf("%s\n",Appkey );
			break;*/
			case 'd'://debug mode
				debug=TRUE;
			break;

			case '?':
				if (optopt == 't')
          				fprintf (stderr, "Option -%c requires Lua file.\n", optopt);
        			else if (isprint (optopt))
          				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        			else
          				fprintf (stderr,
                   			"Unknown option character `\\x%x'.\n",
                   			optopt);
        			return 1;
			default:
				abort();
		}
#endif // TARGET_OS_IPHONE

	//if(opt_appkey==FALSE) return 0;
#ifdef PROTEO_ENET
    enet_initialize();
#endif
	if( initLUA() == FALSE)
	{
		printf( "Failed to initialize LUA!\n" );
		return 1;
	}

	if( initSDL() == FALSE)
	{
		printf( "Failed to initialize SDL!\n" );
		return 2;
	}

    setLuaScreenSize(L);

    if(opt_gmode)
    {
        remote_lua=TRUE;
        local_lua=FALSE;
        strcpy(script,"login");

        proteo_login("","",script);
    }
	else if( (local_lua && remote_lua) || optind<argc)
	{
		printhelp();
		return 0;
	}
	else
	{
		if(opt_username && opt_password && opt_script)
		{
			#ifdef PROTEO2
			proteo_login(username,password,script);
			#else
			proteo_login(username,password,"8756c442-3e57-4754-b3b2-0a82b7b5d295");
			#endif
		}
		else
		{
            //login testuale
            if(!opt_username)
            {
				#ifdef __EMSCRIPTEN__
				char* user=(char*)EM_ASM_INT(
					var jsString = window.prompt('Username: ');
					var lengthBytes = lengthBytesUTF8(jsString)+1;
					var stringOnWasmHeap = _malloc(lengthBytes);
					stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
					return stringOnWasmHeap;
				);
				strcpy(username,user);
				free(user);
				#else
                printf("Username: ");
                scanf("%50[^\n]", username);
                printf("%s",username);
                #endif
            }
            if(!opt_password)
            {
				#ifdef __EMSCRIPTEN__
				char* pass=(char*)EM_ASM_INT(
					var jsString = window.prompt('Password: ');
					var lengthBytes = lengthBytesUTF8(jsString)+1;
					var stringOnWasmHeap = _malloc(lengthBytes);
					stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
					return stringOnWasmHeap;
				);
				strcpy(password,pass);
				free(pass);
				#elif defined(__MINGW32__) || defined(__MINGW64__)

				#else
					char *pass = getpass("Password: ");
					strcpy(password,pass);
                #endif
            }
            if(!opt_script)
            {
				#ifdef __EMSCRIPTEN__
				char* sc=(char*)EM_ASM_INT(
					var jsString = window.prompt('Script: ');
					var lengthBytes = lengthBytesUTF8(jsString)+1;
					var stringOnWasmHeap = _malloc(lengthBytes);
					stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
					return stringOnWasmHeap;
				);
				strcpy(script,sc);
				free(sc);
				#else
                printf("Script: ");
                scanf("%50[^\n]", script);
                printf("%s",script);
                #endif
            }
            #ifdef PROTEO2
            proteo_login(username,password,script);
            #else
            proteo_login(username,password,"abcdc123-abcd-1234-5678-abcdqwerzxcv");
            #endif
		}
	}

	lua_pushlstring(L,username,strlen(username));
	lua_setglobal(L, "USERNAME");

    lua_pushlstring(L,config.server,strlen(config.server));
    lua_setglobal(L, "SERVER");

	signal(SIGINT,exit_cli);
	signal(SIGTERM,exit_cli);

	run = TRUE;

    frameStart = SDL_GetTicks();

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(mainloop, -1, 1);
	//emscripten_request_animation_frame_loop(mainloop,0);
#else
	while(run)
	{
        mainloop();
	}
#endif
     /*if(pthread_create(&t_update,NULL,thread_update,optarg))
	{
		printf("error thread_update\n");
	}*/

	//FINISH
	//pthread_cancel(t_update);

	//free(Appkey);
    
    //CLOSE
    lua_getglobal(L,"close");
    if(lua_isfunction(L, -1) )
    {
        int error = lua_trace_pcall(L,0,0);
    }

    freeaccesspoint();
    freetimers();

	closeSDL();
	closeLUA();


#ifdef PROTEO_ENET
    enet_deinitialize();
#endif
	return 0;
}
