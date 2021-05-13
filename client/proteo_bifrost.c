//==============================================================================
//   BIFROST
//==============================================================================

static int network_bifrost_get(lua_State *state) {

	const char* res = luaL_checkstring(state,1);
	const char* callback = luaL_checkstring(state,2);
	bifrost_get(res,config.appkey,Token,callback);

	return 0;
}

static int network_bifrost_post(lua_State *state) {

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
    
	const char* callback = luaL_checkstring(state,3);
	bifrost_post(res,config.appkey,Token,json,callback);
	//free((void*)json);
    
	return 0;
}

/*static size_t callback_function(void *ptr, size_t size, size_t nmemb, void *stream)
{
 	//printf("CALLBACK: %s\n", ptr);

	json_object * jobj = json_tokener_parse(ptr);
	if(jobj==NULL)
	{
		printf("json_tokener_parse error \n");
	}
	else
	{
		struct json_object *result;
		json_object_object_get_ex(jobj, "result", &result);
		enum json_type type = json_object_get_type(result);
		char* res=json_object_get_string(result);
		printf("Result: %s\n",res);
	}

  	return nmemb;
}*/

/*void call_bifrost_get(const char* res,json_object * jobj)
{
    //lua_pushcfunction(L, traceback);
	lua_getglobal(L,"bifrost_get");
	lua_pushlstring(L,res,strlen(res));
	json_object_to_table(jobj);
    int error = lua_pcall(L, 2, 0, 0);

    if (error) {
        fprintf(stderr, "ERROR pcall(bifrost_get): %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}*/

void bifrost_post(const char* res,const char* appkey, const char* token,const char* post_data, const char* callback)
{
	char url[150]; //TODO concat

	strcpy(url,config.server);

	strcat(url,res);

	char data[2000]; //TODO concat
	strcpy(data,res);
	strcat(data,post_data);

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
    
	char xhmac[100];  //TODO concat
	strcpy(xhmac, "X-HMAC: ");
	strcat(xhmac,b64);
    free(b64);
    
	char xtoken[1000]; //TODO concat
	strcpy(xtoken, "X-TOKEN: ");
	strcat(xtoken,token);

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
    
	if(ret != CURLE_OK) {
    	printf("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(ret));
  	}
  	else {
 		//printf("CALLBACK: %s",chunk.memory);
    	//printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
  		//json_object * jobj = json_tokener_parse(chunk.memory);
        #ifdef PROTEO_USE_TINYJSON
        json_t mem[32];
        json_t const* jobj = json_create( chunk.memory, mem, sizeof mem / sizeof *mem );
        #else
          json_object * jobj = json_tokener_parse(chunk.memory);
        #endif
        //TODO TINY-JSON
        //json_t mem[32];
        //json_t const* jobj = json_create( chunk.memory, mem, sizeof mem / sizeof *mem );
        
  		if(jobj==NULL) printf("bifrost_post %s error: %s\n",res,chunk.memory);
  		else
  		{
			lua_getglobal(L,callback);
			lua_pushlstring(L,res,strlen(res));
            /*#ifdef PROTEO_USE_TINYJSON
              json_to_table(jobj);
            #else
              json_object_to_table(jobj);
            #endif*/
            json_to_table(jobj);
			//json_object_to_table(jobj);
            //TODO TINY-JSON
            //json_to_table(jobj);
   			int error = lua_pcall(L, 2, 0, 0);

   			if (error) {
       			fprintf(stderr, "ERROR pcall(%s): %s\n",callback, lua_tostring(L, -1));
       			lua_pop(L, 1);
   			}

  		}

  	}

  	curl_easy_cleanup (hnd);
  	free(chunk.memory);
}

void bifrost_get(const char* res,const char* appkey, const char* token,const char* callback)
{
	/*char url[150]; //TODO concat
	strcpy(url,config.server);
	strcat(url,res);*/
    
    char* url=concat(config.server,res);

    char* data=concat(res,token);
	/*char data[2000]; //TODO concat
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
    
	/*char xhmac[100]; //TODO concat
	strcpy(xhmac, "X-HMAC: ");
	strcat(xhmac,b64);*/
    char* xhmac=concat("X-HMAC: ",b64);
    free(b64);
    
	/*char xtoken[1000]; //TODO concat
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
 		//printf("CALLBACK: %s",chunk.memory);
    	//printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
  		proteo_get_callback(res,chunk.memory,callback);
  	}

  	curl_easy_cleanup (hnd);
  	free(chunk.memory);
    
}
