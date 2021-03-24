//==============================================================================
//   NETWORK
//==============================================================================

json_object * table_to_json_object(lua_State *state,int index)
{
	json_object * jobj = json_object_new_object();

    lua_pushvalue(state, index);
    lua_pushnil(state);
    while (lua_next(state, -2))
    {
        lua_pushvalue(state, -2);
        const char *key = lua_tostring(state, -1);
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

		const char *value = lua_tostring(state, -2);
		json_object *jstring = json_object_new_string(value);
		json_object_object_add(jobj,key, jstring);
		printf("%s => %s\n", key, value);
			
        lua_pop(state, 2);
    }
    lua_pop(state, 1);
    return jobj;
}

void json_object_to_table(lua_State *state,json_object * jobj)
{
	enum json_type t = json_object_get_type(jobj);
	if(t==json_type_array)
	{
		lua_newtable(state);
		
		int arraylen = json_object_array_length(jobj);
		if(verbose) printf(" array %d\n",arraylen); 
		int i;
		json_object * jvalue;
		for (i=0; i< arraylen; i++)
		{
			jvalue = json_object_array_get_idx(jobj, i);
			if(verbose) printf("  [%d] \n",i);
			lua_pushinteger(state,i+1);
			json_object_to_table(state,jvalue);
			lua_settable(state,-3);
		}
	}
	else if(t==json_type_string)
	{
		const char* svalue=json_object_get_string(jobj);
		if(verbose) printf(" string %s\n",svalue);
		lua_pushlstring(state,svalue,strlen(svalue));
	}
	else if(t==json_type_int)
	{
		if(verbose) printf(" int \n");
	}
	else if(t==json_type_boolean)
	{
		if(verbose) printf(" bool \n");
	}
	else if(t==json_type_double)
	{
		if(verbose) printf(" double \n");
	}
	else if(t==json_type_null)
	{
		if(verbose) printf(" null \n");
	}
	else if(t==json_type_object)
	{
		lua_newtable(state);
		json_object_object_foreach(jobj, key, val) {
				
				if(verbose) printf("object %s -> \n",key);
				json_object_to_table(state,val);
				lua_setfield(state, -2, key);
		}
	}	
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}
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

	if(verbose) printf("download: %s,%s,%s\n",url,name,callback);

	return 0;
}

static int network_socket_connect(lua_State *state) {
	const char* namespace=luaL_checkstring(state,1);
	const char* callback=luaL_checkstring(state,2);
	
	printf("socket_connect: %s,%s\n",namespace,callback);
	return 0;
}

static int network_socket_on(lua_State *state) {
	const char* data=luaL_checkstring(state,1);
	const char* callback=luaL_checkstring(state,2);
	
	printf("socket_on: %s,%s\n",data,callback);
	return 0;
}

static int network_socket_emit(lua_State *state) {
	const char* room=luaL_checkstring(state,1);
	const char* data1=luaL_checkstring(state,2);
	const char* data2=luaL_checkstring(state,3);
	const char* callback=luaL_checkstring(state,4);

	printf("socket_emit: %s,%s\n",room,callback);
	return 0;
}

//==============================================================================
//   REST
//==============================================================================

void server_post(const char* url,const char* appkey, const char* token,const char* post_data, const char* callback)
{
    char* data=concat(url,post_data);

	unsigned int digest_lenght;
 	unsigned char digest[EVP_MAX_MD_SIZE];
	HMAC(EVP_sha256(), appkey, strlen(appkey), (const unsigned char*)data, strlen(data), digest, &digest_lenght);
    free(data);
    
	char *b64 =malloc(Base64encode_len(digest_lenght)+1);
	Base64encode(b64,(const char*)digest,digest_lenght);
	//const char* b64=base64encode(digest,digest_lenght);
	//printf("HMAC B64: %s\n",b64);
	b64[Base64encode_len(digest_lenght)]=0;

    char* xhmac=concat("X-HMAC: ",b64);

    char* xtoken=concat("X-TOKEN: ",token);

	CURLcode ret;
	CURL *hnd;
	struct curl_slist *list = NULL;

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

	list = curl_slist_append(list, xhmac);
	list = curl_slist_append(list, xtoken);
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, list);

	ret = curl_easy_perform (hnd);
    free(b64);
    free((void*)token);
    
	if(ret != CURLE_OK) {
    	printf("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(ret));
  	}
  	else {
 		//printf("CALLBACK: %s",chunk.memory);
    	//printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
  		json_object * jobj = json_tokener_parse(chunk.memory);

  		if(jobj==NULL) printf("bifrost_post %s error: %s\n",url,chunk.memory);
  		else
  		{
			lua_getglobal(L,callback);
			lua_pushlstring(L,url,strlen(url));
			json_object_to_table(L,jobj);
   			int error = lua_trace_pcall(L, 2, 0);

   			if (error) {
       			fprintf(stderr, "ERROR pcall(%s): %s\n",callback, lua_tostring(L, -1));
       			//lua_pop(L, 1);
                if(debug) printf("app_call GetTop %d\n",lua_gettop(L));
   			}

  		}

  	}

  	curl_easy_cleanup (hnd);
  	free(chunk.memory);
}


void server_get(const char* url,const char* appkey, const char* token,const char* callback)
{
    char* data=concat(url,token);

	unsigned int digest_lenght;
 	unsigned char digest[EVP_MAX_MD_SIZE];
	HMAC(EVP_sha256(), appkey, strlen(appkey), (const unsigned char*)data, strlen(data), digest, &digest_lenght);
    free(data);
    
	char *b64 =malloc(Base64encode_len(digest_lenght)+1);
	Base64encode(b64,(const char*)digest,digest_lenght);
	//const char* b64=base64encode(digest,digest_lenght);
	//printf("HMAC B64: %s\n",b64);
	b64[Base64encode_len(digest_lenght)]=0;
	
    char* xhmac=concat("X-HMAC: ",b64);

    char* xtoken=concat("X-TOKEN: ",token);

	CURLcode ret;
	CURL *hnd;
	struct curl_slist *list = NULL;

	struct MemoryStruct chunk;
 
  	chunk.memory = malloc(1);
  	chunk.size = 0;

  	hnd = curl_easy_init ();
  	curl_easy_setopt (hnd, CURLOPT_URL,url);
	curl_easy_setopt (hnd, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, writeMemory_callback);
	curl_easy_setopt (hnd, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt (hnd, CURLOPT_SSL_VERIFYPEER, FALSE);

	list = curl_slist_append(list, xhmac);
	list = curl_slist_append(list, xtoken);
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, list);

	ret = curl_easy_perform (hnd);
    free(b64);
    free((void*)token);
    
	if(ret != CURLE_OK) {
    	printf("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(ret));
  	}
  	else {
 		//printf("CALLBACK: %s",chunk.memory);
    	//printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
  		json_object * jobj = json_tokener_parse(chunk.memory);

  		if(jobj==NULL) printf("bifrost_get %s error: %s\n",url,chunk.memory);
  		else
  		{
			lua_getglobal(L,callback);
			lua_pushlstring(L,url,strlen(url));
			json_object_to_table(L,jobj);
   			int error = lua_trace_pcall(L, 2, 0);

   			if (error) {
       			fprintf(stderr, "ERROR pcall(%s): %s\n",callback, lua_tostring(L, -1));
       			//lua_pop(L, 1);
                if(debug) printf("app_call GetTop %d\n",lua_gettop(L));
   			}
  		}

  	}

  	curl_easy_cleanup (hnd);
  	free(chunk.memory);
}

static int network_get(lua_State *state) {

	const char* url = luaL_checkstring(state,1);
	const char* token = luaL_checkstring(state,2);
	const char* callback = luaL_checkstring(state,3);
	
	server_get(url,(const char*)clientkey,token,callback);
	
	return 0;
}

static int network_post(lua_State *state) {

	const char* url = luaL_checkstring(state,1);
	//const char* data = "";
	//luaL_checkstring(state,2);
	json_object * jobj = table_to_json_object(state,2);
	
	const char *json = json_object_to_json_string(jobj);
	const char* token = luaL_checkstring(state,3);
	const char* callback = luaL_checkstring(state,4);
	
	server_post(url,(const char*)clientkey,token,json,callback);
	
	free((void*)json);

	return 0;
}

static int network_put(lua_State *state) {

	const char* res = luaL_checkstring(state,1);

	return 0;
}

static int network_delete(lua_State *state) {

	const char* res = luaL_checkstring(state,1);

	return 0;
}

static int network_getInterfaces(lua_State *state) {

	//const char* res = luaL_checkstring(state,1);

struct ifaddrs *ifap, *ifa;
struct sockaddr_in *sa;
char *addr;
    
lua_newtable(state);
    
getifaddrs (&ifap);
for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    
    //lua_newtable(state);
    
    if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
        sa = (struct sockaddr_in *) ifa->ifa_addr;
        addr = inet_ntoa(sa->sin_addr);
        
        //lua_pushlstring(state, ifa->ifa_name,strlen( ifa->ifa_name));
        lua_pushlstring(state,addr,strlen( addr));
        lua_setfield(state, -2, ifa->ifa_name);
        //lua_settable(state,-3);
        
        //printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);
    }
}

freeifaddrs(ifap);
    
	return 1;
}
