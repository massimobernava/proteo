
void freetimers()
{
    if(debug)printf("Free timers\n");
    while(timers!=NULL)
    {
        if(timers->callback!=NULL) free(timers->callback);
      
        ProteoTimer* tmp=(ProteoTimer*)timers->next;

        free(timers);
        timers=tmp;
    }
}

//==============================================================================
//   SYSTEM
//==============================================================================

static int system_system(lua_State *state){
    const char* cmd=luaL_checkstring(state,1);
    
    system(cmd);
    
    return 0;
}

static int system_console (lua_State *state) {
    if(!opt_remoteconsole)
        printf("%s\n",luaL_checkstring(state,1));
    else
    {
        char timestamp[100];
        time_t now = time (0);
        strftime (timestamp, 100, "%Y-%m-%d %H:%M:%S", localtime (&now));
        
        char json[512];
        sprintf(json,"{"
                  "\"console\":\"%s\","
                  "\"timestamp\":\"%s\""
                  "}",luaL_checkstring(state,1),timestamp);
        printf("%s\n",json);
        proteo_post("/proteo/console",PROTEO_APP_KEY,Token,json,NULL,-1);
        
    }
  return 0;
}

static int system_clock (lua_State *state) {
  if(verbose) printf("system.clock\n");

  //char timestamp[100];
  //time_t now = time (0);
  //strftime (timestamp, 100, "%Y-%m-%d %H:%M:%S", localtime (&now));
  //lua_pushlstring(state,timestamp,sizeof(timestamp));

	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);

	double n=now.tv_sec+(now.tv_nsec/1000000000.0);
	lua_pushnumber (state,n);

  return 1;
}

static int system_fileexist(lua_State *state) {
	const char* fname=luaL_checkstring(state,1);
    int exist=access( fname, F_OK ) != -1;
    if(debug) printf("system.fileExist: %s %s\n",fname,exist==1?"YES":"NO");
	lua_pushboolean (state, exist);
	return 1;
}

static int system_document(lua_State *state) {
  if(debug) printf("system.document\n");
  //lua_pushliteral(state,"./");
    lua_pushlstring(state,config.basedir,strlen(config.basedir));
  return 1;
}

static int system_writeFile(lua_State *state) {
    
    const char* fname=luaL_checkstring(state,1);
    const char* data=luaL_checkstring(state,2);
    
    if(verbose) printf("system.writeFile: %s\n",fname);
    if(debug) printf("%s\n",data);
    
    int ret=writefile(fname,(char*)data);
    
    lua_pushinteger(state, ret);
    
  return 1;
}

static int system_readFile(lua_State *state) {

 	const char* fname=luaL_checkstring(state,1);
    const char* callback=NULL;//luaL_checkstring(state,2);

    int ref_callback=-1;
    if (lua_isstring(state,2)==1)
        callback=luaL_checkstring(state,2);
    else
        ref_callback=lua_ref(state, TRUE);
    
	if(verbose) printf("system.readFile: %s\n",fname);

	char *data = loadfile((char*)fname);
	if(verbose) printf("%s\n",data);
        
	//lua_getglobal(state,callback);
    if(callback!=NULL)
        lua_getglobal(state,callback);
    else if(ref_callback!=-1)
        lua_getref(state,ref_callback);
    
    lua_pushlstring(state,data,strlen(data));
    lua_pushlstring(state,fname,strlen(fname));
    int error = lua_pcall(state, 2, 0, 0);
    
    free(data);

    if (error) {
        fprintf(stderr, "ERROR pcall(%s): %s\n",callback, lua_tostring(L, -1));
        lua_pop(L, 1);
    }

  return 0;
}

static int system_login(lua_State *state) {

    const char* username=luaL_checkstring(state,1);
    const char* password=luaL_checkstring(state,2);
    const char* new_script=luaL_checkstring(state,3);
    strcpy(script,new_script);
    if(debug) printf("system.login: %s\n",username);
    
    //TODO clear lua state
    /*
    freetextures();
    freefonts();
    freestreams();
    freesounds();
    freeaccesspoint();*/
    freetimers();
    
    //lua_close(state);
    //initLUA();
    //state=L;
    //setLuaScreenSize(state);
    lua_pushinteger(state, NULL);
    lua_setglobal(state, "touch");
    
    lua_pushnil(state);
    lua_setglobal(state, "close");
    
    freeskeletons();
    
    ProteoComponent* current=components;
    while(current!=NULL)
    {
        current->type=Deleted;
        current=current->next;
    }
    
    lua_pushlstring(state,username,strlen(username));
    lua_setglobal(state, "USERNAME");
    
    //TODO sarebbe utile avere un proteo_login che accetta la password già MD5 in modo da poterla salvare non in chiaro
    proteo_login(username, password, new_script);

  return 0;
}

static int system_getNode(lua_State *state) {
 	const char* id=luaL_checkstring(state,1);
 	if(debug) printf("system.getNode: %s\n",id);

 	ProteoComponent* current=components;
	
	while(current!=NULL)
	{
		//printf("search id:%s\n",current->id);
		if(strcmp(current->id,id)==0)
		{
			//printf("found id:%s\n",id);
			lua_pushlightuserdata (state,current);
			break;
		}
		current=(ProteoComponent*)current->next;
	}
  return 1;
}

static int system_getName(lua_State *state) {
	ProteoComponent* pc=toProteoComponent(state,1);
  	if(debug) printf("system.getName\n");

  	lua_pushlstring(state,pc->id,strlen(pc->id));
 	return 1;
}

static int system_getPlatform(lua_State *state){
    lua_pushstring(state,SDL_GetPlatform());
    return 1;
}

static int system_basename(lua_State *state){
    const char* path=luaL_checkstring(state,1);
    lua_pushstring(state,basename(path));
    return 1;
}

static int system_dir(lua_State *state){
    const char* path=luaL_checkstring(state,1);
    
    if(debug) printf("system.dir: %s\n",path);
    
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if (d)
    {
        lua_newtable(state);
        int i=1;
        while ((dir = readdir(d)) != NULL)
        {
            lua_pushinteger(state, i);
            lua_pushstring(state, dir->d_name);
            lua_settable(state,-3);
            i++;
        }

      closedir(d);
    }

    
    return 1;
}

static int system_isdir(lua_State *state){
    const char* path=luaL_checkstring(state,1);
    
    struct stat sb;

    if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
        lua_pushboolean(state,TRUE);
    else
        lua_pushboolean(state,FALSE);
    
    return 1;
}

//==============================================================================
//   TIMER
//==============================================================================

static ProteoTimer *checkProteoTimer (lua_State *L, int index)
{
	ProteoTimer *pt=NULL;
    if(lua_islightuserdata(L, index))
    {
        pt=toProteoTimer(L,index);
    }
    else luaL_typerror(L, index, PROTEOTIMER);
    return pt;
}

static ProteoTimer *pushProteoTimer (lua_State *L)
{
    ProteoTimer *pt =malloc(sizeof(ProteoTimer));
    lua_pushlightuserdata(L, pt);
    
    if(pt==NULL) printf("pushProteoTimer Error: %d\n",currentline(L));
	return pt;
}

static ProteoTimer *toProteoTimer (lua_State *L, int index)
{
	ProteoTimer *pt = (ProteoTimer *)lua_touserdata(L, index);
	if (pt == NULL) luaL_typerror(L, index, PROTEOTIMER);
	return pt;
}

static int system_createTimer(lua_State *state) {
	
	double time=lua_tonumber(state,1);
	char* callback=NULL;
	int ref_callback=-1;
	
	if (lua_isstring(state,2)==1)
        callback=(char*)luaL_checkstring(state,2);
    else
        ref_callback=lua_ref(state, TRUE);
        
	ProteoTimer* pt=pushProteoTimer(state);
	
    pt->time=time;
    //pt->callback=callback;
    if(callback!=NULL)
    {
        pt->callback=malloc(strlen(callback)+1);
        strlcpy(pt->callback,callback,strlen(callback)+1);
    } else pt->callback=NULL;
    pt->ref_callback=ref_callback;
    pt->state=0; //stop
    pt->next=NULL;
    
    struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	double n=now.tv_sec*1000;
	n=n+(now.tv_nsec/1000000.0);
	pt->last=n;
    
	if(timers==NULL)
	{
		timers=pt;
	}
	else
	{
		ProteoTimer* current=timers;
		while(current->next!=NULL) current=(ProteoTimer*)current->next;
		current->next=pt;
	}
  
	return 1;
}

static int system_startTimer(lua_State *state) {

	ProteoTimer* pt=toProteoTimer(state,1);
	
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	double n=now.tv_sec*1000;
	n=n+(now.tv_nsec/1000000.0);
	pt->last=n;
	
	pt->state=1;
	
	
	return 0;
}

static int system_stopTimer(lua_State *state) {

	ProteoTimer* pt=toProteoTimer(state,1);
	pt->state=0;
	return 0;
}

//==============================================================================
//   LUA
//==============================================================================

void add_system_proteo(lua_State* state)
{
    addFunction_proteo(state,"system","console",system_console);
    
    addFunction_proteo(state,"system","fileExist",system_fileexist);
    addFunction_proteo(state,"system","document",system_document);
    addFunction_proteo(state,"system","readFile",system_readFile);
    addFunction_proteo(state,"system","writeFile",system_writeFile);
    addFunction_proteo(state,"system","dir",system_dir);
    addFunction_proteo(state,"system","isdir",system_isdir);
    addFunction_proteo(state,"system","basename",system_basename);
    
    addFunction_proteo(state,"system","getNode",system_getNode);
    addFunction_proteo(state,"system","getName",system_getName);
    addFunction_proteo(state,"system","login",system_login);
    addFunction_proteo(state,"system","clock",system_clock);
    
    addFunction_proteo(state,"system","loadConfig",system_loadConfig);
    addFunction_proteo(state,"system","saveConfig",system_saveConfig);
    addFunction_proteo(state,"system","setConfig",system_setConfig);
    
    addFunction_proteo(state,"system","getPlatform",system_getPlatform);

    addFunction_proteo(state,"system","createTimer",system_createTimer);
	addFunction_proteo(state,"system","startTimer",system_startTimer);
	addFunction_proteo(state,"system","stopTimer",system_stopTimer);
    
    addFunction_proteo(state,"system","system",system_system);
    
    addTable_proteo(state,"system","key");
    addEnum_proteo(state,"system","key","LEFT",SDLK_LEFT);
    addEnum_proteo(state,"system","key","RIGHT",SDLK_RIGHT);
    addEnum_proteo(state,"system","key","UP",SDLK_UP);
    addEnum_proteo(state,"system","key","DOWN",SDLK_DOWN);
}
