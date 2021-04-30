void freetimers()
{
  while(timers!=NULL)
  {
      if(timers->callback!=NULL) free(timers->callback);
    ProteoTimer* tmp=(ProteoTimer*)timers->next;

    if(debug)printf("Free timer\n");
    free(timers);
    timers=tmp;
  }
}

//==============================================================================
//   SYSTEM
//==============================================================================

static int system_console (lua_State *state) {
  printf("%s\n",luaL_checkstring(state,1));
  return 0;
}

static int system_clock (lua_State *state) {
  if(verbose) printf("system.clock\n");

	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);

	double n=now.tv_sec;
	n=n+(now.tv_nsec/1000000000.0);
	lua_pushnumber (state,n);

  return 1;
}

static int system_fileexist(lua_State *state) {
	const char* fname=luaL_checkstring(state,1);
	if(verbose) printf("system.fileExist: %s\n",fname);
 	int exist=access( fname, F_OK ) != -1;
	lua_pushboolean (state, exist);
	return 1;
}

static int system_document(lua_State *state) {
  if(verbose) printf("system.document\n");
  //lua_pushliteral(state,"./");
  lua_pushlstring(state,basedir,strlen(basedir));
  return 1;
}

static int system_readFile(lua_State *state) {

 	const char* fname=luaL_checkstring(state,1);
	//const char* callback=luaL_checkstring(state,2);

	if(verbose) printf("system.readFile: %s \n",fname);

	char *data = loadfile((char*)fname);
	//if(debug) printf("%s\n",data);
	if(data==NULL) {
    printf("Error readFile NULL\n");
    lua_pushnil(state);
    return 1;
  }

	//lua_getglobal(state,callback);
    lua_pushlstring(state,data,strlen(data));
    //int error = lua_pcall(state, 1, 0, 0);
    
    if(data!=NULL) free(data);

    /*if (error) {
        fprintf(stderr, "ERROR pcall(%s): %s\n",callback, lua_tostring(L, -1));
        lua_pop(L, 1);
    }*/

  return 1;
}

static int system_writeFile(lua_State *state) {
  
    const char* fname=luaL_checkstring(state,1);
    const char* data=luaL_checkstring(state,2);
        
    if(verbose) printf("system.writeFile: %s\n",fname);
    //if(debug) printf("%s\n",data);
        
    int ret=writefile(fname,(char*)data);
    
    lua_pushinteger(state, ret);
    
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

static int system_reboot(lua_State *state) {

    paused=TRUE;
    
    if(debug) printf(">>>>>>>Reboot %d\n",toreboot);
    
    if(toreboot==-1)
    {
        toreboot=1;//timer & app call
        if(timers==NULL) toreboot=0;
    }
    else if(toreboot==0) //Reboot after finish current call and fired all timer
    {
        closeLUA();
        freetimers();
        initLUA();
    
        paused=FALSE;
        toreboot=-1;
    }
    else toreboot--;
    
    return 0;
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
	const char* callback=NULL;
	int ref_callback=-1;
	
	if (lua_isstring(state,2)==1)
        callback=luaL_checkstring(state,2);
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

static int system_getUsers(lua_State *state) {

    //TODO
}

static int system_loadConfig(lua_State *state) {

    //TODO
}

static int system_saveConfig(lua_State *state) {

    //TODO
}

//==============================================================================
//   MAIL
//==============================================================================

struct upload_mail_status {
    const char *mail;
    int start;
    int len;
};

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
    struct upload_mail_status *upload_ctx = (struct upload_mail_status *)userp;
 
    if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
        return 0;
    }
 
    int send_len=upload_ctx->len-upload_ctx->start;
    if(send_len>(size*nmemb)) send_len=(size*nmemb);
  
    if(send_len>0) {
        memcpy(ptr, upload_ctx->mail+upload_ctx->start, send_len);
        upload_ctx->start=send_len;
    }
 
  return send_len;
}

void sendMail(const char* to_mail, const char* from_mail, const char* cc_mail, const char* subject, const char* body)
{
    char timestamp[100];
    time_t now = time (0);
    strftime (timestamp, 100, "%c", localtime (&now));
    
    const char *mail=replaceWords(6,
                                  "Date: ##TIME##\r\n"
                                  "To: <##TO_MAIL##> \r\n"
                                  "From: <##FROM_MAIL##> \r\n"
                                  "Cc: <##CC_MAIL##> \r\n"
                                  //"Message-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@rfcpedant.example.org>\r\n"
                                  "Subject: ##SUBJECT##\r\n"
                                  "\r\n" /* empty line to divide headers from body, see RFC5322 */
                                  "##BODY##\r\n",
    "##TIME##",timestamp,
    "##TO_MAIL##",to_mail,
    "##FROM_MAIL##",from_mail,
    "##CC_MAIL##",cc_mail,
    "##SUBJECT##",subject,
    "##BODY##",body);
    
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_mail_status upload_ctx;
     
    upload_ctx.mail = mail;
    upload_ctx.start=0;
    upload_ctx.len=strlen(mail);
    
    curl = curl_easy_init();
    if(curl) {
        /* Set username and password */
        curl_easy_setopt(curl, CURLOPT_USERNAME, "");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "");
     
        /* This is the URL for your mailserver. Note the use of smtps:// rather
         * than smtp:// to request a SSL based connection. */
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
     
        /* If you want to connect to a site who isn't using a certificate that is
         * signed by one of the certs in the CA bundle you have, you can skip the
         * verification of the server's certificate. This makes the connection
         * A LOT LESS SECURE.
         *
         * If you have a CA cert for the server stored someplace else than in the
         * default bundle, then the CURLOPT_CAPATH option might come handy for
         * you. */
    #ifdef SKIP_PEER_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    #endif
     
        /* If the site you're connecting to uses a different host name that what
         * they have mentioned in their server certificate's commonName (or
         * subjectAltName) fields, libcurl will refuse to connect. You can skip
         * this check, but this will make the connection less secure. */
    #ifdef SKIP_HOSTNAME_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    #endif
     
        /* Note that this option isn't strictly required, omitting it will result
         * in libcurl sending the MAIL FROM command with empty sender data. All
         * autoresponses should have an empty reverse-path, and should be directed
         * to the address in the reverse-path which triggered them. Otherwise,
         * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
         * details.
         */
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from_mail);
     
        /* Add two recipients, in this particular case they correspond to the
         * To: and Cc: addressees in the header, but they could be any kind of
         * recipient. */
        recipients = curl_slist_append(recipients, to_mail);
        recipients = curl_slist_append(recipients, cc_mail);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
     
        /* We're using a callback function to specify the payload (the headers and
         * body of the message). You could just use the CURLOPT_READDATA option to
         * specify a FILE pointer to read from. */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
     
        /* Since the traffic will be encrypted, it is very useful to turn on debug
         * information within libcurl to see what is happening during the
         * transfer */
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
     
        /* Send the message */
        res = curl_easy_perform(curl);
     
        /* Check for errors */
        if(res != CURLE_OK)
          fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));
     
        /* Free the list of recipients */
        curl_slist_free_all(recipients);
     
        /* Always cleanup */
        curl_easy_cleanup(curl);
      }

    free((void*)mail);
}
static int system_sendMail(lua_State *state) {

    const char* to_mail=luaL_checkstring(state,1);
    const char* from_mail=luaL_checkstring(state,2);
    const char* cc_mail=luaL_checkstring(state,3);
    const char* subject=luaL_checkstring(state,4);
    const char* body=luaL_checkstring(state,5);
    
    sendMail(to_mail,from_mail,cc_mail,subject,body);
    
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
    
	addFunction_proteo(state,"system","clock",system_clock);
    
	addFunction_proteo(state,"system","createTimer",system_createTimer);
	addFunction_proteo(state,"system","startTimer",system_startTimer);
	addFunction_proteo(state,"system","stopTimer",system_stopTimer);
    
    addFunction_proteo(state,"system","getUsers",system_getUsers);
    
    addFunction_proteo(state,"system","loadConfig",system_loadConfig);
    addFunction_proteo(state,"system","saveConfig",system_saveConfig);
    
    addFunction_proteo(state,"system","sendMail",system_sendMail);
    
    addFunction_proteo(state,"system","reboot",system_reboot);
}
