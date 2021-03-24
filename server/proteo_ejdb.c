
#define RCHECK(rc_,err)          \
  if (rc_) {                 \
    iwlog_ecode_error3(rc_); \
    printf("EJDB Error: %s",err); \
  }
  
//=============================================
// LUA
//=============================================
static int ejdb_lua_exe (lua_State *state) {
  
  const char* db_name=luaL_checkstring(state,1);
  const char* coll=luaL_checkstring(state,2);
  const char* query=luaL_checkstring(state,3);
  
  EJDB_OPTS opts = {
    .kv = {
      .path = db_name,
      //.oflags = IWKV_TRUNC
    }
  };
  EJDB db;     // EJDB2 storage handle
  
  iwrc rc = ejdb_open(&opts, &db);
  RCHECK(rc,"ejdb_open");
  
  //===================================
  EJDB_LIST list = 0;
  IWXSTR *xstr = iwxstr_new();
  rc = ejdb_list2(db,coll, query,0, &list);
  RCHECK(rc,"ejdb_list2");
  int i = 0;
  
  lua_newtable(state);
  
  for (EJDB_DOC doc = list->first; doc; doc = doc->next, ++i) {
	  
	 //jbl_as_json(doc->raw, jbl_fstream_json_printer, stderr, JBL_PRINT_PRETTY);
	  
	iwxstr_clear(xstr);
    rc = jbl_as_json(doc->raw, jbl_xstr_json_printer, xstr, 0);
    RCHECK(rc,"jbl_as_json");
      if(verbose) printf("GET id: %lld doc: %s\n",doc->id,iwxstr_ptr(xstr));

    lua_pushnumber(state,doc->id);
    lua_pushlstring(state,iwxstr_ptr(xstr),strlen(iwxstr_ptr(xstr)));
    lua_settable(state,-3);
  }
  ejdb_list_destroy(&list);
  //===================================
  
  ejdb_close(&db);
  iwxstr_destroy(xstr);

  return 1;
}

static int ejdb_lua_get(lua_State *state) {
  
  const char* db_name=luaL_checkstring(state,1);
  const char* coll=luaL_checkstring(state,2);
  int64_t id=luaL_checkinteger(state,3);
  
  EJDB_OPTS opts = {
    .kv = {
      .path = db_name,
      .oflags = IWKV_RDONLY //Sola lettura
    }
  };
  EJDB db;     // EJDB2 storage handle
  
  iwrc rc = ejdb_open(&opts, &db);
  RCHECK(rc,"ejdb_open");
  
  //===================================
	JBL jbl = 0; // Json document
	IWXSTR *xstr = iwxstr_new();
	rc = ejdb_get(db,coll,id, &jbl);
    RCHECK(rc,"ejdb_get");
	iwxstr_clear(xstr);
    if(rc==0)
    {
        rc = jbl_as_json(jbl, jbl_xstr_json_printer, xstr, 0);
        RCHECK(rc,"jbl_as_json");
        if(verbose) printf("GET id: %lld doc: %s\n",id,iwxstr_ptr(xstr));
	
        lua_pushlstring(state,iwxstr_ptr(xstr),strlen(iwxstr_ptr(xstr)));
    }
    else
    {
        const char* empty="{}";
        lua_pushnil(state);
    }
    
	jbl_destroy(&jbl);
	iwxstr_destroy(xstr);
  //===================================
  
  ejdb_close(&db);
  

  return 1;
}

static int ejdb_lua_del (lua_State *state) {
	
	  const char* db_name=luaL_checkstring(state,1);
  const char* coll=luaL_checkstring(state,2);
  
  EJDB_OPTS opts = {
    .kv = {
      .path = db_name,
      //.oflags = IWKV_TRUNC
    }
  };
  EJDB db;     // EJDB2 storage handle
  
  iwrc rc = ejdb_open(&opts, &db);
  RCHECK(rc,"ejdb_open");
  
  //===================================
  
  //===================================
  
  //if (q) jql_destroy(&q);
  //if (jbl) jbl_destroy(&jbl);
  ejdb_close(&db);
	
}

static int ejdb_lua_put (lua_State *state) {
	
	const char* db_name=luaL_checkstring(state,1);
	const char* coll=luaL_checkstring(state,2);
	const char* data=luaL_checkstring(state,3);
  
	EJDB_OPTS opts = {
		.kv = {
			.path = db_name,
			//.oflags = IWKV_TRUNC
		}
	};
	EJDB db;     // EJDB2 storage handle
  
	iwrc rc = ejdb_open(&opts, &db);
	RCHECK(rc,"ejdb_open");
  
	//===================================
	int64_t id; 
	JBL jbl = 0; // Json document
	rc = jbl_from_json(&jbl, data);
	RCHECK(rc,"jbl_from_json");
	rc = ejdb_put_new(db, coll,jbl, &id);
    if(verbose) printf("PUT collection: %s doc: %s id: %lld\n",coll, data, id);
	RCHECK(rc,"ejdb_put_new");

	/*JBL meta;
	rc = ejdb_get_meta(db, &meta);
	fprintf(stderr, "\n");
	rc = jbl_as_json(meta, jbl_fstream_json_printer, stderr, JBL_PRINT_PRETTY);
	jbl_destroy(&meta);*/
	
	lua_pushinteger(state,id);
	//===================================
  
	jbl_destroy(&jbl);
	ejdb_close(&db);
	
	return 1;
}

static int ejdb_lua_merge (lua_State *state) {
	
	  const char* db_name=luaL_checkstring(state,1);
  const char* coll=luaL_checkstring(state,2);
  const char* merge=luaL_checkstring(state,3);
  int64_t id=luaL_checkinteger(state,4);
    
  EJDB_OPTS opts = {
    .kv = {
      .path = db_name,
      //.oflags = IWKV_TRUNC
    }
  };
  EJDB db;     // EJDB2 storage handle
  
  iwrc rc = ejdb_open(&opts, &db);
  RCHECK(rc,"ejdb_open");
  
  //===================================
  
    rc = ejdb_merge_or_put(db, coll,merge, id);
    if(verbose) printf("MERGE collection: %s patch: %s id: %lld\n",coll, merge, id);
    RCHECK(rc,"ejdb_merge_or_put");
    
    lua_pushinteger(state,rc);
    
  //===================================
  
  //if (q) jql_destroy(&q);
  //if (jbl) jbl_destroy(&jbl);
  ejdb_close(&db);
	
    return 1;
}

static int ejdb_lua_patch (lua_State *state) {
    
    const char* db_name=luaL_checkstring(state,1);
    const char* coll=luaL_checkstring(state,2);
    const char* patch=luaL_checkstring(state,3);
    int64_t id=luaL_checkinteger(state,4);
    
  EJDB_OPTS opts = {
    .kv = {
      .path = db_name,
      //.oflags = IWKV_TRUNC
    }
  };
  EJDB db;     // EJDB2 storage handle
  
  iwrc rc = ejdb_open(&opts, &db);
  RCHECK(rc,"ejdb_open");
  
  //===================================
  
    rc = ejdb_patch(db, coll,patch, id);
    if(verbose) printf("PATCH collection: %s patch: %s id: %lld\n",coll, patch, id);
    RCHECK(rc,"ejdb_patch");
    
    lua_pushinteger(state,rc);
  //===================================
  
  //if (q) jql_destroy(&q);
  //if (jbl) jbl_destroy(&jbl);
  ejdb_close(&db);
    
    return 1;
}

