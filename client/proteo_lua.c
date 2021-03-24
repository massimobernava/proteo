//==============================================================================
//   LUA
//==============================================================================
lua_State *L=NULL;
int local_lua=FALSE;
int remote_lua=FALSE;
char luascript[256];
char script[50];

//==============================================================================
//   LIBS
//==============================================================================

ProteoLib* findLib(const char* name)
{
	if(debug) printf("Search lib: %s\n",name);
    ProteoLib* current=libs;
    while(current!=NULL){
        if(strcmp(current->name,name)==0)
        {
            if(verbose) printf("Found lib: %s\n",name);
            return current;
        }
        current=current->next;
    }
    
    return NULL;
}

ProteoLib* addLib(const char* name,const char* data)
{
    ProteoLib* lib=malloc(sizeof(ProteoLib));
    strlcpy(lib->name,name,PROTEO_MAX_ID);
    lib->lib=malloc(strlen(data)+1);
    strlcpy(lib->lib, data, strlen(data)+1);
    lib->next=NULL;
    
    if(libs==NULL)
    {
        libs=lib;
    }
    else
    {
        ProteoLib* current=libs;
        while(current->next!=NULL) current=(ProteoLib*)current->next;
        current->next=lib;
    }
    
    return lib;
}

/*ProteoLib* newLib(const char* name)
{
    ProteoLib* tmp=findLib(name);
    if(tmp!=NULL) return tmp;

    return addLib(name,data);
}*/

void freelibs()
{
    if(debug)printf("Free libs\n");
    
    while(libs!=NULL)
    {
      ProteoLib* tmp=(ProteoLib*)libs->next;

      
      if(libs->lib!=NULL) free(libs->lib);
      free(libs);
      libs=tmp;
    }
}

static ProteoComponent *checkProteoComponent (lua_State *L, int index)
{
#ifdef LIGHTPROTEOCOMPONENT
    ProteoComponent *pc=NULL;
    if(lua_islightuserdata(L, index))
    {
        pc=toProteoComponent(L,index);
    }
    else
        luaL_typerror(L, index, PROTEOCOMPONENT);
    return pc;
#else
  ProteoComponent *pc;
  luaL_checktype(L, index, LUA_TUSERDATA);
  pc = (ProteoComponent *)luaL_checkudata(L, index, PROTEOCOMPONENT);
  if (pc == NULL) luaL_typerror(L, index, PROTEOCOMPONENT);
  return pc;
#endif
}

static int component_gc(lua_State *l)
{
    ProteoComponent* host = checkProteoComponent(l, 1);
    
    if(verbose) printf("GC Component(%d): %s\n",host->type,host->id);
    host->type=Deleted;
    
    return 0;
}

static ProteoComponent *pushProteoComponent (lua_State *L)
{
#ifdef LIGHTPROTEOCOMPONENT
    ProteoComponent *pc =malloc(sizeof(ProteoComponent));
    lua_pushlightuserdata(L, pc);
#else
    ProteoComponent *pc = (ProteoComponent *)lua_newuserdata(L, sizeof(ProteoComponent));
    //luaL_getmetatable(L, PROTEOCOMPONENT);
    if(luaL_newmetatable(L, PROTEOCOMPONENT)){
        lua_pushcfunction(L, component_gc);
        lua_setfield(L, -2, "__gc");
    }
    lua_setmetatable(L, -2);
#endif
    if(pc==NULL) printf("pushProteoComponent Error: %d\n",currentline(L));
    
  return pc;
}

static ProteoComponent *toProteoComponent (lua_State *L, int index)
{
    ProteoComponent *pc = (ProteoComponent *)lua_touserdata(L, index);
    //if (pc == NULL)
    //    luaL_typerror(L, index, PROTEOCOMPONENT);
    return pc;
}

int currentline(lua_State *L) {
    lua_Debug ar;
    lua_getstack(L, 1, &ar);
    lua_getinfo(L, "nSl", &ar);
    int line = ar.currentline;
    
    return line;
}

static int traceback(lua_State *L)
{
    /*lua_Debug ar;
    int level=1;
    //int top = lua_gettop(L);
    luaL_checkstack(L, 10, NULL);
    printf("proteo stack traceback:");
    while (lua_getstack(L, level++, &ar)) {
        lua_getinfo(L, "Sln", &ar);
        printf("\n\t%s:", ar.short_src);
        printf("%d: in ", ar.currentline);
        if (*ar.namewhat != '\0')
            printf("%s '%s'", ar.namewhat, ar.name);
        else if (*ar.what == 'm')
            printf("main chunk");
        else if (*ar.what != 'C')
            printf("function <%s:%d>", ar.short_src, ar.linedefined);
        //lua_concat(L, lua_gettop(L) - top);
        printf("\n");
    }*/

    lua_Debug ar;
    lua_getstack(L, 1, &ar);
    lua_getinfo(L, "nSl", &ar);

    printf("=======\n Src: %s\n What: %s \n Name: %s \n Line:%d\n=======\n",ar.short_src,ar.what,ar.name,ar.currentline);
    
    //TODO?
    //const char *lua_getlocal (lua_State *L, lua_Debug *ar, int n);
    
    //lua_pushstring(L, "ERROR!!!");
    
    return 1;
}

int lua_trace_pcall( lua_State* state, int nargs, int nret ) {

    //sem_wait(lua_sem);
  int hpos = lua_gettop( state ) - nargs;
  int ret = 0;

  lua_pushcfunction( state, traceback );

  lua_insert( state, hpos );

  ret = lua_pcall( state, nargs, nret, hpos );

  lua_remove( state, hpos );
    //sem_post(lua_sem);
    
  return ret;
}

int setLuaScreenSize(lua_State *state)
{
#if TARGET_OS_IPHONE
    SDL_DisplayMode displayMode;
    if( SDL_GetCurrentDisplayMode( 0, &displayMode ) == 0 )
    {
        gWidth = displayMode.w;
        gHeight = displayMode.h;
    }
#endif
    
    lua_pushinteger(state, gWidth);
    lua_setglobal(state, "MAX_X");

    lua_pushinteger(state, gWidth/2);
    lua_setglobal(state,  "MID_X");

    lua_pushinteger(state, 0);
    lua_setglobal(state,  "MIN_X");

    lua_pushinteger(state, gHeight);
    lua_setglobal(state,  "MAX_Y");

    lua_pushinteger(state, gHeight/2);
    lua_setglobal(state,  "MID_Y");

    lua_pushinteger(state, 0);
    lua_setglobal(state, "MIN_Y");
    
    lua_pushinteger(state, -1);
    lua_setglobal(state, "TOP");
    
    lua_pushinteger(state, -2);
    lua_setglobal(state, "BOTTOM");
    
    return 0;
}

//==============================================================================
//   CLOSELUA
//==============================================================================
void closeLUA()
{
	lua_close(L);
    freelibs();
}
