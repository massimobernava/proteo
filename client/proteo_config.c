
//==============================================================================
//   CONFIG
//==============================================================================

typedef struct conf
{
	char version[10];
	//char server[64];
	int ssl;

    char username[50];
    char password[50];
    char script[50];

    char basedir[256];
    char server[256];

    int load_error;

} conf;

conf config;

//TODO
conf default_config()
{
    conf _config;

    strcpy(_config.basedir,"./");
    strcpy(_config.version,PROTEO_VERSION);
    strcpy(_config.server,"http://localhost:8888");
    _config.ssl=0;

#ifdef __EMSCRIPTEN__
    //Nella versione web si può scrivere un config parziale dentro il js (default_config), se non presente chiede il server
    char* server=(char*)EM_ASM_INT(

        var jsString = default_config.server;

        if(jsString==null)
            jsString=window.prompt('Server: ');


        var lengthBytes = lengthBytesUTF8(jsString)+1;
        var stringOnWasmHeap = _malloc(lengthBytes);
        stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
        return stringOnWasmHeap;
    );
    strcpy(_config.server,server);
    free(server);
#endif

	if(verbose) printf("Default config server: %s\n",_config.server);

    return _config;
}

conf load_config()
{
    conf _config;

#ifdef __EMSCRIPTEN__
	char* json_config=(char*)EM_ASM_INT(
		var the_cookie = document.cookie.split(';');
		if (the_cookie[0]) {
			var jsString = unescape(the_cookie[0]);

			var lengthBytes = lengthBytesUTF8(jsString)+1;
			var stringOnWasmHeap = _malloc(lengthBytes);
			stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
			return stringOnWasmHeap;
		}
	);
#else
#if TARGET_OS_IPHONE
    char path_config[256];
    char *home = getenv("HOME");
    strcpy(path_config,home);
    strcat(path_config,"/Documents/config.json");
#else
	char *path_config="./config.json";
#endif
    if(verbose) printf("LOAD CONFIG: %s\n",path_config);
	char* json_config=loadfile(path_config);
#endif

    if(verbose) printf("CONFIG: %s\n",json_config);

	if(json_config==NULL)
    {
        _config.load_error=2;
        return _config;
    }

#ifdef PROTEO_USE_TINYJSON
    json_t mem[32];
    json_t const* json = json_create( json_config, mem, sizeof mem / sizeof *mem );

    if(json==NULL)
    {
        _config.load_error=1;
        return _config;
    }

    json_t const* ver = json_getProperty( json, "version" );
    if ( !ver || JSON_TEXT != json_getType( ver ) ) printf("Error, the version property is not found.");
    else
        strcpy(_config.version,json_getValue(ver));

    json_t const* username = json_getProperty( json, "username" );
    if ( !username || JSON_TEXT != json_getType( username ) ) printf("Error, the username property is not found.");
    else
        strcpy(_config.username,json_getValue(username));

    json_t const* password = json_getProperty( json, "password" );
    if ( !password || JSON_TEXT != json_getType( password ) ) printf("Error, the password property is not found.");
    else
        strcpy(_config.password,json_getValue(password));

    json_t const* script = json_getProperty( json, "script" );
    if ( !script || JSON_TEXT != json_getType( script ) ) printf("Error, the script property is not found.");
    else
        strcpy(_config.script,json_getValue(script));

    json_t const* basedir = json_getProperty( json, "basedir" );
    if ( !basedir || JSON_TEXT != json_getType( basedir ) ) printf("Error, the basedir property is not found.");
    else
        strcpy(_config.basedir,json_getValue(basedir));

    json_t const* server = json_getProperty( json, "server" );
    if ( !server || JSON_TEXT != json_getType( server ) ) printf("Error, the server property is not found.");
    else
        strcpy(_config.server,json_getValue(server));

    json_t const* ssl = json_getProperty( json, "ssl" );
    if ( !ssl || JSON_INTEGER != json_getType( ssl ) ) printf("Error, the ssl property is not found.");
    else
        _config.ssl=(int)json_getInteger(ssl);
#else
	//========
    json_object * jobj = json_tokener_parse(json_config);
	if(jobj==NULL)
	{
		_config.load_error=1;
        return _config;
	}
	//json_parse(jobj);
	struct json_object *obj;

	json_object_object_get_ex(jobj, "version", &obj);
	const char* ver=json_object_get_string(obj);
	if(ver!=NULL) strcpy(_config.version,ver);

    json_object_object_get_ex(jobj, "username", &obj);
    const char* username=json_object_get_string(obj);
    if(username!=NULL) strcpy(_config.username,username);

    json_object_object_get_ex(jobj, "password", &obj);
    const char* password=json_object_get_string(obj);
    if(password!=NULL) strcpy(_config.password,password);

    json_object_object_get_ex(jobj, "script", &obj);
    const char* sc=json_object_get_string(obj);
    if(sc!=NULL) strcpy(_config.script,sc);

    json_object_object_get_ex(jobj, "basedir", &obj);
    const char* basedir=json_object_get_string(obj);
    if(basedir!=NULL) strcpy(_config.basedir,basedir);

	json_object_object_get_ex(jobj, "server", &obj);
	const char* server=json_object_get_string(obj);
	if(server!=NULL) strcpy(_config.server,server);

	json_object_object_get_ex(jobj, "ssl", &obj);
	_config.ssl=json_object_get_int(obj);

	if(jobj!=NULL)json_object_put(jobj);
#endif

	free(json_config);

	if(verbose) printf("Load config file!\n");

    _config.load_error=0;
    return _config;
}

int save_config(conf _config)
{
	#ifdef __EMSCRIPTEN__
	char json_config[512];
	sprintf(json_config,"{"
              "\"version\":\"%s\","
              "\"basedir\":\"%s\","
              "\"server\":\"%s\","
              "\"username\":\"%s\","
              "\"password\":\"%s\","
              "\"script\":\"%s\","
                    "\"ssl\":%d"
              "}",_config.version,_config.basedir,_config.server,
            _config.username,_config.password,_config.script,_config.ssl);
	EM_ASM({
		var d = new Date(2021, 01, 01);
        var p = '/';
        document.cookie = escape(UTF8ToString($0))
                          + ';path=' + p
                          + ';expires=' + d.toUTCString();
                          },json_config);
    #else
    #if TARGET_OS_IPHONE
        char path_config[256];
        char *home = getenv("HOME");
        strcpy(path_config,home);
        strcat(path_config,"/Documents/config.json");
    #else
        char *path_config="./config.json";
    #endif

    FILE *f = fopen(path_config, "w");
    if (f == NULL)
    {
        return 1;
    }
    fprintf(f,    "{"
                    "\"version\":\"%s\","
                    "\"basedir\":\"%s\","
                    "\"server\":\"%s\","
            "\"username\":\"%s\","
            "\"password\":\"%s\","
            "\"script\":\"%s\","
                    "\"ssl\":%d"
            "}",_config.version,_config.basedir,_config.server,
            _config.username,_config.password,_config.script,_config.ssl);
    fclose(f);
    #endif
    return 0;
}

//==============================================================================
//   LUA
//==============================================================================

static int system_loadConfig (lua_State *state) {

    conf _config = load_config();

    if(_config.load_error!=0)
    {
        printf("Error Load Config!\n");
        /*strcpy(_config.server,"http://localhost:8888");
        strcpy(_config.basedir,"./");
        strcpy(_config.version,PROTEO_VERSION);
        _config.ssl=0;*/

        _config=default_config();
    }

    lua_newtable(state);

    lua_pushstring(state, _config.server);
    lua_setfield(state, -2, "server");

    lua_pushstring(state, _config.basedir);
    lua_setfield(state, -2, "basedir");

    lua_pushstring(state, _config.username);
    lua_setfield(state, -2, "username");

    lua_pushstring(state, _config.password);
    lua_setfield(state, -2, "password");

    lua_pushstring(state, _config.script);
    lua_setfield(state, -2, "script");

    lua_pushstring(state, _config.version);
    lua_setfield(state, -2, "version");

    lua_pushinteger(state, _config.ssl);
    lua_setfield(state, -2, "ssl");

    return 1;
}

//La funzione salva la configurazione passata ma non la imposta come configurazione attuale
static int system_saveConfig (lua_State *state) {

    conf _config;

    lua_pushvalue(state, 1);
    lua_pushnil(state);
    while (lua_next(state, -2))
    {
        lua_pushvalue(state, -2);
        const char *key = lua_tostring(state, -1);
        if (lua_isnumber(state, -2)==1)
        {
            double value = lua_tonumber(state, -2);
            if(strcmp(key,"ssl")==0) _config.ssl=(int)value;
            //printf("%s => %f\n", key, value);
        }
        else
        {
            const char *value = lua_tostring(state, -2);

            if(strcmp(key,"server")==0) strcpy(_config.server,value);
            else if(strcmp(key,"basedir")==0) strcpy(_config.basedir,value);
            else if(strcmp(key,"version")==0) strcpy(_config.version,value);
            else if(strcmp(key,"username")==0) strcpy(_config.username,value);
            else if(strcmp(key,"password")==0) strcpy(_config.password,value);
            else if(strcmp(key,"script")==0) strcpy(_config.script,value);

            //printf("%s => %s\n", key, value);
        }

        lua_pop(state, 2);
    }
    lua_pop(state, 1);

    save_config(_config);

    return 0;
}

//La funzione importa la configurazione attuale ma non la salva
static int system_setConfig (lua_State *state) {

    return 0;
}
