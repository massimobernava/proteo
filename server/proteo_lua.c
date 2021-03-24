//==============================================================================
//   LUA
//==============================================================================
lua_State *L=NULL;

static int traceback(lua_State *state) {
  /*lua_Debug ar;
  int level=1;
  //int top = lua_gettop(L);
  luaL_checkstack(state, 10, NULL);
  printf("proteo stack traceback:");
  while (lua_getstack(state, level++, &ar)) {
    lua_getinfo(state, "Sln", &ar);
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
    
    //lua_pushstring(L, "ERROR!!!");
    
    return 1;
}

sem_t* lua_sem;
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

int currentline(lua_State *L) {
    lua_Debug ar;
    lua_getstack(L, 1, &ar);
    lua_getinfo(L, "nSl", &ar);
    int line = ar.currentline;
    
    return line;
}

void init(lua_State *state,const char* app)  //chiamato da initLUA
{
  char path[100];
  snprintf(path,100,"%splugin/%s.lua",config.basedir,app);

  int error = luaL_dofile(state,path);

  if (error) {
    fprintf(stderr, "ERROR dofile(%s): %s\n",path, lua_tostring(state, -1));
    lua_pop(state, 1);
  }
}

//==============================================================================
//   CLOSELUA
//==============================================================================
void closeLUA()
{
    lua_close(L);
    sem_close(lua_sem);
}
