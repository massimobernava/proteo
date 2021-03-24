struct MHD_Response * proteo_getadmin()
{
	struct MHD_Response *response;
    
	/*char* path=concat(config.basedir,"web/index.html");
    
    char *page=loadfile(path);//"./web/index.html");
  	char *loginform = "<div class='login-block'>"
        						"<h1>Login</h1>"
        						"<form method='post' action='' name='loginform'>"
            						"<input type='text' placeholder='Username' id='username' name='username' />"
            						"<input type='password' placeholder='Password' id='password' name='password' />"
            						//"<input type='text'  placeholder='Project' id='project' name='project' />"
            						"<button type='submit'>Submit</button>"
        						"</form></div>";

	char *newpage=replaceWord(page,"##BODY##", loginform);
  	free(page);
    free(path);

  	response = MHD_create_response_from_buffer (strlen (newpage),(void*) newpage, MHD_RESPMEM_MUST_COPY);
  	free(newpage);
  	
  	return response;*/
  	
  	char* path=concat(config.basedir,"web/index.html"); //TODO admin dovrebbe chiamare direttamente il plugin corretto
	char *page=loadfile(path);
    
    response = MHD_create_response_from_buffer (strlen (page),(void*) page, MHD_RESPMEM_MUST_FREE);
	free(path);
				
	return response;
}

int parse_post(char *in,char *out[100][2])
{
	//char *out[1000][2] = {}; // default initialise all of them to MULL
 	int KVP = 0;

 	#define KVP_KEY 0
 	#define KVP_VALUE 1
  	int key_value = KVP_KEY;

  	out[KVP][key_value] = strtok(in,"=");

  	while(KVP < 1000 && out[KVP][key_value] != NULL)
  	{
    	if (key_value == KVP_VALUE)
    	{
      		KVP++;
      		key_value = KVP_KEY;
      		out[KVP][key_value] = strtok(NULL,"=");
    	}
    	else
    	{
      		key_value = KVP_VALUE;
      		out[KVP][key_value] = strtok(NULL,"&");
    	}
  	}

  	return KVP;
}

/*struct MHD_Response * proteo_postadmin(struct connection_info_struct *con_info)
{
	struct MHD_Response *response;
	
		char *out[100][2] = {};
		int login_correct=0;
		int KVP=parse_post(con_info->data,out);
		int password_index=-1;
		int username_index=-1;
		//int project_index=-1;
		for(int i=0; i<KVP; i++)
  		{
    		//printf("\nkey[%d] = %s\nvalue[%d] = %s\n",
       		//i, out[i][KVP_KEY],
        	//i, out[i][KVP_VALUE]);
        	if(strcmp(out[i][KVP_KEY],"password")==0) password_index=i;
        	else if(strcmp(out[i][KVP_KEY],"username")==0) username_index=i;
        	//else if(strcmp(out[i][KVP_KEY],"project")==0) project_index=i;
  		}
		//if(strcmp(out[password_index][KVP_VALUE],config.admin_password)==0) password_correct=1;

    char *password=NULL;//[1+MD5_DIGEST_LENGTH*2];
		char* username=replaceWord(out[username_index][KVP_VALUE],"%40", "@");
		if(username!=NULL && getPassword(username,&password)==0)
		{

			//printf("DB password: %s\n",password);
  		
			unsigned char md5_res[MD5_DIGEST_LENGTH];
			unsigned char md5_hex[1+MD5_DIGEST_LENGTH*2];

			MD5(out[password_index][KVP_VALUE], strlen(out[password_index][KVP_VALUE]), md5_res);

			int j=0;
			for (int i=0; i < MD5_DIGEST_LENGTH; i++)
  			{
					//printf("%02x",  md5_res[i]);
				sprintf (md5_hex +j,"%02x",  md5_res[i]);
				j+=2;
 	 		}
 	 		//printf("Login password: %s\n",md5_hex);
 	 		login_correct=strcmp(password,md5_hex)==0;
 	 	}
    free(password);
    
  		if (login_correct==1)
  		{
  			char *page=loadfile("./web/index.html");
  			char *body=loadfile("./web/proteo_body.html");
  			char *script=loadfile("./script/attenzione_visiva2.lua");

  			char *statemachine=loadfile("./lib/statemachine.lua");
			char *WebForm=loadfile("./lib/WebForm.lua");
			char *genericforms=loadfile("./lib/genericforms.lua");
			char *json=loadfile("./lib/json.lua");

  			const char *libs=replaceWords(4,"<script type='application/lua' data-modname='statemachine'>##STATEMACHINE##</script>"
  											"<script type='application/lua' data-modname='Form'>##WEBFORM##</script>"
  											"<script type='application/lua' data-modname='genericforms'>##GENERICFORMS##</script>"
  											"<script type='application/lua' data-modname='json'>##JSON##</script>"
  											,"##STATEMACHINE##",statemachine,
  											"##WEBFORM##",WebForm,
  											"##GENERICFORMS##",genericforms,
  											"##JSON##",json);

  			//printf("LIBS: %s\n",libs);
  			char token[512];
			//char Tickets[512];
			json_object *jarray_ticket;
			createTokenAndTicket(username, "attenzione_visiva2",token,&jarray_ticket);
			//printf("TOKEN: %s\n",token);

			//const char* script=proteo_get_script(out[project_index][KVP_VALUE],clientkey,token);
			//printf("Script: %s\n",script);

			const char *newpage=replaceWords(4,page,"##BODY##", body,"##USERNAME##",username,"##SCRIPT##",script,"##LIBS##",libs);
			//printf("PAGE: %s\n",newpage);
  			free((void*)page);
  			free((void*)body);
  			free((void*)script);
  			free((void*)statemachine);
			free((void*)WebForm);
			free((void*)genericforms);
			free((void*)json);
			free((void*)libs);

  			response = MHD_create_response_from_buffer (strlen (newpage),(void*) newpage, MHD_RESPMEM_MUST_COPY);
  			free((void*)newpage);
  		}
  		else
  		{
  			const char *page  = "<html><body>Proteo!</body></html>";
  			response = MHD_create_response_from_buffer (strlen (page),(void*) page, MHD_RESPMEM_PERSISTENT);
  		}
  		if(username!=NULL) free(username);
  		
  		return response;
}*/
