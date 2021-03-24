
unsigned char *serverkey = (unsigned char *)"01234567890123456789012345678901"; //TODO va nel config
unsigned char *clientkey = (unsigned char *)"1234567890123456789012"; //TODO va nel config

struct connection_info_struct
{
  int type;
  char *url;
  char *data;
  size_t size;
  //struct MHD_PostProcessor *postprocessor;
};

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

int aes_encrypt(const unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
	/*for (int i = 0; i < plaintext_len; i++)
    {
        ciphertext[i] = plaintext[i] ^ key[i%strlen(key)];
    }
	
	return plaintext_len;*/
	
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, serverkey, iv))
        handleErrors();


    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int aes_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
	/*for (int i = 0; i < ciphertext_len; i++)
    {
        plaintext[i] = ciphertext[i] ^ key[i%strlen(key)];
        //printf("%c",inpString[i]);
    }
	
	return ciphertext_len;*/
	
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, serverkey, iv))
        handleErrors();

    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

//==============================================================================
//   SERVICE
//==============================================================================

int addUser(char* user,char* password)
{
    //TODO
    //IF SQLITE
    return sqlite_addUser(user,password);
}

int getPassword(const char* username,char** password)
{
	//TODO
	//IF SQLITE
	return sqlite_getPassword(username,password);
}

int getPermission(const char* user,const char* app,char** permission)
{
    //TODO
	//IF SQLITE
	return sqlite_getPermission(user,app,permission)==SQLITE_OK?0:1;
}

int create_auth_user_db(const char* db_name)
{
    //TODO
	//IF SQLITE
	return sqlite_create_auth_user_db(db_name);
}

//==============================================================================
int createTokenAndTicket(const char* username,const char* scriptId, char* token,json_object **jarray_ticket)
{
	char path[100];

    snprintf(path,100,"%sscript/%s.json",config.basedir,scriptId);
	//printf("Path: %s\n",path);

	char* json_info=loadfile(path);
	//printf("JSON info: %s\n",json_info);
	if(json_info==NULL) return 1;

	json_object * jobj = json_tokener_parse(json_info);
	struct json_object *version;
	json_object_object_get_ex(jobj, "version", &version);
	const char* ver=json_object_get_string(version);

	struct json_object *plugins;
	json_object_object_get_ex(jobj, "plugins", &plugins);
	struct array_list *plug=json_object_get_array(plugins);

	char timestamp[100];
    time_t now = time (0);
    strftime (timestamp, 100, "%Y-%m-%d %H:%M:%S", localtime (&now));

	json_object * token_jobj = json_object_new_object();
	json_object_object_add(token_jobj,"date", json_object_new_string(timestamp));
	json_object_object_add(token_jobj,"username", json_object_new_string(username));
	
	json_object *jarray_perm = json_object_new_array();

	//json_object * t_jobj = json_object_new_object();
	*jarray_ticket = json_object_new_array();

	for (int i = 0; i < array_list_length(plug); i++) {
		struct json_object *p = (struct json_object *) array_list_get_idx(plug, i);
		//printf("Plugin: %s\n",json_object_get_string(p));
		const char* plug_name=json_object_get_string(p);
		//printf("Plug: %s\n",plug_name);
		char* url=NULL;
		if(0==getTicket(plug_name,&url))
		{
			printf("Ticket: [%s]=%s\n",plug_name,url);
			json_object * ticket_jobj = json_object_new_object();
			json_object_object_add(ticket_jobj,plug_name, json_object_new_string(url));
			json_object_array_add(*jarray_ticket,ticket_jobj);
		}
        free(url);

        char* permission=NULL;//[256];
		if(0==getPermission(username,plug_name,&permission))
		{
			printf("Permission: [%s]=%s\n",plug_name,permission);
			json_object * plug_jobj = json_object_new_object();
			json_object_object_add(plug_jobj,plug_name, json_object_new_string(permission));
			json_object_array_add(jarray_perm,plug_jobj);
		}
        free(permission);
	}
	//printf("Script: %s v%s\n",scriptId,ver);
	

	//json_object_object_add(token_jobj,"scriptid", json_object_new_string(scriptId));
	json_object_object_add(token_jobj,"permissions", jarray_perm);
	
	const char *json_token = json_object_to_json_string(token_jobj);


    /* A 128 bit IV */
    unsigned char iv[17];
    rand_string(iv, 17);
    //unsigned char *iv = (unsigned char *)"0123456789012345"; //TODO Nella versione definitiva deve essere random e accodato

    //printf("IV:%s\n",iv);
    if(debug) printf("Create Token: %s\n",json_token);
    
	unsigned char ciphertext[512];
    int ciphertext_len = aes_encrypt ((const unsigned char *)json_token, strlen (json_token), serverkey, iv, ciphertext);
    strcpy(ciphertext+ciphertext_len, "::");
	strcpy(ciphertext+ciphertext_len+2,iv);
	ciphertext_len+=18;
	//printf("IN %d-%d: %s\n",ciphertext_len,strlen(ciphertext),ciphertext);
	//printf("IV: %s\n",iv);
    //char *base64_encoded = base64encode(ciphertext,ciphertext_len);
	
	char *base64_encoded =malloc(Base64encode_len(ciphertext_len)+1);
	Base64encode(base64_encoded,ciphertext,ciphertext_len);
		
    if(base64_encoded!=NULL)
    {
    	base64_encoded[Base64encode_len(ciphertext_len)]=0;
    	strcpy(token,base64_encoded);
    	free(base64_encoded);
    }
    //printf("Token is: %s\n",token);

    //free((void*)json_token);
    
    //if(jarray_perm!=NULL)json_object_put(jarray_perm);
    if(token_jobj!=NULL)json_object_put(token_jobj);
	//if(t_jobj!=NULL)json_object_put(t_jobj);
	if(jobj!=NULL)json_object_put(jobj);
	free(json_info);

    return 0;
}

int verifyToken(char* username,char* permissions,const char* app,const char* token)
{
	//if token==NULL || token=="NULL" return 1

    /* A 128 bit IV */
    //unsigned char *iv = (unsigned char *)"0123456789012345"; //TODO Nella versione definitiva deve essere random e accodato 

	char decryptedtext[512];
	//unsigned char *tmp;
	//int len=base64decode(token,&tmp);
	
	unsigned char *tmp=malloc(Base64decode_len(token)+1);
	int len = Base64decode(tmp, token);
    tmp[Base64decode_len(token)]=0;
    
	unsigned char iv[17];
	strlcpy(iv,tmp+len-16,17);
	//unsigned char* json=malloc(len-17);
	//strlcpy(json,tmp,len-17);
	
	//printf("OUT %d-%d: %s\n",len,strlen(json),json);
	//printf("IV: %s\n",iv);
	
	int decryptedtext_len = aes_decrypt(tmp,len-18, serverkey, iv,(unsigned char*)decryptedtext);
	free(tmp);
	
	//unsigned char *iv=tmp+len-16;
	//len-=18;//=strlen(tmp)-18;
	//tmp[len]=0;
	//printf("NEW : %s\n",tmp);

	//int decryptedtext_len = aes_decrypt(tmp, len, serverkey, iv,(unsigned char*)decryptedtext);
	
	//if(tmp!=NULL) free(tmp);

    /* Add a NULL terminator. We are expecting printable text */
    decryptedtext[decryptedtext_len] = 0;//'\0';
	//if(debug) printf("TOKEN value: %s\n",decryptedtext);

    //TODO
    //VERIFICA DEL SOLO TIMESTAMP

    json_object * jobj = json_tokener_parse(decryptedtext);
	//json_parse(jobj);

	if(jobj==NULL)
	{
		printf("Token error: %s\n",iv);
		return 1;
	}

	struct json_object *uname;
	json_object_object_get_ex(jobj, "username", &uname);
	const char* u=json_object_get_string(uname);
	//printf("Username : %s\n", u);
	if(u!=NULL) strcpy(username,u);
	else
	{
		printf("Token error: %s\n",iv);
		strcpy(username,"NULL");
		return 2;
	} 

	
	struct json_object *perm;
	json_object_object_get_ex(jobj, "permissions", &perm);
	struct array_list *perms=json_object_get_array(perm);
	if(perms==NULL)
	{
		printf("Token error: %s\n",iv);
		strcpy(permissions,"NULL");
		return 3;
	}
	for (int i = 0; i < array_list_length(perms); i++) {
		struct json_object *p = (struct json_object *) array_list_get_idx(perms, i);
		json_object_object_foreach(p, key, val)
		{
			if(strcmp(app,key)==0 && val!=NULL)
				strcpy(permissions,json_object_get_string(val));

			//printf("Script: %s Permission: %s\n",key,json_object_get_string(val));
		}
		//char* perm_name=json_object_get_string(p);
	}
	//printf("Permission : %s\n", permissions);

    return 0;
}

int verifyHMAC(const char* res, const char* body, const char* token,const char* key, const char* hmac)
{
	if(verbose) printf("verifyHMAC: %s\n",res);
	
	if(key==NULL || strlen(key)==0) return 3;
	
	int jsondata_len=strlen(res);
	
	if(body == NULL || strlen(body)==0) jsondata_len+=strlen(token);
	else jsondata_len+=strlen(body);
	
	char* jsondata=(char *)malloc( jsondata_len +1);
	
	if(jsondata==NULL) return 2;
	
	strcpy(jsondata, res);
	if (body == NULL || strlen(body)==0)
		strcat(jsondata,token);
	else
		strcat(jsondata,body);
	
	jsondata[jsondata_len]=0;

	//if(debug) printf("HMAC \n json) %s\n",jsondata);

	//if(strlen(body)>150) printf("BODY JSON: %s %d\n",body,strlen(body));
	//printf("key: %s %d\n",key,strlen(key));

	unsigned int digest_lenght;
 	unsigned char digest[EVP_MAX_MD_SIZE];
	HMAC(EVP_sha256(), key, strlen(key), jsondata, strlen(jsondata), digest, &digest_lenght);
	free(jsondata);

	//char* b64=base64encode(digest,digest_lenght);
	char *b64 =malloc(Base64encode_len(digest_lenght)+1);
    if(b64==NULL) return 1;
    
	Base64encode(b64,digest,digest_lenght);
	b64[Base64encode_len(digest_lenght)]=0;

	//if(debug) printf("HMAC     : %s\n",hmac);
	//if(debug) printf("HMAC calc: %s\n",b64);

	int ret=2;
	if(strcmp(hmac,b64)==0) ret=0;
	free(b64);
	
	/*if(debug && ret!=0)
	{
		printf(" key) %s\n",key);
	}
	if(verbose) printf("HMAC %c= HMAC\n",ret==0?'=':'!');*/

	return ret;

}

int checkPermissions()
{
	return 0;
}


//==============================================================================
//   LOGIN
//==============================================================================

struct json_object* my_json_tokener(const char *str)
{
    struct json_tokener* tok;
    struct json_object* obj;

    tok = json_tokener_new();
    printf("Body size: %lu\n",strlen(str));
    if (!tok)
      return NULL;
    obj = json_tokener_parse_ex(tok, str, strlen(str));

    if(tok->err != json_tokener_success) {

		if (obj != NULL)
			json_object_put(obj);
        obj = NULL;
    }

    json_tokener_free(tok);
    return obj;
}
struct MHD_Response * proteo_login(char* body,const char* hmac)
{
    struct MHD_Response *response;
    json_object * resp_jobj = json_object_new_object();
    
	if(verbose) printf("LOGIN\n");
    if(body==NULL)
    {
        if(verbose) printf ("LOGIN MESSAGE 4\n");
        json_object_object_add(resp_jobj,"message", json_object_new_int(4));
        json_object_object_add(resp_jobj,"info", json_object_new_string("post error"));

        const char *json = json_object_to_json_string(resp_jobj);
        response = MHD_create_response_from_buffer (strlen (json),(void*) json, MHD_RESPMEM_MUST_FREE);
        MHD_add_response_header (response, "X-Token", "NULL");
        
        return response;
    }
    
	if(debug) printf("BODY: \n%s\n",body);
	
	
	char Token[512];
	char Tickets[512];
    
	json_object * jobj = json_tokener_parse(body);
	
	if(jobj==NULL)
	{
		if(verbose) printf ("LOGIN MESSAGE 2\n");
		json_object_object_add(resp_jobj,"message", json_object_new_int(2));
		json_object_object_add(resp_jobj,"info", json_object_new_string("json error"));

		const char *json = json_object_to_json_string(resp_jobj);
		response = MHD_create_response_from_buffer (strlen (json),(void*) json, MHD_RESPMEM_MUST_FREE);
		MHD_add_response_header (response, "X-Token", "NULL");
		
		return response;
	}
	
	//struct json_object *j_appId;
	//json_object_object_get_ex(jobj, "appId", &j_appId);
	//char* appId=json_object_get_string(j_appId);
	//printf("APPID: %s\n",appId);

	struct json_object *j_scriptId;
	json_object_object_get_ex(jobj, "scriptId", &j_scriptId);
	const char* scriptId=json_object_get_string(j_scriptId);
	if(verbose) printf("SCRIPTID: %s\n",scriptId);

	struct json_object *j_username;
	json_object_object_get_ex(jobj, "username", &j_username);
	const char* username=json_object_get_string(j_username);
	if(verbose) printf("USERNAME: %s\n",username);

	struct json_object *j_timestamp;
	json_object_object_get_ex(jobj, "timestamp", &j_timestamp);
	const char* timestamp=json_object_get_string(j_timestamp);
	//printf("TIMESTAMP: %s\n",timestamp);

	
	//LOGIN SCREEN
	if(strcmp(username,"")==0 && strcmp(scriptId,"login")==0)
	{
		json_object_object_add(resp_jobj,"message", json_object_new_int(2));
		json_object_object_add(resp_jobj,"result", json_object_new_string("OK"));
        char* login_path=concat(config.basedir,"login.lua");
		char* login_script=loadfile(login_path);
		json_object_object_add(resp_jobj,"script", json_object_new_string(login_script));
		free(login_script);
        free(login_path);
		strcpy(Token,"NULL");
	}
	else
	{
		//TEST HMAC
        char* password=NULL;//[1+MD5_DIGEST_LENGTH*2];
		getPassword(username,&password);
		//printf("Password: %s\n",password);

		//=============================================
	
		//if(strcmp(hmac,b64)==0)
		//if(strlen(body)<=2 || strlen(body)>150) printf("ERROR proteo_login!\n");
		if(verifyHMAC("/login",body,"",password,hmac)==0)
		{
			//TODO
			//VERIFICA DEL TIMESTAMP

			
			//json_object_object_add(resp_jobj,"message", json_object_new_int(0));
			json_object *jarray_ticket;
			if(createTokenAndTicket(username, scriptId,Token,&jarray_ticket)==0)
            {
                if(verbose) printf ("LOGIN MESSAGE 0\n");
                json_object_object_add(resp_jobj,"message", json_object_new_int(0));
				json_object_object_add(resp_jobj,"tickets", jarray_ticket);
            }
            else
            {
                if(verbose) printf ("LOGIN MESSAGE 3\n");
                json_object_object_add(resp_jobj,"message", json_object_new_int(3));
            }
		}
		else
		{
			if(verbose) printf ("LOGIN MESSAGE 1\n");
			json_object_object_add(resp_jobj,"message", json_object_new_int(1));
			json_object_object_add(resp_jobj,"info", json_object_new_string("wrong password"));
			strcpy(Token,"NULL");
			//RETURN
			//message = 1
			//info "password errata"
		}
        
        free(password);
	}
	const char *json = json_object_to_json_string(resp_jobj);
	//printf("RESPONSE: %s\n",json);

	response = MHD_create_response_from_buffer (strlen (json),(void*) json, MHD_RESPMEM_MUST_FREE);
	if(MHD_NO==MHD_add_response_header (response, "X-Token", Token))
	{
		fprintf(stderr, "Error MHD_add_response_header\n");
	}

	return response;
}





