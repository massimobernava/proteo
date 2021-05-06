
const char* formatString(int format)
{
    switch(format)
    {
        case AUDIO_U16SYS: return "AUDIO_U16SYS"; break;
        case AUDIO_S16SYS: return "AUDIO_S16SYS"; break;
        case AUDIO_S32SYS: return "AUDIO_S32SYS"; break;
        case AUDIO_F32SYS: return "AUDIO_F32SYS"; break;
    }
    
    return "?";
}

const char* statusString(int status)
{
    switch(status)
    {
        case SDL_AUDIO_STOPPED: return "AUDIO_STOPPED"; break;
        case SDL_AUDIO_PLAYING: return "AUDIO_PLAYING"; break;
        case SDL_AUDIO_PAUSED: return "AUDIO_PAUSED"; break;
    }
    
    return "?";
}

void audioRecordCallback(void*  userdata, Uint8* stream, int len)
{
    RSound * sounds = (RSound *) userdata;
    //SDL_AudioFormat audio_format;
    Uint32 audio_len;
    Uint8 *audio_pos;
    //SDL_memset(stream, 0, len);
    /*for(int j=0;j<len;j++)
        if(stream[j]!=0)
            printf("Stream: %d=%d\n",j,stream[j]);*/
    
    for(int i=0;i<MAX_REC;i++)
    {
        if(sounds[i].sound!=NULL && sounds[i].state==1)
        {
            audio_len=sounds[i].sound->size-sounds[i].pos;
            audio_len = ( len > audio_len ? audio_len : len );
            audio_pos = sounds[i].sound->buffer;
            audio_pos += sounds[i].pos;
            SDL_memcpy (audio_pos,stream, audio_len);
            
            //printf("PLAY: %s %d \n",playingsounds[i].sound->id, playingsounds[i].pos);
            sounds[i].pos+= audio_len;
            
            if(sounds[i].pos>=sounds[i].sound->size)
            {
                sounds[i].state=2;
                if(debug) printf("End Record: %s\n",sounds[i].sound->id);
                //TODO Callback?
            }
        }
    }
    
    ProteoStream* current=streams;
    while(current!=NULL)
    {
        if(current->state==2 && current->stream!=NULL)
        {
            int rc = SDL_AudioStreamPut(current->stream, stream, len);
            if (rc == -1) {
                printf("AudioRecordCallback error: %s\n", SDL_GetError());
            }
        }
        current=current->next;
    }
}

void audioPlayCallback(void*  userdata, Uint8* stream, int len)
{
    PSound * sounds = (PSound *) userdata;
    SDL_AudioFormat audio_format;
    Uint32 audio_len;
    Uint8 *audio_pos;
    SDL_memset(stream, 0, len);
    
    for(int i=0;i<MAX_POLYPHONY;i++)
    {
        if(sounds[i].sound!=NULL)
        {
            audio_format=sounds[i].sound->spec.format;
            audio_len=sounds[i].sound->size-sounds[i].pos;
            audio_len = ( len > audio_len ? audio_len : len );
            audio_pos = sounds[i].sound->buffer;
            audio_pos += sounds[i].pos;
            //SDL_memcpy (stream, audio_pos, audio_len);
            //SDL_MixAudio(stream, audio_pos, audio_len, SDL_MIX_MAXVOLUME);
            SDL_MixAudioFormat(stream, audio_pos, audio_format, audio_len, SDL_MIX_MAXVOLUME);
            
            //printf("PLAY: %s %d \n",playingsounds[i].sound->id, playingsounds[i].pos);
            sounds[i].pos+= audio_len;
            
            if(sounds[i].pos>=sounds[i].sound->size)
            {
                if(debug) printf("End Play: %s\n",sounds[i].sound->id);
                sounds[i].sound=NULL;

                lua_getref(L,sounds[i].callback);
                if(lua_isfunction(L, -1) )
                {
                    if(debug) printf("Audio callback\n");
                    
                    lua_pushinteger(L, i);
                    int error = lua_trace_pcall(L,1,0);//lua_pcall(L, 1, 0, 0);
                }
            }
        }
    }
    
    ProteoStream* current=streams;
    while(current!=NULL)
    {
        if(current->state==1 && current->stream!=NULL)
        {
            if(SDL_AudioStreamAvailable(current->stream)>=len)
            {
                Uint8 *buffer=malloc(len); //Si può evitare?
                int gotten = SDL_AudioStreamGet(current->stream, buffer, len);
                if (gotten == -1) {
                    printf("AudioPlayCallback error: %s\n", SDL_GetError());
                }
                else SDL_MixAudioFormat(stream, buffer, current->out.format, gotten, SDL_MIX_MAXVOLUME);
                free(buffer);
            }
        }
        current=current->next;
    }
    
    /*lua_getglobal(L,"proteo_audio_out");
    if(lua_isfunction(L, -1) )
    {
        lua_pushinteger(L, len); //dt
        lua_pcall(L,1,1,0);
    }*/
}

void freesounds()
{
    if(debug)printf("Free sounds\n");
    
    for(int i=0;i<MAX_POLYPHONY;i++)
    {
        if(playingsounds[i].sound!=NULL)
        {
            SDL_PauseAudioDevice(playingsounds[i].device, true);
        }
    }
    while(sounds!=NULL)
    {
        ProteoSound* tmp=(ProteoSound*)sounds->next;

    
        if(sounds->buffer!=NULL) SDL_FreeWAV(sounds->buffer);
        sounds->buffer=NULL;
      
        #ifdef LIGHTPROTEOCOMPONENT
        free(components);
        #endif
      
        sounds=tmp;
    }
}

void freestreams()
{
  if(debug)printf("Free streams\n");
  while(streams!=NULL)
  {
    ProteoStream* tmp=(ProteoStream*)streams->next;

    
    if(streams->stream!=NULL) SDL_FreeAudioStream(streams->stream);
    streams->stream=NULL;
    streams->state=0;
      
    free(streams);
      
    streams=tmp;
  }
}

//==============================================================================
//   LUA
//==============================================================================

static ProteoSound *checkProteoSound (lua_State *L, int index)
{
    ProteoSound *ps=NULL;
#ifdef LIGHTPROTEOSOUND
    if(lua_islightuserdata(L, index))
        ps=toProteoSound(L,index);
    else luaL_typerror(L, index, PROTEOSOUND);
#else
    //ProteoSound *ps;
    luaL_checktype(L, index, LUA_TUSERDATA);
    ps = (ProteoSound *)luaL_checkudata(L, index, PROTEOSOUND);
    if (ps == NULL) luaL_typerror(L, index, PROTEOSOUND);
#endif
  return ps;
}

static int sound_gc(lua_State *l) {
    
    ProteoSound* sound = checkProteoSound(l, 1);
    
    if(sound!=NULL)
    {
        if(verbose) printf("GC Sound(%s)\n",sound->id);
        if(sound->buffer!=NULL) free(sound->buffer);
        sound->buffer=NULL;
        //sound->type=Deleted;
    }
    return 0;
}

static ProteoSound *pushProteoSound (lua_State *L)
{
#ifdef LIGHTPROTEOSOUND
        ProteoSound *ps =malloc(sizeof(ProteoSound));
        lua_pushlightuserdata(L, ps);
#else
    ProteoSound *ps = (ProteoSound *)lua_newuserdata(L, sizeof(ProteoSound));
    
    if(luaL_newmetatable(L, PROTEOSOUND)){
        lua_pushcfunction(L, sound_gc);
        lua_setfield(L, -2, "__gc");
    }
    lua_setmetatable(L, -2);
#endif
    return ps;
}

static ProteoSound *toProteoSound (lua_State *L, int index)
{
  ProteoSound *ps = (ProteoSound *)lua_touserdata(L, index);
  if (ps == NULL) luaL_typerror(L, index, PROTEOSOUND);
  return ps;
}

static ProteoStream *checkProteoStream (lua_State *L, int index)
{
    ProteoStream *ps=NULL;

    if(lua_islightuserdata(L, index))
        ps=toProteoStream(L,index);
    else luaL_typerror(L, index, PROTEOSTREAM);

    return ps;
}

static ProteoStream *pushProteoStream (lua_State *state, long in_freq, long in_format, long in_channels,
                                                         long out_freq, long out_format, long out_channels)
{
    ProteoStream *ps =malloc(sizeof(ProteoStream));
    
    SDL_zero(ps->in);
    SDL_zero(ps->out);
    ps->in.freq = in_freq;
    ps->in.format = in_format;
    ps->in.channels = in_channels;
    ps->out.freq = out_freq;
    ps->out.format = out_format;
    ps->out.channels = out_channels;
    ps->next=NULL;
    ps->device=-1;
    ps->state=0;
    
    ps->stream = SDL_NewAudioStream(in_format, in_channels, in_freq, out_format, out_channels, out_freq);
    
    if(ps->stream==NULL)
    {
        free(ps);
        return NULL;
    }
    lua_pushlightuserdata(state, ps);
    
    return ps;
}

static ProteoStream *toProteoStream (lua_State *L, int index)
{
    ProteoStream *ps = (ProteoStream *)lua_touserdata(L, index);
    if (ps == NULL) luaL_typerror(L, index, PROTEOSTREAM);
    return ps;
}

//==============================================================================
//   AUDIO
//==============================================================================

int audio_infoDrivers(lua_State* state)
{
    int dcount = SDL_GetNumAudioDrivers();
    for (int i = 0; i < dcount; i++) {
         printf(" Driver #%d: '%s'\n", i, SDL_GetAudioDriver(i));
    }
    return 0;
}

int audio_init(lua_State* state)
{
    if (lua_isstring(state,1)==1)
    {
        const char* driver=luaL_checkstring(state,1);
        int ret=SDL_AudioInit(driver);
        lua_pushinteger(state,ret);
        return 1;
    }
    else
    {
        int ret=SDL_AudioInit(SDL_GetCurrentAudioDriver());
        lua_pushinteger(state,ret);
        return 1;
    }
    return 0;
}

int audio_quit(lua_State* state)
{
    SDL_AudioQuit();
    
    return 0;
}

int audio_infoDevices(lua_State* state)
{
    printf("Audio driver: %s\n", SDL_GetCurrentAudioDriver());

    int devcount = SDL_GetNumAudioDevices(SDL_TRUE);
    for (int i = 0; i < devcount; i++) {
        printf(" Capture device #%d: '%s'\n", i, SDL_GetAudioDeviceName(i, SDL_TRUE));
    }
    
    devcount=SDL_GetNumAudioDevices(SDL_FALSE);
    for (int i = 0; i < devcount; i++) {
        printf(" Playback device #%d: '%s'\n", i, SDL_GetAudioDeviceName(i, SDL_FALSE));
    }
    return 0;
}

int audio_getAudioStatus(lua_State* state)
{
    lua_pushinteger(state,SDL_GetAudioStatus());
    return 1;
}

int audio_pauseDevice(lua_State* state)
{
    int devid=luaL_checkinteger(state,1);
    long pause=luaL_checkbool(state,2);
    
    SDL_PauseAudioDevice(devid, pause);
    
    return 0;
}
int audio_openDevice(lua_State* state)
{
    const char* device=NULL;
    if(lua_isstring(state,1)==1) device=luaL_checkstring(state,1);
    long iscapture=luaL_checkbool(state,2);
    long freq=luaL_checkinteger(state,3);
    long format=luaL_checkinteger(state,4);
    long channels=luaL_checkinteger(state,5);
    long samples=luaL_checkinteger(state,6);
    //const char* callback=luaL_checkstring(state,7);
    
    SDL_AudioDeviceID devid = 0;
    SDL_AudioSpec wanted,spec;
    
    SDL_zero(wanted);
    wanted.freq = freq;
    wanted.format = format;
    wanted.channels = channels;
    wanted.samples = samples;
    
    if(iscapture)
    {
        wanted.callback=audioRecordCallback;
        wanted.userdata=(void*)recordingsounds;
    }
    else
    {
        wanted.callback=audioPlayCallback;
        wanted.userdata=(void*)playingsounds;
    }
    SDL_zero(spec);

    devid = SDL_OpenAudioDevice(device, iscapture, &wanted, &spec, SDL_AUDIO_ALLOW_ANY_CHANGE);
    
    //SDL_PauseAudioDevice(devid, SDL_FALSE);
    
    printf("DEVICE(%s) ID:%d: Fr:%d %s Ch:%d \n" ,statusString(SDL_GetAudioDeviceStatus(devid)),devid, spec.freq,formatString(spec.format),spec.channels);
    
    if (!devid) {
        printf("Couldn't open an audio device for playback: %s!\n", SDL_GetError());
    }
    
    lua_pushinteger(state,devid);
    
    return 1;
}

int audio_convertAudio(lua_State* state)
{
    ProteoSound* sound=checkProteoSound(state,1);
    long freq=luaL_checkinteger(state,2);
    long format=luaL_checkinteger(state,3);
    long channels=luaL_checkinteger(state,4);
    
    // Change 1024 stereo sample frames at 48000Hz from float32 to int16.
    SDL_AudioCVT cvt;
    SDL_BuildAudioCVT(&cvt,sound->spec.format, sound->spec.channels, sound->spec.freq, format, channels, freq);
    if(cvt.needed==1)
    {
        cvt.len = sound->size;
        cvt.buf = (Uint8 *) SDL_malloc(MAX(sound->size,sound->size * cvt.len_mult));
        memcpy(cvt.buf, sound->buffer, sound->size);
        if(SDL_ConvertAudio(&cvt)<0)
        {
            printf("SDL_ConvertAudio failed: %s\n", SDL_GetError());
            SDL_free(cvt.buf);
        }
        else
        {
            free(sound->buffer);
            sound->buffer=cvt.buf;
            sound->size=cvt.len_cvt;
            sound->spec.format=format;
            sound->spec.freq=freq;
            sound->spec.channels=channels;
        }
    }
    lua_pushinteger(state,cvt.needed);
    return 1;
}

int audio_infoAudio(lua_State* state)
{
    ProteoSound* sound=checkProteoSound(state,1);
    printf("SOUND %s: Fr:%d %s Sz:%d \n" ,sound->id,sound->spec.freq,formatString(sound->spec.format),sound->size);
    return 0;
}

int audio_queueAudio(lua_State* state)
{
    int dev=luaL_checkinteger(state,1);
    ProteoSound* sound=checkProteoSound(state,2);
    
    if (SDL_QueueAudio(dev, sound->buffer, sound->size)<0)
    {
        printf("SDL_QueueAudio failed: Device: %d %s\n", dev, SDL_GetError());
    }
    
    return 0;
}

int audio_playAudio(lua_State* state)
{
    int dev=luaL_checkinteger(state,1);
    ProteoSound* sound=checkProteoSound(state,2);
    int vol=luaL_checkinteger(state,3);
    
    int ref=lua_ref(state, TRUE);
    
    SDL_LockAudio();
    playingsounds[playIndex].sound=sound;
    playingsounds[playIndex].pos=0;
    playingsounds[playIndex].volume=vol;
    playingsounds[playIndex].device=dev;
    playingsounds[playIndex].callback=ref;
    SDL_UnlockAudio();
    
    lua_pushinteger(state, playIndex);
    playIndex=(playIndex+1)%MAX_POLYPHONY;
    
    return 1;
}

int audio_stopAudio(lua_State* state)
{
    int index=luaL_checkinteger(state,1);
    
    SDL_LockAudio();
    playingsounds[index].sound=NULL;
    playingsounds[index].pos=0;
    playingsounds[index].volume=0;
    playingsounds[index].device=-1;
    SDL_UnlockAudio();
    
    return 0;
}

int audio_loadWav(lua_State* state)
{
    const char* file=luaL_checkstring(state,1);
    ProteoSound* sound=pushProteoSound(state);

    MD5((unsigned char*)file, strlen(file), (unsigned char*)sound->id);
    sound->next=NULL;
    
    ProteoSound *current=sounds;
    
    while(current!=NULL){
        if(memcmp(current->id,sound->id,MD5_DIGEST_LENGTH)==0)
        {
            if(verbose) printf("Found sound: %s\n",file);
            sound->size=current->size;
            sound->buffer=malloc(sound->size);
            memcpy(sound->buffer,current->buffer,sound->size);
            sound->spec=current->spec;
            sound->next=current->next;
            current->next=sound;
            
            return 1;
        }
        current=current->next;
    }
    
    if(SDL_LoadWAV (file, &sound->spec, &sound->buffer, &sound->size)==NULL)
    {
        printf("SDL_LoadWAV failed: %s %s\n", file, SDL_GetError());
    }
    
    
    if(sounds==NULL) sounds=sound;
    else
    {
        current=sounds;
        while(current->next!=NULL) current=(ProteoSound*)current->next;
        current->next=sound;
    }
    
    
    return 1;
}

int audio_newStream(lua_State* state)
{
    long in_freq=luaL_checkinteger(state,1);
    long in_format=luaL_checkinteger(state,2);
    long in_channels=luaL_checkinteger(state,3);

    long out_freq=luaL_checkinteger(state,4);
    long out_format=luaL_checkinteger(state,5);
    long out_channels=luaL_checkinteger(state,6);

    ProteoStream *stream = pushProteoStream(state,in_freq,  in_format,in_channels, out_freq,  out_format,out_channels);
    printf("STREAM -IN: Fr:%ld %s -OUT: Fr:%ld %s \n" ,in_freq,formatString(in_format),out_freq,formatString(out_format));
    
    if (stream == NULL) {
        printf("NewStream error: %s\n", SDL_GetError());
    } else {
        if(streams==NULL) streams=stream;
        else
        {
            //ProteoStream* current=streams;
            //while(current->next!=NULL) current=(ProteoStream*)current->next;
            //current->next=stream;
            
            //TODO da testare
            stream->next=streams;
            streams=stream;
        }
   }
    
    return 1;
}

int audio_playStream(lua_State* state)
{
    int dev=luaL_checkinteger(state,1);
    ProteoStream *stream = checkProteoStream(state, 2);
    
    stream->state=1;
    stream->device=dev;
    
    return 0;
}

int audio_recordStream(lua_State* state)
{
    int dev=luaL_checkinteger(state,1);
    ProteoStream *stream = checkProteoStream(state, 2);
    
    stream->state=2;
    stream->device=dev;
    
    return 0;
}

int audio_stopStream(lua_State* state)
{
    int dev=luaL_checkinteger(state,1);
    ProteoStream *stream = checkProteoStream(state, 2);
    
    stream->state=0;
    stream->device=-1;
    
    return 0;
}

int audio_getStream(lua_State* state)
{
    ProteoStream *stream = checkProteoStream(state, 1);
    int req=luaL_checkinteger(state,2);
    int avail=SDL_AudioStreamAvailable(stream->stream);
    if ( avail < req) {
        if(debug) printf("AVAIL: %d < REQ: %d  \n",avail,req);
        lua_pushnil(state);
        return 1;
    }

    Uint8 *buffer=malloc(avail); //Si può evitare?
    int gotten = SDL_AudioStreamGet(stream->stream, buffer, avail);
    if(debug) printf("AVAIL: %d REQ: %d GOT: %d \n",avail,req,gotten);
    
    if(gotten==-1)
    {
        printf("SDL_AudioStreamGet Error: %s\n", SDL_GetError());
        lua_pushnil(state);
        free(buffer);
        return 1;
    }
    char *b64 =(char*)malloc(1+Base64encode_len(avail));
    Base64encode(b64,(char*)buffer,avail);
    b64[Base64encode_len(avail)]=0;
    
    lua_pushstring(state, b64);
    free(b64);
    free(buffer);
    
    return 1;
}

int audio_putStream(lua_State* state)
{
    ProteoStream *stream = checkProteoStream(state, 1);
    const char* data=luaL_checkstring(state,2);
    
    char *tmp=(char *)malloc(Base64decode_len(data));
    int len = Base64decode(tmp, data);
    
    int rc = SDL_AudioStreamPut(stream->stream, tmp, len);
    if (rc == -1) {
        printf("SDL_AudioStreamPut Error: %s\n", SDL_GetError());
        return 0;
    }

    return 0;
}

int audio_createBuffer(lua_State* state)
{
    const char* id=luaL_checkstring(state,1);
    long freq=luaL_checkinteger(state,2);
    long format=luaL_checkinteger(state,3);
    long channels=luaL_checkinteger(state,4);
    long tsize=luaL_checkinteger(state,5);
    
    ProteoSound* sound=pushProteoSound(state);

    strlcpy(sound->id,id,MD5_DIGEST_LENGTH);
    sound->next=NULL;
    sound->spec.freq=freq;
    sound->spec.format=format;
    sound->spec.channels=channels;

    sound->size=tsize * freq * channels * ( SDL_AUDIO_BITSIZE( format ) / 8 );
    
    sound->buffer=malloc(sound->size);

    if(sounds==NULL) sounds=sound;
    else
    {
        ProteoSound* current=sounds;
        while(current->next!=NULL) current=(ProteoSound*)current->next;
        current->next=sound;
    }
    
    
    return 1;
}

int audio_recordAudio(lua_State* state)
{
    int dev=luaL_checkinteger(state,1);
    ProteoSound* sound=checkProteoSound(state,2);

    SDL_LockAudio();
    recordingsounds[recIndex].sound=sound;
    recordingsounds[recIndex].pos=0;
    recordingsounds[recIndex].device=dev;
    recordingsounds[recIndex].state=1;
    SDL_UnlockAudio();
    
    lua_pushinteger(state, recIndex);
    recIndex=(recIndex+1)%MAX_REC;
    
    return 1;
}

//==============================================================================
//   LUA
//==============================================================================
void add_audio_proteo(lua_State* state)
{
	addFunction_proteo(state,"audio","init",audio_init);
    addFunction_proteo(state,"audio","quit",audio_quit);
    addFunction_proteo(state,"audio","infoDrivers",audio_infoDrivers);
    addFunction_proteo(state,"audio","infoDevices",audio_infoDevices);
    addFunction_proteo(state,"audio","infoAudio",audio_infoAudio);
    addFunction_proteo(state,"audio","convertAudio",audio_convertAudio);
    addFunction_proteo(state,"audio","getAudioStatus",audio_getAudioStatus);
    addFunction_proteo(state,"audio","openDevice",audio_openDevice);
    addFunction_proteo(state,"audio","queueAudio",audio_queueAudio);
    addFunction_proteo(state,"audio","playAudio",audio_playAudio);
    addFunction_proteo(state,"audio","stopAudio",audio_stopAudio);
    addFunction_proteo(state,"audio","loadWav",audio_loadWav);
    addFunction_proteo(state,"audio","pauseDevice",audio_pauseDevice);
    addFunction_proteo(state,"audio","newStream",audio_newStream);
    addFunction_proteo(state,"audio","playStream",audio_playStream);
    addFunction_proteo(state,"audio","recordStream",audio_recordStream);
    addFunction_proteo(state,"audio","stopStream",audio_stopStream);
    addFunction_proteo(state,"audio","getStream",audio_getStream);
    addFunction_proteo(state,"audio","putStream",audio_putStream);
    addFunction_proteo(state,"audio","createBuffer",audio_createBuffer);
    addFunction_proteo(state,"audio","recordAudio",audio_recordAudio);
    
    addTable_proteo(state,"audio","Format");
    addEnum_proteo(state,"audio","Format","AUDIO_U16SYS",AUDIO_U16SYS);
    addEnum_proteo(state,"audio","Format","AUDIO_S16SYS",AUDIO_S16SYS);
    addEnum_proteo(state,"audio","Format","AUDIO_S32SYS",AUDIO_S32SYS);
    addEnum_proteo(state,"audio","Format","AUDIO_F32SYS",AUDIO_F32SYS);
    
    addTable_proteo(state,"audio","Status");
    addEnum_proteo(state,"audio","Status","AUDIO_PLAYING",SDL_AUDIO_PLAYING);
    addEnum_proteo(state,"audio","Status","AUDIO_STOPPED",SDL_AUDIO_STOPPED);
    addEnum_proteo(state,"audio","Status","AUDIO_PAUSED",SDL_AUDIO_PAUSED);
}
