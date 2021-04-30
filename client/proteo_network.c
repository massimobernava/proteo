//==============================================================================
//   NETWORK
//==============================================================================
char Token[1000];

typedef struct AccessPoint
{
	char app[32];
	char url[64];
	struct AccessPoint* next;
} AccessPoint;

struct AccessPoint* accesspoints=NULL;

void freeaccesspoint()
{
    if(debug)printf("Free accesspoints\n");

	while(accesspoints!=NULL)
 	{
    	AccessPoint* tmp=(AccessPoint*)accesspoints->next;


    	free(accesspoints);

    	accesspoints=tmp;
  	}
}

char* getaccesspoint(const char* app)
{
	AccessPoint* current=accesspoints;
	while(current!=NULL)
 	{
    	AccessPoint* tmp=(AccessPoint*)current->next;

    	if(strcmp(app,current->app)==0)
    	{
    		return current->url;
    	}

    	current=tmp;
  	}
	return NULL;
}

int addaccesspoint(const char* app, const char* url)
{
	AccessPoint* current=accesspoints;
	while(current!=NULL)
 	{
    	AccessPoint* tmp=(AccessPoint*)current->next;

    	if(strcmp(app,current->app)==0)
    	{
    		return 1;
    	}

    	current=tmp;
  	}
  	AccessPoint *newAP=(AccessPoint *) malloc(sizeof(AccessPoint));
  	strcpy(newAP->app,app);
  	strcpy(newAP->url,url);
  	newAP->next=accesspoints;
  	accesspoints=newAP;

	return 0;
}

struct MemoryStruct {
  char *memory;
  size_t size;
};


/*char *base64encode (const void *b64_encode_this, int encode_this_many_bytes){
    BIO *b64_bio, *mem_bio;      //Declares two OpenSSL BIOs: a base64 filter and a memory BIO.
    BUF_MEM *mem_bio_mem_ptr;    //Pointer to a "memory BIO" structure holding our base64 data.
    b64_bio = BIO_new(BIO_f_base64());                      //Initialize our base64 filter BIO.
    mem_bio = BIO_new(BIO_s_mem());                           //Initialize our memory sink BIO.
    BIO_push(b64_bio, mem_bio);            //Link the BIOs by creating a filter-sink BIO chain.
    BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);  //No newlines every 64 characters or less.
    BIO_write(b64_bio, b64_encode_this, encode_this_many_bytes); //Records base64 encoded data.
    BIO_flush(b64_bio);   //Flush data.  Necessary for b64 encoding, because of pad characters.
    BIO_get_mem_ptr(mem_bio, &mem_bio_mem_ptr);  //Store address of mem_bio's memory structure.
    BIO_set_close(mem_bio, BIO_NOCLOSE);   //Permit access to mem_ptr after BIOs are destroyed.
    BIO_free_all(b64_bio);  //Destroys all BIOs in chain, starting with b64 (i.e. the 1st one).
    //BIO_free_all(mem_bio);
    BUF_MEM_grow(mem_bio_mem_ptr, (*mem_bio_mem_ptr).length + 1);   //Makes space for end null.
    (*mem_bio_mem_ptr).data[(*mem_bio_mem_ptr).length] = '\0';  //Adds null-terminator to tail.
    return (*mem_bio_mem_ptr).data; //Returns base-64 encoded data. (See: "buf_mem_st" struct).
}*/

static size_t writeMemory_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

#ifndef PROTEO_USE_TINYJSON
json_object * table_to_json_object(int index)
{
	json_object * jobj = json_object_new_object();

    lua_pushvalue(L, index);
    lua_pushnil(L);
    while (lua_next(L, -2))
    {
        lua_pushvalue(L, -2);
        const char *key = lua_tostring(L, -1);
        /*if (lua_isnumber(L, -2)==1)
        {
			double value = lua_tonumber(L, -2);
			printf("%s => %f\n", key, value);
		}
		else
		{
			const char *value = lua_tostring(L, -2);
			printf("%s => %s\n", key, value);
		}*/

		const char *value = lua_tostring(L, -2);
		json_object *jstring = json_object_new_string(value);
		json_object_object_add(jobj,key, jstring);
		if(debug) printf("%s => %s\n", key, value);

        lua_pop(L, 2);
    }
    lua_pop(L, 1);
    return jobj;
}
#endif

char* table_to_json(int index)
{
	//TODO
    printf(">>>>>>>>>>>>>ERROR TODO<<<<<<<<<<<<<<<<<<<<<");
	return "";
}

#ifdef PROTEO_USE_TINYJSON
void json_to_table(json_t const * json)
{
    jsonType_t const type = json_getType( json );

    if(type==JSON_ARRAY)
    {
		lua_newtable(L);
        json_t const* child;
        int i=1;
        for( child = json_getChild( json ); child != 0; child = json_getSibling( child ) ) {
            if(verbose) printf("  [%d] \n",i);
            lua_pushinteger(L,i);
            json_to_table(child);
            lua_settable(L,-3);
            i++;
        }
    }
    else if(type==JSON_TEXT)
    {
        const char* svalue=json_getValue(json);
        if(verbose) printf(" string %s\n",svalue);
        lua_pushlstring(L,svalue,strlen(svalue));
    }
    else if(type==JSON_INTEGER)
    {
        int ivalue=json_getInteger(json);
        if(verbose) printf(" int %d\n",ivalue);
        lua_pushinteger(L,ivalue);
    }
    else if(type==JSON_BOOLEAN)
    {
        bool bvalue=json_getBoolean(json);
        if(verbose) printf(" bool %d\n",bvalue);
        lua_pushboolean(L,bvalue);
    }
    else if(type==JSON_NULL)
    {
        if(verbose) printf(" null\n");
        lua_pushnil(L);
    }
    else if(type==JSON_REAL)
    {
        double dvalue=json_getReal(json);
        if(verbose) printf(" real %f\n",dvalue);
        lua_pushnumber(L,dvalue);
    }
    else if(type==JSON_OBJ)
    {
		lua_newtable(L);
        json_t const* child;
        for( child = json_getChild( json ); child != 0; child = json_getSibling( child ) ) {
            char const* key = json_getName( child );
            if(verbose) printf("object %s -> ",key);
            json_to_table(child);
            lua_setfield(L, -2, key);
        }
    }
}
#endif

#ifndef PROTEO_USE_TINYJSON
void json_to_table/*json_object_to_table*/(json_object * jobj)
{
	enum json_type t = json_object_get_type(jobj);
	if(t==json_type_array)
	{
		lua_newtable(L);

		int arraylen = json_object_array_length(jobj);
		if(verbose) printf(" array %d\n",arraylen);
		int i;
		json_object * jvalue;
		for (i=0; i< arraylen; i++)
		{
			jvalue = json_object_array_get_idx(jobj, i);
			if(verbose) printf("  [%d] \n",i);
			lua_pushinteger(L,i+1);
			json_to_table(jvalue);//json_object_to_table(jvalue);
			lua_settable(L,-3);
		}
	}
	else if(t==json_type_string)
	{
		const char* svalue=json_object_get_string(jobj);
		if(verbose) printf(" string %s\n",svalue);
		lua_pushlstring(L,svalue,strlen(svalue));
	}
	else if(t==json_type_int)
	{
        int ivalue=json_object_get_int(jobj);
		if(verbose) printf(" int %d\n",ivalue);
        lua_pushinteger(L, ivalue);
	}
	else if(t==json_type_boolean)
	{
        json_bool bvalue=json_object_get_boolean(jobj);
		if(verbose) printf(" bool \n");
        lua_pushboolean(L, bvalue);
	}
	else if(t==json_type_double)
	{
        double dvalue=json_object_get_double(jobj);
        if(verbose) printf(" double %f\n",dvalue);
        lua_pushnumber(L, dvalue);
	}
	else if(t==json_type_null)
	{
		if(verbose) printf(" null \n");
	}
	else if(t==json_type_object)
	{
		lua_newtable(L);
		json_object_object_foreach(jobj, key, val) {

				if(verbose) printf("object %s -> \n",key);
				json_to_table(val);//json_object_to_table(val);
				lua_setfield(L, -2, key);
		}
	}
}
#endif

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

#ifdef __EMSCRIPTEN__
 void downloadSucceeded(emscripten_fetch_t *fetch)
{
  printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
  emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void downloadFailed(emscripten_fetch_t *fetch)
{
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
}

static int network_download(lua_State *state)
{
	const char* url = luaL_checkstring(state,1);
	const char* name = luaL_checkstring(state,2);
	const char* callback = luaL_checkstring(state,3);

	/*EM_ASM(
    alert('network_download!');
  );*/

  //TODO completamente da scrivere, vedi newFont

  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");
  //attr.userData callback + name
  attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_PERSIST_FILE;
  attr.onsuccess = downloadSucceeded;
  attr.onerror = downloadFailed;
  emscripten_fetch(&attr, "myfile.dat");

}
#else
static int network_download(lua_State *state) {
	const char* url = luaL_checkstring(state,1);
	const char* name = luaL_checkstring(state,2);
	const char* callback = luaL_checkstring(state,3);

	CURLcode ret;
	CURL *hnd;

  	hnd = curl_easy_init ();
  	curl_easy_setopt (hnd, CURLOPT_URL,url);
	curl_easy_setopt (hnd, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, write_data);

	mkpath((char*)name);

	FILE * newfile = fopen(name, "wb");
  	if(newfile) {
		curl_easy_setopt (hnd, CURLOPT_WRITEDATA, newfile);

		ret = curl_easy_perform (hnd);
    	fclose(newfile);
  	}
  	curl_easy_cleanup (hnd);
  	hnd = NULL;

	if(verbose) printf("Download: %s,%s,%s\n",url,name,callback);

	return 0;
}
#endif

#ifdef __EMSCRIPTEN__
static int network_getData(lua_State *state) {

	const char* path = luaL_checkstring(state,1);
    const char* callback = luaL_checkstring(state,2);


    return 1;
}
#else
static int network_getData(lua_State *state) {
    const char* url = luaL_checkstring(state,1);
    const char* callback = luaL_checkstring(state,2);

   // const char* url=concat3(config.server,"/",path);
    if(debug) printf("getData url: %s\n",url);
    CURLcode ret;
    CURL *hnd;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    hnd = curl_easy_init ();
    curl_easy_setopt (hnd, CURLOPT_URL,url);
    curl_easy_setopt (hnd, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, writeMemory_callback);
    curl_easy_setopt (hnd, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt (hnd, CURLOPT_SSL_VERIFYPEER, FALSE);

    ret = curl_easy_perform (hnd);
    //free(url);

    if(ret != CURLE_OK) {
        printf("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(ret));
    }
    else {
           if(debug) printf("%lu bytes retrieved\n", (unsigned long)chunk.size);

            lua_getglobal(state,callback);
            lua_pushlstring(state,url,strlen(url));
            lua_pushlstring(state,chunk.memory,chunk.size);

            int error = lua_pcall(state, 2, 0, 0);

            if (error) {
              fprintf(stderr, "ERROR pcall(getData): %s\n", lua_tostring(state, -1));
              lua_pop(state, 1);
            }
    }

    curl_easy_cleanup (hnd);
    free(chunk.memory);
   // free(url);

    return 1;
}
#endif

/*static int network_roomInit(lua_State *state) {
	const char* server=luaL_checkstring(state,1);

	printf("roomInit: %s\n",server);
	return 0;
}

static int network_roomConnect(lua_State *state) {
	const char* room=luaL_checkstring(state,1);
	const char* callback=luaL_checkstring(state,2);

	printf("roomConnect: %s,%s\n",room,callback);
	return 0;
}

static int network_roomDisconnect(lua_State *state) {
	const char* room=luaL_checkstring(state,1);

	printf("roomDisconnect: %s\n",room);
	return 0;
}

static int network_roomSend(lua_State *state) {
	const char* room=luaL_checkstring(state,1);
	const char* data=luaL_checkstring(state,2);

	printf("roomSend: %s,%s\n",room,data);
	return 0;
}*/

#ifdef PROTEO_USE_TINYJSON
void get_lib(json_t const * jobj)
{
	json_t const* result = json_getProperty( jobj, "result" );
	if ( !result || JSON_TEXT != json_getType( result ) )
	{
		printf("Error, the result property is not found.");
		return;
	}

	const char* res=json_getValue(result);
	if(strcmp(res,"OK")==0)
	{
		json_t const* script = json_getProperty( jobj, "lib" );
		if ( !script || JSON_TEXT != json_getType( script ) )
		{
			printf("Error, the script property is not found.");
			return;
		}
		const char* scr=json_getValue(script);

        json_t const* lib_name = json_getProperty( jobj, "name" );
        if ( !lib_name || JSON_TEXT != json_getType( lib_name ) )
        {
            printf("Error, the name property is not found.");
            return;
        }
        const char* name=json_getValue(lib_name);

        addLib(name,scr);

		luaL_dostring(L,scr);

	}
}

void get_script(json_t const * jobj)
{
	json_t const* result = json_getProperty( jobj, "result" );
	if ( !result || JSON_TEXT != json_getType( result ) )
	{
		printf("Error, the result property is not found.");
		return;
	}

	const char* res=json_getValue(result);
	if(strcmp(res,"OK")==0 || strcmp(res,"FAIL")==0)
	{
        json_t const* libs = json_getProperty( jobj, "libs" );
		if ( libs && JSON_ARRAY != json_getType( libs ) )
		{
			if(debug) printf("Libs property found.\n");
			json_t const* lib;
			for( lib = json_getChild( libs ); lib != 0; lib = json_getSibling( lib ) ) {
				if ( JSON_TEXT == json_getType( lib ) ) {
					char const* name = json_getName(lib);
					char const* script = json_getValue(lib);
					if(debug) printf( "Lib: %s\n", name, script );
					addLib(name,script);
				}
			}
		}

		json_t const* script = json_getProperty( jobj, "script" );
		if ( !script || JSON_TEXT != json_getType( script ) )
		{
			printf("Error, the script property is not found.");
			return;
		}
		const char* scr=json_getValue(script);
		luaL_dostring(L,scr);
		//lua_pushcfunction(L, traceback);
		lua_getglobal(L,"init");
        //int error = lua_pcall(L, 0, 0, -2);
        int error = lua_trace_pcall(L,0,0);

        if (error) {
        	fprintf(stderr, "ERROR pcall(init): %s\n", lua_tostring(L, -1));
        	lua_pop(L, 1);
    	}
	}
}

#else
void get_lib(json_object * jobj)
{
    	struct json_object *result;
		json_object_object_get_ex(jobj, "result", &result);

		if(result==NULL)
		{
			printf("Get Error get_script");
		}
		else
		{
			//enum json_type type = json_object_get_type(result);
			const char* res=json_object_get_string(result);
			//printf("Result: %s\n",res);

			if(strcmp(res,"OK")==0)
			{
				//printf("CALLBACK: %s",chunk.memory);
				struct json_object *script;
				json_object_object_get_ex(jobj, "lib", &script);
				//enum json_type type = json_object_get_type(script);
				const char* scr=json_object_get_string(script);

                struct json_object *lib_name;
                json_object_object_get_ex(jobj, "name", &lib_name);
                const char* name=json_object_get_string(lib_name);

                addLib(name,scr);

				//printf("Download LUA: %s\n",scr);
				luaL_dostring(L,scr);

			}
		}
}

void get_script(json_object * jobj)
{
    	struct json_object *result;
		json_object_object_get_ex(jobj, "result", &result);

		if(result==NULL)
		{
			printf("Get Error get_script\n");
		}
		else
		{
			//enum json_type type = json_object_get_type(result);
			const char* res=json_object_get_string(result);
			//printf("Result: %s\n",res);

			if(strcmp(res,"OK")==0 || strcmp(res,"FAIL")==0)
			{
                if(strcmp(res,"FAIL")==0) printf("Error get_script: downloading failed\n");
                //TODO verifico se c'è "libs" nel caso salvo libs, vedi versione TINY-JSON
                //addLib(const char* name,scr);

				struct json_object *script;
				json_object_object_get_ex(jobj, "script", &script);
				//enum json_type type = json_object_get_type(script);
				const char* scr=json_object_get_string(script);

				if(scr!=NULL)
				{
					//printf("Downloaded LUA: %s\n",scr);

					luaL_dostring(L,scr);
					//lua_pushcfunction(L, traceback);
					lua_getglobal(L,"init");
        			//int error = lua_pcall(L, 0, 0, -2);
                    int error = lua_trace_pcall(L,0,0);

        			if (error) {
        				fprintf(stderr, "ERROR pcall(init): %s\n", lua_tostring(L, -1));
        				lua_pop(L, 1);
    				}
    			}
    			else
    				printf("get_script ERROR!\n");
			}
		}
}
#endif

//==============================================================================
//   PROTEO
//==============================================================================
#ifdef __EMSCRIPTEN__
void proteo_post(const char* res,const char* appkey, const char* token,const char* post_data, const char* callback, int ref_callback)
{

   	EM_ASM({
		var http = new XMLHttpRequest();
		var url = UTF8ToString($0)+UTF8ToString($1);

		var hash = CryptoJS.HmacSHA256(UTF8ToString($1)+UTF8ToString($4), UTF8ToString($3));
        var hashInBase64 = CryptoJS.enc.Base64.stringify(hash);

        http.onreadystatechange = function() {
			if(this.status == 200) {
				//alert("readyState: "+http.readyState);
				if(this.readyState == this.DONE )
				{
					//proteo_post_callback(const char* res,const char* data,const char* callback)
					var result = Module.ccall('proteo_post_callback',
					'null',
					['string','string','string'],
					[UTF8ToString($1),this.responseText,UTF8ToString($5)]);
				}

			}
			else if(this.status >= 400)
			{

			}

        };
        http.open("POST", url, true);
        http.setRequestHeader("X-HMAC", hashInBase64);
        http.setRequestHeader("X-TOKEN", UTF8ToString($2));
        http.send(UTF8ToString($4));


		},config.server,res,token,appkey,post_data,callback);

		/*//console.log(data.strValues);
                    var http = new XMLHttpRequest();
                    var url = "https://"+BASE_URL+"/bifrost";
                    var jsonData=JSON.stringify(data.strValues);
                    //console.log(jsonData);

                    var hash = CryptoJS.HmacSHA256(path+jsonData, appKey);
                    var hashInBase64 = CryptoJS.enc.Base64.stringify(hash);

                    http.onreadystatechange = function() {
                        if(http.readyState == 4 && http.status == 200) {
                            //console.log(http.responseText);
                            var data = JSON.parse(http.responseText);
                            var t = new starlight.runtime.T(data);
                            starlight.runtime.globalScope.get(callback)(path,t);
                        }

                    }
                    http.open("POST", url+path, true);
                    http.setRequestHeader("X-HMAC", hashInBase64);
                    http.setRequestHeader("X-TOKEN", token);
                    http.send(jsonData);*/
}
#else
void proteo_post(const char* res,const char* appkey, const char* token,const char* post_data, const char* callback,const int ref_callback)
{
    char* url=NULL;
    //TODO foreach app
    if(strstr(res,"deepblue")!=NULL)
    {
        url=concat(getaccesspoint("deepblue"),res);
    }
    else
        url=concat(config.server,res);

    char* data=concat(res,post_data);

   // if(debug) printf("HMAC \n json) %s\n",data);

    unsigned int digest_lenght;
    unsigned char digest[EVP_MAX_MD_SIZE];
    HMAC(EVP_sha256(), appkey, strlen(appkey), data, strlen(data), digest, &digest_lenght);

    char *b64 =malloc(Base64encode_len(digest_lenght)+1);
    Base64encode(b64,digest,digest_lenght);
    b64[Base64encode_len(digest_lenght)]=0;

    char* xhmac=concat("X-HMAC: ",b64);
    free(b64);

    char* xtoken=concat("X-TOKEN: ",token);

    CURLcode ret;
    CURL *hnd;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

      hnd = curl_easy_init ();
      curl_easy_setopt (hnd, CURLOPT_POST,1L);
      curl_easy_setopt (hnd, CURLOPT_URL,url);
    curl_easy_setopt (hnd, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, writeMemory_callback);
    curl_easy_setopt (hnd, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt (hnd, CURLOPT_SSL_VERIFYPEER, FALSE);
    curl_easy_setopt (hnd, CURLOPT_POSTFIELDS, post_data);

    struct curl_slist *list = NULL;
    list = curl_slist_append(list, xhmac);
    list = curl_slist_append(list, xtoken);
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, list);

    ret = curl_easy_perform (hnd);
    curl_slist_free_all(list);
    free(url);
    free(data);
    free(xhmac);
    free(xtoken);

    if(ret != CURLE_OK) {
        printf("curl_easy_perform() failed: %s\n",
               curl_easy_strerror(ret));
    }
    else {
        if(debug) printf("CALLBACK: %s\n",chunk.memory);
        proteo_post_callback(res,chunk.memory,callback,ref_callback);

    }

    curl_easy_cleanup (hnd);
    free(chunk.memory);

}
#endif

void proteo_post_callback(const char* res,const char* data,const char* callback,const int ref_callback)
{
    //printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
    #ifdef PROTEO_USE_TINYJSON
    json_t mem[32];
    json_t const* jobj = json_create( data, mem, sizeof mem / sizeof *mem );
    #else
      json_object * jobj = json_tokener_parse(data);
    #endif

      if(jobj==NULL) printf("proteo_post %s error: %s\n",res,data);
      else
      {
          int callback_valid=false;
              //lua_getglobal(L,callback);
              if(callback!=NULL)
              {
                 lua_getglobal(L,callback);
                  callback_valid=true;
              }
              else if(ref_callback!=-1)
              {
                 lua_getref(L,ref_callback);
                  callback_valid=true;
              }
          if(callback_valid)
           {

              lua_pushlstring(L,res,strlen(res));
            /*#ifdef PROTEO_USE_TINYJSON
              json_to_table(jobj);
            #else
              json_object_to_table(jobj);
            #endif*/
              json_to_table(jobj);
              int error = lua_pcall(L, 2, 0, 0);

              if (error) {
                  fprintf(stderr, "ERROR pcall(proteo_post): %s\n", lua_tostring(L, -1));
                  lua_pop(L, 1);
              }
          }
      }

}

#ifdef __EMSCRIPTEN__
void proteo_get(const char* res,const char* appkey, const char* token,const char* callback)
{
   	EM_ASM({
		var http = new XMLHttpRequest();
		var url = UTF8ToString($0)+UTF8ToString($1);

		var hash = CryptoJS.HmacSHA256(UTF8ToString($1)+UTF8ToString($2), UTF8ToString($3));
        var hashInBase64 = CryptoJS.enc.Base64.stringify(hash);

        http.onreadystatechange = function() {
			if(this.status == 200) {
				//alert("readyState: "+http.readyState);
				//alert("get: "+UTF8ToString($1)+ " " + this.responseText);
				if(this.readyState == this.DONE )
				{
					//proteo_get_callback(const char* res,const char* data,const char* callback)
					var result = Module.ccall('proteo_get_callback',
					'null',
					['string','string','string'],
					[UTF8ToString($1),this.responseText,UTF8ToString($4)]);
				}

			}
			else if(this.status >= 400)
			{

			}

        };
        http.open("GET", url, true);
        http.setRequestHeader("X-HMAC", hashInBase64);
        http.setRequestHeader("X-TOKEN", UTF8ToString($2));
        http.send();


		},config.server,res,token,appkey,callback);

}
#else
void proteo_get(const char* res,const char* appkey, const char* token,const char* callback)
{
	//char url[150];

	/*if(strstr(res,"deepblue")!=NULL)
	{
		printf("New base: %s -> %s\n","deepblue",getaccesspoint("deepblue"));
		strcpy(url,getaccesspoint("deepblue"));
	}
	else
        strcpy(url,config.server);

	strcat(url,res);*/

	char* url=NULL;
	//TODO foreach app
	if(strstr(res,"deepblue")!=NULL && getaccesspoint("deepblue")!=NULL)
	{
		url=concat(getaccesspoint("deepblue"),res);
	}
	else
		url=concat(config.server,res);

	char* data=concat(res,token);

	/*char data[2000];
	strcpy(data,res);
	strcat(data,token);*/

	//$hmac=base64_encode (hash_hmac('sha256', $res.$token, $appkey,TRUE));
	//printf("AppKey: %s,%d\n",appkey,strlen(appkey));
	//printf("Data: %s,%d\n",data,strlen(data));

	unsigned int digest_lenght;
 	unsigned char digest[EVP_MAX_MD_SIZE];
	HMAC(EVP_sha256(), appkey, strlen(appkey), data, strlen(data), digest, &digest_lenght);

	char *b64 =malloc(Base64encode_len(digest_lenght)+1);
	Base64encode(b64,digest,digest_lenght);
	//const char* b64=base64encode(digest,digest_lenght);
	//printf("HMAC B64: %s\n",b64);
    b64[Base64encode_len(digest_lenght)]=0;

	/*char xhmac[100];
	strcpy(xhmac, "X-HMAC: ");
	strcat(xhmac,b64);*/
	char* xhmac=concat("X-HMAC: ",b64);
    free(b64);

	/*char xtoken[1000];
	strcpy(xtoken, "X-TOKEN: ");
	strcat(xtoken,token);*/
	char* xtoken=concat("X-TOKEN: ",token);

	CURLcode ret;
	CURL *hnd;

	struct MemoryStruct chunk;

  	chunk.memory = malloc(1);
  	chunk.size = 0;

  	hnd = curl_easy_init ();
  	curl_easy_setopt (hnd, CURLOPT_URL,url);
	curl_easy_setopt (hnd, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, writeMemory_callback);
	curl_easy_setopt (hnd, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt (hnd, CURLOPT_SSL_VERIFYPEER, FALSE);

    struct curl_slist *list = NULL;
    list = curl_slist_append(list, xhmac);
    list = curl_slist_append(list, xtoken);
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, list);


	ret = curl_easy_perform (hnd);
    curl_slist_free_all(list);
    free(url);
    free(data);
    free(xhmac);
    free(xtoken);

	if(ret != CURLE_OK) {
    	printf("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(ret));
  	}
  	else {
 		if(debug) printf("CALLBACK: %s\n",chunk.memory);
    	//printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
		proteo_get_callback(res,chunk.memory,callback);
  	}

  	curl_easy_cleanup (hnd);
  	free(chunk.memory);
}
#endif

void proteo_get_callback(const char* res,const char* data,const char* callback)
{
    if(verbose) printf("proteo_get_callback: %s \n",res);//%s \n%s\n",res,data,callback);

	//printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
    #ifdef PROTEO_USE_TINYJSON
    json_t mem[32];
	json_t const* jobj = json_create( data, mem, sizeof mem / sizeof *mem );
    #else
  	json_object * jobj = json_tokener_parse(data);
	#endif

  	if(jobj==NULL) printf("proteo_get %s error: %s\n",res,data);
  	else
  	{
  		if(strncmp(res, "/proteo/script", strlen("/proteo/script")) == 0 ) //include /proteo/script/ and /proteo/scriptandlibs
  			get_script(jobj);
  		else if(strncmp(res, "/proteo/lib/", strlen("/proteo/lib/")) == 0)
  			get_lib(jobj);
  		else if(callback!=NULL)
  		{


  			lua_getglobal(L,callback);
			lua_pushlstring(L,res,strlen(res));
			/*#ifdef PROTEO_USE_TINYJSON
			json_to_table(jobj);
			#else
			json_object_to_table(jobj);
			#endif*/
            json_to_table(jobj);
    		int error = lua_pcall(L, 2, 0, 0);

    		if (error) {
        		fprintf(stderr, "ERROR pcall(proteo_get): %s\n", lua_tostring(L, -1));
        		lua_pop(L, 1);
    		}
  		}
  	}

}
static int network_proteo_get(lua_State *state) {

	const char* res = luaL_checkstring(state,1);
	const char* callback = luaL_checkstring(state,2);
	proteo_get(res,PROTEO_APP_KEY,Token,callback);

	return 0;
}

static int network_proteo_post(lua_State *state) {

    const char* res = luaL_checkstring(state,1);
    //const char* data = "";
    //luaL_checkstring(state,2);

    /*#ifdef PROTEO_USE_TINYJSON
    const char *json = table_to_json(2);
    #else
    json_object * jobj = table_to_json_object(2);
    const char *json = json_object_to_json_string(jobj);
    #endif*/

    const char* json = luaL_checkstring(state,2);

    //const char* callback = luaL_checkstring(state,3);

    char* callback=NULL;
    int ref_callback=-1;
    if (lua_isstring(state,3)==1)
        callback=(char*)luaL_checkstring(state,3);
    else
        ref_callback=lua_ref(state, TRUE);

    proteo_post(res,PROTEO_APP_KEY,Token,json,callback,ref_callback);
    //free((void*)json);

	return 0;
}

static int network_proteo_put(lua_State *state) {

	const char* res = luaL_checkstring(state,1);

	return 0;
}

static int network_proteo_delete(lua_State *state) {

	const char* res = luaL_checkstring(state,1);

	return 0;
}

//==============================================================================
//   LOGIN
//==============================================================================

#ifdef PROTEO_USE_TINYJSON
//static size_t login_function(void *ptr, size_t size, size_t nmemb, void *stream)
static int proteo_login_callback(char* ptr)
{
	//if(debug) printf("Login BODY: %s\n", ptr);
    if(verbose) printf("Login BODY \n");

	if(ptr==NULL)
	{
		luaL_dostring(L,server_form);

		//lua_pushcfunction(L, traceback);
		lua_getglobal(L,"init");
		//int error = lua_pcall(L, 0, 0, -2);
        int error = lua_trace_pcall(L,0,0);

		if (error) {
			fprintf(stderr, "ERROR pcall(init): %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		}

		return 1;
	}
	else
	{
		json_t mem[16];
		json_t const* json = json_create( ptr, mem, sizeof mem / sizeof *mem );

		if(json==NULL)
		{
			return proteo_login_callback(NULL);
		}

		/*json_t const* server = json_getProperty( json, "server" );
		if ( !server || JSON_TEXT != json_getType( server ) ) printf("Error, the server property is not found.");
		else
			strcpy(_config.server,json_getValue(server));*/

		json_t const* message = json_getProperty( json, "message" );
		if ( !message || JSON_INTEGER != json_getType( message ) ) printf("Error, the message property is not found.");
		else
		{
			int res=(int)json_getInteger(message);

			if(res==0)
			{
				//Read Tickets
		//TODO
		/*#ifdef PROTEO2
		struct json_object *tick;
		json_object_object_get_ex(jobj, "tickets", &tick);
		struct array_list *tickets=json_object_get_array(tick);
		for (int i = 0; i < array_list_length(tickets); i++) {
			struct json_object *t = (struct json_object *) array_list_get_idx(tickets, i);
			json_object_object_foreach(t, key, val)
			{
				//if(strcmp(app,key)==0 && val!=NULL)
				//	strcpy(permissions,json_object_get_string(val));
				char* url=json_object_get_string(val);
				printf("Ticket: %s url: %s\n",key,url);
				addaccesspoint(key,url);
			}
		}
		#endif
*/
				char get_script[50];

				strcpy(get_script,"/proteo/script/");
				strcat(get_script,script);
				printf("Download LUA: %s\n",get_script);
				proteo_get(get_script,PROTEO_APP_KEY,Token,"");

			}
			else if(res==2)
			{
				//LOGIN SCREEN
				if(debug) printf("Login screen\n");
				get_script(json);
			}
			else{
                //TODO
			}
		}
	}

	return 0;
}
#else
//static size_t login_function(void *ptr, size_t size, size_t nmemb, void *stream)
static int proteo_login_callback(char* ptr)
{
 	//if(debug) printf("Login BODY: %s\n", ptr);
    if(verbose) printf("Login BODY \n");

	/*JSONObject *json = parseJSON(ptr);
  	printf("Count: %i\n", json->count);
  	printf("Key: %s, Value: %s\n", json->pairs[0].key, json->pairs[0].value->stringValue);*/
	json_object * jobj = json_tokener_parse(ptr);
	//json_parse(jobj);
    if(jobj==NULL) return 0;

	struct json_object *message;
	json_object_object_get_ex(jobj, "message", &message);
	int res=json_object_get_int(message);
	//printf("Message: %d\n", res);

	if(res==0)
	{
		//Read Tickets
		//TODO
		#ifdef PROTEO2
		struct json_object *tick;
		json_object_object_get_ex(jobj, "tickets", &tick);
		struct array_list *tickets=json_object_get_array(tick);
		for (int i = 0; i < array_list_length(tickets); i++) {
			struct json_object *t = (struct json_object *) array_list_get_idx(tickets, i);
			json_object_object_foreach(t, key, val)
			{
				//if(strcmp(app,key)==0 && val!=NULL)
				//	strcpy(permissions,json_object_get_string(val));
				char* url=(char*)json_object_get_string(val);
				printf("Ticket: %s url: %s\n",key,url);
				addaccesspoint(key,url);
			}
		}
		#endif

		if(remote_lua)
		{
			char get_script[50];

			strcpy(get_script,"/proteo/script/");
			strcat(get_script,script);
			printf("Download LUA: %s\n",get_script);
			bifrost_get(get_script,PROTEO_APP_KEY,Token,"");
		}
		else if(local_lua)
		{
			strcpy(luascript,config.basedir);
			strcat(luascript,script);
			strcat(luascript,".lua");
			printf("Load LUA: %s\n",luascript);
			luaL_dofile(L,luascript);

      		//lua_pushcfunction(L, traceback);
			lua_getglobal(L,"init");
        	//int error = lua_pcall(L, 0, 0, -2);
            int error = lua_trace_pcall(L,0,0);

        	if (error) {
        		fprintf(stderr, "ERROR pcall(init): %s\n", lua_tostring(L, -1));
        		lua_pop(L, 1);
    		}
		}
	}
    else if(res==2)
    {
        //LOGIN SCREEN
        get_script(jobj);
    }
	else{
		struct json_object *info;
		json_object_object_get_ex(jobj, "info", &info);
		printf("Login error: %s \n",json_object_get_string(info));
		//exit(0);
	}
    json_object_put(jobj);
  	//return nmemb;
  	return 0;
}
#endif

static size_t header_callback(char *buffer,   size_t size,   size_t nitems,   void *userdata)
{
	//if(debug) printf("HEADER: %s\n", buffer);
	//"X-Token: "
	if(strncmp("X-Token: ", buffer, strlen("X-Token: ")) == 0)
	{
		strcpy(Token,buffer+9);
		Token[strlen(Token)-2]=0;//Remove /n
		if(debug) printf("Token is: %s\n", Token);
		//printf("%s\n", buffer);
	}
  	return nitems;
}

#ifdef __EMSCRIPTEN__

int em_login_function(void *ptr)
{
	//if(debug) printf("Login BODY: %s\n", ptr);
    if(verbose) printf("Login BODY \n");

	if(ptr==NULL)
	{
		luaL_dostring(L,server_form);

		//lua_pushcfunction(L, traceback);
		lua_getglobal(L,"init");
		//int error = lua_pcall(L, 0, 0, -2);
        int error = lua_trace_pcall(L,0,0);

		if (error) {
			fprintf(stderr, "ERROR pcall(init): %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		}

		return 1;
	}
	else
	{
		json_t mem[16];
		json_t const* json = json_create( ptr, mem, sizeof mem / sizeof *mem );

		if(json==NULL)
		{
			return em_login_function(NULL);
		}

		/*json_t const* server = json_getProperty( json, "server" );
		if ( !server || JSON_TEXT != json_getType( server ) ) printf("Error, the server property is not found.");
		else
			strcpy(_config.server,json_getValue(server));*/

		json_t const* message = json_getProperty( json, "message" );
		if ( !message || JSON_INTEGER != json_getType( message ) ) printf("Error, the message property is not found.");
		else
		{
			int res=(int)json_getInteger(message);

			if(res==0)
			{
				//Read Tickets
		//TODO
		/*#ifdef PROTEO2
		struct json_object *tick;
		json_object_object_get_ex(jobj, "tickets", &tick);
		struct array_list *tickets=json_object_get_array(tick);
		for (int i = 0; i < array_list_length(tickets); i++) {
			struct json_object *t = (struct json_object *) array_list_get_idx(tickets, i);
			json_object_object_foreach(t, key, val)
			{
				//if(strcmp(app,key)==0 && val!=NULL)
				//	strcpy(permissions,json_object_get_string(val));
				char* url=json_object_get_string(val);
				printf("Ticket: %s url: %s\n",key,url);
				addaccesspoint(key,url);
			}
		}
		#endif
*/
				char get_script[50];

				strcpy(get_script,"/proteo/scriptandlibs/");
				strcat(get_script,script);
				printf("Download LUA: %s\n",get_script);
				proteo_get(get_script,PROTEO_APP_KEY,Token,"");

			}
			else if(res==2)
			{
				//LOGIN SCREEN
				if(debug) printf("Login screen\n");
				get_script(json);
			}
			else{
                //TODO
			}
		}
	}

	return 0;
}

int em_header_callback(char *response)
{
	//if(debug) printf("HEADER: %s\n", response);
	//"X-Token: "

	char* tmp = strcasestr(response, "X-Token:");
	char* end = strstr(tmp,"\r\n");
	int end_position = end - tmp;
	strncpy(Token,tmp+9,end_position-9);
	Token[end_position]=0;
	if(debug) printf("Token is: %s\n", Token);
	/*if(strncmp("X-Token: ", buffer, strlen("X-Token: ")) == 0)
	{
		strcpy(Token,buffer+9);
		Token[strlen(Token)-2]=0;//Remove /n
		if(debug) printf("Token is: %s\n", Token);
		//printf("%s\n", buffer);
	}*/

  	return 0;
}

void proteo_login(const char* username,const char* password, const char* id)
{
	if(debug) printf("Login: %s id:%s\n",username,id);

	char timestamp[100];
    time_t now = time (0);
    strftime (timestamp, 100, "%Y-%m-%d %H:%M:%S", localtime (&now));

 	EM_ASM({
		var http = new XMLHttpRequest();
		var url = UTF8ToString($0)+"/login";
		var json = JSON.stringify({ username: UTF8ToString($1), timestamp: UTF8ToString($4), scriptId: UTF8ToString($2)});
		var jsondata = "/login"+json;
		var md5_pass = CryptoJS.MD5(UTF8ToString($3));
		md5_pass = md5_pass.toString(CryptoJS.enc.Hex);
		var hash = CryptoJS.HmacSHA256(jsondata, md5_pass);
        var hashInBase64 = CryptoJS.enc.Base64.stringify(hash);
        //alert("key: " +md5_pass +" data: "+jsondata + " hmac: "+hashInBase64);
        http.onreadystatechange = function() {
			if(this.status == 200) {
				//alert("readyState: "+http.readyState);
				if(this.readyState == this.DONE )
				{
					//console.log("Http 200 ready: "+this.readyState);
					var result = Module.ccall('em_login_function', // name of C function. TODO use proteo_login_callback (?)
					'number', // return type
					['string'], // argument types
					[this.responseText]);

					/*var lengthBytes = lengthBytesUTF8(http.responseText)+1;
					var stringOnWasmHeap = _malloc(lengthBytes);
					stringToUTF8(http.responseText, stringOnWasmHeap, lengthBytes);
					return stringOnWasmHeap;*/

				}
				else if(this.readyState == this.HEADERS_RECEIVED)
				{
					/*var token = http.getResponseHeader("X-Token");
					alert("Token: "+token);*/
					/*var headers = this.getAllResponseHeaders();
					console.log("Headers: "+headers);*/
					var result = Module.ccall('em_header_callback',
					'number',
					['string'],
					[this.getAllResponseHeaders()]);
				}
			}
			else if(this.status >= 400)
			{
				console.log("Http status: "+this.status);
				var result = Module.ccall('em_login_function', //  TODO use proteo_login_callback (?)
				'number',
				['string'],
				[null]);
			}

        };
        http.open("POST", url, true);
        http.setRequestHeader("X-HMAC", hashInBase64);
        http.send(json);


		},config.server,username,id,password,timestamp);

}
#else



void proteo_login_md5(const char* username,const char* md5_hex, const char* id)
{

	/*unsigned char md5_res[MD5_DIGEST_LENGTH];
	unsigned char md5_hex[1+MD5_DIGEST_LENGTH*2];

	MD5(password, strlen(password), md5_res);

	int j=0;
	for (int i=0; i < MD5_DIGEST_LENGTH; i++)
  	{
		//printf("%02x",  md5_res[i]);
		sprintf (md5_hex +j,"%02x",  md5_res[i]);
		j+=2;
    }*/

	//const char* url = "https://messina.isasi.it/bifrost/login";
	char url[150]; //TODO
	strcpy(url,config.server);
	strcat(url,"/login");

    if(debug) printf("Login URL: %s\n",url);

	char timestamp[100];
    time_t now = time (0);
    strftime (timestamp, 100, "%Y-%m-%d %H:%M:%S", localtime (&now));
    //printf ("%s\n", timestamp);

#ifdef PROTEO_USE_TINYJSON
	char json[512];
	sprintf(json,"{"
              "\"username\":\"%s\","
              "\"scriptId\":\"%s\","
              "\"timestamp\":\"%s\""
              "}",username,id,timestamp);
#else
    json_object * jobj = json_object_new_object();
    #ifndef PROTEO2
    json_object *jstring1 = json_object_new_string(id);
    #endif

  	json_object *jstring2 = json_object_new_string(username);
  	json_object *jstring3 = json_object_new_string(timestamp);

  	#ifdef PROTEO2
	json_object *jstring4 = json_object_new_string(id);
	#endif

	#ifndef PROTEO2
  	json_object_object_add(jobj,"appId", jstring1); //Nel nuovo proteo va rimosso
  	#endif

  	json_object_object_add(jobj,"username", jstring2);
  	json_object_object_add(jobj,"timestamp", jstring3);
  	#ifdef PROTEO2
	json_object_object_add(jobj,"scriptId", jstring4);
	#endif

	const char *json = json_object_to_json_string(jobj);
    //json_object_put(jobj);
#endif

    if(debug) printf("login json: %s\n",json);

	char jsondata[250]; //TODO
	strcpy(jsondata, "/login");
	strcat(jsondata,json);

    struct MemoryStruct chunk;

  	chunk.memory = malloc(1);
  	chunk.size = 0;

	CURLcode ret;
	CURL *hnd;
	struct curl_slist *list = NULL;

  	hnd = curl_easy_init ();
  	curl_easy_setopt (hnd, CURLOPT_POST,1L);
  	curl_easy_setopt (hnd, CURLOPT_URL,url);
	curl_easy_setopt (hnd, CURLOPT_VERBOSE, 0L);

    curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, writeMemory_callback);
    curl_easy_setopt (hnd, CURLOPT_WRITEDATA, (void *)&chunk);

	//curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, login_function);
	//curl_easy_setopt (hnd, CURLOPT_RETURNTRANSFER, TRUE);
	curl_easy_setopt (hnd, CURLOPT_POSTFIELDS, json);
	curl_easy_setopt (hnd, CURLOPT_SSL_VERIFYPEER, FALSE);
	//curl_easy_setopt (hnd, CURLOPT_HEADER, TRUE);
	curl_easy_setopt (hnd, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt (hnd, CURLOPT_CONNECTTIMEOUT, 10); //LOGIN TIMEOUT
	//size_t output_length;
	//const char* hmac=get_hmac(md5_res,jsondata);
	//printf("Json Data: %s\n",jsondata);
	//printf("Key: %s\n",md5_hex);
 	unsigned int digest_lenght;
 	unsigned char digest[EVP_MAX_MD_SIZE];
	HMAC(EVP_sha256(), md5_hex, 1+MD5_DIGEST_LENGTH*2, jsondata, strlen(jsondata), digest, &digest_lenght);

	char *b64 =malloc(1+Base64encode_len(digest_lenght));
	Base64encode(b64,digest,digest_lenght);
	//const char* b64=base64encode(digest,digest_lenght);
	//printf("login HMAC B64: %s\n",b64);
    b64[Base64encode_len(digest_lenght)]=0;

	char xhmac[100];
	strcpy(xhmac, "X-HMAC: ");
	strcat(xhmac,b64);
    free(b64);

	//list = curl_slist_append(list, "Content-Type: application/json");
	//list = curl_slist_append(list, "Content-Length: ");
	list = curl_slist_append(list, xhmac);
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, list);

	ret = curl_easy_perform (hnd);
    curl_slist_free_all(list);

    if(ret!=CURLE_OK)
    {
        printf("Login curl Error: %s\n",curl_easy_strerror(ret));
        //TODO
        //if CURLE_COULDNT_CONNECT
        //CURLE_URL_MALFORMAT
        //call config_login.lua to change url
//#if TARGET_OS_IPHONE
        //char conf[1024];

        //sprintf(conf,"\tlocal label_server=nil\n\tlocal text_server=nil\n\tlocal btn_save=nil\n\tlocal json=require \"json\"\n\n\tfunction init()\n\t\tlabel_server=proteo.gui.newLabel('label_server','Server:','Helvetica',18,\"#FBFBFF\",'Clear',proteo.gui.LabelAlignment.Left,MIN_X +30,MIN_Y + 170,150,30)\n\t\ttext_server=proteo.gui.newTextField('text_server','%s','Helvetica',18,'Black','White',MIN_X +150,MIN_Y + 170,280,30,'')\n\t\tbtn_save=proteo.gui.newButton('btn_save','Save','Helvetica',20,\"#FBFBFF\",\"#20BF55\",1,\"#20BF55\",true,MID_X - 100,MAX_Y - 100,100,50,\"save\")\n    end\n    \n    function update(dt)\n    end\n    \n    function save(sender)\n    \tdata={}\n    \tdata[\"url\"]=proteo.gui.getText(text_server)\n    \tproteo.system.writeFile(proteo.system.document()..\"config.json\",json.encode(data))\n    \tproteo.system.login(\"\",\"\",\"login\")\n    \t--hide\n    \t--and set nil\n\tend",config.server);

                //sprintf(conf,server_form,config.server);

        luaL_dostring(L,server_form);

        //lua_pushcfunction(L, traceback);
        lua_getglobal(L,"init");
        //int error = lua_pcall(L, 0, 0, -2);
        int error = lua_trace_pcall(L,0,0);

        if (error) {
            fprintf(stderr, "ERROR pcall(init): %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
//#endif

    }
    else
    {
		proteo_login_callback(chunk.memory);
	}

  	curl_easy_cleanup (hnd);
    #ifndef PROTEO_USE_TINYJSON
  	free( (void*)json );
    #endif

    free(chunk.memory);
}

void proteo_login(const char* username,const char* password, const char* id)
{
    unsigned char md5_res[MD5_DIGEST_LENGTH];
    unsigned char md5_hex[1+MD5_DIGEST_LENGTH*2];

    MD5(password, strlen(password), md5_res);

    int j=0;
    for (int i=0; i < MD5_DIGEST_LENGTH; i++)
      {
        //printf("%02x",  md5_res[i]);
        sprintf (md5_hex +j,"%02x",  md5_res[i]);
        j+=2;
    }

    proteo_login_md5(username,md5_hex,id);
}

#endif

//==============================================================================
//   LUA
//==============================================================================
void add_network_proteo(lua_State* state)
{
	addFunction_proteo(state,"network","download",network_download);
    addFunction_proteo(state,"network","getData",network_getData);
	/*addFunction_proteo(state,"network","roomInit",network_roomInit);
	addFunction_proteo(state,"network","roomConnect",network_roomConnect);
	addFunction_proteo(state,"network","roomDisconnect",network_roomDisconnect);
	addFunction_proteo(state,"network","roomSend",network_roomSend);*/

	//TODO queste due funzioni sono da considerarsi deprecate
#ifndef __EMSCRIPTEN__
	addFunction_proteo(state,"network","bifrost_get",network_bifrost_get);
	addFunction_proteo(state,"network","bifrost_post",network_bifrost_post);
#endif

	addFunction_proteo(state,"network","proteo_get",network_proteo_get);
	addFunction_proteo(state,"network","proteo_post",network_proteo_post);
	addFunction_proteo(state,"network","proteo_put",network_proteo_put); //TODO https://curl.haxx.se/libcurl/c/CURLOPT_PUT.html
	addFunction_proteo(state,"network","proteo_delete",network_proteo_delete); //TODO https://curl.haxx.se/libcurl/c/CURLOPT_CUSTOMREQUEST.html
}
