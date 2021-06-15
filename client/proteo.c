//==============================================================================
//=>                         Proteo v0.2
//=>
//=>
//=>                         CC BY-NC-SA 3.0
//=>
//=> Massimo Bernava
//=> massimo.bernava@gmail.com
//=> 2021-06-15
//==============================================================================

//#define TFLITE //CMAKE
//#define GLEW //CMAKE

//#if defined(__MINGW32__) || defined(__MINGW64__)
//    #define _WIN32
//#endif
#define TARGET_OS_WINDOWS 0

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

   #define TARGET_OS_WINDOWS 1
   #define TFLITE //For code::blocks

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
#else //if defined(_WIN32)
   // #define PROTEO_USE_TINYJSON
    #define PROTEO_ZMQ
    #define PROTEO_FFMPEG
    #define LUA_JIT

#endif

#ifdef GLEW
//#define USEOPENGL
#endif

#define PROTEO_OPENCV
//#define BACKBUFFER

//#define PROTEO_USE_TINYJSON

//TODO
//#define DLINK_LIST_COMPONENT

#define PROTEO_ENET

#define BEZIER
//#define DEBUG_BEZIER

#include "proteo.h"

#include "proteo_b64.c"
#include "proteo_config.c"
#include "proteo_sdl.c"
#ifdef USEOPENGL
#include "proteo_gl.c"
#endif
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
//#ifdef __EMSCRIPTEN__
    //#ifdef PROTEO_OPENCV
    //#include "proteo_opencv.c"
    //#endif
//#endif
#ifdef PROTEO_FFMPEG
#include "proteo_ffmpeg.c"
#endif
#ifdef TFLITE
#include "proteo_tensorflow.c"
#endif

#include "proteo_utility.c"

//==============================================================================
//   TODO
//==============================================================================

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
        bifrost_get(get_lib,config.appkey,Token,"");
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

    lua_newtable(state);
    lua_setfield(state, -2, "tflite");

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

    #ifdef TFLITE
    add_tensorflow_proteo(state);
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
    addFunction_proteo(L,"opencv","copy",opencv_copy);
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

void drawloop(ProteoComponent* current)
{
    //SDL_RenderClear(gRenderer);
    //SDL_RenderCopy(renderer, texture, NULL, NULL);
    //SDL_RenderCopy(renderer, spriteTexture, &rcSprite, &gdSprite);
    SDL_SetRenderDrawColor( gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255 );

    //frameStart = SDL_GetTicks();
    // Clear window
#ifdef BACKBUFFER
    SDL_SetRenderTarget(gRenderer, gTarget); //BACKBUFFER
#endif
    SDL_RenderClear( gRenderer );

    //ProteoComponent* current=components;
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
            else if(current->type==Particle) graphics_drawParticle (gRenderer,current);
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
#ifdef BACKBUFFER
SDL_SetRenderTarget(gRenderer, NULL); //BACKBUFFER
SDL_RenderCopy(gRenderer, gTarget, NULL, NULL ); //BACKBUFFER
#endif

SDL_RenderPresent(gRenderer);

}

#ifdef USEOPENGL
void drawloop_GL(ProteoComponent* current)
{
    glViewport(0, 0, gWidth, gHeight);
    glClearColor(backgroundColor.r/255.0f, backgroundColor.g/255.0f, backgroundColor.b/255.0f, backgroundColor.a/255.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //glColor3f(0.8f, 0.2f, 0.4f);
    //glRectf(-0.5f, -0.5f, 0.5f, 0.5f);

    //glEnable(GL_TEXTURE_2D);
    //glActiveTexture(GL_TEXTURE0);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

#ifdef DLINK_LIST_COMPONENT
    ProteoComponent* last=NULL;
#endif
    while(current!=NULL)
    {
        if(current->type!=Deleted && current->hidden==FALSE && current->parent==NULL)
        {
            //SDL_SetRenderDrawBlendMode(gRenderer,SDL_BLENDMODE_BLEND);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            if(current->type==Rect) graphics_drawRect_GL (current);
            //else if(current->type==Ellips) graphics_drawEllipse (gRenderer,current);
            else if(current->type==Label)  gui_drawLabel_GL (current);
            //else if(current->type==Image) graphics_drawImage (gRenderer,current);
            //else if(current->type==Icon) graphics_drawIcon (gRenderer,current);
            else if(current->type==Button) gui_drawButton_GL (current);
            else if(current->type==TextField) gui_drawTextField_GL (current);
            //else if(current->type==List) gui_drawList (gRenderer,current);
            //else if(current->type==Checkbox) gui_drawCheckbox (gRenderer,current);
            //else if(current->type==Sprite) graphics_drawSprite (gRenderer,current);
            //else if(current->type==Sprite) graphics_drawParticle (gRenderer,current);
            //else if(current->type==Polyg) graphics_drawPolygon (gRenderer,current);
            //else if(current->type==Bezier) graphics_drawBezier (gRenderer,current);
            //else if(current->type==Shape) graphics_drawShape (gRenderer,current);
            //else if(current->type==DropDown) gui_drawDropDown (gRenderer,current);
            else if(current->type==Form) gui_drawForm_GL (current);
            //else if(current->type==Container) gui_drawContainer (gRenderer,current);
        }
#ifdef DLINK_LIST_COMPONENT
        last=current;
#endif
        if(current->next!=NULL && current->next->type==Deleted) //TODO non cancella il primo elemento
            freecomponent(current,current->next);
        current=current->next;
    }

    SDL_GL_SwapWindow(gWindow);
}
#endif

void mainloop()
{
	if(components==NULL) sleep(1);

#ifdef USEOPENGL
    drawloop_GL(components);
#else
    drawloop(components);
#endif

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
		double n=now.tv_sec*1000.0;
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

#ifdef __EMSCRIPTEN__
    printf("Proteo Emscripten\n");
#else
	printf("Proteo %s\n",argv[0]);
#endif

#if TARGET_OS_MAC
    wordexp_t exp_result;
    wordexp("~/Library/Preferences/Proteo", &exp_result, 0);
    mkpath(exp_result.we_wordv[0]);
    wordfree(&exp_result);

#endif

	//char luafile[50];
	char username[50];
	char password[50];

	int opt_username=FALSE;
	int opt_password=FALSE;
	int opt_script=FALSE;
    int opt_gmode=FALSE;

#ifdef __EMSCRIPTEN__
    strcpy(app_path,"");
#else
    strcpy(app_path,dirname(argv[0]));
#endif

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
#elif TARGET_OS_MAC
    opt_gmode=TRUE;
    opt_remoteconsole=TRUE;

    char basedir_tmp[PATH_MAX];
    strcpy(basedir_tmp,config.basedir);
    strcpy(config.basedir,app_path);
    strcat(config.basedir,"/../Resources/");
    //strcpy(config.basedir,"~/Library/Preferences/Proteo/");
    strcat(config.basedir,basedir_tmp);


    debug=TRUE;
    verbose=FALSE;
#elif TARGET_OS_WINDOWS
    opt_gmode=TRUE;
    opt_remoteconsole=TRUE;

#elif __EMSCRIPTEN__
	opt_gmode=TRUE;

    debug=TRUE;
    verbose=FALSE;

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
            case 'e': //Remote Console
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

#ifdef LUAJIT_VERSION
    printf("%s\n",LUAJIT_VERSION);
#else
    printf("%s\n",LUA_VERSION);
#endif


	//if(opt_appkey==FALSE) return 0;
#ifdef PROTEO_ENET
    enet_initialize();
#endif
	if( initLUA() == FALSE)
	{
		printf( "Failed to initialize LUA!\n" );
		return 1;
	}
#ifdef LUA_JIT
    lua_pushinteger(L, 1);
#else
    lua_pushinteger(L, 0);
#endif
    lua_setglobal(L,  "LUAJIT");

#ifdef __EMSCRIPTEN__
    lua_pushinteger(L, 1);
#else
    lua_pushinteger(L, 0);
#endif
    lua_setglobal(L,  "EMSCRIPTEN");

    lua_pushboolean(L, debug);
    lua_setglobal(L,  "DEBUG");

    lua_pushboolean(L, verbose);
    lua_setglobal(L,  "VERBOSE");

	if( initSDL() == FALSE)
	{
		printf( "Failed to initialize SDL!\n" );
		return 2;
	}

#ifdef USEOPENGL
    printf("GLEW Version: %s \n",glewGetString(GLEW_VERSION));
    printf("Shader Version: %s \n",glGetString(GL_SHADING_LANGUAGE_VERSION));
    lua_pushinteger(L, 1);
#else
    lua_pushinteger(L, 0);
#endif
    lua_setglobal(L,"SHADER");

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
            proteo_login(username,password,"8756c442-3e57-4754-b3b2-0a82b7b5d295");
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
    //emscripten_set_main_loop_timing(EM_TIMING_RAF,2);
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
