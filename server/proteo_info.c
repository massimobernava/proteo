struct MemoryStruct {
  char *memory;
  size_t size;
};

int addTicket(const char* app,const char* url,int value)
{
	//TODO
	//IF SQLITE
	return sqlite_addTicket(app,url,value);
}

int getTicket(const char* app,char** url)
{
	//TODO
	//IF SQLITE
	return sqlite_getTicket(app,url);
}

struct MHD_Response * proteo_info()
{
	struct MHD_Response *response;
	char json[1024];
	strcpy(json,"{");
	

	printf("Plugins count: %d\n",config.plugin_count);
	for(int i=0;i<config.plugin_count;i++)
	{
      	if(verbose) printf("%d) %s\n",i, config.plugins[i].name);
      	
		char new_url[100];
        snprintf(new_url,100,"/%s/info",config.plugins[i].name);
		
		const char* single_info=app_call(GET,new_url,"","_ALL","_MASTER","_TOKEN");
		if(verbose) printf("INFO %s: %s\n",new_url,single_info);

		if(single_info!=NULL)
		{
			strcat(json,"\"");
			strcat(json,config.plugins[i].name);
			strcat(json,"\":");
            if(strcmp(single_info,"?")==0)
                strcat(json,"{\"result\":\"FAIL\"}");
            else
                strcat(json,single_info);
		}
		else
			strcat(json,"NULL");

		if(i<config.plugin_count-1) strcat(json,",");
	}
	strcat(json,"}");
	//printf("Test json: %s\n",json);
  	response = MHD_create_response_from_buffer (strlen (json),(void*) json, MHD_RESPMEM_MUST_COPY);

  	return response;
}

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
void get_info(char* url)
{
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
    curl_easy_setopt (hnd, CURLOPT_CONNECTTIMEOUT, 5);
	//curl_easy_setopt (hnd, CURLOPT_SSL_VERIFYPEER, FALSE);
	//curl_easy_setopt (hnd, CURLOPT_SSL_VERIFYHOST, FALSE);

	ret = curl_easy_perform (hnd);

	if(ret != CURLE_OK) {
    	printf("curl_easy_perform() failed: %s,%s\n",
            curl_easy_strerror(ret),url);
  	}
  	else {
 		//printf("CALLBACK: %s",chunk.memory);
    	//printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
  		json_object * jobj = json_tokener_parse(chunk.memory);

  		if(jobj==NULL) printf("bifrost_get %s error: %s\n",url,chunk.memory);
  		else //if(strncmp(res, "/info", strlen("/info")) == 0)
  		{
  			url[strlen(url)-5]=0;//remove /info
  			//update table
  			json_object_object_foreach(jobj, key, val) {

  				struct json_object *obj;
  				json_object_object_get_ex(val, "ticket", &obj);
				int v=json_object_get_int(obj);

  				if(verbose) printf("Info: %s script: %s value:%d\n",url,key,v);
  				addTicket(key,url,v);
  			}
  		}

  	}

  	curl_easy_cleanup (hnd);
  	free(chunk.memory);
}

void get_server_info()
{
	char path[100];
	strcpy(path,config.local);
	strcat(path,"/info");
	if(verbose) printf("get_info: %s\n",path);
	get_info(path);

	for(int i=0;i<config.server_count;i++)
	{
		char path[100];
		strcpy(path,config.servers[i].url);
		strcat(path,"/info");
		if(verbose) printf("get_info: %s\n",path);
		get_info(path); 
	}
}
