//==============================================================================
//   ZMQ
//==============================================================================

#define ZMQ_CTX "ZeroMQContext"
#define ZMQ_SOCKET "ZeroMQSocket"

//==============================================================================
//   CHECK
//==============================================================================

static void *checkCtx (lua_State *state, int index)
{
    void *context;
    luaL_checktype(state, index, LUA_TUSERDATA);
    context = *(void**)luaL_checkudata(state, index, ZMQ_CTX);
    if (context == NULL) luaL_typerror(state, index, ZMQ_CTX);
    return context;
}

static void *checkSocket (lua_State *state, int index)
{
    void *socket;
    luaL_checktype(state, index, LUA_TUSERDATA);
    socket = *(void**)luaL_checkudata(state, index, ZMQ_SOCKET);
    if (socket == NULL) luaL_typerror(state, index, ZMQ_SOCKET);
    return socket;
}

//==============================================================================
//   GC
//==============================================================================

static int ctx_gc(lua_State *state) {

    void* context = checkCtx(state, 1);

    if(context!=NULL)
    {
        printf("Context release\n");
        zmq_ctx_destroy (context);
    }

    return 0;
}

static int socket_gc(lua_State *state) {

    void* socket = checkSocket(state, 1);

    if(socket!=NULL)
    {
        printf("Socket release\n");
        zmq_close  (socket);
    }

    return 0;
}

//==============================================================================
//   PUSH
//==============================================================================


static void * pushCtx (lua_State *state)
{
    //Mat *mat = (Mat *)lua_newuserdata(L, sizeof(Mat));
    void* context = zmq_ctx_new ();
    if(context==NULL) printf("Zmq context error\n");
    
    *(void**)lua_newuserdata(state, sizeof(void*)) = context;

    if(luaL_newmetatable(state, ZMQ_CTX)){
          lua_pushcfunction(state, ctx_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return context;
}

static void * pushSocket (lua_State *state,void* context,int type)
{
    //Mat *mat = (Mat *)lua_newuserdata(L, sizeof(Mat));
    void* socket = zmq_socket  (context,type);
    if(socket==NULL) printf("Zmq socket error: %s\n",strerror(errno));
    
    *(void**)lua_newuserdata(state, sizeof(void*)) = socket;

    if(luaL_newmetatable(state, ZMQ_SOCKET)){
          lua_pushcfunction(state, socket_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return socket;
}

//==============================================================================
//   ZMQ
//==============================================================================

static int ctx_new(lua_State *state) {

    void* context = pushCtx(state);
    return 1;
}

static int socket_new(lua_State *state) {

    void *context=checkCtx(state,1);
    int type=luaL_checkint(state,2);

    void* socket=pushSocket(state,context,type);
    
    return 1;
}

static int socket_bind(lua_State *state) {

    void *socket=checkSocket(state,1);
    const char* endpoint=luaL_checkstring(state,2);

    int rc = zmq_bind (socket,endpoint);
    if(rc==-1) printf("Zmq bind error: %s\n",strerror(errno));

    return 0;
}

static int socket_unbind(lua_State *state) {

    void *socket=checkSocket(state,1);
    const char* endpoint=luaL_checkstring(state,2);

    int rc = zmq_unbind (socket,endpoint);
    if(rc==-1) printf("Zmq unbind error: %s\n",strerror(errno));

    return 0;
}

static int socket_connect(lua_State *state) {

    void *socket=checkSocket(state,1);
    const char* endpoint=luaL_checkstring(state,2);

    int rc = zmq_connect (socket,endpoint);
    if(rc==-1) printf("Zmq connect error: %s\n",strerror(errno));

    return 0;
}

static int socket_disconnect(lua_State *state) {

    void *socket=checkSocket(state,1);
    const char* endpoint=luaL_checkstring(state,2);

    int rc = zmq_disconnect (socket,endpoint);
    if(rc==-1) printf("Zmq connect error: %s\n",strerror(errno));

    return 0;
}

static int socket_send(lua_State *state) {

    //VECCHIA VERSIONE
    /*
    void *socket=checkSocket(state,1);
    const char* data=luaL_checkstring(state,2);
    int size=luaL_checkint(state,3);

    int rc = zmq_send (socket,data,size,ZMQ_DONTWAIT);
    if(rc==-1) printf("Zmq send data %s error: %s\n",data,strerror(errno));

    return 0;*/
    
    void *socket=checkSocket(state,1);
    size_t len;
    const char* data = luaL_checklstring(state, 2, &len);
    int flag = luaL_optint(state, 3, 0);
    
    int rc = zmq_send (socket,data,len,flag);
    if (rc > 0)
    {
        lua_pushinteger(L, rc);
        return 1;
    }
    else
    {
        lua_pushnil(L);
        lua_pushinteger(L, zmq_errno());
        if(flag!=ZMQ_DONTWAIT &&  verbose) printf("Zmq send data %s error: %s\n",data,strerror(errno));
        return 2;
    }
    
    //VERSIONE ONLINE
    /*void* socket = check_socket(L);
       size_t len;
       const char* data = luaL_checklstring(L, 2, &len);
       int flag = luaL_optint(L, 3, 0);
       int rc = zmq_send(socket, data, len, flag);
       if (rc > 0)
       {
           lua_pushinteger(L, rc);
           return 1;
       }
       else
       {
           lua_pushnil(L);
           lua_pushinteger(L, zmq_errno());
           return 2;
       }
    */
}

static int socket_recv(lua_State *state) {

    //VECCHIA VERSIONE
    /*
    void *socket=checkSocket(state,1);
    int size=luaL_checkint(state,2);
    
    char* data=malloc(size);

    int rc = zmq_recv (socket,data,size,ZMQ_DONTWAIT);
    
    if(rc!=-1) { data[rc]=0; lua_pushstring(state, data); }
    else
    {
        printf("Zmq recv error: %s\n",strerror(errno));
        lua_pushnil(state);
    }
    
    free(data);

    return 1;
    */
    void *socket=checkSocket(state,1);
    int flag = luaL_optint(state, 2, 0);
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    
    int rc = zmq_recvmsg(socket, &msg, flag);
    if (rc > 0)
    {
        size_t len = zmq_msg_size(&msg);
        void* data = zmq_msg_data(&msg);
        lua_pushlstring(state, (const char*)data, len);
        zmq_msg_close(&msg);
        return 1;
    }
    else
    {
        if(flag!=ZMQ_DONTWAIT &&  verbose) printf("Zmq recv error: %s\n",strerror(errno));
        lua_pushnil(state);
        zmq_msg_close(&msg);
        return 0;
    }
    
    //VERSIONE ONLINE
    /*
     void* socket = check_socket(L);
         int flag = luaL_optint(L, 2, 0);
         zmq_msg_t msg;
         zmq_msg_init(&msg);
         int rc = zmq_recvmsg(socket, &msg, flag);
         if (rc > 0)
         {
             size_t len = zmq_msg_size(&msg);
             void* data = zmq_msg_data(&msg);
             lua_pushlstring(L, (const char*)data, len);
             zmq_msg_close(&msg);
             return 1;
         }
         else
         {
             zmq_msg_close(&msg);
             return 0;
         }*/
     
}

static int socket_close(lua_State *state) {

}

//==============================================================================
//   LUA
//==============================================================================
void add_zmq_proteo(lua_State* state)
{
    addFunction_proteo(state,"zmq","ctx_new",ctx_new);
    addFunction_proteo(state,"zmq","socket_new",socket_new);
    addFunction_proteo(state,"zmq","bind",socket_bind);
    addFunction_proteo(state,"zmq","unbind",socket_unbind);
    addFunction_proteo(state,"zmq","connect",socket_connect);
    addFunction_proteo(state,"zmq","disconnect",socket_disconnect);
    addFunction_proteo(state,"zmq","send",socket_send);
    addFunction_proteo(state,"zmq","recv",socket_recv);
    addFunction_proteo(state,"zmq","socket_close",socket_close);
    
    addTable_proteo(state,"zmq","flag");
    addEnum_proteo(state,"zmq","flag","ZMQ_DONTWAIT",ZMQ_DONTWAIT);
    addEnum_proteo(state,"zmq","flag","ZMQ_SNDMORE",ZMQ_SNDMORE);
    
    addTable_proteo(state,"zmq","sockType");
    addEnum_proteo(state,"zmq","sockType","ZMQ_PAIR",ZMQ_PAIR);
    addEnum_proteo(state,"zmq","sockType","ZMQ_PUB",ZMQ_PUB);
    addEnum_proteo(state,"zmq","sockType","ZMQ_SUB",ZMQ_SUB);
    addEnum_proteo(state,"zmq","sockType","ZMQ_REQ",ZMQ_REQ);
    addEnum_proteo(state,"zmq","sockType","ZMQ_REP",ZMQ_REP);
    addEnum_proteo(state,"zmq","sockType","ZMQ_DEALER",ZMQ_DEALER);
    addEnum_proteo(state,"zmq","sockType","ZMQ_ROUTER",ZMQ_ROUTER);
    addEnum_proteo(state,"zmq","sockType","ZMQ_PULL",ZMQ_PULL);
    addEnum_proteo(state,"zmq","sockType","ZMQ_PUSH",ZMQ_PUSH);
    addEnum_proteo(state,"zmq","sockType","ZMQ_XPUB",ZMQ_XPUB);
    addEnum_proteo(state,"zmq","sockType","ZMQ_XSUB",ZMQ_XSUB);
    addEnum_proteo(state,"zmq","sockType","ZMQ_STREAM",ZMQ_STREAM);
}
