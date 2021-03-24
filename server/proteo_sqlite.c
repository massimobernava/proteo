
int sqlite_exec(const char* path,const char* sql)
{
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open(path, &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return 1;
    } 
    
    sqlite3_close(db);
    
    return 0;
}
int sqlite_create_auth_user_db(const char* path)
{

 char *sql = "DROP TABLE IF EXISTS `users`;"
			 "CREATE TABLE `users` ("
  			 "`id_user` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
  			 "`username` TEXT NOT NULL UNIQUE,"
  			 "`password` TEXT NOT NULL,"
  			 "`created_by` INTEGER DEFAULT NULL,"
  			 "`date_created` TEXT NULL DEFAULT CURRENT_TIMESTAMP,"
  			 "`updated_by` INTEGER DEFAULT NULL,"
  			 "`date_updated` TEXT NULL DEFAULT CURRENT_TIMESTAMP,"
  			 "`cancelled` INTEGER NOT NULL DEFAULT '0');"

			 "INSERT INTO `users` VALUES (1,'admin','21232f297a57a5a743894a0e4a801fc3',NULL,CURRENT_TIMESTAMP,1,CURRENT_TIMESTAMP,0);"

			"DROP TABLE IF EXISTS `permissions`;"
			 "CREATE TABLE `permissions` ("
  			 "`id_permission` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
  			 "`id_user` INTEGER NOT NULL,"
  			 "`app` TEXT NOT NULL,"
  			 "`permission` TEXT NOT NULL,"
  			 "`updated_by` INTEGER DEFAULT NULL,"
  			 "`date_updated` TEXT NULL DEFAULT CURRENT_TIMESTAMP"
  			// "`cancelled` INTEGER NOT NULL DEFAULT '0'"
             ");"

  			 "INSERT INTO `permissions` VALUES (1,1,'proteo','admin',1,CURRENT_TIMESTAMP);"
             "INSERT INTO `permissions` VALUES (2,1,'admin','admin',1,CURRENT_TIMESTAMP);"
    
             "DROP TABLE IF EXISTS `tickets`;"
             "CREATE TABLE `tickets` ("
             "`id_tick` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
             "`app` TEXT NOT NULL,"
             "`url` TEXT NOT NULL,"
             "`ticket` INTEGER NOT NULL,"
             "UNIQUE(app,url));";

    return sqlite_exec(path,sql);
}

int RunSqlNoCallback(sqlite3 * db, const char * zSql)
{
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, zSql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
        return rc;

    int rowCount = 0; //TODO ATTENZIONE Forse bisogna usare 1?
    rc = sqlite3_step(stmt);
    while (rc != SQLITE_DONE && rc != SQLITE_OK)
    {
        rowCount++;
        int colCount = sqlite3_column_count(stmt);
        for (int colIndex = 0; colIndex < colCount; colIndex++)
        {
            int type = sqlite3_column_type(stmt, colIndex);
            const char * columnName = sqlite3_column_name(stmt, colIndex);
            if (type == SQLITE_INTEGER)
            {
                int valInt = sqlite3_column_int(stmt, colIndex);
                printf("columnName = %s, Integer val = %d", columnName, valInt);
            }
            else if (type == SQLITE_FLOAT)
            {
                double valDouble = sqlite3_column_double(stmt, colIndex);
                printf("columnName = %s,Double val = %f", columnName, valDouble);
            }
            else if (type == SQLITE_TEXT)
            {
                const unsigned char * valChar = sqlite3_column_text(stmt, colIndex);
                printf("columnName = %s,Text val = %s", columnName, valChar);
                //free(valChar);
            }
            else if (type == SQLITE_BLOB)
            {
                printf("columnName = %s,BLOB", columnName);
            }
            else if (type == SQLITE_NULL)
            {
                printf("columnName = %s,NULL", columnName);
            }
        }
        printf("Line %d, rowCount = %d", rowCount, colCount);

        rc = sqlite3_step(stmt);
    }

    rc = sqlite3_finalize(stmt);

    return rc;
}




int sqlite_getString(char* dbname,char* sql,char** data)
{
	sqlite3 *db;
	sqlite3_stmt *stmt = NULL;
    //*data=NULL;
    
    int rc = sqlite3_open(dbname, &db);
    
    if (rc != SQLITE_OK) { 
    	printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return rc;
    }
        
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) { 
    	printf("Error sqlite3_prepare_v2: %s\n", sqlite3_errmsg(db)); 
    	sqlite3_close(db);
    	return rc; 
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) { 
    	printf("Error sqlite3_step: %s\n", sqlite3_errmsg(db));
    	sqlite3_close(db);
    	return rc; 
    }

    const char * d = (char*) sqlite3_column_text(stmt, 0);
    if(d==NULL)
    {
    	sqlite3_finalize(stmt);
    	sqlite3_close(db);
    	return SQLITE_ERROR;
    }

    //======================== STRING
    *data=malloc(strlen(d)+1);
    strcpy(*data,d);
    //========================

    //rc = sqlite3_step(stmt);
    rc = sqlite3_finalize(stmt);
    //rc=RunSqlNoCallback(db, sql);

    if (rc != SQLITE_OK) { 
    	printf("Error sqlite3_finalize: %s\n", sqlite3_errmsg(db));
    	sqlite3_close(db);
    	return rc; 
    }

    sqlite3_close(db);
    
    return rc;
}

//=============================================
// LUA
//=============================================
static int sqlite_exe (lua_State *state) {
  
    const char* path=luaL_checkstring(state,1);
    const char* sql=luaL_checkstring(state,2);
  
    //TODO
    //Test if exist
    //aggiungere un prefisso al nome del db per evitare che diversi plugin usino lo stesso db?

    if(debug) printf("sqlite_exe: %s\n",sql);
    
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open(path, &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 0;
    }

    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot prepare database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 0;
    }

    lua_newtable(state);

    int rowCount = 0;
    rc = sqlite3_step(stmt);
    while (rc != SQLITE_DONE && rc != SQLITE_OK)
    {
        rowCount++;
        if(debug) printf("%d: ",rowCount);
        lua_pushnumber(state,rowCount);
        lua_newtable(state);
        int colCount = sqlite3_column_count(stmt);
        for (int colIndex = 0; colIndex < colCount; colIndex++)
        {
            int type = sqlite3_column_type(stmt, colIndex);
            const char * columnName = sqlite3_column_name(stmt, colIndex);
            if (type == SQLITE_INTEGER)
            {
                int valInt = sqlite3_column_int(stmt, colIndex);
                if(debug) printf("%s = %d; ", columnName, valInt);
                
                lua_pushlstring(state,columnName,strlen(columnName));
                lua_pushinteger(state,valInt);
                lua_settable(state,-3);
            }
            else if (type == SQLITE_FLOAT)
            {
                double valDouble = sqlite3_column_double(stmt, colIndex);
                //printf("columnName = %s,Double val = %f", columnName, valDouble);
                if(debug) printf("%s = %f; ", columnName, valDouble);
                
                lua_pushlstring(state,columnName,strlen(columnName));
                lua_pushnumber(state,valDouble);
                lua_settable(state,-3);
            }
            else if (type == SQLITE_TEXT)
            {
                const char * valChar = (char *)sqlite3_column_text(stmt, colIndex);
                //printf("columnName = %s,Text val = %s", columnName, valChar);
                if(debug) printf("%s = %s; ", columnName, valChar);
                
                lua_pushlstring(state,columnName,strlen(columnName));
                lua_pushlstring(state,valChar,strlen(valChar));
                lua_settable(state,-3);

                //free(valChar);
            }
            else if (type == SQLITE_BLOB)
            {
                printf("columnName = %s,BLOB\n", columnName);
            }
            else if (type == SQLITE_NULL)
            {
                //printf("columnName = %s,NULL\n", columnName);
                if(debug) printf("%s = NULL; ", columnName);
                lua_pushlstring(state,columnName,strlen(columnName));
                lua_pushnil(state);
                lua_settable(state,-3);
            }
        }
        if(debug) printf("\n");
        //printf("  Line %d, rowCount = %d\n", rowCount, colCount);
        lua_settable(state,-3);

        rc = sqlite3_step(stmt);
    }
    
    int last_id=sqlite3_last_insert_rowid(db);
    
    if(rowCount==0)
    {
        if(debug) printf("EMPTY; LAST ID:%d\n",last_id);
        lua_pushnil(state);
    }
    
    lua_pushinteger(state, last_id);

    rc = sqlite3_finalize(stmt);
    sqlite3_close(db);

  return 2;
}
//=============================================
// TICKETS
//=============================================
int sqlite_addTicket(const char* app,const char* url,int value)
{
    char sql[256];

    snprintf(sql,256, "INSERT OR REPLACE INTO tickets (app,url,ticket) VALUES('%s','%s',%d)", app,url,value);

    //"INSERT OR REPLACE INTO cloud_plugin (app,url,value) VALUES('proteo','http://localhost:8888',1)"
    char* aut_user_path=concat(config.basedir,"auth_user.db");
    int ret=sqlite_exec(aut_user_path,sql);
    free(aut_user_path);
    return ret;
    //return sqlite_exec("auth_user.db",sql);
}

int sqlite_checkTickets(const char* app)
{
    return 0;
}
int sqlite_getTicket(const char* app,char** url)
{
    sqlite3 *db;
    char *err_msg = 0;
    //*url=NULL;
    
    char* aut_user_path=concat(config.basedir,"auth_user.db");
    int rc=sqlite3_open(aut_user_path,&db);
    free(aut_user_path);
    //int rc = sqlite3_open("auth_user.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, &err_msg);
    if (rc != SQLITE_OK) { 
        printf("Error BEGIN TRANSACTION: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1; 
    }
    char sql[256];
    //"SELECT id_tick,ticket,url FROM tickets WHERE ticket=(SELECT MAX(ticket) FROM tickets WHERE app='%s');"
    snprintf(sql,256, "SELECT id_tick,MAX(ticket),url FROM tickets WHERE app='%s';", app);

    rc = sqlite3_prepare_v2(db,sql , -1, &stmt, NULL);
    if (rc != SQLITE_OK) { 
        printf("Error sqlite3_prepare_v2: %s - %s\n", sql, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1; 
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) { 
        printf("Error sqlite3_step: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1; 
    }
    int id = sqlite3_column_int(stmt, 0);
    int ticket = sqlite3_column_int(stmt, 1);
    const char * tmp = (char *)sqlite3_column_text(stmt, 2);
    if(tmp==NULL)
    {
        printf("Error sqlite3_column_text NULL \n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }
    
    //========================= URL
    *url=malloc(strlen(tmp)+1);
    strcpy(*url,tmp);
    //=========================
    
    rc = sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) { 
        printf("Error sqlite3_finalize: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1; 
    }
    if(ticket>0)
    {
        char sql[128];
        snprintf(sql,128, "UPDATE tickets SET ticket=ticket-1 WHERE id_tick=%d", id);
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
        if (rc != SQLITE_OK) { 
            printf("Error sqlite3_exec: %s - %s\n", sql, sqlite3_errmsg(db));
            sqlite3_close(db);
            return 1; 
        }
        rc = sqlite3_exec(db, "COMMIT;", 0, 0, &err_msg);
        if (rc != SQLITE_OK) { 
            printf("Error COMMIT: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return 1; 
        }
        sqlite3_close(db);
        return 0;
    }


    rc = sqlite3_exec(db, "COMMIT;", 0, 0, &err_msg);
    if (rc != SQLITE_OK) { 
        printf("Error COMMIT: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1; 
    }
    sqlite3_close(db);
    return 1;

}

//=============================================
// AUTH
//=============================================
int sqlite_addUser(char* user,char* password)
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
    
    char sql[256];
    snprintf(sql,256, "INSERT INTO users ('username','password' ) VALUES (%s,%s)", user,md5_hex);

    char* aut_user_path=concat(config.basedir,"auth_user.db");
    return sqlite_exec(aut_user_path,sql);

}

int sqlite_getUserID(char* user,int* userID)
{
    //TODO
    return 0;
}

int sqlite_getPassword(const char* user,char** password)
{
	char sql[256];
    snprintf(sql,256, "SELECT password FROM users WHERE cancelled=0 AND username = '%s'", user);
    
	/*strcpy(sql,"SELECT password FROM users WHERE username = '");
	strcat(sql,user);
	strcat(sql,"'");*/

	//return sqlite_getString("auth_user.db",sql,password);
    char* aut_user_path=concat(config.basedir,"auth_user.db");
    int ret=sqlite_getString(aut_user_path,sql,password);
    free(aut_user_path);
    return ret;
}

int sqlite_getPermission(const char* user,const char* app,char** permission)
{
	char sql[256];
    snprintf(sql,256, "SELECT GROUP_CONCAT(permission,';') FROM permissions,users WHERE permissions.id_user = users.id_user AND username='%s' AND app='%s'", user,app);
    
	/*strcpy(sql,"SELECT permission FROM permissions,users WHERE permissions.id_user = users.id_user AND username='");
	strcat(sql,user);
	strcat(sql,"' AND app='");
	strcat(sql,app);
	strcat(sql,"'");*/

	//return sqlite_getString("auth_user.db",sql,permission);
    char* aut_user_path=concat(config.basedir,"auth_user.db");
    int ret=sqlite_getString(aut_user_path,sql,permission);
    free(aut_user_path);
    return ret;
}

int sqlite_setPassword(char* user,char* password)
{
    //TODO
    return 0;
}

int sqlite_setPermission(char* user,char* app,char* permission)
{
    //TODO
    return 0;
}

int sqlite_deleteUser(char* user,int delete)
{
    //TODO
    return 0;
}

int sqlite_deletePermission(char* user,char* app,int delete)
{
    //TODO
    return 0;
}
