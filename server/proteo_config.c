
struct plugin
{
	char name[16];
	int value;

} plugin;

struct server
{
	char url[128];
	int enabled;

} server;

struct conf
{
	char version[10];
	//char local[64];
    char basedir[256];
    char baseurl[64];
    int port;
	int master;
	struct server servers[16];
	int server_count;
	struct plugin plugins[64];
	int plugin_count;
	int admin_enabled;
	int ssl;
	char ssl_key[64];
	char ssl_cert[64];
    char server_key[33];
    char client_key[33];

} conf;

struct conf config;

int load_config(char *path_config)
{
	if(path_config==NULL) return 2;

	if(access( path_config, F_OK ) == -1)
	{
		FILE *f = fopen(path_config, "w");
		if (f == NULL)
		{
			return 1;
		}
		fprintf(f,	"{"
						"\"version\":\"0.1\","
						//"\"local\":\"http://localhost:8888\","
						"\"master\":1,"
                        "\"basedir\":\"%s/\","
                        "\"baseurl\":\"localhost\","
                        "\"port\":8888,"
						"\"ssl\":0,"
                        "\"server_key\":\"01234567890123456789012345678901\","
                        "\"client_key\":\"01234567890123456789012345678901\","
						"\"admin_enabled\":1,"
						"\"plugins\":[\"proteo\",\"admin\"],"
						"\"servers\":[\"http://remote.server:8888\"]"
					"}",dirname(path_config));
		fclose(f);
	}
	
	char* json_config=loadfile(path_config);
	//printf("JSON info: %s\n",json_info);

	json_object * jobj = json_tokener_parse(json_config);
	if(jobj==NULL)
	{
		return 1;
	}
	//json_parse(jobj);
	struct json_object *obj;

	json_object_object_get_ex(jobj, "version", &obj);
	const char* ver=json_object_get_string(obj);
	strcpy(config.version,ver);

	//json_object_object_get_ex(jobj, "local", &obj);
	//const char* lcl=json_object_get_string(obj);
	//strcpy(config.local,lcl);

    json_object_object_get_ex(jobj, "basedir", &obj);
    const char* basedir=json_object_get_string(obj);
    if(basedir!=NULL) strcpy(config.basedir,basedir);
    
    json_object_object_get_ex(jobj, "baseurl", &obj);
    const char* baseurl=json_object_get_string(obj);
    if(baseurl!=NULL) strcpy(config.baseurl,baseurl);
    
    json_object_object_get_ex(jobj, "server_key", &obj);
    const char* server_key=json_object_get_string(obj);
    if(server_key!=NULL) strcpy(config.server_key,server_key);
    
    json_object_object_get_ex(jobj, "client_key", &obj);
    const char* client_key=json_object_get_string(obj);
    if(client_key!=NULL) strcpy(config.client_key,client_key);
    
    json_object_object_get_ex(jobj, "port", &obj);
    config.port=json_object_get_int(obj);
    
	json_object_object_get_ex(jobj, "master", &obj);
	config.master=json_object_get_int(obj);

	json_object_object_get_ex(jobj, "ssl", &obj);
	config.ssl=json_object_get_int(obj);
	if(config.ssl==1)
	{
		json_object_object_get_ex(jobj, "ssl_key", &obj);
		const char* key=json_object_get_string(obj);
		strcpy(config.ssl_key,key);

		json_object_object_get_ex(jobj, "ssl_cert", &obj);
		const char* cert=json_object_get_string(obj);
		strcpy(config.ssl_cert,cert);
	}

	json_object_object_get_ex(jobj, "admin_enabled", &obj);
	config.admin_enabled=json_object_get_int(obj);

	struct json_object *plugins;
	json_object_object_get_ex(jobj, "plugins", &plugins);
	if(plugins!=NULL)
	{
		struct array_list *plug=json_object_get_array(plugins);
	
		config.plugin_count=0;
		for (int i = 0; i < array_list_length(plug); i++) {
			struct json_object *p = (struct json_object *) array_list_get_idx(plug, i);

			const char* plug_name=json_object_get_string(p);
			printf("Plug: %s\n",plug_name);
			//init(plug_name); //proteo_lua.c
			strcpy(config.plugins[i].name,plug_name);
			config.plugins[i].value=0;
			config.plugin_count++;
		}
	}
	struct json_object *servers;
	json_object_object_get_ex(jobj, "servers", &servers);
	if(servers!=NULL)
	{
		struct array_list *serv=json_object_get_array(servers);
		config.server_count=0;
		for (int j = 0; j < array_list_length(serv); j++) {
			struct json_object *s = (struct json_object *) array_list_get_idx(serv, j);

			const char* server_url=json_object_get_string(s);
			printf("Server: %s\n",server_url);
			strcpy(config.servers[j].url,server_url);
			config.server_count++;
		}
	}
	if(jobj!=NULL) json_object_put(jobj);
	free(json_config);
	
	if(verbose) printf("Load config file!\n");
	return 0;
}
