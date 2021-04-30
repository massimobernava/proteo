//==============================================================================
//   TENSORFLOW
//==============================================================================

#define TFLITE_MODEL "TfLiteModel"
#define TFLITE_INTERPRETEROPTIONS "TfLiteInterpreterOptions"
#define TFLITE_INTERPRETER "TfLiteInterpreter"
#define TFLITE_TENSOR "TfLiteTensor"

//==============================================================================
//   CHECK
//==============================================================================

static void *checkTFLModel (lua_State *state, int index)
{
    void *model;
    luaL_checktype(state, index, LUA_TUSERDATA);
    model = *(void**)luaL_checkudata(state, index, TFLITE_MODEL);
    if (model == NULL) luaL_typerror(state, index, TFLITE_MODEL);
    return model;
}

static void *checkTFLInterpreterOptions (lua_State *state, int index)
{
    void *options;
    luaL_checktype(state, index, LUA_TUSERDATA);
    options = *(void**)luaL_checkudata(state, index, TFLITE_INTERPRETEROPTIONS);
    if (options == NULL) luaL_typerror(state, index, TFLITE_INTERPRETEROPTIONS);
    return options;
}

static void *checkTFLInterpreter (lua_State *state, int index)
{
    void *interpreter;
    luaL_checktype(state, index, LUA_TUSERDATA);
    interpreter = *(void**)luaL_checkudata(state, index, TFLITE_INTERPRETER);
    if (interpreter == NULL) luaL_typerror(state, index, TFLITE_INTERPRETER);
    return interpreter;
}

static TfLiteTensor *toTFLTensor (lua_State *state, int index)
{
    TfLiteTensor *tensor = (TfLiteTensor *)lua_touserdata(state, index);
    if (tensor == NULL) luaL_typerror(state, index, TFLITE_TENSOR);
    return tensor;
}

static void *checkTFLTensor (lua_State *state, int index)
{
    TfLiteTensor *tensor=NULL;

    if(lua_islightuserdata(state, index))
        tensor=toTFLTensor(state,index);
    else luaL_typerror(state, index, TFLITE_TENSOR);
    
    return tensor;
}

//==============================================================================
//   GC
//==============================================================================

static int tflmodel_gc(lua_State *state) {

    void* model = checkTFLModel(state, 1);

    if(model!=NULL)
    {
        printf("TFLite Model release\n");
        TfLiteModelDelete(model);
    }

    return 0;
}

static int tflinterpreteroptions_gc(lua_State *state) {

    void* options = checkTFLInterpreterOptions(state, 1);

    if(options!=NULL)
    {
        printf("TFLite InterpreterOptions release\n");
        TfLiteInterpreterOptionsDelete(options);
    }

    return 0;
}

static int tflinterpreter_gc(lua_State *state) {

    void* interpreter = checkTFLInterpreter(state, 1);

    if(interpreter!=NULL)
    {
        printf("TFLite Interpreter release\n");
        TfLiteInterpreterDelete(interpreter);
    }

    return 0;
}

//==============================================================================
//   PUSH
//==============================================================================

static void * pushTFLModelFromFile (lua_State *state,const char *modelpath)
{
    TfLiteModel* model = TfLiteModelCreateFromFile(modelpath);
    if(model==NULL) printf("TfLiteModelCreateFromFile error\n");
    
    *(void**)lua_newuserdata(state, sizeof(void*)) = model;

    if(luaL_newmetatable(state, TFLITE_MODEL)){
          lua_pushcfunction(state, tflmodel_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return model;
}

static void * pushTFLInterpreterOptions (lua_State *state)
{
    TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
    if(options==NULL) printf("TfLiteInterpreterOptionsCreate error\n");
    
    *(void**)lua_newuserdata(state, sizeof(void*)) = options;

    if(luaL_newmetatable(state, TFLITE_INTERPRETEROPTIONS)){
          lua_pushcfunction(state, tflinterpreteroptions_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return options;
}

static void * pushTFLInterpreter (lua_State *state,const TfLiteModel* model, const TfLiteInterpreterOptions* optional_options)
{
    TfLiteInterpreter* interpreter = TfLiteInterpreterCreate(model,optional_options);
    if(interpreter==NULL) printf("TfLiteInterpreterCreate error\n");
    
    *(void**)lua_newuserdata(state, sizeof(void*)) = interpreter;

    if(luaL_newmetatable(state, TFLITE_INTERPRETER)){
          lua_pushcfunction(state, tflinterpreter_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return interpreter;
}

static void * pushTFLInterpreterInputTensor (lua_State *state,TfLiteInterpreter* interpreter,int index)
{
    TfLiteTensor* input_tensor =
        TfLiteInterpreterGetInputTensor(interpreter, index);
    
    if(input_tensor==NULL) printf("TfLiteInterpreterGetInputTensor error \n");
    
    lua_pushlightuserdata(state, input_tensor);

    return input_tensor;
}

static void * pushTFLInterpreterOutputTensor (lua_State *state,TfLiteInterpreter* interpreter,int index)
{
    TfLiteTensor* output_tensor =
        TfLiteInterpreterGetOutputTensor(interpreter, index);
    
    if(output_tensor==NULL) printf("TfLiteInterpreterGetOutputTensor error \n");
    
    lua_pushlightuserdata(state, output_tensor);

    return output_tensor;
}

//==============================================================================
//   TFLITE
//==============================================================================

static int tflite_modelfromfile(lua_State *state) {

    const char* modelfile=luaL_checkstring(state,1);
    
    TfLiteModel* model = pushTFLModelFromFile(state,modelfile);
    return 1;
}

static int tflite_createinterpreteroptions(lua_State *state) {

    TfLiteInterpreterOptions* options = pushTFLInterpreterOptions(state);
    return 1;
}

static int tflite_createinterpreter(lua_State *state) {

    TfLiteModel* model=checkTFLModel(state, 1);
    TfLiteInterpreterOptions* options =checkTFLInterpreterOptions(state, 2);
    
    TfLiteInterpreter* interpreter = pushTFLInterpreter(state,model,options);
    
    return 1;
}

static int tflite_interpreterallocatetensors(lua_State *state) {

    TfLiteInterpreter* interpreter = checkTFLInterpreter(state,1);
    TfLiteStatus ret=TfLiteInterpreterAllocateTensors(interpreter);
    if(ret!=kTfLiteOk)
    {
        if(debug) printf("TFLite AllocateTensor error \n");
    }
    return 0;
}

static int tflite_interpretergetinputtensor(lua_State *state) {

    TfLiteInterpreter* interpreter = checkTFLInterpreter(state,1);
    int index=luaL_checkinteger(state, 2);
    TfLiteTensor* input_tensor = pushTFLInterpreterInputTensor(state,interpreter,index);
    
    return 1;
}

static int tflite_interpretergetoutputtensor(lua_State *state) {

    TfLiteInterpreter* interpreter = checkTFLInterpreter(state,1);
    int index=luaL_checkinteger(state, 2);
    TfLiteTensor* output_tensor = pushTFLInterpreterOutputTensor(state,interpreter,index);

    return 1;
}

static int tflite_gettensorsize(lua_State *state)
{
    /*int ndim=TfLiteTensorNumDims(tensor);
     printf("Tensor size(%d",TfLiteTensorDim(tensor,0));
     for(int i=1;i<ndim;i++)printf("x%d",TfLiteTensorDim(tensor,i));
     printf(")\n");
     */
    TfLiteTensor* tensor = checkTFLTensor(state,1);
    if(tensor!=NULL)
    {
        int ndim=TfLiteTensorNumDims(tensor);
        lua_newtable(state);
        printf("Tensor size ");
        for(int i=0;i<ndim;i++)
        {
            lua_pushinteger(state, i+1);
            int dim=TfLiteTensorDim(tensor,i);
            printf("%d ",TfLiteTensorDim(tensor,i));
            lua_pushnumber(state,dim);
            lua_settable(state,-3);
        }
        printf("\n");
        
        return 1;
    }
    
    return 0;
}

static int tflite_copyimage(lua_State *state) {

    TfLiteTensor* tensor = checkTFLTensor(state,1);
    
#ifdef PROTEO_OPENCV
        void* mat = luaL_checkudata(L, 2, "OpenCVMat");
    toRGB(mat);
        OCVImage img=getImage(mat);
    
#endif
    
    //float input[128*128*3] = { 0.0 };
    //TfLiteStatus ret=TfLiteTensorCopyFromBuffer(tensor,input,128*128*3*sizeof(float));
    
    TfLiteStatus ret=TfLiteTensorCopyFromBuffer(tensor,img.data,img.width*img.height*img.channels*sizeof(float));
    if(ret!=kTfLiteOk)
    {
        if(debug) printf("TFLite CopyImage error \n");
    }
    toBGR(mat);
    
    return 0;
}

static int tflite_copytensor(lua_State *state) {

#ifdef PROTEO_OPENCV
        void* mat = luaL_checkudata(L, 1, "OpenCVMat");
        
        OCVImage img=getImage(mat);
#endif
    
    TfLiteTensor* tensor = checkTFLTensor(state,2);
    
    //float input[128*128*3] = { 0.0 };
    //TfLiteStatus ret=TfLiteTensorCopyFromBuffer(tensor,input,128*128*3*sizeof(float));
    
    TfLiteStatus ret=TfLiteTensorCopyToBuffer(tensor,img.data,img.width*img.height*img.channels*sizeof(float));
    if(ret!=kTfLiteOk)
    {
        if(debug) printf("TFLite CopyTensor error \n");
    }
    
    return 0;
}

float ReverseFloat( const float inFloat )
{
   float retVal;
   char *floatToConvert = ( char* ) & inFloat;
   char *returnFloat = ( char* ) & retVal;

   // swap the bytes into a temporary buffer
   returnFloat[0] = floatToConvert[3];
   returnFloat[1] = floatToConvert[2];
   returnFloat[2] = floatToConvert[1];
   returnFloat[3] = floatToConvert[0];

   return retVal;
}

static int tflite_tensortotable(lua_State *state)
{
    TfLiteTensor* tensor = checkTFLTensor(state,1);
    
    if(tensor!=NULL)
    {
        TfLiteType type=TfLiteTensorType(tensor);
        
        //if type is float
        int size=TfLiteTensorByteSize(tensor)/sizeof(float);
        float* data=TfLiteTensorData(tensor);
        
        lua_newtable(state);
        for(int i=0;i<size;i++)
        {
            lua_pushinteger(state, i+1);
            lua_pushnumber(state,data[i]);
            lua_settable(state,-3);
        }
        
        return 1;
    }
    
}
static int tflite_interpreterinvoke(lua_State *state) {

    TfLiteInterpreter* interpreter = checkTFLInterpreter(state,1);

    TfLiteStatus ret=TfLiteInterpreterInvoke(interpreter);
    if(ret!=kTfLiteOk)
    {
        if(debug) printf("TFLite Invoke error \n");
    }
    
    return 1;
}

//==============================================================================
//   LUA
//==============================================================================
void add_tensorflow_proteo(lua_State* state)
{
    addFunction_proteo(state,"tflite","modelFromFile",tflite_modelfromfile);
    addFunction_proteo(state,"tflite","createInterpreterOptions",tflite_createinterpreteroptions);
    addFunction_proteo(state,"tflite","createInterpreter",tflite_createinterpreter);
    addFunction_proteo(state,"tflite","InterpreterAllocateTensors",tflite_interpreterallocatetensors);
    addFunction_proteo(state,"tflite","InterpreterGetInputTensor",tflite_interpretergetinputtensor);
    addFunction_proteo(state,"tflite","InterpreterGetOutputTensor",tflite_interpretergetoutputtensor);
    addFunction_proteo(state,"tflite","copyImage",tflite_copyimage);
    addFunction_proteo(state,"tflite","copyTensor",tflite_copytensor);
    addFunction_proteo(state,"tflite","interpreterInvoke",tflite_interpreterinvoke);
    addFunction_proteo(state,"tflite","getTensorSize",tflite_gettensorsize);
    addFunction_proteo(state,"tflite","tensorToTable",tflite_tensortotable);
}
