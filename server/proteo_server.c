//==============================================================================
//=>                       Proteo Server v0.0.1
//=>                        
//=>                         CC BY-NC-SA 3.0
//=>
//=> Massimo Bernava
//=> massimo.bernava@gmail.com
//=> 2021-05-06
//==============================================================================

#define PROTEO_OPENCV

#include "proteo_server.h"
#include "proteo_b64.c"
#include "proteo_config.c"
#include "proteo_lua.c"
#include "proteo_auth.c"
#include "proteo_sqlite.c"
#include "proteo_ejdb.c"
#include "proteo_system.c"
#include "proteo_info.c"
#include "proteo_network.c"
#include "proteo_enet.c"
#include "proteo_admin.c"
#include "proteo_zmq.c"
#include "proteo_ffmpeg.c"
#include "proteo_tensorflow.c"

//==============================================================================
//   TODO
//==============================================================================

// 1) TODO sparsi nel codice
//
//==============================================================================
//   UTILITY
//==============================================================================

void mkpath(const char* dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    dirname(tmp);
    len = strlen(tmp);
    if(tmp[len - 1] == '/')
            tmp[len - 1] = 0;
    for(p = tmp + 1; *p; p++)
            if(*p == '/') {
                    *p = 0;
                    mkdir(tmp, S_IRWXU);
                    *p = '/';
            }
    mkdir(tmp, S_IRWXU);
}

int writefile(char* filename,char* data)
{
    FILE *handler=fopen(filename,"w");
    if (handler)
    {
        fprintf(handler,"%s",data);
        fclose(handler);
        return 0;
    }
    else printf("Write data %s error: %s\n",filename,strerror(errno));
    
    return 1;
}

int writedatafile(char* filename,char* data,int data_size)
{
    int write_size;
    FILE *handler=fopen(filename,"wb");
    if (handler)
    {
        write_size = fwrite(data, sizeof(char), data_size, handler);
        fclose(handler);
        if(debug) printf("Write data file: %s,%d,%d\n",filename,data_size,write_size);
        return write_size;
    }
    else printf("Write data %s error: %s\n",filename,strerror(errno));

    return 0;
}

char* loadfile(char *filename)
{
	char *buffer = NULL;
  	int string_size, read_size;
	FILE *handler = fopen(filename, "r");

	if (handler)
	{
    	fseek(handler, 0, SEEK_END);
    	string_size = ftell(handler);
    	rewind(handler);
  
    	buffer = (char*) malloc(sizeof(char) * (string_size + 1) );
    	if(buffer!=NULL)
    	{
    		read_size = fread(buffer, sizeof(char), string_size, handler);
    		buffer[string_size] = '\0';

    		if (string_size != read_size)
    		{
           		free(buffer);
           		buffer = NULL;
           		return buffer;
       		}
			//if(debug) printf("Load file: %s,%d,%d\n",filename,strlen(buffer),string_size);
            if(debug) printf("Load file: %s\n",filename);
		}
      fclose(handler);
    }

    return buffer;
}

int loaddatafile(char *filename,char **buffer)
{
  	int data_size, read_size;
	FILE *handler = fopen(filename, "r");

	if (handler)
	{
    	fseek(handler, 0, SEEK_END);
    	data_size = ftell(handler);
    	rewind(handler);
  
    	*buffer = (char*) malloc(sizeof(char) * (data_size) );
    	if(buffer!=NULL)
    	{
    		read_size = fread(*buffer, sizeof(char), data_size, handler);

    		if (data_size != read_size)
    		{
           		free(*buffer);
           		*buffer = NULL;
           		return 0;
       		}
			//if(debug) printf("Load data file: %s,%d,%d\n",filename,data_size,read_size);
            if(debug) printf("Load data file: %s\n",filename);
		}
      fclose(handler);
    }

    return read_size;
}

void hexDump (const char * desc, const void * addr, const int len) {
    int i;
    unsigned char buff[17];
    const unsigned char * pc = (const unsigned char *)addr;

    if (desc != NULL)
        printf ("%s:\n", desc);

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    else if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    for (i = 0; i < len; i++) {
         if ((i % 16) == 0) {
            if (i != 0)
                printf ("  %s\n", buff);

            printf ("  %04x ", i);
        }
		printf (" %02x", pc[i]);

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }
	while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }
    printf ("  %s\n", buff);
}

char *rand_string(char *str, size_t size)
{
	time_t t;
	srand((unsigned) time(&t));
    const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof(charset) - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

// You must free the result if result is non-NULL.
char *replaceWord(const char *s, const char *oldW, const char *newW) 
{ 
    char *result; 
    int i, cnt = 0; 
    int newWlen = strlen(newW); 
    int oldWlen = strlen(oldW); 
 
    for (i = 0; s[i] != '\0'; i++) 
    { 
        if (strstr(&s[i], oldW) == &s[i]) 
        { 
            cnt++; 
            i += oldWlen - 1; 
        } 
    } 
  
    result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1); 
  
    i = 0; 
    while (*s) 
    { 
        // compare the substring with the result 
        if (strstr(s, oldW) == s) 
        { 
            strcpy(&result[i], newW); 
            i += newWlen; 
            s += oldWlen; 
        } 
        else
            result[i++] = *s++; 
    } 
  
    result[i] = '\0'; 
    return result; 
} 

const char *replaceWords(int n,...)
{
	n=n*2+1;
	va_list arg;
	va_start(arg, n);

	const char * orig=va_arg(arg,const char *);
	const char * ret=NULL;
	const char * tmp=NULL;

	for (int i=1;i<n;i+=2)
  	{
  		
    	const char * old=va_arg(arg,const char *);
    	const char * new=va_arg(arg,const char *);
    	if(ret!=NULL)
    	{ 
    		tmp=replaceWord(ret,old,new);
    		free((void*)ret);
    		ret=tmp;
    	}
    	else ret=replaceWord(orig,old,new);
  	}
	va_end(arg);

	return ret;
}

char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1);
    
    if(result!=NULL)
    {
        memcpy(result, s1, len1);
        memcpy(result + len1, s2, len2 + 1);
    }
    
    return result;
}

char* concat3(const char *s1, const char *s2, const char *s3)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    const size_t len3 = strlen(s3);
    
    char *result = malloc(len1 + len2 + len3 + 1);
    
    if(result!=NULL)
    {
        memcpy(result, s1, len1);
        memcpy(result + len1, s2, len2);
        memcpy(result + len1 + len2, s3, len3 + 1); // +1 to copy the null-terminator
    }
    
    return result;
}

//==============================================================================
//   PROTEO
//==============================================================================

/*static int require (lua_State *state) {
	
	const char* lib = luaL_checkstring(state,1);
	printf("require %s\n",lib);

	luaL_dofile(state,lib2path(lib));

	return 1;
}*/

//==============================================================================
//   TEST
//==============================================================================
void* thread_poll_event(void* arg)
{

	return NULL;
}

//==============================================================================
//   EXIT_CLI
//==============================================================================
void exit_cli(int sig)
{
	run = FALSE;
	printf("\rproteo_server closing down...\n\n");
}

//==============================================================================
//   PRINTHELP
//==============================================================================
void printhelp()
{
	printf("Usage: proteo_server [vh] \n");
	printf("Options:\n");
	printf("\tv: verbose mode\n");
	printf("\th: show this help\n");
	printf("\n\n");
}

//==============================================================================
//   PROTEO_LIB
//==============================================================================
LUALIB_API int luaopen_proteo (lua_State *state) {
	
	lua_newtable(state);
	
	lua_newtable(state);
	lua_setfield(state, -2, "system");

	lua_newtable(state);
	lua_setfield(state, -2, "network");

	lua_newtable(state);
	lua_setfield(state, -2, "sqlite");

	lua_newtable(state);
	lua_setfield(state, -2, "darknet");

	lua_newtable(state);
	lua_setfield(state, -2, "debug");

    lua_newtable(state);
    lua_setfield(state, -2, "zmq");
    
	lua_newtable(state);
	lua_setfield(state, -2, "enet");

    lua_newtable(state);
    lua_setfield(state, -2, "opencv");
    
	lua_newtable(state);
	lua_setfield(state, -2, "ejdb");
	
    lua_newtable(state);
    lua_setfield(state, -2, "ffmpeg");
    
    lua_newtable(state);
    lua_setfield(state, -2, "tflite");
    
	lua_setglobal(state,"proteo");

	add_system_proteo(state);
    add_zmq_proteo(state);
    add_ffmpeg_proteo(state);
    add_tensorflow_proteo(state);
    
  return 1;
}

void addFunction_proteo(lua_State *state,const char *lib,const char *fname,lua_CFunction f)
{
	lua_getglobal(state,"proteo");
	lua_getfield(state,-1,lib);
	lua_pushcfunction(state,f);
	lua_setfield(state, -2, fname);
	lua_pop(state,2);
}

void addTable_proteo(lua_State *state,const char *lib,const char *tname)
{
	lua_getglobal(state,"proteo");
	lua_getfield(state,-1,lib);
	lua_newtable(state);
	lua_setfield(state, -2, tname);
	lua_pop(state,2);
}

void addEnum_proteo(lua_State *state,const char *lib,const char *enumname,const char *vname,int value)
{
	lua_getglobal(state,"proteo");
	lua_getfield(state,-1,lib);
	lua_getfield(state,-1,enumname);
	lua_pushinteger(state,value);
	lua_setfield(state, -2, vname);
	lua_pop(state,3);
}

//==============================================================================
//   INITLUA
//==============================================================================
int initLUA()
{
    //sem_init(&lua_mutex,0,1);
    sem_unlink("/lua_call_sem");
    lua_sem = sem_open ("/lua_call_sem", O_CREAT , 0644, 1);//| O_EXCL
    if(lua_sem==SEM_FAILED)
    {
        printf("Sem_open: Failed: %s!\n",strerror(errno));
    }
    L = luaL_newstate();//lua_open();

    luaL_openlibs(L);

    /*lua_getglobal(L, "require");
    lua_setglobal(L, "local_require");
    lua_pushcfunction(L,require);
	lua_setglobal(L, "require");*/
	//lua_pop(L,2);

	luaopen_proteo(L);

	//addFunction_proteo(L,"media","bhrDiscover",media_bhrDiscover);
	
    #ifdef PROTEO_OPENCV
        addFunction_proteo(L,"opencv","imread",opencv_imread);
        addFunction_proteo(L,"opencv","imwrite",opencv_imwrite);
        addFunction_proteo(L,"opencv","videocapture",opencv_videocapture);
        addFunction_proteo(L,"opencv","videowriter",opencv_videowriter);
        addFunction_proteo(L,"opencv","write",opencv_write);
        addFunction_proteo(L,"opencv","frame",opencv_frame);
        addFunction_proteo(L,"opencv","readnet",opencv_readnet);
        addFunction_proteo(L,"opencv","forward",opencv_forward);
        addFunction_proteo(L,"opencv","forwardTable",opencv_forwardTable);
        //addFunction_proteo(L,"opencv","getpoints",opencv_getpoints);
        addFunction_proteo(L,"opencv","circle",opencv_circle);
        addFunction_proteo(L,"opencv","rectangle",opencv_rectangle);
        addFunction_proteo(L,"opencv","sliceImg",opencv_sliceImg);
        addFunction_proteo(L,"opencv","minMaxLoc",opencv_minMaxLoc);
        addFunction_proteo(L,"opencv","img",opencv_img);
        addFunction_proteo(L,"opencv","line",opencv_line);
        addFunction_proteo(L,"opencv","putText",opencv_putText);
        addFunction_proteo(L,"opencv","size",opencv_size);
        addFunction_proteo(L,"opencv","cropImg",opencv_cropImg);
        addFunction_proteo(L,"opencv","infoImg",opencv_infoImg);
        addFunction_proteo(L,"opencv","infoNet",opencv_infoNet);
        addFunction_proteo(L,"opencv","setImg",opencv_setImg);
        addFunction_proteo(L,"opencv","setSize",opencv_setSize);
        addFunction_proteo(L,"opencv","fill",opencv_fill);
        addFunction_proteo(L,"opencv","resize",opencv_resize);
        addFunction_proteo(L,"opencv","mul",opencv_mul);
        addFunction_proteo(L,"opencv","add",opencv_add);
        addFunction_proteo(L,"opencv","addWeighted",opencv_addWeighted);
        addFunction_proteo(L,"opencv","flip",opencv_flip);
        addFunction_proteo(L,"opencv","threshold",opencv_threshold);
        addFunction_proteo(L,"opencv","print",opencv_print);
        addFunction_proteo(L,"opencv","setFrameSize",opencv_setFrameSize);
        addFunction_proteo(L,"opencv","getFrameSize",opencv_getFrameSize);
        addFunction_proteo(L,"opencv","setBufferSize",opencv_setBufferSize);
        addFunction_proteo(L,"opencv","imencode",opencv_imencode);
        addFunction_proteo(L,"opencv","imdecode",opencv_imdecode);
        addFunction_proteo(L,"opencv","convert",opencv_convert);
        addFunction_proteo(L,"opencv","getAffineTransform",opencv_getaffinetransform);
        addFunction_proteo(L,"opencv","warpAffine",opencv_warpaffine);
        addFunction_proteo(L,"opencv","toTable",opencv_totable);
    
        addTable_proteo(L,"opencv","matType");
        addEnum_proteo(L,"opencv","matType","CV_8U",0);
        addEnum_proteo(L,"opencv","matType","CV_8S",1);
        addEnum_proteo(L,"opencv","matType","CV_16U",2);
        addEnum_proteo(L,"opencv","matType","CV_16S",3);
        addEnum_proteo(L,"opencv","matType","CV_32S",4);
        addEnum_proteo(L,"opencv","matType","CV_32F",5);
        addEnum_proteo(L,"opencv","matType","CV_64F",6);
        addEnum_proteo(L,"opencv","matType","CV_16F",7);
    
        addTable_proteo(L,"opencv","threshType");
        addEnum_proteo(L,"opencv","threshType","THRESH_BINARY",0);
        addEnum_proteo(L,"opencv","threshType","THRESH_BINARY_INV",1);
        addEnum_proteo(L,"opencv","threshType","THRESH_TRUNC",2);
        addEnum_proteo(L,"opencv","threshType","THRESH_TOZERO",3);
        addEnum_proteo(L,"opencv","threshType","THRESH_TOZERO_INV",4);
    
        addTable_proteo(L,"opencv","fourcc");
        addEnum_proteo(L,"opencv","fourcc","MJPG",0);
    
    #endif
    
	addFunction_proteo(L,"sqlite","exe",sqlite_exe);
	
	addFunction_proteo(L,"ejdb","exe",ejdb_lua_exe);
	//addFunction_proteo(L,"ejdb","new",ejdb_lua_new);
	addFunction_proteo(L,"ejdb","del",ejdb_lua_del);
	addFunction_proteo(L,"ejdb","get",ejdb_lua_get);
	addFunction_proteo(L,"ejdb","put",ejdb_lua_put);
	addFunction_proteo(L,"ejdb","merge",ejdb_lua_merge);
    addFunction_proteo(L,"ejdb","patch",ejdb_lua_patch);
	
	//addFunction_proteo(L,"network","download",network_download);
	addFunction_proteo(L,"network","get",network_get);
	addFunction_proteo(L,"network","post",network_post);
	addFunction_proteo(L,"network","getInterfaces",network_getInterfaces);
	//addFunction_proteo(L,"network","put",network_put);
	//addFunction_proteo(L,"network","delete",network_delete);

	//addFunction_proteo(L,"debug","traceback",traceback);
	

	addFunction_proteo(L,"enet","linked_version",linked_version);
	addFunction_proteo(L,"enet","host_create",host_create);

	//printf("GetTop %d\n",lua_gettop(L));
	// create metatables
	luaL_newmetatable(L, "enet_host");
	lua_newtable(L); // index
	luaL_register(L, NULL, enet_host_funcs);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, host_gc);
	lua_setfield(L, -2, "__gc");
	lua_pop(L,1);
	
	luaL_newmetatable(L, "enet_peer");
	lua_newtable(L);
	luaL_register(L, NULL, enet_peer_funcs);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, peer_tostring);
	lua_setfield(L, -2, "__tostring");
	lua_pop(L,1);
	
	// set up peer table
	lua_newtable(L);

	lua_newtable(L); // metatable
	lua_pushstring(L, "v");
	lua_setfield(L, -2, "__mode");
	lua_setmetatable(L, -2);

	lua_setfield(L, LUA_REGISTRYINDEX, "enet_peers");

	if(debug) printf("initLua GetTop %d\n",lua_gettop(L));

    char* package_path=concat3("package.path = '",config.basedir,"lib/?.lua;' .. package.path");
    luaL_dostring(L, package_path);
    free(package_path);
    
    lua_pushlstring(L,config.basedir,strlen(config.basedir));
    lua_setglobal(L, "BASEDIR");
    
    lua_pushlstring(L,config.baseurl,strlen(config.baseurl));
    lua_setglobal(L, "BASEURL");
    
    lua_pushinteger(L,config.port);
    lua_setglobal(L, "PORT");
    
	//luaL_dostring(L, "package.path = './lib/?.lua;' .. package.path");
    char* route_path=concat(config.basedir,"route.lua");
	luaL_dofile(L,route_path);
    free(route_path);
    
    for (int i = 0; i < config.plugin_count; i++) {
        printf("Init Plug: %s\n",config.plugins[i].name);
        //init(plug_name); //proteo_lua.c
        init(L,config.plugins[i].name);
    }
    
	return 1;
}

const char* app_call(int type,const char* url,char* data,char* permissions,char* username,const char* token)
{
    sem_wait(lua_sem);
	lua_getglobal(L,"route_call");
	/*int type=0;
	if(0 == strcmp (method, "GET") ) type=GET;
	else if(0 == strcmp (method, "POST") ) type=POST;
	else if(0 == strcmp (method, "PUT") ) type=PUT;
	else if(0 == strcmp (method, "DELETE") ) type=DELETE;*/

	//printf("ROUTE: %s\n",url);
	
	//lua_pushcfunction(L, traceback);
	
	lua_pushinteger(L, type); //Type
	lua_pushlstring(L,url,strlen(url)); //Url
	if(data!=NULL) lua_pushlstring(L,data,strlen(data)); //Data
	else lua_pushlstring(L,"",0);
	lua_pushlstring(L,permissions,strlen(permissions)); //Permessi
	lua_pushlstring(L,username,strlen(username)); //Username
	lua_pushlstring(L,token,strlen(token)); //Token	
	
	int error = lua_trace_pcall(L, 6, 1);
	if (error) {
        printf("ERROR pcall(route.call): %s\n", lua_tostring(L, -1));
		//lua_pop(L, 1);
        if(debug) printf("App_call GetTop %d\n",lua_gettop(L));
        sem_post(lua_sem);
        return NULL;
        
	}

	const char* ret=NULL;
	int iType = lua_type(L, -1);
	switch (iType)
	{
		case LUA_TSTRING:
            ret = lua_tostring(L, -1);//Oppure 1?
            lua_pop(L, 1);
			break;
		default:
			printf("WHAT?!?!?!\n");
	}

	if(ret==NULL){
		//ret="?!"; //Non dovrebbe capitare mai
		printf("App_call error: %d %s\n",type,url);
	}
    int top=lua_gettop(L);
    if(top>0)
        lua_pop(L,top);
	
    if(debug) printf("App_call GetTop %d\n",top);
    
	//lua_pop(L, 1);
    sem_post(lua_sem);
    
    if(paused && toreboot>=0)
    {
        if(debug) printf("<<<<<<< Reboot (app_call) \n");
        system_reboot(L); //Reboot after finish current call
    }
    
	return ret;
}

//==============================================================================
//   REST
//==============================================================================

struct MHD_Response* parse_info(struct connection_info_struct *con_info,const char* token ,const char* hmac)
{
	struct MHD_Response *response;
	
    if(paused)
    {
        char *guru = "<html><head><title>Guru Meditation!</title></head>"
                 "<body bgcolor='000000' textcolor='ff0000'><center><a href='#'>"
                 "<img src='https://upload.wikimedia.org/wikipedia/commons/d/db/Guru_meditation.gif'>"
                 "</a></center><br></body></html>";

        response = MHD_create_response_from_buffer (strlen (guru),(void*) guru, MHD_RESPMEM_MUST_COPY);
        return response;
    }
    
	if (con_info->type == POST && 0 == strcmp (con_info->url, "/login") && config.master==1)
	{
		printf("POST URL: %s\n",con_info->url);
		printf("Data: %s\n",con_info->data);
		
		if(con_info->data!=NULL && strlen(con_info->data)!=con_info->size)
		{
            printf("ERROR proteo_login: %lu!=%zu \n",strlen(con_info->data),con_info->size);
			printf("%s \n",con_info->data);
		}
		response = proteo_login(con_info->data,hmac);

	}
	/*else if (con_info->type == GET && 0 == strcmp (con_info->url, "/login") && config.master==1)
	{
		const char *page  = "<html><body>Hello, browser!</body></html>";
  		response = MHD_create_response_from_buffer (strlen (page),(void*) page, MHD_RESPMEM_PERSISTENT);
	}*/
	else if (con_info->type == GET && 0 == strcmp (con_info->url, "/admin") && config.admin_enabled==1)
	{
		printf("GET URL: %s\n",con_info->url);
		printf("Data: %s\n",con_info->data);

  		response = proteo_getadmin();
	}
    else if (con_info->type == GET && 0 == strcmp (con_info->url, "/edit") && config.admin_enabled==1)
    {
        printf("GET URL: %s\n",con_info->url);
        printf("Data: %s\n",con_info->data);

        char* path=concat(config.basedir,"web/template_login.html");
        
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
          
         // return response;
    }
    else if (con_info->type == POST && 0 == strcmp (con_info->url, "/edit") && config.admin_enabled==1)
    {
        printf("POST URL: %s\n",con_info->url);
        printf("Data: %s\n",con_info->data);

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
        
        //TODO non basta utente e password, ma l'utente deve avere anche il permesso corretto (che ancora non è stato definito)
        if (login_correct==1)
        {
            char* path=concat(config.basedir,"web/blockly/index.html");
            char *page=loadfile(path);
            free(path);
           
            //printf("LIBS: %s\n",libs);
            char token[512];
          //char Tickets[512];
          json_object *jarray_ticket;
          createTokenAndTicket(username, "edit",token,&jarray_ticket);
          //printf("TOKEN: %s\n",token);

          //const char* script=proteo_get_script(out[project_index][KVP_VALUE],clientkey,token);
          //printf("Script: %s\n",script);

          const char *newpage=replaceWord(page,"##TOKEN##",token);
          //printf("PAGE: %s\n",newpage);
            free((void*)page);


            response = MHD_create_response_from_buffer (strlen (newpage),(void*) newpage, MHD_RESPMEM_MUST_COPY);
            free((void*)newpage);
        }
        if(username!=NULL) free(username);
    }
	else if (con_info->type == POST && 0 == strcmp (con_info->url, "/register") && config.admin_enabled==1)
	{
		printf("POST URL: %s\n",con_info->url);
		printf("Data: %s\n",con_info->data);
	
        const char* json=app_call(POST,"/admin/register",con_info->data,"_ALL","_MASTER","_TOKEN");
        
        response = MHD_create_response_from_buffer (strlen (json),(void*) json, MHD_RESPMEM_PERSISTENT);
        
  		//response = proteo_register(con_info);
	}
	else if (con_info->type == GET && 0 == strcmp (con_info->url, "/info") )
	{
		printf("GET URL: %s\n",con_info->url);
		printf("Data: %s\n",con_info->data);
		//const char* hmac = MHD_lookup_connection_value (connection, MHD_HEADER_KIND, "X-HMAC");
		//printf("HMAC: %s\n",hmac);
		
		//TODO
		//if(verifyHMAC(url,con_info->data,token,serverkey,hmac)==0)

		response = proteo_info();
	}
	else if (con_info->type == GET && 0 == strcmp (con_info->url, "/favicon.ico") )
	{
        char* path=concat(config.basedir,"favicon.ico");
		const char *ico  = NULL;
		int ico_size=loaddatafile(path,&ico);
        if(ico!=NULL)
        {
            response = MHD_create_response_from_buffer (ico_size,(void*) ico, MHD_RESPMEM_MUST_COPY);
            free((void*)ico);
        }
        free(path);
	}
	else
	{
		if(verbose) printf("URL: %s\n",con_info->url);
		if(verbose) printf("Data: %s\n",con_info->data);
		
		//const char* token = MHD_lookup_connection_value (connection, MHD_HEADER_KIND, "X-TOKEN");
		//const char* hmac = MHD_lookup_connection_value (connection, MHD_HEADER_KIND, "X-HMAC");
		//printf("Token is: %s\n",token);

		char tmp[256];
		strcpy(tmp,con_info->url);
		char* app=strtok(tmp+1, "/");

		if(app!=NULL && token!=NULL && hmac!=NULL && strlen(app)!=0 && strlen(token)!=0 && strlen(hmac)!=0)
		{		
			char permissions[512];
			char username[100];
			
			if(verifyToken(username,permissions,app,token)==0 && verifyHMAC(con_info->url,con_info->data,token,clientkey,hmac)==0)
			{
				if(verbose) printf("Token and HMAC ok\n");
				//luaL_dostring(L, "package.path = './lib/?.lua;' .. package.path");
				
				const char* json=app_call(con_info->type,con_info->url,con_info->data,permissions,username,token);
				//if(debug) printf("\nRESPONSE: %s\n",json);
				//const char *page  = "<html><body>Hello, browser!</body></html>";
				if(json==NULL ) { printf("APPCALL ERROR %s\n", con_info->url); json="{\"result\":\"ERROR\"}";}
		  		response = MHD_create_response_from_buffer (strlen (json),(void*) json, MHD_RESPMEM_PERSISTENT);
			}
			else
			{
				printf("Token or HMAC error\n");
				const char *page  = "{\"result\":\"Token or HMAC error\"}"; //TODO messaggio di errore in json
  				response = MHD_create_response_from_buffer (strlen (page),(void*) page, MHD_RESPMEM_PERSISTENT);
  				if(response==NULL)
  				{
  					printf("MHD_create_response_from_buffer ERROR\n");
  				}
			}
		}
		else
		{
            if(strlen(con_info->url)>1)
            {
                char* path=concat3(config.basedir,"web",con_info->url);

                int exist=access( path, F_OK ) != -1;

                if(exist==1)
                {
                    if(verbose) printf("Web file: %s\n",path);
                    //char *page=loadfile(path);
                    const char *page  = NULL;
					int page_size=loaddatafile(path,&page);
                    response = MHD_create_response_from_buffer (page_size,(void*) page, MHD_RESPMEM_MUST_FREE);//MHD_RESPMEM_MUST_COPY);
                    //free(page);
                    const char* ext = strrchr(path, '.');
                    //if(verbose) printf("Ext: %s \n",ext);
                    if(strcmp(ext,".js")==0)
                    {
                        if(verbose) printf("Content-Type: text/javascript \n");
                        MHD_add_response_header(response, "Content-Type", "text/javascript");
                    }
                    else if(strcmp(ext,".png")==0)
                    {
                        if(verbose) printf("Content-Type: image/png \n");
                        MHD_add_response_header(response, "Content-Type", "image/png");
                    }
                    else if(strcmp(ext,".html")==0)
                    {
                        if(verbose) printf("Content-Type: text/html \n");
                        MHD_add_response_header(response, "Content-Type", "text/html");
                    }
                    else if(strcmp(ext,".css")==0)
                    {
                        if(verbose) printf("Content-Type: text/css \n");
                        MHD_add_response_header(response, "Content-Type", "text/css");
                    }
                    else if(strcmp(ext,".wasm")==0)
                    {
                        if(verbose) printf("Content-Type: text/css \n");
                        MHD_add_response_header(response, "Content-Type", "application/wasm");
                    }
                    else
                        MHD_add_response_header(response, "Content-Type", "application/octet-stream");
                    
                }
                else
                {
                    if(verbose) printf("Page not found: %s\n",con_info->url);
                    
                    const char *page  = "<html><body>404!</body></html>";
                    response = MHD_create_response_from_buffer (strlen (page),(void*) page, MHD_RESPMEM_PERSISTENT);
                }
                free(path);
            }
            else
            {
				char* path=concat(config.basedir,"web/index.html");
				char *page=loadfile(path);
				
				//char *newpage=replaceWords(2,page,"##DEFAULT_SERVER##", config.local,"##DEFAULT_SSL##", config.ssl);
				//free(page);
  	
                //const char *page  = "<html><body>404!</body></html>";
                response = MHD_create_response_from_buffer (strlen (page),(void*) page, MHD_RESPMEM_MUST_FREE);
                //free(page);
                MHD_add_response_header(response, "Content-Type", "text/html");
				free(path);
            }
		}
	}
	
	return response;
}

int url_handler (void *cls, 
				struct MHD_Connection *connection,
                const char *url,
                const char *method, const char *version,
                const char *upload_data,
                size_t *upload_data_size, void **con_cls)
{
	if (NULL == *con_cls)
    {
    	//printf("1 url: %s \n",url);
    	struct connection_info_struct *con_info;

		con_info = malloc (sizeof (struct connection_info_struct));

        con_info->size=0;
		con_info->data=NULL;
    	/*con_info->postprocessor=MHD_create_post_processor (connection,
                                   POSTBUFFERSIZE,
                                   &iterate_post,
                                   (void *) con_info);

		if (NULL == con_info->postprocessor)
        {
        	printf("Error MHD_create_post_processor \n");
           	free (con_info);
            return MHD_NO;
        }*/

        //nr_of_uploading_clients++;
        /*if (0 == strcmp (method, "POST")) con_info->type=POST;
        else if (0 == strcmp (method, "GET")) con_info->type=GET;
        else if (0 == strcmp (method, "PUT")) con_info->type=PUT;
        else if (0 == strcmp (method, "DELETE")) con_info->type=DELETE;*/

		con_info->url=malloc(strlen(url)+1);
		strlcpy(con_info->url,url,strlen(url)+1);
		
     	*con_cls = (void *)con_info;

    	return MHD_YES;
    }

    //struct MHD_Response *response;
	struct connection_info_struct *con_info = *con_cls;
	
    if(*upload_data_size!=0)
    {
    	//printf("data: %s \n",upload_data);

		//if (MHD_YES != MHD_post_process (con_info->postprocessor,upload_data,*upload_data_size))
        //{
        	//printf("4 url: %s \n",url);
            //con_info->data = "error";
            //con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
        //}
        if(*upload_data_size>0)
        {
            if(con_info->data==NULL)
                con_info->data = (char *)malloc( ((*upload_data_size)+1) * sizeof(char));
            else
                con_info->data = realloc(con_info->data, con_info->size+(*upload_data_size)+1);
        	if(con_info->data!=NULL)
        	{
        		strncpy(con_info->data+con_info->size,upload_data,*upload_data_size);
        		con_info->size += *upload_data_size;
                con_info->data[con_info->size]=0;
        	}
        	*upload_data_size = 0;
    	}
        return MHD_YES;
    }
    if (0 == strcmp (method, "POST")) con_info->type=POST;
    else if (0 == strcmp (method, "GET")) con_info->type=GET;
    else if (0 == strcmp (method, "PUT")) con_info->type=PUT;
    else if (0 == strcmp (method, "DELETE")) con_info->type=DELETE;
    
	const char* token = MHD_lookup_connection_value (connection, MHD_HEADER_KIND, "X-TOKEN");
	const char* hmac = MHD_lookup_connection_value (connection, MHD_HEADER_KIND, "X-HMAC");
	struct MHD_Response *response=parse_info(con_info,token,hmac);
	
	MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	/*if(allow_origin) {
		MHD_add_response_header(response, "Access-Control-Allow-Credentials", "true");
		MHD_add_response_header(response, "Vary", "Origin");
	}
	MHD_add_response_header(response, "Access-Control-Max-Age", "86400");
	MHD_add_response_header(response, "Access-Control-Allow-Methods", msg->acrm);*/
	MHD_add_response_header(response, "Access-Control-Allow-Headers", "*");
	MHD_add_response_header(response, "Access-Control-Expose-Headers", "*");
	
	if (con_info->data!=NULL)
	{
		if(verbose) printf("FREE INFO DATA\n");
		free (con_info->data);
		con_info->data=NULL;
	}
	free(con_info->url);
	free (con_info);
	
 	int ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
 	MHD_destroy_response (response);

	return ret; 
}

void request_completed (void *cls, struct MHD_Connection *connection,
                   void **con_cls, enum MHD_RequestTerminationCode toe)
{
  struct connection_info_struct *con_info = *con_cls;
  (void)cls;         /* Unused. Silent compiler warning. */
  (void)connection;  /* Unused. Silent compiler warning. */
  (void)toe;         /* Unused. Silent compiler warning. */

	printf("request_completed\n");

	if (NULL == con_info)
    	return;

	//MHD_destroy_post_processor (con_info->postprocessor);
	/*if (con_info->data!=NULL)
	{
		free (con_info->data);
		con_info->data=NULL;
	}
	free(con_info->url);*/
	//free(con_info);
  
  *con_cls = NULL;
}




static void panicCallback(void* cls, const char* file, unsigned int line, const char* reason)
{
    printf("PANIC %d : %s - %s",line,file,reason);
}

//==============================================================================
//   MAIN
//==============================================================================
int main(int argc,char **argv)
{
	int c;
	char config_path[256];
	strcpy(config_path,"./config.json");

	while ((c = getopt (argc, argv, "vhdc:")) != -1)
    	switch (c)
		{
			case 'v': //verbose mode
				verbose=TRUE;
			break;
			
			case 'd': //debug mode
				debug=TRUE;
			break;
			
			case 'c':
				printf("Config path: %s\n",optarg);
				strcpy(config_path,optarg);
			break;

			case 'h': //print help
				printhelp();
				return 0;
			break;

			case '?':
				if (optopt == 't')
          				fprintf (stderr, "Option -%c requires Lua file.\n", optopt);
        			else if (isprint (optopt))
          				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        			else
          				fprintf (stderr,
                   			"Unknown option character `\\x%x'.\n",
                   			optopt);
        			return 1;
			default:
				abort();
		}

	//if(opt_appkey==FALSE) return 0;

	if(optind<argc)
	{
		printhelp();
		return 0;
	}
	
	//========================== START
    if(load_config(config_path) !=0)
    {
        printf( "Failed to load config!\n" );
        return 1;
    }
    
    enet_initialize();
	if( initLUA() == FALSE)  //TODO è possibile inizializzare tante macchine LUA quanti sono i plugin in modo da tenerli separati?
	{
		printf( "Failed to initialize LUA!\n" );
		return 1;
	}
	
	iwrc rc = ejdb_init();
	if (rc) {
		iwlog_ecode_error3(rc);
		printf( "Failed to initialize EJDB!\n" );
		return 1;
	}
	
	//pthread_t poll_event;

	if(config.master==1)
	{
        char* aut_user_path=concat(config.basedir,"auth_user.db");
		if(access(aut_user_path, F_OK ) == -1)
			create_auth_user_db(aut_user_path);

		printf("Proteo server master v%s\n",config.version);
        free(aut_user_path);
	}
	else
	{
		printf("Proteo server slave v%s\n",config.version);
	}
#ifdef LUAJIT_VERSION
    printf("%s\n",LUAJIT_VERSION);
#else
    printf("%s\n",LUA_VERSION);
#endif
	printf("JSON v.%s\n",JSON_C_VERSION);
    printf("TensorFlowLite v.%s\n", TfLiteVersion());
    
	MHD_set_panic_func(&panicCallback, NULL);

	struct MHD_Daemon *daemon;
  	char *key_pem=NULL;
  	char *cert_pem=NULL;

	if(config.ssl==0)
	{
  		daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, 8888,
  							 NULL, NULL,
                             &url_handler,NULL,
                             MHD_OPTION_NOTIFY_COMPLETED, &request_completed,
                             NULL, MHD_OPTION_END);
  	}
  	else
  	{
        loaddatafile (config.ssl_key,&key_pem);
  		loaddatafile (config.ssl_cert,&cert_pem);

  		if(verbose) printf("KEY: \n%s\n",key_pem);
  		if(verbose) printf("CERT: \n%s\n",cert_pem);

  		if ((key_pem == NULL) || (cert_pem == NULL))
  		{
    		printf ("The key/certificate files could not be read.\n");
    		return 1;
  		}

  		daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_DEBUG | MHD_USE_SSL, 8888,
  							 NULL, NULL,
                             &url_handler,NULL,
                             MHD_OPTION_NOTIFY_COMPLETED, &request_completed,NULL,
                             MHD_OPTION_HTTPS_MEM_KEY, key_pem,
                             MHD_OPTION_HTTPS_MEM_CERT, cert_pem,
                             MHD_OPTION_END);
  	}
  	if (NULL == daemon)
  	{
  		printf("MHD_start_daemon error\n");
  	 	//return 1;
  	}

	if(config.master==1)
		get_server_info();
	
	//lua_pushlstring(L,username,strlen(username));
	//lua_setglobal(L, "USERNAME");

	signal(SIGINT,exit_cli);
	signal(SIGTERM,exit_cli);

	run = TRUE;
    paused = FALSE;
	
	/*if(pthread_create(&poll_event,NULL,thread_poll_event,optarg))
	{
		printf("error thread_poll_event\n");
	}*/

	while(run)
	{
		struct timespec now;
		clock_gettime(CLOCK_REALTIME, &now);
		double n=now.tv_sec*1000;
		n=n+(now.tv_nsec/1000000.0);
		
        if(paused && toreboot>=0)
        {
            if(debug) printf("<<<<<<< Reboot (timer) \n");
            system_reboot(L);
        }
        
        ProteoTimer* timer=timers;
        double min=1000;
		while(timer!=NULL)
		{
            if(timer->state==1)
            {
                //TODO se sono tutti disabilitati si può sospendere questo while fino al prossimo "timer.start"
                if(timer->time<=(n-timer->last))
                {
                    if(paused) continue;
                    
                    sem_wait(lua_sem);
                    if(timer->callback!=NULL)
                        lua_getglobal(L,timer->callback);
                    else if(timer->ref_callback!=-1)
                        lua_getref(L,timer->ref_callback);

                    if(lua_isfunction(L, -1) )
                    {
                        lua_pushinteger(L, n-timer->last);
                        int error = lua_trace_pcall(L,1,0);//lua_pcall(L, 1, 0, 0);
                        
                        if (error) {
                            //printf("ERROR pcall(timer->callback)\n");
                            printf("ERROR pcall(timer->callback): %s\n",lua_tostring(L, -1));
                            //lua_pop(L, 1);
                            int top=lua_gettop(L);
                            if(top>0)
                                lua_pop(L,top);
                            if(debug) printf("app_call GetTop %d\n",top);
                        }
                    }
                    timer->last=n;
                    sem_post(lua_sem);
                }
                else
                {
                    double next=timer->time -(n-timer->last);
                    if(next<min) min=next;
                }
            }
			timer=timer->next;
		}
        if(min>0) usleep((unsigned int)min);

		/*lua_getglobal(L,"event_call");
		int error = lua_pcall(L, 0, 0, 0);
		if (error) {
			fprintf(stderr, "ERROR pcall(event_call): %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		}*/
	}

	//FINISH
	//pthread_cancel(poll_event);
	
	//free(Appkey);
	enet_deinitialize();

	closeLUA();
	MHD_stop_daemon (daemon);
	if(key_pem!=NULL) free (key_pem);
    if(cert_pem!=NULL) free (cert_pem);

    freetimers();
    
	return 0;
}
