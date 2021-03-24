
//==============================================================================
//   SDL
//==============================================================================

SDL_Window* gWindow = NULL;
//SDL_Surface* gScreenSurface = NULL;
SDL_Renderer* gRenderer = NULL;
float gScale=1.0f;

ProteoColor backgroundColor={255,0,0,0};
int gWidth=SCREEN_WIDTH;
int gHeight=SCREEN_HEIGHT;

int HandleAppEvents(void *userdata, SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_APP_TERMINATING:
        /* Terminate the app.
           Shut everything down before returning from this function.
        */
             if(debug) printf("APP_EVENT SDL_APP_TERMINATING\n");

        return 0;
    case SDL_APP_LOWMEMORY:
        /* You will get this when your app is paused and iOS wants more memory.
           Release as much memory as possible.
        */
            if(debug) printf("APP_EVENT SDL_APP_LOWMEMORY\n");
        return 0;
    case SDL_APP_WILLENTERBACKGROUND:
        /* Prepare your app to go into the background.  Stop loops, etc.
           This gets called when the user hits the home button, or gets a call.
        */
            if(debug) printf("APP_EVENT SDL_APP_WILLENTERBACKGROUND\n");
        return 0;
    case SDL_APP_DIDENTERBACKGROUND:
        /* This will get called if the user accepted whatever sent your app to the background.
           If the user got a phone call and canceled it, you'll instead get an SDL_APP_DIDENTERFOREGROUND event and restart your loops.
           When you get this, you have 5 seconds to save all your state or the app will be terminated.
           Your app is NOT active at this point.
        */
            if(debug) printf("APP_EVENT SDL_APP_DIDENTERBACKGROUND\n");
        return 0;
    case SDL_APP_WILLENTERFOREGROUND:
        /* This call happens when your app is coming back to the foreground.
           Restore all your state here.
        */
            if(debug) printf("APP_EVENT SDL_APP_WILLENTERFOREGROUND\n");
        return 0;
    case SDL_APP_DIDENTERFOREGROUND:
        /* Restart your loops here.
           Your app is interactive and getting CPU again.
        */
            if(debug) printf("APP_EVENT SDL_APP_DIDENTERFOREGROUND\n");
        return 0;
    default:
        /* No special processing, add it to the event queue */
             if(debug) printf("APP_EVENT type: %d\n",event->type);
        return 1;
    }
}

#ifdef __EMSCRIPTEN__

const char *emscripten_result_to_string(EMSCRIPTEN_RESULT result) {
  if (result == EMSCRIPTEN_RESULT_SUCCESS) return "EMSCRIPTEN_RESULT_SUCCESS";
  if (result == EMSCRIPTEN_RESULT_DEFERRED) return "EMSCRIPTEN_RESULT_DEFERRED";
  if (result == EMSCRIPTEN_RESULT_NOT_SUPPORTED) return "EMSCRIPTEN_RESULT_NOT_SUPPORTED";
  if (result == EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED) return "EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED";
  if (result == EMSCRIPTEN_RESULT_INVALID_TARGET) return "EMSCRIPTEN_RESULT_INVALID_TARGET";
  if (result == EMSCRIPTEN_RESULT_UNKNOWN_TARGET) return "EMSCRIPTEN_RESULT_UNKNOWN_TARGET";
  if (result == EMSCRIPTEN_RESULT_INVALID_PARAM) return "EMSCRIPTEN_RESULT_INVALID_PARAM";
  if (result == EMSCRIPTEN_RESULT_FAILED) return "EMSCRIPTEN_RESULT_FAILED";
  if (result == EMSCRIPTEN_RESULT_NO_DATA) return "EMSCRIPTEN_RESULT_NO_DATA";
  return "Unknown EMSCRIPTEN_RESULT!";
}

#define TEST_RESULT(x) if (ret != EMSCRIPTEN_RESULT_SUCCESS) printf("%s returned %s.\n", #x, emscripten_result_to_string(ret));


static inline const char *emscripten_event_type_to_string(int eventType) {
  const char *events[] = { "(invalid)", "(none)", "keypress", "keydown", "keyup", "click", "mousedown", "mouseup", "dblclick", "mousemove", "wheel", "resize",
    "scroll", "blur", "focus", "focusin", "focusout", "deviceorientation", "devicemotion", "orientationchange", "fullscreenchange", "pointerlockchange",
    "visibilitychange", "touchstart", "touchend", "touchmove", "touchcancel", "gamepadconnected", "gamepaddisconnected", "beforeunload",
    "batterychargingchange", "batterylevelchange", "webglcontextlost", "webglcontextrestored", "mouseenter", "mouseleave", "mouseover", "mouseout", "(invalid)" };
  ++eventType;
  if (eventType < 0) eventType = 0;
  if (eventType >= sizeof(events)/sizeof(events[0])) eventType = sizeof(events)/sizeof(events[0])-1;
  return events[eventType];
}

EM_BOOL uievent_callback(int eventType, const EmscriptenUiEvent *e, void *userData)
{
  /*printf("%s, detail: %ld, document.body.client size: (%d,%d), window.inner size: (%d,%d), scrollPos: (%d, %d)\n",
    emscripten_event_type_to_string(eventType), e->detail, e->documentBodyClientWidth, e->documentBodyClientHeight,
    e->windowInnerWidth, e->windowInnerHeight, e->scrollTop, e->scrollLeft);
*/

	//int w = SCREEN_WIDTH;
	//int h = SCREEN_HEIGHT;
	float windowWidth  = (float)e->documentBodyClientWidth;//e->windowInnerWidth;
	float windowHeight = (float)e->documentBodyClientHeight;//e->windowInnerHeight;
	//float r = 1.0f;
	if((windowWidth/(float)e->windowInnerHeight) >= RATIO)
	{
		gWidth = windowHeight * RATIO;
		gHeight = windowHeight;
		gScale = gHeight/(float)SCREEN_HEIGHT;
	}
	else
	{
		gWidth = windowWidth;
		gHeight = windowWidth / RATIO;
		gScale = gWidth/(float)SCREEN_WIDTH;
	}

	if (gWidth>SCREEN_WIDTH || gHeight > SCREEN_HEIGHT)
	{
		gWidth = SCREEN_WIDTH;
		gHeight = SCREEN_HEIGHT;
		gScale = 1.0f;
	}

	SDL_SetWindowSize(gWindow,gWidth,gHeight);
	SDL_RenderSetScale(gRenderer,gScale,gScale);

  return 0;
}
#endif

//==============================================================================
//   INITSDL
//==============================================================================
int initSDL()
{
	//Initialization flag
	int success = TRUE;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = FALSE;
	}
	else
	{
		if(verbose) printf( "SDL initialized!\n");
#if TARGET_OS_IPHONE
        SDL_SetEventFilter(HandleAppEvents, NULL);
 #endif

		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        
        //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetSwapInterval(1);

		//Create window
#if TARGET_OS_IPHONE
        gWindow = SDL_CreateWindow( NULL, 0, 0, 0, 0, SDL_WINDOW_SHOWN| SDL_WINDOW_BORDERLESS );
#else
		gWindow = SDL_CreateWindow( "Proteo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN/*|SDL_WINDOW_RESIZABLE*/ );
		SDL_SetWindowResizable(gWindow,TRUE);
#endif


		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = FALSE;
		}
		else
		{
			//Get window surface
			//gScreenSurface = SDL_GetWindowSurface( gWindow );

            gRenderer =  SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
			SDL_SetRenderDrawBlendMode(gRenderer,SDL_BLENDMODE_BLEND);

		}

		if(TTF_Init()==-1) {
    		printf("TTF_Init error: %s\n", TTF_GetError());
    		success = FALSE;
		}

#ifdef __EMSCRIPTEN__
		EMSCRIPTEN_RESULT ret = emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, uievent_callback);
		//TEST_RESULT(emscripten_set_resize_callback);
#endif

	}

	return success;
}

//==============================================================================
//   CLOSESDL
//==============================================================================
void closeSDL()
{

	freecomponents();
    freetextures();
    freefonts();
    freeicons();
    freestreams();
    freesounds();
    freeskeletons();

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	TTF_Quit();
	SDL_Quit();
}
