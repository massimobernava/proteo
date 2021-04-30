//==============================================================================
//   FFMPEG
//==============================================================================

#define AV_CODEC "AVCodec"
#define AV_CODECCONTEXT "AVCodecContext"
#define AV_FRAME "AVFrame"
#define AV_PACKET "AVPacket"

//==============================================================================
//   CHECK
//==============================================================================

static void *checkAvCodecContext (lua_State *state, int index)
{
    void *context;
    luaL_checktype(state, index, LUA_TUSERDATA);
    context = *(void**)luaL_checkudata(state, index, AV_CODECCONTEXT);
    if (context == NULL) luaL_typerror(state, index, AV_CODECCONTEXT);
    return context;
}

static void *checkAvFrame (lua_State *state, int index)
{
    void *frame;
    luaL_checktype(state, index, LUA_TUSERDATA);
    frame = *(void**)luaL_checkudata(state, index, AV_FRAME);
    if (frame == NULL) luaL_typerror(state, index, AV_FRAME);
    return frame;
}

static void *checkAvPacket (lua_State *state, int index)
{
    void *packet;
    luaL_checktype(state, index, LUA_TUSERDATA);
    packet = *(void**)luaL_checkudata(state, index, AV_PACKET);
    if (packet == NULL) luaL_typerror(state, index, AV_PACKET);
    return packet;
}

static AVCodec *toAvCodec (lua_State *state, int index)
{
    AVCodec *ac = (AVCodec *)lua_touserdata(state, index);
    if (ac == NULL) luaL_typerror(state, index, AV_CODEC);
    return ac;
}

static void *checkAvCodec (lua_State *state, int index)
{
    AVCodec *ac=NULL;

    if(lua_islightuserdata(state, index))
        ac=toAvCodec(state,index);
    else luaL_typerror(state, index, AV_CODEC);
    
    return ac;
}



//==============================================================================
//   GC
//==============================================================================

static int avcodeccontext_gc(lua_State *state) {

    void* context = checkAvCodecContext(state, 1);

    if(context!=NULL)
    {
        printf("AvCodecContext release\n");
        avcodec_close(context);
        av_free(context);
    }

    return 0;
}

static int avframe_gc(lua_State *state) {

    AVFrame* frame = checkAvFrame(state, 1);

    if(frame!=NULL)
    {
        printf("AvFrame release\n");
        av_freep(&frame->data[0]);
        av_frame_free(&frame);
    }

    return 0;
}

static int avpacket_gc(lua_State *state) {

    AVPacket* packet = checkAvPacket(state, 1);

    if(packet!=NULL)
    {
        printf("AvPacket release\n");
        av_packet_free(&packet);
    }

    return 0;
}


//==============================================================================
//   PUSH
//==============================================================================


static void * pushAvCodecContext (lua_State *state,AVCodec *codec)
{
    void* context = avcodec_alloc_context3(codec);
    if(context==NULL) printf("AvCodecContext error\n");
    
    *(void**)lua_newuserdata(state, sizeof(void*)) = context;

    if(luaL_newmetatable(state, AV_CODECCONTEXT)){
          lua_pushcfunction(state, avcodeccontext_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return context;
}

static void * pushAvCodec (lua_State *state,int id)
{
    void* codec = avcodec_find_encoder(id);
    if(codec==NULL) printf("AvCodec error: not found\n");
    
    lua_pushlightuserdata(state, codec);

    return codec;
}

static void * pushAvFrame (lua_State *state)
{
    AVFrame* frame = av_frame_alloc();
    if(frame==NULL) printf("AvFrame error\n");
    
    *(void**)lua_newuserdata(state, sizeof(void*)) = frame;

    if(luaL_newmetatable(state, AV_FRAME)){
          lua_pushcfunction(state, avframe_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return frame;
}
static void * pushAvPacket (lua_State *state)
{
    AVPacket* packet = av_packet_alloc();
    if(packet==NULL) printf("AvPacket error\n");
    
    *(void**)lua_newuserdata(state, sizeof(void*)) = packet;

    if(luaL_newmetatable(state, AV_PACKET)){
          lua_pushcfunction(state, avpacket_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return packet;
}

//==============================================================================
//   FFMPEG
//==============================================================================

/*static int ffmpeg_avcodec_find_encoder(lua_State *state) {

    //void* context = pushCtx(state);
    int id=luaL_checkinteger(state, 1);
    void* codec=pushAvCodec(state, id);
    return 1;
}

static int ffmpeg_avcodec_register_all(lua_State *state) {

    //void* context = pushCtx(state);
    avcodec_register_all();
    return 0;
}

static int ffmpeg_avcodec_alloc_context3(lua_State *state) {
    
    //AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    void* codec = checkAvCodec(state,1);
    void* context = pushAvCodecContext(state,codec);
    return 1;
}

static int ffmpeg_av_frame_alloc(lua_State *state) {
    
    void* frame = pushAvFrame(state);
    return 1;
}


static int ffmpeg_avcodec_open2(lua_State *state) {
    
    void* context = checkAvCodecContext(state,1);
    void* codec = checkAvCodec(state,2);
    
    int ret=avcodec_open2(context, codec, NULL);
    lua_pushinteger(state,ret);
    return 1;
}*/

static int ffmpeg_newEncoder(lua_State *state) {
    
    int id=luaL_checkinteger(state, 1);
    int w=luaL_checkinteger(state, 2);
    int h=luaL_checkinteger(state, 3);
    
    AVCodec* codec = avcodec_find_encoder(id);
    AVCodecContext* context = avcodec_alloc_context3(codec);
    if(context==NULL) printf("newEndcoder AvCodecContext error\n");
    
    context->bit_rate = 400000;
       // resolution must be a multiple of two
    context->width = w;
    context->height = h;
      // frames per second
    context->time_base= (AVRational){1,25};
    context->gop_size = 10; // emit one intra frame every ten frames
    context->max_b_frames=1;
    context->pix_fmt = AV_PIX_FMT_YUV420P;
      //if(id == AV_CODEC_ID_H264)
      // av_opt_set(context->priv_data, "preset", "slow", 0);
     
    
    *(void**)lua_newuserdata(state, sizeof(void*)) = context;

    if(luaL_newmetatable(state, AV_CODECCONTEXT)){
          lua_pushcfunction(state, avcodeccontext_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);
    
    int ret=avcodec_open2(context, codec, NULL);
    
    return 1;
}

static int ffmpeg_newDecoder(lua_State *state) {
    
    int id=luaL_checkinteger(state, 1);

    
    AVCodec* codec = avcodec_find_decoder(id);
    AVCodecContext* context = avcodec_alloc_context3(codec);
    if(context==NULL) printf("newDecoder AvCodecContext error\n");
    
    *(void**)lua_newuserdata(state, sizeof(void*)) = context;

    if(luaL_newmetatable(state, AV_CODECCONTEXT)){
          lua_pushcfunction(state, avcodeccontext_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);
    
    int ret=avcodec_open2(context, codec, NULL);
    
    return 1;
}

static int ffmpeg_newFrame(lua_State *state) {
    
    AVCodecContext* context = checkAvCodecContext(state,1);
    
    AVFrame* frame = pushAvFrame(state);
    
    if(frame==NULL  && debug)
        printf("Ffmpeg newFrame error\n");
        
    if(context->width>0)
    {
        frame->format = context->pix_fmt;
        frame->width  = context->width;
        frame->height = context->height;

        int ret = av_image_alloc(frame->data, frame->linesize, frame->width, frame->height, frame->format, 32);
    
        if(ret<0)
        {
            if(debug) printf("Ffmpeg newFrame error\n");
        
            lua_pushinteger(state,ret);
            return 2;
        }
    }
    
    return 1;
}

static int ffmpeg_newPacket(lua_State *state) {
    
    AVPacket* packet = pushAvPacket(state);
    
    return 1;
}

static int ffmpeg_decodePacket(lua_State *state) {
    
    AVPacket* packet = checkAvPacket(state,1);
    const char* data=luaL_checkstring(state,2);
    
    uint8_t *tmp=(uint8_t *)av_malloc(Base64decode_len(data));
    int len = Base64decode((char*)tmp, data);

    if(len==0)
    {
        printf("copyPacket len is 0");
        av_free(tmp);
        return 0;
    }
    av_packet_from_data(packet,(uint8_t*)tmp,len);
    
    //free(tmp);
    
    return 0;
}

static int ffmpeg_encodePacket(lua_State *state) {
    
    AVPacket* packet = checkAvPacket(state,1);
    
    char *b64 =(char*)malloc(1+Base64encode_len(packet->size));
    Base64encode(b64,(char*)packet->data,packet->size);
    b64[Base64encode_len(packet->size)]=0;

    lua_pushstring(state, b64);
    free(b64);
    
    return 1;
}

/*void avframeToMat(const AVFrame * frame, cv::Mat& image)
  {
    int width = frame->width;
    int height = frame->height;

    // Allocate the opencv mat and store its stride in a 1-element array
    if (image.rows != height || image.cols != width || image.type() != CV_8UC3) image = cv::Mat(height, width, CV_8UC3);
    int cvLinesizes[1];
    cvLinesizes[0] = image.step1();

    // Convert the colour format and write directly to the opencv matrix
    SwsContext* conversion = sws_getContext(width, height, (AVPixelFormat) frame->format, width, height, AVPixelFormat::AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    sws_scale(conversion, frame->data, frame->linesize, 0, height, &image.data, cvLinesizes);
    sws_freeContext(conversion);

  }*/
static int _pts=1;
static int ffmpeg_copyImage(lua_State *state) {
    
    AVFrame* frame = checkAvFrame(state,1);
#ifdef PROTEO_OPENCV
        void* mat = luaL_checkudata(L, 2, "OpenCVMat");

        OCVImage img=getImage(mat);
    
#endif
    
    av_frame_make_writable(frame);
    
    struct SwsContext* swsctx = sws_getContext(img.width, img.height, AV_PIX_FMT_BGR24,
                                              frame->width, frame->height, frame->format, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    const int stride[] = { img.step };
    sws_scale(swsctx, &img.data, stride, 0, img.height, frame->data, frame->linesize);
    sws_freeContext(swsctx);
    
    /*for(int y=0;y<frame->height;y++) {
     for(int x=0;x<frame->width;x++) {
       frame->data[0][y * frame->linesize[0] + x] = x + y;
      }
     }

     for(int y=0;y<frame->height/2;y++) {
      for(int x=0;x<frame->width/2;x++) {
       frame->data[1][y * frame->linesize[1] + x] = 128 + y ;
       frame->data[2][y * frame->linesize[2] + x] = 64 + x ;
      }
    }*/
    frame->pts=_pts++;
    return 0;
}

static int ffmpeg_copyFrame(lua_State *state) {
    
#ifdef PROTEO_OPENCV
    void* mat = luaL_checkudata(L, 1, "OpenCVMat");

    OCVImage image=getImage(mat);
#endif
    
    AVFrame* frame = checkAvFrame(state,2);

    
    if(frame!=NULL && frame->width>0)
    {
        struct SwsContext* conversion = sws_getContext(frame->width, frame->height, frame->format, image.width, image.height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
        const int stride[] = { image.step };
        sws_scale(conversion, frame->data, frame->linesize, 0, image.height, &image.data, stride);
        sws_freeContext(conversion);
    }
    /*for(int y=0;y<frame->height;y++) {
     for(int x=0;x<frame->width;x++) {
       frame->data[0][y * frame->linesize[0] + x] = x + y;
      }
     }

     for(int y=0;y<frame->height/2;y++) {
      for(int x=0;x<frame->width/2;x++) {
       frame->data[1][y * frame->linesize[1] + x] = 128 + y ;
       frame->data[2][y * frame->linesize[2] + x] = 64 + x ;
      }
    }
    frame->pts=(unsigned)time(NULL);*/
    return 0;
}

static int ffmpeg_encodeFrame(lua_State *state) {
    
    AVCodecContext* context = checkAvCodecContext(state,1);
    AVFrame* frame = checkAvFrame(state,2);
    
    AVPacket *avpkt=checkAvPacket(state,3);
    
    int ret=avcodec_send_frame(context,frame);
    if(ret<0)
    {
        if(ret==AVERROR(EAGAIN))
        {
            lua_pushinteger(state,ret);
            return 1;
        }
        if(debug) printf("Ffmpeg encodeFrame error (send frame)\n");
    }
    ret=avcodec_receive_packet(context,avpkt);
    if(ret<0)
    {
        if(ret==AVERROR(EAGAIN))
        {
            lua_pushinteger(state,ret);
            return 1;
        }
        if(debug) printf("Ffmpeg encodeFrame error (receive packet)\n");
    }
    if(avpkt->size==0)
    {
        //hexDump ("Codec Packet", avpkt->data, avpkt->size);
        lua_pushinteger(state,ret);
        return 1;
    }
   // char *b64 =(char*)malloc(1+Base64encode_len(avpkt->size));
    //Base64encode(b64,(char*)avpkt->data,avpkt->size);
    //b64[Base64encode_len(avpkt->size)]=0;

    lua_pushinteger(state,ret);
    //lua_pushstring(state, b64);
    //free(b64);
    
    return 1;
}

static int ffmpeg_decodeFrame(lua_State *state) {
    
    AVCodecContext* context = checkAvCodecContext(state,1);
    AVPacket *avpkt=checkAvPacket(state,2);
    AVFrame* frame = checkAvFrame(state,3);
    
    int ret=avcodec_send_packet(context,avpkt);
    if(ret<0)
    {
        if(ret!=AVERROR(EAGAIN) && ret!=AVERROR_EOF && debug) printf("Ffmpeg decodeFrame error (send packet) %s\n",av_err2str(ret));
       
        lua_pushinteger(state,ret);
        return 1;
    }
    while (ret >= 0)
        ret=avcodec_receive_frame(context,frame);
    if(ret<0)
    {
        if(ret!=AVERROR(EAGAIN) && ret!=AVERROR_EOF && debug) printf("Ffmpeg decodeFrame error (receive frame) %s\n",av_err2str(ret));
        
        if(ret==AVERROR(EAGAIN)) ret=0;
        
        lua_pushinteger(state,ret);
        return 1;
    }
    /*if(avpkt->size==0)
    {
        //hexDump ("Codec Packet", avpkt->data, avpkt->size);
    }*/
   // char *b64 =(char*)malloc(1+Base64encode_len(avpkt->size));
    //Base64encode(b64,(char*)avpkt->data,avpkt->size);
    //b64[Base64encode_len(avpkt->size)]=0;

    lua_pushinteger(state,ret);
    //lua_pushstring(state, b64);
    //free(b64);
    
    return 1;
}
//==============================================================================
//   LUA
//==============================================================================
void add_ffmpeg_proteo(lua_State* state)
{
    //addFunction_proteo(state,"ffmpeg","avcodec_register_all",ffmpeg_avcodec_register_all);
    //addFunction_proteo(state,"ffmpeg","avcodec_find_encoder",ffmpeg_avcodec_find_encoder);
    //addFunction_proteo(state,"ffmpeg","avcodec_alloc_context",ffmpeg_avcodec_alloc_context3);
    //addFunction_proteo(state,"ffmpeg","avcodec_open",ffmpeg_avcodec_open2);
    //addFunction_proteo(state,"ffmpeg","av_frame_alloc",ffmpeg_av_frame_alloc);
    addFunction_proteo(state,"ffmpeg","newEncoder",ffmpeg_newEncoder);
    addFunction_proteo(state,"ffmpeg","newDecoder",ffmpeg_newDecoder);
    addFunction_proteo(state,"ffmpeg","newFrame",ffmpeg_newFrame);
    addFunction_proteo(state,"ffmpeg","copyImage",ffmpeg_copyImage);
    addFunction_proteo(state,"ffmpeg","copyFrame",ffmpeg_copyFrame);
    addFunction_proteo(state,"ffmpeg","encodeFrame",ffmpeg_encodeFrame);
    addFunction_proteo(state,"ffmpeg","decodeFrame",ffmpeg_decodeFrame);
    addFunction_proteo(state,"ffmpeg","newPacket",ffmpeg_newPacket);
    addFunction_proteo(state,"ffmpeg","decodePacket",ffmpeg_decodePacket);
    addFunction_proteo(state,"ffmpeg","encodePacket",ffmpeg_encodePacket);
    
    addTable_proteo(state,"ffmpeg","CodecID");
    addEnum_proteo(state,"ffmpeg","CodecID","AV_CODEC_ID_H264",AV_CODEC_ID_H264);
}
