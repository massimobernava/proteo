//==============================================================================
//   COMPONENT
//==============================================================================

int findListLength(ProteoComponent *list) //returns length of list
{
	int counter = 0;
	for (; list != NULL; list = list->next)
	{
		++counter;
	}
	return counter;
}

ProteoComponent *findRandomPivotNode(ProteoComponent *head_node) //finds pivot node in list
{
	int list_length = findListLength(head_node);
	int pivot_position = rand() % list_length;

	for (int count = 0; count < pivot_position; count++)
	{
		head_node = head_node->next;
	}
	return head_node;
}

ProteoComponent * findLastNode(ProteoComponent * current_node)
{
	for (; current_node->next != NULL; current_node = current_node->next);
	return current_node;
}

ProteoComponent * join(ProteoComponent* left_list, ProteoComponent * pivot, ProteoComponent * right_list)
{
    #ifdef DLINK_LIST_COMPONENT
    if(right_list!=NULL) right_list->prev=pivot;
    pivot->prev = NULL;
    #endif
	pivot->next = right_list;


	if (left_list == NULL)	return pivot;

	ProteoComponent * left_tail = findLastNode(left_list);
    #ifdef DLINK_LIST_COMPONENT
    pivot->prev = left_tail;
    #endif
    left_tail->next = pivot;

	return left_list;
}

ProteoComponent * sort(ProteoComponent * list)
{
	if (list == NULL || list->next == NULL) return list;

    ProteoComponent *pivot = findRandomPivotNode(list);

	ProteoComponent *left_sub_list = NULL, *right_sub_list = NULL;

	for (ProteoComponent * current_node = list; current_node != NULL;)
	{
		ProteoComponent * next_node = current_node->next;

		if (current_node != pivot)
		{
			if (current_node->layer <= pivot->layer)
			{
                #ifdef DLINK_LIST_COMPONENT
                if(left_sub_list!=NULL) left_sub_list->prev = current_node;
                #endif
                current_node->next = left_sub_list;
				left_sub_list = current_node;
			}
			else
			{
                #ifdef DLINK_LIST_COMPONENT
                if(right_sub_list!=NULL) right_sub_list->prev = current_node;
                #endif
                current_node->next = right_sub_list;
				right_sub_list = current_node;
			}
		}
		current_node = next_node;
	}

	return join(sort(left_sub_list), pivot, sort(right_sub_list));
}

void addComponent(ProteoComponent * component,ProteoComponent** list)
{
    if(*list==NULL)
    {
        component->next=NULL;
        #ifdef DLINK_LIST_COMPONENT
        component->prev=NULL;
        #endif
        *list=component;
    }
    else
    {
        #ifdef DLINK_LIST_COMPONENT
        (*list)->prev=component;
        component->prev=NULL;
        #endif
        component->next=*list;
        (*list)=component;
        /*ProteoComponent* current=*list;
        while(current->next!=NULL) current=(ProteoComponent*)current->next;
        current->next=component;*/
        components=sort(*list);
    }
}

//==============================================================================
//   TEXTURE
//==============================================================================

#ifdef __EMSCRIPTEN__
void downloadTextureSucceeded(emscripten_fetch_t *fetch)
{
  if(verbose) printf("Finished downloading texture %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];

    if(fetch->userData!=NULL)
    {

		SDL_RWops *stream = SDL_RWFromConstMem(fetch->data,fetch->numBytes);
        if (stream == NULL) {
            printf("SDL_RWFromConstMem Error: %s\n",SDL_GetError());
        }
        else
        {
            ((ProteoTexture*)fetch->userData)->texture=IMG_LoadTexture_RW(gRenderer, stream, 1);
            if(((ProteoTexture*)fetch->userData)->texture==NULL)
             printf("Error download texture %s : %s\n",((ProteoTexture*)fetch->userData)->id,SDL_GetError());

		}
    }

  emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void downloadTextureFailed(emscripten_fetch_t *fetch)
{
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
}
void downloadImage(const char* path,ProteoTexture* texture)
{
	emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;// | EMSCRIPTEN_FETCH_PERSIST_FILE;
    attr.userData = texture;
	attr.onsuccess = downloadTextureSucceeded;
	attr.onerror = downloadTextureFailed;
	//attr.destinationPath=path;
	const char* full_path=concat3(config.server,"/",basename(path));
	printf("Download texture: %s\n",full_path);
	emscripten_fetch_t *fetch = emscripten_fetch(&attr, full_path);
	free(full_path);
}
#else
void downloadImage(const char* path,ProteoTexture* texture)
{
    const char* url=concat3(config.server,"/",path);
    if(verbose) printf("Download image: %s\n",url);
    CURLcode ret;
    CURL *hnd;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    hnd = curl_easy_init ();
    curl_easy_setopt (hnd, CURLOPT_URL,url);
    curl_easy_setopt (hnd, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, writeMemory_callback);
    curl_easy_setopt (hnd, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt (hnd, CURLOPT_SSL_VERIFYPEER, FALSE);

    ret = curl_easy_perform (hnd);
    free(url);

    if(ret != CURLE_OK) {
        printf("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(ret));
    }
    else {
           if(debug) printf("%lu bytes retrieved\n", (unsigned long)chunk.size);

        SDL_RWops *stream = SDL_RWFromConstMem(chunk.memory, chunk.size);
        if (stream == NULL) {
            printf("SDL_RWFromConstMem Error: %s\n",SDL_GetError());
        }
        else
            texture->texture=IMG_LoadTexture_RW(gRenderer, stream, 1);
    }

       curl_easy_cleanup (hnd);
       free(chunk.memory);

    //return texture->texture;
}
#endif

static ProteoTexture* findTexture(const char* path)
{
    char md5_res[MD5_DIGEST_LENGTH];

    MD5((unsigned char*)path, strlen(path), (unsigned char*)md5_res);

    ProteoTexture* current=textures;
    while(current!=NULL){
        if(memcmp(current->id,md5_res,sizeof(md5_res))==0)
        {
            if(verbose) printf("Found texture: %s\n",path);
            return current;//->texture;
        }
        current=current->next;
    }

    return NULL;
}

static ProteoTexture* addTexture(const char* path)
{
    char md5_res[MD5_DIGEST_LENGTH];

    MD5((unsigned char*)path, strlen(path), (unsigned char*)md5_res);

    ProteoTexture* texture=malloc(sizeof(ProteoTexture));

    memcpy(texture->id,md5_res,sizeof(md5_res));
    //texture->texture=t;
    texture->texture=NULL;
    texture->next=NULL;

    if(textures==NULL)
    {
        textures=texture;
    }
    else
    {
        texture->next=textures;
        textures=texture;

        /*ProteoTexture* current=textures;
        while(current->next!=NULL) current=(ProteoTexture*)current->next;
        current->next=texture;*/
    }

    return texture;
}

static ProteoTexture* newTexture(SDL_Renderer* renderer,const char* path)
{
    /*char md5_res[MD5_DIGEST_LENGTH];

    MD5((unsigned char*)path, strlen(path), (unsigned char*)md5_res);

    ProteoTexture* current=textures;
    while(current!=NULL){
        if(memcmp(current->id,md5_res,sizeof(md5_res))==0)
        {
            if(verbose) printf("Found texture: %s\n",path);
            return current->texture;
        }
        current=current->next;
    }*/
    if(path[0]=='@')
    {
        //Create empty texture
        ProteoTexture* tmp=addTexture(path);
        return tmp;
    }
    ProteoTexture* tmp=findTexture(path);
    if(tmp!=NULL) return tmp;

    tmp=addTexture(path);
    SDL_Texture* texture=IMG_LoadTexture(renderer,path);

    if(texture==NULL) downloadImage(path,tmp);
    else
        tmp->texture=texture;

    /*ProteoTexture* texture=malloc(sizeof(ProteoTexture));

    memcpy(texture->id,md5_res,sizeof(md5_res));
    texture->texture=ret;
    texture->next=NULL;

    if(textures==NULL)
    {
        textures=texture;
    }
    else
    {
        ProteoTexture* current=textures;
        while(current->next!=NULL) current=(ProteoTexture*)current->next;
        current->next=texture;
    }*/

    return tmp;
}

void freetextures()
{
    if(debug)printf("Free textures\n");
  while(textures!=NULL)
  {
    ProteoTexture* tmp=(ProteoTexture*)textures->next;


    if(textures->texture!=NULL) SDL_DestroyTexture(textures->texture);
    free(textures);
    textures=tmp;
  }
}

//==============================================================================
//   FONT
//==============================================================================

#ifdef __EMSCRIPTEN__
void downloadFontSucceeded(emscripten_fetch_t *fetch)
{
  if(verbose) printf("Finished downloading font %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];

    if(fetch->userData!=NULL)
    {
		writedatafile(((ProteoFont*)fetch->userData)->path,fetch->data,fetch->numBytes);

		EM_ASM(
			FS.syncfs(function (err) {
				//console.log("Download font error: "+err);
			});
		);

        ((ProteoFont*)fetch->userData)->font=TTF_OpenFont(((ProteoFont*)fetch->userData)->path,((ProteoFont*)fetch->userData)->size);

		if(((ProteoFont*)fetch->userData)->font==NULL)
			printf("Error download %s : \n",((ProteoFont*)fetch->userData)->path,SDL_GetError());
		//foreach component if component-> font == font updatetxt
		else
		{
			ProteoComponent* current=components;

			while(current!=NULL)
			{
				if(current->font==(ProteoFont*)fetch->userData)
				{
					if(debug) printf("Update font: %s\n",current->id);
					updatetxt(current,gRenderer);
				}
				current=current->next;
			}
		}
    }

  emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void downloadFontFailed(emscripten_fetch_t *fetch)
{
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
}

void downloadFont(const char* path,ProteoFont* font)
{
    emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;// | EMSCRIPTEN_FETCH_PERSIST_FILE;
    attr.userData = font;
	attr.onsuccess = downloadFontSucceeded;
	attr.onerror = downloadFontFailed;
	//attr.destinationPath=path;
	const char* full_path=concat3(config.server,"/fonts/",basename(path));
	if(verbose) printf("Download font: %s\n",full_path);
	emscripten_fetch_t *fetch = emscripten_fetch(&attr, full_path);
	free(full_path);

	/*EM_ASM({
		var http = new XMLHttpRequest();
		var url = UTF8ToString($0)+UTF8ToString($1);
		http.onreadystatechange = function() {
			if(http.readyState == 4 && http.status == 200) {
				//alert("response: "+http.responseURL);
				//var data = JSON.parse(http.responseText);

				//FS.write....

			}


        };
        http.open("GET", url, true);


		},config.server,path);*/

}
#else
void downloadFont(const char* path,ProteoFont* font)
{
    const char* url=concat3(config.server,"/fonts/",basename(path));
    if(verbose) printf("Download font: %s\n",url);
    CURLcode ret;
    CURL *hnd;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    hnd = curl_easy_init ();
    curl_easy_setopt (hnd, CURLOPT_URL,url);
    curl_easy_setopt (hnd, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, writeMemory_callback);
    curl_easy_setopt (hnd, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt (hnd, CURLOPT_SSL_VERIFYPEER, FALSE);

    ret = curl_easy_perform (hnd);
    free(url);

    if(ret != CURLE_OK) {
        printf("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(ret));
    }
    else {
           if(debug) printf("%lu bytes retrieved\n", (unsigned long)chunk.size);

        writedatafile(path,chunk.memory,chunk.size);
        font->font=TTF_OpenFont(path,font->size);
        if(font->font==NULL) printf("TTF_OpenFont Error: %s \n",path);

        //TODO sarebbe meglio un download che non salva ma se uso "TTF_OpenFontRW" non funziona

        /*SDL_RWops *stream = SDL_RWFromConstMem(chunk.memory, chunk.size);
        if (stream == NULL) {
            printf("SDL_RWFromConstMem Error: %s\n",SDL_GetError());
        }*/
        else
        {
            //font->font=TTF_OpenFontRW(stream, 1,font->size);
            ProteoComponent* current=components;

			while(current!=NULL)
			{

				if(current->font==font)
				{
					if(debug) printf("Update font: %s\n",current->id);
					updatetxt(current,gRenderer);
				}
				current=current->next;
			}
		}
    }

       curl_easy_cleanup (hnd);
       free(chunk.memory);

    //return texture->texture;
}
#endif

static ProteoFont* findFont(const char* path,int size)
{
    char md5_res[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)path, strlen(path), (unsigned char*)md5_res);

    ProteoFont* current=fonts;
    while(current!=NULL){
        if(memcmp(current->id,md5_res,sizeof(md5_res))==0 && size==current->size)
        {
            if(verbose) printf("Found font: %s size: %d\n",path,size);
            return current;//->font;
        }
        current=current->next;
    }

    return NULL;
}

static ProteoFont* addFont(const char* path,int size)
{
    char md5_res[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)path, strlen(path), (unsigned char*)md5_res);

    ProteoFont* font=malloc(sizeof(ProteoFont));
     memcpy(font->id,md5_res,sizeof(md5_res));
    font->font=NULL;
    font->size=size;
    font->next=NULL;
    strcpy(font->path,path);


    if(fonts==NULL)
    {
        fonts=font;
    }
    else
    {
        font->next=fonts;
        fonts=font;

        //ProteoFont* current=fonts;
        //while(current->next!=NULL) current=(ProteoFont*)current->next;
        //current->next=font;
    }

    return font;
}
static ProteoFont* newFont(const char* path,int size)
{
    /*char md5_res[MD5_DIGEST_LENGTH];

    MD5((unsigned char*)path, strlen(path), (unsigned char*)md5_res);

    ProteoFont* current=fonts;
    while(current!=NULL){
        if(memcmp(current->id,md5_res,sizeof(md5_res))==0 && size==current->size)
        {
            if(verbose) printf("Found font: %s\n",path);
            return current;//->font;
        }
        current=current->next;
    }*/
    ProteoFont* font=findFont(path,size);
    if(font!=NULL) return font;

    font=addFont(path,size);
    TTF_Font* f = TTF_OpenFont(path,size);
    //#ifdef __EMSCRIPTEN__
    if(f==NULL) downloadFont(path,font);
    else
    //#endif
		font->font=f;

    return font;

    /*ProteoFont* font=malloc(sizeof(ProteoFont));
     memcpy(font->id,md5_res,sizeof(md5_res));
    font->font=NULL;
    font->size=size;
    font->next=NULL;
    strcpy(font->path,path);

    #ifdef __EMSCRIPTEN__
    font->font = TTF_OpenFont(path,size);
    if(font->font==NULL) downloadFont(path,font);
    #else
    font->font = TTF_OpenFont(path,size);
    #endif

    //TTF_Font* ret=TTF_OpenFont(path,size);

    if(fonts==NULL)
    {
        fonts=font;
    }
    else
    {
        ProteoFont* current=fonts;
        while(current->next!=NULL) current=(ProteoFont*)current->next;
        current->next=font;
    }*/

    //return font;//ret;
}

void freefonts()
{
    if(debug)printf("Free fonts\n");
  while(fonts!=NULL)
  {
    ProteoFont* tmp=(ProteoFont*)fonts->next;


    if(fonts->font!=NULL) TTF_CloseFont(fonts->font);
    free(fonts);
    fonts=tmp;
  }
}

//==============================================================================
//   ICON
//==============================================================================

#ifdef __EMSCRIPTEN__

void downloadIconSucceeded(emscripten_fetch_t *fetch)
{
  if(verbose) printf("Finished downloading icon %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];

    if(fetch->userData!=NULL)
    {

		mkpath((char*)((ProteoIcon*)fetch->userData)->path);
		writedatafile(((ProteoIcon*)fetch->userData)->path,fetch->data,fetch->numBytes);

		//hexDump (((ProteoIcon*)fetch->userData)->path, fetch->data, fetch->numBytes);


		EM_ASM(
			FS.syncfs(function (err) {
				//console.log("Download icon error: "+err);
			});
		);

        ((ProteoIcon*)fetch->userData)->texture=IMG_LoadTexture(gRenderer,((ProteoIcon*)fetch->userData)->path);
		if(((ProteoIcon*)fetch->userData)->texture!=NULL)
			SDL_SetTextureColorMod( ((ProteoIcon*)fetch->userData)->texture, ((ProteoIcon*)fetch->userData)->color.r, ((ProteoIcon*)fetch->userData)->color.g, ((ProteoIcon*)fetch->userData)->color.b );
		else printf("Error download icon %s : %s\n",((ProteoIcon*)fetch->userData)->path,SDL_GetError());
		//foreach component if component-> font == font updatetxt
		/*ProteoComponent* current=components;

		while(current!=NULL)
		{

            if(current->font==(ProteoFont*)fetch->userData)
			{
				if(debug) printf("Update font: %s\n",current->id);
				updatetxt(current,gRenderer);
			}
			current=current->next;
		}*/
    }

  emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void downloadIconFailed(emscripten_fetch_t *fetch)
{
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
}

void downloadIcon(const char* path,ProteoIcon* icon)
{
    emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;// | EMSCRIPTEN_FETCH_PERSIST_FILE;
    attr.userData = icon;
	attr.onsuccess = downloadIconSucceeded;
	attr.onerror = downloadIconFailed;
	//attr.destinationPath=path;
	const char* full_path=concat3(config.server,"/icons/",basename(path));
	printf("Download icon: %s\n",full_path);
	emscripten_fetch_t *fetch = emscripten_fetch(&attr, full_path);
	free(full_path);

}
#else
void downloadIcon(const char* path,ProteoIcon* icon)
{
    const char* url=concat3(config.server,"/icons/",basename(path));
    printf("Download icon: %s\n",url);
    CURLcode ret;
    CURL *hnd;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    hnd = curl_easy_init ();
    curl_easy_setopt (hnd, CURLOPT_URL,url);
    curl_easy_setopt (hnd, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, writeMemory_callback);
    curl_easy_setopt (hnd, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt (hnd, CURLOPT_SSL_VERIFYPEER, FALSE);

    ret = curl_easy_perform (hnd);
    free(url);

    if(ret != CURLE_OK) {
        printf("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(ret));
    }
    else {
           if(debug) printf("%lu bytes retrieved\n", (unsigned long)chunk.size);

        SDL_RWops *stream = SDL_RWFromConstMem(chunk.memory, chunk.size);
        if (stream == NULL) {
            printf("SDL_RWFromConstMem Error: %s\n",SDL_GetError());
        }
        else
        {
            icon->texture=IMG_LoadTexture_RW(gRenderer, stream, 1);
            SDL_SetTextureColorMod( icon->texture, icon->color.r, icon->color.g, icon->color.b );
        }
    }

       curl_easy_cleanup (hnd);
       free(chunk.memory);

    //return texture->texture;
}
#endif

static ProteoIcon* findIcon(const char* path,ProteoColor color)
{
    char md5_res[MD5_DIGEST_LENGTH];

    MD5((unsigned char*)path, strlen(path), (unsigned char*)md5_res);

    ProteoIcon* current=icons;
    while(current!=NULL){
        if(memcmp(current->id,md5_res,sizeof(md5_res))==0 && current->color.r==color.r && current->color.g==color.g && current->color.b==color.b)
        {
            if(verbose) printf("Found icon: %s\n",path);
            return current;
        }
        current=current->next;
    }

    return NULL;
}

static ProteoIcon* addIcon(const char* path)
{
    char md5_res[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)path, strlen(path), (unsigned char*)md5_res);

    ProteoIcon* icon=malloc(sizeof(ProteoIcon));
     memcpy(icon->id,md5_res,sizeof(md5_res));
    icon->next=NULL;
    strcpy(icon->path,path);

    if(icons==NULL)
    {
        icons=icon;
    }
    else
    {
        icon->next=icons;
        icons=icon;

        //ProteoIcon* current=icons;
        //while(current->next!=NULL) current=(ProteoFont*)current->next;
        //current->next=font;
    }

    return icon;}

static ProteoIcon* newIcon(SDL_Renderer* renderer,const char* path,ProteoColor color)
{
    ProteoIcon* icon=findIcon(path,color);
    if(icon!=NULL) return icon;

    icon=addIcon(path);
    icon->color=color;
    SDL_Texture* i=IMG_LoadTexture(renderer,path);
    if(i!=NULL) SDL_SetTextureColorMod( i, color.r, color.g, color.b );
    if(i==NULL) printf("Error Icon IMG_LoadTexture: %s\n",path);
    //#ifdef __EMSCRIPTEN__
    if(i==NULL) downloadIcon(path,icon);
    else
    //#endif
		icon->texture=i;
    icon->color=color;
    return icon;
}

void freeicons(void)
{
    if(debug)printf("Free icons\n");
    while(icons!=NULL)
    {
      ProteoIcon* tmp=(ProteoIcon*)icons->next;


      if(icons->texture!=NULL) SDL_DestroyTexture(icons->texture);
      free(icons);
      icons=tmp;
    }
}

//==============================================================================
//   GRAPHICS
//==============================================================================

static int graphics_setbackground (lua_State *state) {

	backgroundColor=hex2color(luaL_checkstring(state,1));

  	return 0;
}

static int graphics_setLayer (lua_State *state) {
	ProteoComponent* pc=toProteoComponent(state,1);
	int value=luaL_checkint(state,2);

    if(pc!=NULL)
    {
        if(verbose) printf("graphics.setLayer\n");

        if(value==-1) //TOP
        {
            ProteoComponent* current=components;
            while(current!=NULL)
            {
                if(current->layer>value) value=current->layer;
                current=current->next;
            }
            value++;
        }
        else if(value==-2) //BOTTOM
        {
            value=INT32_MAX;
            ProteoComponent* current=components;
            while(current!=NULL)
            {
                if(current->layer<value) value=current->layer;
                current=current->next;
            }
            value--;
        }
        pc->layer=value;
        components=sort(components);
    }

  	return 0;
}

static int graphics_setColor (lua_State *state) {

  ProteoComponent* pc=toProteoComponent(state,1);
  const char* color=luaL_checkstring(state,2);

    if(pc->type==Rect)
    {
        pc->component.rect.color=hex2color(color);
    }
 // pc->color=hex2color(color);
 // if(debug) printf("graphics.setColor: %s\n",pc->id);

  return 0;
}

static int graphics_changeImage (lua_State *state)
{
	ProteoComponent* pc=toProteoComponent(state,1);

    if(pc->type==Deleted) return 0;

    if (lua_isstring(state,2)==1)
    {
        const char* file=luaL_checkstring(state,2);

        if(debug) printf("graphics.changeImage: %s\n",file);

        if(pc->type==Image)
        {
            //if(pc->component.image.image->texture!=NULL) SDL_DestroyTexture(pc->component.image.image->texture);
            pc->component.image.image=newTexture(gRenderer,file);
        }
        else if(pc->type==Sprite)
        {
            //if(pc->component.sprite.image->texture!=NULL) SDL_DestroyTexture(pc->component.sprite.image->texture);
            pc->component.sprite.image=newTexture(gRenderer,file);
        }
        //if(pc->texture!=NULL) SDL_DestroyTexture(pc->texture);
        //pc->texture=newTexture(gRenderer,file); //TODO da testare IMG_LoadTexture(gRenderer,file);
    }
    else
    {
#ifdef PROTEO_OPENCV
        void* mat = luaL_checkudata(L, 2, "OpenCVMat");

        OCVImage opencvimg=getImage(mat);
        //if(debug) printf("graphics.changeImage OpenCV(%d,%d) Depth:%d Channels:%d Step:%lu\n",opencvimg.width, opencvimg.height,opencvimg.depth,opencvimg.channels,opencvimg.step);
        if(pc->surface!=NULL) SDL_FreeSurface(pc->surface);

        pc->surface = SDL_CreateRGBSurfaceFrom((void*)opencvimg.data,
                                               opencvimg.width, opencvimg.height,
                                               opencvimg.depth*opencvimg.channels,
                                               opencvimg.step,
                                               //0xffffff, 0xffffff, 0xffffff,0);
                                               0xff0000, 0x00ff00, 0x0000ff, 0);
        if(pc->surface == NULL)
        {
          printf("ChangeImage ERROR: couldn't convert Mat to Surface.\n");
          return 0;
        }

        SDL_Texture* texture=SDL_CreateTextureFromSurface(gRenderer, pc->surface);
        if(pc->type==Image)
        {
            if(pc->component.image.image->texture!=NULL) SDL_DestroyTexture(pc->component.image.image->texture);
            pc->component.image.image->texture=texture;
        }
        else if(pc->type==Sprite)
        {
            if(pc->component.sprite.image->texture!=NULL) SDL_DestroyTexture(pc->component.sprite.image->texture);
            pc->component.sprite.image->texture=texture;
        }

        //if(pc->texture!=NULL) SDL_DestroyTexture(pc->texture);
        //pc->texture = SDL_CreateTextureFromSurface(gRenderer, pc->surface);
        if(texture == NULL)
        {
          printf("ChangeImage ERROR: couldn't convert Mat(converted to surface) to Texture.\n");
          return 0;
        }
        /*unsigned char * texture_data = NULL;
        int texture_pitch = 0;

        SDL_LockTexture(pc->texture, 0, (void **)&texture_data, &texture_pitch);
        fillTexture(texture_data,mat);
        SDL_UnlockTexture(pc->texture);*/
#endif
    }



  	return 0;
}

static int graphics_isInside(lua_State *state)
{
	ProteoComponent* pc=toProteoComponent(state,1);
	const int x=luaL_checkint(state,2);
	const int y=luaL_checkint(state,3);

	if(debug) printf("graphics.isInside: x=%d y=%d\n",x,y);

	int inside=FALSE;
	SDL_Point point={x,y};

    SDL_Rect pc_rect=pc->rect;

    if(pc->parent!=NULL)
    {
        pc_rect.x+=pc->parent->rect.x;
        pc_rect.y+=pc->parent->rect.y;
    }

	if(pc->type==Ellips)
	{
		SDL_Rect ellipse={pc_rect.x-pc_rect.w,pc_rect.y-pc_rect.h,2*pc_rect.w,2*pc_rect.h};
		inside=SDL_PointInRect(&point,&ellipse)==TRUE;
	}
	else inside=SDL_PointInRect(&point,&pc_rect)==TRUE;

	lua_pushboolean (state, inside);

  	return 1;
}

static int graphics_setHidden (lua_State *state) {

	ProteoComponent* pc=toProteoComponent(state,1);
	const int hidden=lua_toboolean(state,2);
	pc->hidden=hidden;

	return 0;
}

static int graphics_setPosition (lua_State *state) {

	ProteoComponent* pc=toProteoComponent(state,1);
	const int x=luaL_checkint(state,2);
	const int y=luaL_checkint(state,3);

	if(debug) printf("graphics.setPosition : %s\n",pc->id);

	if(pc->type==Image)
	{
		pc->rect.x=x-pc->rect.w/2;
		pc->rect.y=y-pc->rect.h/2;
	}
	else
	{
		pc->rect.x=x;
		pc->rect.y=y;
	}

	return 0;
}

static int graphics_getPosition (lua_State *state) {

    ProteoComponent* pc=toProteoComponent(state,1);

    if(debug) printf("graphics.getPosition : %s\n",pc->id);

    lua_newtable(state);

    lua_pushnumber(state, pc->rect.x);
    lua_setfield(state, -2, "x");

    lua_pushnumber(state, pc->rect.y);
    lua_setfield(state, -2, "y");

    return 1;
}

static int graphics_addFrame (lua_State *state) {

    ProteoComponent* sprite=toProteoComponent(state,1);
    const int pos_x=luaL_checkint(state,2);
    const int pos_y=luaL_checkint(state,3);
    const int width=luaL_checkint(state,4);
    const int height=luaL_checkint(state,5);

    sprite->framesSource[sprite->nFrame].x=pos_x;
    sprite->framesSource[sprite->nFrame].y=pos_y;
    sprite->framesSource[sprite->nFrame].w=width;
    sprite->framesSource[sprite->nFrame].h=height;

    sprite->framesDestination[sprite->nFrame].x=0;
    sprite->framesDestination[sprite->nFrame].y=0;

    sprite->nFrame+=1;

    if(debug) printf("graphics.addFrame : %s(%d)\n",sprite->id,sprite->nFrame);

    return 0;
}

static int graphics_addFrameSource(lua_State *state) {

    ProteoComponent* sprite=toProteoComponent(state,1);
    const int pos_x=luaL_checkint(state,2);
    const int pos_y=luaL_checkint(state,3);
    const int width=luaL_checkint(state,4);
    const int height=luaL_checkint(state,5);
    const int tw=luaL_checkint(state,6);
    const int th=luaL_checkint(state,7);

    sprite->framesSource[sprite->nFrame].x=pos_x;
    sprite->framesSource[sprite->nFrame].y=pos_y;
    sprite->framesSource[sprite->nFrame].w=width;
    sprite->framesSource[sprite->nFrame].h=height;

    sprite->framesDestination[sprite->nFrame].x=tw;
    sprite->framesDestination[sprite->nFrame].y=th;

    sprite->nFrame+=1;

    if(debug) printf("graphics.addFrame : %s(%d)\n",sprite->id,sprite->nFrame);

    return 0;
}

static int graphics_setFrame (lua_State *state) {

    ProteoComponent* sprite=toProteoComponent(state,1);
    const int frame=luaL_checkint(state,2);

    if(debug) printf("graphics.setFrame : %s(%d)\n",sprite->id,frame);

    sprite->currentFrame=frame;

    return 0;
}

static int graphics_flipH (lua_State *state) {

    return 0;
}

static int graphics_flipV (lua_State *state) {

    return 0;
}

static int graphics_infoComponent (lua_State *state) {

    return 0;
}

#ifdef BEZIER
void addBezierToPolygon(ProteoComponent* polygon,int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3,float step)
{
    if(polygon->component.polygon.nbeziers == 0 )
    {
        polygon->component.polygon.beziers=malloc(sizeof(_bezier));

       /* polygon->component.polygon.beziers[0].P0.x=x0;
        polygon->component.polygon.beziers[0].P0.y=y0;
        polygon->component.polygon.beziers[0].P1.x=x1;
        polygon->component.polygon.beziers[0].P1.y=y1;
        polygon->component.polygon.beziers[0].P2.x=x2;
        polygon->component.polygon.beziers[0].P2.y=y2;
        polygon->component.polygon.beziers[0].P3.x=x3;
        polygon->component.polygon.beziers[0].P3.y=y3;

        polygon->component.polygon.nbeziers=1;
        polygon->component.polygon.beziers[0].step=step;*/
    }
    else
    {
        polygon->component.polygon.beziers=realloc(polygon->component.polygon.beziers,(polygon->component.polygon.nbeziers+1)*sizeof(_bezier));


    }

    polygon->component.polygon.beziers[polygon->component.polygon.nbeziers].P0.x=x0;
    polygon->component.polygon.beziers[polygon->component.polygon.nbeziers].P0.y=y0;
    polygon->component.polygon.beziers[polygon->component.polygon.nbeziers].P1.x=x1;
    polygon->component.polygon.beziers[polygon->component.polygon.nbeziers].P1.y=y1;
    polygon->component.polygon.beziers[polygon->component.polygon.nbeziers].P2.x=x2;
    polygon->component.polygon.beziers[polygon->component.polygon.nbeziers].P2.y=y2;
    polygon->component.polygon.beziers[polygon->component.polygon.nbeziers].P3.x=x3;
    polygon->component.polygon.beziers[polygon->component.polygon.nbeziers].P3.y=y3;

    /*if(x0!=x3)
    {
        float m=(float)(y0-y3)/(float)(x0-x3);
        float q=(float)(x0*y3-x3*y0)/(float)(x0-x3);
        polygon->component.polygon.beziers[polygon->component.polygon.nbeziers].m=m;
        polygon->component.polygon.beziers[polygon->component.polygon.nbeziers].q=q;
    }*/

    polygon->component.polygon.beziers[polygon->component.polygon.nbeziers].step=step;
    polygon->component.polygon.nbeziers+=1;

}
#endif

void addPointToPolygon(ProteoComponent* polygon,int x,int y)
{
    if(polygon->component.polygon.npoints==0)
    {
        polygon->component.polygon.vx=malloc(sizeof(Sint16));
        polygon->component.polygon.vy=malloc(sizeof(Sint16));
        polygon->component.polygon.tvx=malloc(sizeof(Sint16));
        polygon->component.polygon.tvy=malloc(sizeof(Sint16));

        polygon->component.polygon.vx[0]=x;
        polygon->component.polygon.vy[0]=y;
        polygon->component.polygon.npoints=1;
    }
    else
    {
        polygon->component.polygon.vx=realloc(polygon->component.polygon.vx,(polygon->component.polygon.npoints+1)*sizeof(Sint16));
        polygon->component.polygon.vy=realloc(polygon->component.polygon.vy,(polygon->component.polygon.npoints+1)*sizeof(Sint16));
        polygon->component.polygon.tvx=realloc(polygon->component.polygon.tvx,(polygon->component.polygon.npoints+1)*sizeof(Sint16));
        polygon->component.polygon.tvy=realloc(polygon->component.polygon.tvy,(polygon->component.polygon.npoints+1)*sizeof(Sint16));

        polygon->component.polygon.vx[polygon->component.polygon.npoints]=x;
        polygon->component.polygon.vy[polygon->component.polygon.npoints]=y;

        polygon->component.polygon.npoints+=1;
    }

}
static int graphics_addPoint(lua_State *state) {

    ProteoComponent* polygon=toProteoComponent(state,1);
    if(polygon->type!=Polyg) return 0;

    int x=luaL_checkinteger(state, 2);
    int y=luaL_checkinteger(state, 3);

    addPointToPolygon(polygon, x, y);

    return 0;
}

static int graphics_addBezier(lua_State *state) {

    /* Function that take input as Control Point x_coordinates and
    Control Point y_coordinates and draw bezier curve */
    ProteoComponent* polygon=toProteoComponent(state,1);
    if(polygon->type!=Polyg) return 0;

    int x0=luaL_checkinteger(state, 2);
    int y0=luaL_checkinteger(state, 3);

    int x1=luaL_checkinteger(state, 4);
    int y1=luaL_checkinteger(state, 5);

    int x2=luaL_checkinteger(state, 6);
    int y2=luaL_checkinteger(state, 7);

    int x3=luaL_checkinteger(state, 8);
    int y3=luaL_checkinteger(state, 9);

    float step=luaL_optnumber(state, 10, -1.0);
    if(step==-1.0)
    step=0.1;

    #ifdef BEZIER
    addBezierToPolygon(polygon,x0,y0,x1,y1,x2,y2,x3,y3,step);
    #endif

    float xu = 0.0 , yu = 0.0 , u = 0.0 ;
    //int i = 0 ;
    for(u = 0.0 ; u <= 1.0 ; u += step)
    {
        xu = pow(1-u,3)*x0+3*u*pow(1-u,2)*x1+3*pow(u,2)*(1-u)*x2
                 +pow(u,3)*x3;
        yu = pow(1-u,3)*y0+3*u*pow(1-u,2)*y1+3*pow(u,2)*(1-u)*y2
                +pow(u,3)*y3;

        addPointToPolygon(polygon, (int)xu, (int)yu);
    }

    return 0;
}

static int graphics_loadSVG(lua_State *state)
{
    const char* filepath=luaL_checkstring(state, 1);
    int dpi=luaL_optint(state, 2, -1);

    if(dpi==-1) dpi=96;

    lua_newtable(state);

    // Load
    struct NSVGimage* image;
    image = nsvgParseFromFile(filepath, "px", dpi);
    if(image==NULL)
    {
        lua_pushnil(state);
        return 1;

    }
    printf("size: %f x %f\n", image->width, image->height);

    struct NSVGshape* shape;
    struct NSVGpath* path;

    int nshape=1;
    for (shape = image->shapes; shape != NULL; shape = shape->next) {
        //printf("Shape: %s\n",shape->id);

        lua_pushinteger(state, nshape);
        lua_newtable(state);

        lua_pushstring(state, shape->id);
        lua_setfield(state, -2, "id");

        char color[8];
        sprintf(color, "#%02X%02X%02X",(shape->fill.color) & 0xff,(shape->fill.color >> 8) & 0xff,(shape->fill.color>>16) & 0xff);
        //printf("Color:%s\n",color);
        //lua_pushinteger(state, shape->fill.color);
        lua_pushstring(state, color);
        lua_setfield(state, -2, "fill_color");

        lua_pushinteger(state, shape->fill.type);
        lua_setfield(state, -2, "fill_type");

        int npath=1;
        for (path = shape->paths; path != NULL; path = path->next) {
            //printf("   Path: %d\n",path->npts);
            lua_pushinteger(state, npath);
            lua_newtable(state);

            int nbez=1;
            for (int i = 0; i < path->npts-1; i += 3) {
                float* p = &path->pts[i*2];
                //printf("x0=%f y0=%f x1=%f y1=%f x2=%f y2=%f x3=%f y3=%f\n",p[0],p[1], p[2],p[3], p[4],p[5], p[6],p[7]);
                lua_pushinteger(state, nbez);
                lua_newtable(state);

                lua_pushnumber(state, p[0]);
                lua_setfield(state, -2, "x0");

                lua_pushnumber(state, p[1]);
                lua_setfield(state, -2, "y0");

                lua_pushnumber(state, p[2]);
                lua_setfield(state, -2, "x1");

                lua_pushnumber(state, p[3]);
                lua_setfield(state, -2, "y1");

                lua_pushnumber(state, p[4]);
                lua_setfield(state, -2, "x2");

                lua_pushnumber(state, p[5]);
                lua_setfield(state, -2, "y2");

                lua_pushnumber(state, p[6]);
                lua_setfield(state, -2, "x3");

                lua_pushnumber(state, p[7]);
                lua_setfield(state, -2, "y3");

                lua_settable(state,-3);
                nbez++;
            }

            lua_settable(state,-3);
            npath++;
        }

        lua_settable(state,-3);
        nshape++;
    }

    // Delete
    nsvgDelete(image);

    return 1;
}

//==============================================================================
//   SKELETON
//==============================================================================

void freejoint(ProteoJoint* joint)
{
    /*#ifndef BEZIER
    for(int i=0;i<joint->nweights;i++)
        if(joint->weights[i]!=NULL) { free(joint->weights[i]); joint->weights[i]=NULL; }

    free(joint->weights);
    #endif
    #ifdef BEZIER
    for(int i=0;i<joint->nbez_wei;i++)
        if(joint->bez_wei[i]!=NULL) { free(joint->bez_wei[i]); joint->bez_wei[i]=NULL; }

    free(joint->bez_wei);
    #endif*/

}

void freebone(ProteoBone* bone)
{
    for(int i=0;i<bone->nweights;i++)
    {
        if(bone->weights[i]!=NULL) { free(bone->weights[i]); bone->weights[i]=NULL; }
        if(bone->transformPoints[i]!=NULL) { free(bone->transformPoints[i]); bone->transformPoints[i]=NULL; }
        if(bone->anchorPercs[i]!=NULL) { free(bone->anchorPercs[i]); bone->anchorPercs[i]=NULL; }
    }

    free(bone->weights);
    free(bone->transformPoints);
    free(bone->anchorPercs);
}

void freeskeletons()
{
  if(debug)printf("Free skeletons\n");
  while(skeletons!=NULL)
  {
    ProteoSkeleton* tmp=(ProteoSkeleton*)skeletons->next;


      if(skeletons->joints!=NULL)
      {
          for(int i=0;i<skeletons->njoints;i++) freejoint(&skeletons->joints[i]);
          free(skeletons->joints);
      }
      if(skeletons->bones!=NULL)
      {
          for(int i=0;i<skeletons->nbones;i++) freebone(&skeletons->bones[i]);
          free(skeletons->bones);
      }

      skeletons->joints=NULL;
      skeletons->bones=NULL;

      free(skeletons);

      skeletons=tmp;
  }
}

static ProteoSkeleton *checkProteoSkeleton (lua_State *L, int index)
{
    ProteoSkeleton *ps=NULL;

    if(lua_islightuserdata(L, index))
        ps=toProteoSkeleton(L,index);
    else luaL_typerror(L, index, PROTEOSKELETON);

    return ps;
}

static ProteoSkeleton *pushProteoSkeleton (lua_State *L)
{
    ProteoSkeleton *ps =malloc(sizeof(ProteoSkeleton));

    /*ps->joints=NULL;
    ps->bones=NULL;
    ps->next=NULL;*/
    //ps->hidden=FALSE;

    lua_pushlightuserdata(L, ps);

    return ps;
}

static ProteoSkeleton *toProteoSkeleton (lua_State *L, int index)
{
    ProteoSkeleton *ps = (ProteoSkeleton *)lua_touserdata(L, index);
    if (ps == NULL) luaL_typerror(L, index, PROTEOSKELETON);
    return ps;
}

static int graphics_drawSkeleton(SDL_Renderer* renderer,ProteoSkeleton* skeleton)
{
    int tx=0;
    int ty=0;

    if(skeleton->shape->hidden) return 0;

    if(skeleton->shape!=NULL)
    {
        tx=skeleton->shape->rect.x;
        ty=skeleton->shape->rect.y;
    }

    for(int i=0;i<skeleton->nbones;i++)
    {
        /*SDL_Point newA=affineTrasformation(skeleton->bones[i].a->pos,skeleton->bones[i].a->affine);
        SDL_Point newB=affineTrasformation(skeleton->bones[i].b->pos,skeleton->bones[i].b->affine);

        thickLineRGBA(renderer,
         tx+newA.x,ty+newA.y
        ,tx+newB.x,ty+newB.y

        ,4,255,255,0,255);*/

        thickLineRGBA(renderer,
         tx+skeleton->bones[i].a->tpos.x,ty+skeleton->bones[i].a->tpos.y
        ,tx+skeleton->bones[i].b->tpos.x,ty+skeleton->bones[i].b->tpos.y

        ,4,255,255,0,255);

    }

    for(int i=0;i<skeleton->njoints;i++)
    {
       /* SDL_Point newPoint=affineTrasformation(skeleton->joints[i].pos,skeleton->joints[i].affine);

        filledEllipseRGBA (renderer, tx+newPoint.x, ty+newPoint.y, 5, 5,
        255, 0, 0, 255);*/

        filledEllipseRGBA (renderer, tx+skeleton->joints[i].tpos.x, ty+skeleton->joints[i].tpos.y, 5, 5,
        255, 0, 0, 255);

        /*filledEllipseRGBA (renderer, tx+skeleton->joints[i].pos.x+skeleton->joints[i].move.x, ty+skeleton->joints[i].pos.y+skeleton->joints[i].move.y, 5, 5,
                           255, 0, 0, 255);*/
    }
    return 1;
}


static int graphics_newSkeleton(lua_State *state)
{
    //newSkeleton(id,x,y)
    const char* id=luaL_checkstring(state,1);
    //long x=luaL_checkinteger(state,2);
    //long y=luaL_checkinteger(state,3);

    ProteoSkeleton *skeleton = pushProteoSkeleton(state);

    MD5((unsigned char*)id, strlen(id), (unsigned char*)skeleton->id);
    //skeleton->pos.x=x;
    //skeleton->pos.y=y;
    skeleton->hidden=FALSE;
    skeleton->shape=NULL;
    skeleton->nbones=0;
    skeleton->bones=NULL;
    skeleton->njoints=0;
    skeleton->joints=NULL;
    skeleton->next=NULL;

    if (skeleton == NULL) {
         printf("NewSkeleton Error\n");
     } else {
         if(skeletons==NULL) skeletons=skeleton;
         else
         {
            skeleton->next=skeletons;
            skeletons=skeleton;
         }
    }

     return 1;
}

static int graphics_addJoint(lua_State *state) {

    //addJoint(id,skeleton,x,y)

    const char* id=luaL_checkstring(state,1);
    ProteoSkeleton* ps=checkProteoSkeleton(state, 2 );
    long x=luaL_checkinteger(state,3);
    long y=luaL_checkinteger(state,4);

    printf("graphics.addJoint: %s\n",id);

    if(ps->joints==NULL)
    {
        ps->joints=malloc(sizeof(ProteoJoint));

        MD5((unsigned char*)id, strlen(id), (unsigned char*)ps->joints[0].id);
        ps->joints[0].pos.x=x;
        ps->joints[0].pos.y=y;
        ps->joints[0].tpos.x=0;
        ps->joints[0].tpos.y=0;

#ifdef AFFINE
        ps->joints[0].affine[0][0]=1;
        ps->joints[0].affine[0][1]=0;
        ps->joints[0].affine[0][2]=0;
        ps->joints[0].affine[1][0]=0;
        ps->joints[0].affine[1][1]=1;
        ps->joints[0].affine[1][2]=0;
        ps->joints[0].affine[2][0]=0;
        ps->joints[0].affine[2][1]=0;
        ps->joints[0].affine[2][2]=1;
#else

        ps->joints[0].move.x=0;
        ps->joints[0].move.y=0;
#endif
        /* OLD
        #ifndef BEZIER
        ps->joints[0].nweights=0;
        ps->joints[0].weights=NULL;
        #endif

        #ifdef BEZIER
        ps->joints[0].nbez_wei=0;
        ps->joints[0].bez_wei=NULL;
        #endif*/

        ps->njoints=1;
    }
    else
    {
        ps->joints=realloc(ps->joints,(ps->njoints+1)*sizeof(ProteoJoint));

        MD5((unsigned char*)id, strlen(id), (unsigned char*)ps->joints[ps->njoints].id);
        ps->joints[ps->njoints].pos.x=x;
        ps->joints[ps->njoints].pos.y=y;
        ps->joints[ps->njoints].tpos.x=0;
        ps->joints[ps->njoints].tpos.y=0;

        #ifdef AFFINE
        ps->joints[ps->njoints].affine[0][0]=1;
        ps->joints[ps->njoints].affine[0][1]=0;
        ps->joints[ps->njoints].affine[0][2]=0;
        ps->joints[ps->njoints].affine[1][0]=0;
        ps->joints[ps->njoints].affine[1][1]=1;
        ps->joints[ps->njoints].affine[1][2]=0;
        ps->joints[ps->njoints].affine[2][0]=0;
        ps->joints[ps->njoints].affine[2][1]=0;
        ps->joints[ps->njoints].affine[2][2]=1;

#else
        ps->joints[ps->njoints].move.x=0;
        ps->joints[ps->njoints].move.y=0;
#endif
        /* OLD

         #ifndef BEZIER
        ps->joints[ps->njoints].nweights=0;
        ps->joints[ps->njoints].weights=NULL;
        #endif

        #ifdef BEZIER
        ps->joints[ps->njoints].nbez_wei=0;
        ps->joints[ps->njoints].bez_wei=NULL;
        #endif
        */
        ps->njoints+=1;
    }

    //return 1;//TODO

    return 0;
}

void resetShape(ProteoComponent* shape)
{
    for(int j=0;j<shape->component.shape.nchilds;j++)
    {
        ProteoComponent* polygon=shape->component.shape.childs[j];

        for(int i=0;i<polygon->component.polygon.npoints;i++)
        {
            polygon->component.polygon.tvx[i]=shape->rect.x;//polygon->component.polygon.vx[i]+shape->rect.x;
            polygon->component.polygon.tvy[i]=shape->rect.y;//polygon->component.polygon.vy[i]+shape->rect.y;
        }
        #ifdef BEZIER
        for(int i=0;i<polygon->component.polygon.nbeziers;i++)
        {
            polygon->component.polygon.beziers[i].T0.x=shape->rect.x;
            polygon->component.polygon.beziers[i].T0.y=shape->rect.y;
            polygon->component.polygon.beziers[i].T1.x=shape->rect.x;
            polygon->component.polygon.beziers[i].T1.y=shape->rect.y;
            polygon->component.polygon.beziers[i].T2.x=shape->rect.x;
            polygon->component.polygon.beziers[i].T2.y=shape->rect.y;
            polygon->component.polygon.beziers[i].T3.x=shape->rect.x;
            polygon->component.polygon.beziers[i].T3.y=shape->rect.y;

            /*polygon->component.polygon.beziers[i].T0.x=polygon->component.polygon.beziers[i].P0.x;//+shape->rect.x;
            polygon->component.polygon.beziers[i].T0.y=polygon->component.polygon.beziers[i].P0.y;//+shape->rect.y;
            polygon->component.polygon.beziers[i].T1.x=polygon->component.polygon.beziers[i].P1.x;//+shape->rect.x;
            polygon->component.polygon.beziers[i].T1.y=polygon->component.polygon.beziers[i].P1.y;//+shape->rect.y;
            polygon->component.polygon.beziers[i].T2.x=polygon->component.polygon.beziers[i].P2.x;//+shape->rect.x;
            polygon->component.polygon.beziers[i].T2.y=polygon->component.polygon.beziers[i].P2.y;//+shape->rect.y;
            polygon->component.polygon.beziers[i].T3.x=polygon->component.polygon.beziers[i].P3.x;//+shape->rect.x;
            polygon->component.polygon.beziers[i].T3.y=polygon->component.polygon.beziers[i].P3.y;//+shape->rect.y;*/

            /*polygon->component.polygon.beziers[i].T0.x=0;
            polygon->component.polygon.beziers[i].T0.y=0;
            polygon->component.polygon.beziers[i].T1.x=0;
            polygon->component.polygon.beziers[i].T1.y=0;
            polygon->component.polygon.beziers[i].T2.x=0;
            polygon->component.polygon.beziers[i].T2.y=0;
            polygon->component.polygon.beziers[i].T3.x=0;
            polygon->component.polygon.beziers[i].T3.y=0;*/
        }
        #endif
    }
}

/*void updateAttractor(ProteoBone* bone)
{
    SDL_Point newA=affineTrasformation(bone->a->pos,bone->a->affine);
    SDL_Point newB=affineTrasformation(bone->b->pos,bone->b->affine);

    float mr=1.0f;
    if(newA.y != newB.y) mr=(float)(newA.x - newB.x)/(float)(newB.y - newA.y);

    //float xm=(float)(newA.x + newB.x)/2.0f;
    //float ym=(float)(newA.y + newB.y)/2.0f;

    //float d=20.0f;
    //float Xa=(d/sqrtf(1+mr*mr))+xm;
    //float Ya=(d*mr/sqrtf(1+mr*mr))+ym;

    bone->Ax=(1.0f/sqrtf(1+mr*mr));
    bone->Ay=(mr/sqrtf(1+mr*mr));

    //bone->A.x=Xa;
    //bone->A.y=Ya;

}*/

float distancePointBone(float x,float y,ProteoBone* bone)
{
    float A = x - bone->a->pos.x;
    float B = y - bone->a->pos.y;
    float C = bone->b->pos.x - bone->a->pos.x;
    float D = bone->b->pos.y - bone->a->pos.y;

    float dot = A * C + B * D;
    float len_sq = C * C + D * D;
    float param = -1;
    if (len_sq != 0) //in case of 0 length line
      param = dot / len_sq;

    float xx, yy;

    if (param < 0) {
        xx = bone->a->pos.x;
        yy = bone->a->pos.y;
    }
    else if (param > 1) {
        xx = bone->b->pos.x;
        yy = bone->b->pos.y;
    }
    else {
        xx = bone->a->pos.x + param * C;
        yy = bone->a->pos.y + param * D;
    }

    float dx = x - xx;
    float dy = y - yy;

    return sqrtf(dx * dx + dy * dy);
}

ProteoBone* getBone(ProteoSkeleton* skeleton,ProteoComponent* polygon)
{
    ProteoBone* nearBone=NULL;
    float minDistance=999999999;
    for(int j=0;j<skeleton->nbones;j++)
    {
        ProteoBone* bone=&skeleton->bones[j];

        for(int i=0;i<polygon->component.polygon.npoints;i++)
        {
            float d=distancePointBone(polygon->component.polygon.vx[i], polygon->component.polygon.vy[i],bone);
            if(d<minDistance)
            {
                minDistance=d;
                nearBone=bone;
            }
        }
    }

    return nearBone;
}

static int graphics_hideSkeleton(lua_State *state) {

    ProteoSkeleton* ps=checkProteoSkeleton(state, 1 );
    const int hidden=lua_toboolean(state,2);
    ps->hidden=hidden;

    return 0;
}

SDL_FPoint trasform(ProteoBone* bone,SDL_FPoint transformPoint,float anchorPerc)
{
    /*let dir = this.kp1.currentPosition.subtract(this.kp0.currentPosition).normalize();
    let n = dir.clone();
    n.angle += 90;
    let anchor = this.kp0.currentPosition.multiply(1 - trans.anchorPerc).add(this.kp1.currentPosition.multiply(trans.anchorPerc));
    let p = anchor.add(dir.multiply(trans.transform.x * scale)).add(n.multiply(trans.transform.y * scale));*/

    SDL_FPoint dir=normalize(subtract(bone->b->tpos, bone->a->tpos));
    SDL_FPoint n={-dir.y,dir.x}; //+=90

    SDL_FPoint anchor = sum(multiply(bone->a->tpos,1-anchorPerc),multiply(bone->b->tpos,anchorPerc));
    SDL_FPoint p = sum(sum(anchor,multiply(dir, transformPoint.x)),multiply(n, transformPoint.y));

    return p;
}

static int graphics_updateSkeleton(lua_State *state) {

    ProteoSkeleton* ps=checkProteoSkeleton(state, 1 );

    if(ps->shape!=NULL)
    {
        resetShape(ps->shape);

        for(int k=0;k<ps->nbones;k++)
        {
            ProteoBone* bone=&ps->bones[k];
            #ifdef AFFINE
            bone->a->tpos=affineTrasformation(bone->a->pos,bone->a->affine);
            bone->b->tpos=affineTrasformation(bone->b->pos,bone->b->affine);
            #else
            bone->a->tpos=sum(bone->a->pos,bone->a->move);
            bone->b->tpos=sum(bone->b->pos,bone->b->move);

            #endif

            if(bone->weights!=NULL)
            for(int i=0;i<ps->shape->component.shape.nchilds;i++)
            {
                int nbeziers=ps->shape->component.shape.childs[i]->component.polygon.nbeziers;
                _bezier* beziers=ps->shape->component.shape.childs[i]->component.polygon.beziers;

                if(bone->weights[i]!=NULL)
                for(int j=0;j<nbeziers;j++)
                {
                   /*beziers[j].T0=add(beziers[j].T0,
                                      multiply(
                                               add(beziers[j].P0,trasform(bone, bone->transformPoints[i][j*4], bone->anchorPercs[i][j*4]))
                                               ,bone->weights[i][j*4])
                                      );

                    beziers[j].T1=add(beziers[j].T1,multiply(add(beziers[j].P1,trasform(bone, bone->transformPoints[i][j*4+1], bone->anchorPercs[i][j*4+1])),bone->weights[i][j*4+1]));

                    beziers[j].T2=add(beziers[j].T2,multiply(add(beziers[j].P2,trasform(bone, bone->transformPoints[i][j*4+2], bone->anchorPercs[i][j*4+2])),bone->weights[i][j*4+2]));

                    beziers[j].T3=add(beziers[j].T3,multiply(add(beziers[j].P3,trasform(bone, bone->transformPoints[i][j*4+3], bone->anchorPercs[i][j*4+3])),bone->weights[i][j*4+3]));
                    */
                    beziers[j].T0=sum(beziers[j].T0,multiply(trasform(bone, bone->transformPoints[i][j*4], bone->anchorPercs[i][j*4])
                                               ,bone->weights[i][j*4])
                                      );

                    beziers[j].T1=sum(beziers[j].T1,multiply(trasform(bone, bone->transformPoints[i][j*4+1], bone->anchorPercs[i][j*4+1]),bone->weights[i][j*4+1])
                                      );

                    beziers[j].T2=sum(beziers[j].T2,multiply(trasform(bone, bone->transformPoints[i][j*4+2], bone->anchorPercs[i][j*4+2]),bone->weights[i][j*4+2])
                                      );

                    beziers[j].T3=sum(beziers[j].T3,multiply(trasform(bone, bone->transformPoints[i][j*4+3], bone->anchorPercs[i][j*4+3]),bone->weights[i][j*4+3])
                                      );

                }
            }
        }

        //OLD VERSION
        /*for(int k=0;k<ps->njoints;k++)
        {
            ProteoJoint* joint=&ps->joints[k];
            for(int i=0;i<ps->shape->component.shape.nchilds;i++)// joint->nweights;i++)
            {
                #ifndef BEZIER
                if(joint->weights[i]!=NULL)
                #else
                if(joint->bez_wei[i]!=NULL)
                #endif
                {
                    #ifndef BEZIER
                    int npoints=ps->shape->component.shape.childs[i]->component.polygon.npoints;
                    Sint16* vx=ps->shape->component.shape.childs[i]->component.polygon.vx;
                    Sint16* vy=ps->shape->component.shape.childs[i]->component.polygon.vy;
                    Sint16* tvx=ps->shape->component.shape.childs[i]->component.polygon.tvx;
                    Sint16* tvy=ps->shape->component.shape.childs[i]->component.polygon.tvy;

                    for(int j=0;j<npoints;j++)
                    {
                        SDL_Point v={vx[j],vy[j]};
                        SDL_Point t=affineTrasformation(v,joint->affine);
                        tvx[j]+=t.x*joint->weights[i][j];
                        tvy[j]+=t.y*joint->weights[i][j];

                        //tvx[j]+=joint->move.x*joint->weights[i][j];//+ps->shape->rect.x;
                        //tvy[j]+=joint->move.y*joint->weights[i][j];//+ps->shape->rect.y;
                    }
                    #endif

                    #ifdef BEZIER
                    int nbeziers=ps->shape->component.shape.childs[i]->component.polygon.nbeziers;
                    _bezier* beziers=ps->shape->component.shape.childs[i]->component.polygon.beziers;

                    for(int j=0;j<nbeziers;j++)
                    {
                        SDL_Point t=affineTrasformation(beziers[j].P0,joint->affine);
                        beziers[j].T0.x+=t.x*joint->bez_wei[i][j*4];
                        beziers[j].T0.y+=t.y*joint->bez_wei[i][j*4];

                        t=affineTrasformation(beziers[j].P1,joint->affine);
                        beziers[j].T1.x+=t.x*joint->bez_wei[i][j*4+1];
                        beziers[j].T1.y+=t.y*joint->bez_wei[i][j*4+1];

                        t=affineTrasformation(beziers[j].P2,joint->affine);
                        beziers[j].T2.x+=t.x*joint->bez_wei[i][j*4+2];
                        beziers[j].T2.y+=t.y*joint->bez_wei[i][j*4+2];

                        t=affineTrasformation(beziers[j].P3,joint->affine);
                        beziers[j].T3.x+=t.x*joint->bez_wei[i][j*4+3];
                        beziers[j].T3.y+=t.y*joint->bez_wei[i][j*4+3];
                    }
                    #endif

                }
            }
        }*/

       /* for(int i=0;i<ps->nbones;i++)
            updateAttractor(&ps->bones[i]);*/
        #ifdef BEZIER
        for(int i=0;i<ps->shape->component.shape.nchilds;i++)
        {
            int npoints=ps->shape->component.shape.childs[i]->component.polygon.npoints;
            //Sint16* vx=ps->shape->component.shape.childs[i]->component.polygon.vx;
            //Sint16* vy=ps->shape->component.shape.childs[i]->component.polygon.vy;
            Sint16* tvx=ps->shape->component.shape.childs[i]->component.polygon.tvx;
            Sint16* tvy=ps->shape->component.shape.childs[i]->component.polygon.tvy;
            int nbeziers = ps->shape->component.shape.childs[i]->component.polygon.nbeziers;
            _bezier *beziers = ps->shape->component.shape.childs[i]->component.polygon.beziers;

            int p=0;
            float step=0.1; //TODO deve essere legata allo step con cui la curva è generata
            for(int j=0;j<nbeziers;j++)
            {
                float xu = 0.0 , yu = 0.0 , u = 0.0 ;
                //int i = 0 ;
                float x0=beziers[j].T0.x;
                float y0=beziers[j].T0.y;
                float x1=beziers[j].T1.x;
                float y1=beziers[j].T1.y;
                float x2=beziers[j].T2.x;
                float y2=beziers[j].T2.y;
                float x3=beziers[j].T3.x;
                float y3=beziers[j].T3.y;

                if(x0==NAN || y0==NAN || x1==NAN || y1==NAN || x2==NAN || y2==NAN || x3==NAN || y3==NAN) continue;

                /*if(x0!=x3)
                {
                    float m=(float)(y0-y3)/(float)(x0-x3);
                    float q=(float)(x0*y3-x3*y0)/(float)(x0-x3);
                    beziers[j].m=m;
                    beziers[j].q=q;

                    //beziers[j].S1=y1-m*x1-q;
                    //beziers[j].S2=y2-m*x2-q;
                }*/

                float step=beziers[j].step;

                for(u = 0.0 ; u <= 1.0 ; u += step)
                {
                    xu = powf(1-u,3)*x0+3*u*powf(1-u,2)*x1+3*powf(u,2)*(1-u)*x2
                     +powf(u,3)*x3;
                    yu = powf(1-u,3)*y0+3*u*powf(1-u,2)*y1+3*powf(u,2)*(1-u)*y2
                    +powf(u,3)*y3;

                    tvx[p]=xu;
                    tvy[p]=yu;

                    p++;
                }
            }
        }
        #endif
    }

    return 0;
}

static int graphics_setJoint(lua_State *state) {

}

#ifdef AFFINE
static int graphics_resetJoint(lua_State *state) {

    const char* id=luaL_checkstring(state,1); //TODO bisogna trasformarlo in puntatore
    ProteoSkeleton* ps=checkProteoSkeleton(state, 2 );

    char md5_id[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)id, strlen(id), (unsigned char*)md5_id);

    ProteoJoint* joint=NULL;
    for(int i=0;i<ps->njoints;i++)
    {
        if(memcmp(ps->joints[i].id,md5_id,sizeof(md5_id))==0)
        {
            joint=&ps->joints[i];
            break;
        }
    }

    if(joint==NULL) return 0;

    joint->affine[0][0]=1;
    joint->affine[0][1]=0;
    joint->affine[0][2]=0;
    joint->affine[1][0]=0;
    joint->affine[1][1]=1;
    joint->affine[1][2]=0;
    joint->affine[2][0]=0;
    joint->affine[2][1]=0;
    joint->affine[2][2]=1;
}
#endif

static int graphics_moveJoint(lua_State *state) {

    //setJoint(id,skeleton,x,y)
    const char* id=luaL_checkstring(state,1); //TODO bisogna trasformarlo in puntatore
    ProteoSkeleton* ps=checkProteoSkeleton(state, 2 );
    long x=luaL_checkinteger(state,3);
    long y=luaL_checkinteger(state,4);

    char md5_id[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)id, strlen(id), (unsigned char*)md5_id);

    ProteoJoint* joint=NULL;
    for(int i=0;i<ps->njoints;i++)
    {
        if(memcmp(ps->joints[i].id,md5_id,sizeof(md5_id))==0)
        {
            joint=&ps->joints[i];
            break;
        }
    }

    if(joint==NULL) return 0;



    #ifdef AFFINE
    /*joint->affine[0][0]=1;
    joint->affine[1][1]=1;
    joint->affine[2][2]=1;

    joint->affine[2][0]=x;
    joint->affine[2][1]=y;*/

    //float a[3][3] ={{1,0,0},{0,1,0},{0,0,1}};
    float t[3][3] ={{1,0,0},{0,1,0},{x,y,1}};

    laderman_mul(joint->affine,t,joint->affine);
    //laderman_mul(a,t,joint->affine);
#else
    joint->move.x=x;
    joint->move.y=y;
#endif

    return 0;
}

#ifdef AFFINE
static int graphics_rotateJoint(lua_State *state) {

    //setJoint(id,skeleton,x,y)
    const char* id=luaL_checkstring(state,1); //TODO bisogna trasformarlo in puntatore
    ProteoSkeleton* ps=checkProteoSkeleton(state, 2 );
    float r=luaL_checknumber(state,3);

    char md5_id[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)id, strlen(id), (unsigned char*)md5_id);

    ProteoJoint* joint=NULL;
    for(int i=0;i<ps->njoints;i++)
    {
        if(memcmp(ps->joints[i].id,md5_id,sizeof(md5_id))==0)
        {
            joint=&ps->joints[i];
            break;
        }
    }

    if(joint==NULL) return 0;

    //float a[3][3] ={{1,0,0},{0,1,0},{0,0,1}};
    float t[3][3] ={{cosf(r),sinf(r),0},{-sinf(r),cosf(r),0},{0,0,1}};
    //mult_matrices(joint->affine,t,r);
    laderman_mul(joint->affine,t,joint->affine);

    return 0;
}
#endif

static int graphics_getJoint(lua_State *state) {

    //getJoint(id,skeleton)
}

ProteoJoint *getJoint(ProteoSkeleton* skeleton,const char* joint_id)
{
    char md5_id[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)joint_id, strlen(joint_id), (unsigned char*)md5_id);

    for(int i=0;i<skeleton->njoints;i++)
    {
        if(memcmp(skeleton->joints[i].id,md5_id,sizeof(md5_id))==0)
        {
            return &skeleton->joints[i];
        }
    }

    return NULL;

}
static int graphics_addBone(lua_State *state) {

    //addBone(id,skeleton,id_joint1,id_joint2,group_name)

    const char* id=luaL_checkstring(state,1);
    ProteoSkeleton* ps=checkProteoSkeleton(state, 2 );
    const char* id_joint1=luaL_checkstring(state,3);
    const char* id_joint2=luaL_checkstring(state,4);
    int group_id=luaL_optint(state, 5, -1);

    printf("graphics.addBone: %s\n",id);

    if(ps->bones==NULL)
    {
        ps->bones=malloc(sizeof(ProteoBone));

        /*MD5((unsigned char*)id, strlen(id), (unsigned char*)ps->bones[0].id);
        ps->bones[0].a=getJoint(ps,id_joint1);
        ps->bones[0].b=getJoint(ps,id_joint2);
        ps->bones[0].group_id=group_id;

        ps->nbones=1;*/
    }
    else
    {
        ps->bones=realloc(ps->bones,(ps->nbones+1)*sizeof(ProteoBone));

    }

    MD5((unsigned char*)id, strlen(id), (unsigned char*)ps->bones[ps->nbones].id);
    ps->bones[ps->nbones].a=getJoint(ps,id_joint1);
    ps->bones[ps->nbones].b=getJoint(ps,id_joint2);
    ps->bones[ps->nbones].group_id=group_id;
    ps->bones[ps->nbones].nweights=0;
    ps->bones[ps->nbones].weights=NULL;
    ps->bones[ps->nbones].transformPoints=NULL;
    ps->bones[ps->nbones].anchorPercs=NULL;

    ps->nbones+=1;
    //updateAttractor(&ps->bones[ps->nbones-1]);

    return 0;
}

int* getJoints(ProteoSkeleton* skeleton,ProteoBone* nearBone,ProteoComponent* polygon)
{
    int* ret=malloc(skeleton->njoints*sizeof(int));

    //Per tutti i punti del poligono calcolare la distanza con tutti i bone dello scheletro
    //prendere il gruppo del bone più vicino
    /*ProteoBone* nearBone=NULL;
    float minDistance=999999999;
    for(int j=0;j<skeleton->nbones;j++)
    {
        ProteoBone* bone=&skeleton->bones[j];

        for(int i=0;i<polygon->component.polygon.npoints;i++)
        {
            float d=distancePointBone(polygon->component.polygon.vx[i], polygon->component.polygon.vy[i],bone);
            if(d<minDistance)
            {
                minDistance=d;
                nearBone=bone;
            }
        }
    }*/

    //restituire tutti i joint che fanno parte del gruppo del bone più vicino
    if(nearBone!=NULL)
    {
        for(int i=0;i<skeleton->njoints;i++)
        {
            ret[i]=0;
            for(int j=0;j<skeleton->nbones;j++)
            {
                ProteoBone* bone=&skeleton->bones[j];
                if(bone->group_id==nearBone->group_id)
                    if(bone->a==&skeleton->joints[i] ||  bone->b==&skeleton->joints[i])
                        ret[i]=1;
            }
        }
    }
    return ret;
}


int* getBonesGroup(ProteoSkeleton* skeleton,ProteoBone* nearBone,ProteoComponent* polygon)
{
    int* ret=malloc(skeleton->nbones*sizeof(int));
    if(nearBone!=NULL)
    {
        for(int i=0;i<skeleton->nbones;i++)
        {
            ret[i]=0;
            if(skeleton->bones[i].group_id==nearBone->group_id) ret[i]=1;
        }
    }
    return ret;
}

float* getWeights(ProteoJoint* joint,ProteoComponent* polygon)
{
    float* ret=malloc(polygon->component.polygon.npoints*sizeof(float));

    //float totalW=0;
    for(int i=0;i<polygon->component.polygon.npoints;i++)
    {
        //float d=hypot(joint->pos.x-polygon->component.polygon.vx[i],joint->pos.y-polygon->component.polygon.vy[i]);
        //float w=1/(d*d);

        float a=joint->pos.x-polygon->component.polygon.vx[i];
        float b=joint->pos.y-polygon->component.polygon.vy[i];

        float w = 1/ (a*a+b*b);

        ret[i]=w;

        //totalW+=w;
    }

    return ret;
}

/*float* getBoneWeights(ProteoJoint * joint,ProteoBone* bone,ProteoComponent* polygon)
{
    float* ret=malloc(polygon->component.polygon.npoints*sizeof(float));

    for(int i=0;i<polygon->component.polygon.npoints;i++)
    {
        float d = distancePointBone(polygon->component.polygon.vx[i], polygon->component.polygon.vy[i], bone);

        float a=joint->pos.x-polygon->component.polygon.vx[i];
        float b=joint->pos.y-polygon->component.polygon.vy[i];

        float w = (1.0f/ (a*a+b*b))-(0.0001f/d*d);

        ret[i]=w;
    }

    return ret;
}*/

#ifdef BEZIER
float* getBez_Weights(ProteoJoint* joint,ProteoComponent* polygon)
{
    float* ret=malloc(polygon->component.polygon.nbeziers*4*sizeof(float));

    //float totalW=0;
    for(int i=0;i<polygon->component.polygon.nbeziers;i++)
    {
        //float d=hypot(joint->pos.x-polygon->component.polygon.vx[i],joint->pos.y-polygon->component.polygon.vy[i]);
        //float w=1/(d*d);

        float a=joint->pos.x-polygon->component.polygon.beziers[i].P0.x;
        float b=joint->pos.y-polygon->component.polygon.beziers[i].P0.y;
        float w = 1/ (a*a+b*b);
        ret[i*4]=w;

        a=joint->pos.x-polygon->component.polygon.beziers[i].P1.x;
        b=joint->pos.y-polygon->component.polygon.beziers[i].P1.y;
        w = 1/ (a*a+b*b);
        ret[i*4+1]=w;

        a=joint->pos.x-polygon->component.polygon.beziers[i].P2.x;
        b=joint->pos.y-polygon->component.polygon.beziers[i].P2.y;
        w = 1/ (a*a+b*b);
        ret[i*4+2]=w;

        a=joint->pos.x-polygon->component.polygon.beziers[i].P3.x;
        b=joint->pos.y-polygon->component.polygon.beziers[i].P3.y;
        w = 1/ (a*a+b*b);
        ret[i*4+3]=w;
        //totalW+=w;
    }

    return ret;
}
#endif

float* getAnchorPercs(ProteoBone* bone,ProteoComponent* polygon)
{
    float* ret=malloc(polygon->component.polygon.nbeziers*4*sizeof(float));

    float d = magnitude(subtract(bone->a->pos,bone->b->pos));

    for(int i=0;i<polygon->component.polygon.nbeziers;i++)
    {
        SDL_FPoint closestP = getClosestPointOnSegment(bone->a->pos, bone->b->pos, polygon->component.polygon.beziers[i].P0);
        ret[i*4]=magnitude(subtract(closestP, bone->a->pos))/d;

        closestP = getClosestPointOnSegment(bone->a->pos, bone->b->pos, polygon->component.polygon.beziers[i].P1);
        ret[i*4+1]=magnitude(subtract(closestP, bone->a->pos))/d;

        closestP = getClosestPointOnSegment(bone->a->pos, bone->b->pos, polygon->component.polygon.beziers[i].P2);
        ret[i*4+2]=magnitude(subtract(closestP, bone->a->pos))/d;

        closestP = getClosestPointOnSegment(bone->a->pos, bone->b->pos, polygon->component.polygon.beziers[i].P3);
        ret[i*4+3]=magnitude(subtract(closestP, bone->a->pos))/d;
    }
    //let d = this.kp0.position.subtract(this.kp1.position).length;
    //let anchorPerc = closestP.subtract(this.kp0.position).length / d;

    return ret;
}
SDL_FPoint* getTransformPoints(ProteoBone* bone,ProteoComponent* polygon)
{
    SDL_FPoint* ret=malloc(polygon->component.polygon.nbeziers*4*sizeof(SDL_FPoint));

    SDL_FPoint dir=normalize(subtract(bone->b->pos, bone->a->pos));
    SDL_FPoint n={-dir.y,dir.x}; //+=90

    for(int i=0;i<polygon->component.polygon.nbeziers;i++)
    {
        SDL_FPoint closestP = getClosestPointOnSegment(bone->a->pos, bone->b->pos, polygon->component.polygon.beziers[i].P0);
        SDL_FPoint v = subtract(polygon->component.polygon.beziers[i].P0, closestP);

        float dirProjD = dot(v,dir);
        float dirProjN = dot(v,n);

        SDL_FPoint d={dirProjD,dirProjN};
        ret[i*4]=d;

        closestP = getClosestPointOnSegment(bone->a->pos, bone->b->pos, polygon->component.polygon.beziers[i].P1);
        v = subtract(polygon->component.polygon.beziers[i].P1, closestP);

        dirProjD = dot(v,dir);
        dirProjN = dot(v,n);

        d.x=dirProjD;
        d.y=dirProjN;
        ret[i*4+1]=d;

        closestP = getClosestPointOnSegment(bone->a->pos, bone->b->pos, polygon->component.polygon.beziers[i].P2);
        v = subtract(polygon->component.polygon.beziers[i].P2, closestP);

        dirProjD = dot(v,dir);
        dirProjN = dot(v,n);

        d.x=dirProjD;
        d.y=dirProjN;
        ret[i*4+2]=d;

        closestP = getClosestPointOnSegment(bone->a->pos, bone->b->pos, polygon->component.polygon.beziers[i].P3);
        v = subtract(polygon->component.polygon.beziers[i].P3, closestP);

        dirProjD = dot(v,dir);
        dirProjN = dot(v,n);

        d.x=dirProjD;
        d.y=dirProjN;
        ret[i*4+3]=d;

    }
    /*
     let dir = this.kp1.position.subtract(this.kp0.position).normalize();
     let n = dir.clone();
     n.angle += 90;
     let closestP = MathUtils.getClosestPointOnSegment(this.kp0.position, this.kp1.position, p);
     let v = p.subtract(closestP);
     let dirProjD = v.dot(dir);
     let dirProjN = v.dot(n);
     let d = this.kp0.position.subtract(this.kp1.position).length;
     let anchorPerc = closestP.subtract(this.kp0.position).length / d;
     return {
         transform: new paper.default.Point(dirProjD, dirProjN),
         anchorPerc: anchorPerc,
     };
     */
    return ret;
}

float* getBoneWeights(ProteoBone* bone,ProteoComponent* polygon)
{
    float* ret=malloc(polygon->component.polygon.nbeziers*4*sizeof(float));

    for(int i=0;i<polygon->component.polygon.nbeziers;i++)
    {
        float d=distance(getClosestPointOnSegment(bone->a->pos,bone->b->pos,polygon->component.polygon.beziers[i].P0),polygon->component.polygon.beziers[i].P0);
        float w = 1/ (d*d);
        ret[i*4]=w;

        d=distance(getClosestPointOnSegment(bone->a->pos,bone->b->pos,polygon->component.polygon.beziers[i].P1),polygon->component.polygon.beziers[i].P1);
        w = 1/ (d*d);
        ret[i*4+1]=w;

        d=distance(getClosestPointOnSegment(bone->a->pos,bone->b->pos,polygon->component.polygon.beziers[i].P2),polygon->component.polygon.beziers[i].P2);
        w = 1/ (d*d);
        ret[i*4+2]=w;

        d=distance(getClosestPointOnSegment(bone->a->pos,bone->b->pos,polygon->component.polygon.beziers[i].P3),polygon->component.polygon.beziers[i].P3);
        w = 1/ (d*d);
        ret[i*4+3]=w;

    }

    return ret;
}
static int graphics_bindSkeleton(lua_State *state)
{
    //bindSkeleton(skeleton,shape)
    ProteoSkeleton* ps=checkProteoSkeleton(state,1);
    ProteoComponent* shape=toProteoComponent(state,2);

    ps->shape=shape;

    shape->component.shape.hasSkeleton=TRUE;

    for(int i=0;i<shape->component.shape.nchilds;i++)
          {
              ProteoComponent* current=shape->component.shape.childs[i];

              if(current->type==Polyg)
              {
                  ProteoBone* nearBone=getBone(ps,current);

                   int* bones=getBonesGroup(ps,nearBone,current);
                   for(int k=0;k<ps->nbones;k++)
                   {
                      if(bones[k]==1)
                      {
                        if(ps->bones[k].weights==NULL)
                        {
                            ps->bones[k].nweights=shape->component.shape.nchilds;
                            ps->bones[k].weights=malloc(shape->component.shape.nchilds*sizeof(float*));
                            ps->bones[k].anchorPercs=malloc(shape->component.shape.nchilds*sizeof(float*));
                            ps->bones[k].transformPoints=malloc(shape->component.shape.nchilds*sizeof(SDL_FPoint*));
                            for(int j=0;j<shape->component.shape.nchilds;j++)
                            {
                                ps->bones[k].weights[j]=NULL;
                                ps->bones[k].transformPoints[j]=NULL;
                                ps->bones[k].anchorPercs[j]=NULL;
                            }
                        }

                        ps->bones[k].weights[i]=getBoneWeights(&ps->bones[k],current);
                        ps->bones[k].transformPoints[i]=getTransformPoints(&ps->bones[k],current);
                        ps->bones[k].anchorPercs[i]=getAnchorPercs(&ps->bones[k],current);
                      }
                   }

                  /* OLD
                  int* joints=getJoints(ps,nearBone,current);
                  for(int k=0;k<ps->njoints;k++)
                  {
                      /*
                      if(joints[k]==1)
                      {
                          #ifndef BEZIER
                          if(ps->joints[k].weights==NULL)
                          {
                              ps->joints[k].nweights=shape->component.shape.nchilds;
                              ps->joints[k].weights=malloc(shape->component.shape.nchilds*sizeof(float*));
                              for(int j=0;j<shape->component.shape.nchilds;j++) ps->joints[k].weights[j]=NULL;
                          }

                          ps->joints[k].weights[i]=getWeights(&ps->joints[k],current);
                          #endif

                          #ifdef BEZIER
                          if(ps->joints[k].bez_wei==NULL)
                          {
                              ps->joints[k].nbez_wei=shape->component.shape.nchilds;
                              ps->joints[k].bez_wei=malloc(shape->component.shape.nchilds*sizeof(float*));
                              for(int j=0;j<shape->component.shape.nchilds;j++) ps->joints[k].bez_wei[j]=NULL;
                          }

                          ps->joints[k].bez_wei[i]=getBez_Weights(&ps->joints[k],current);
                          #endif
                      }

                  }
                  #ifndef BEZIER
                  for(int j=0;j<current->component.polygon.npoints;j++)
                  {
                      float totalW=0;
                      for(int k=0;k<ps->njoints;k++)
                      {
                          if(joints[k]==1)
                              totalW+=ps->joints[k].weights[i][j];
                      }
                      for(int k=0;k<ps->njoints;k++)
                      {
                          if(joints[k]==1)
                              ps->joints[k].weights[i][j]/=totalW;
                      }
                  }
                  #endif
                  */
                  #ifdef BEZIER
                  for(int j=0;j<current->component.polygon.nbeziers*4;j++)
                  {
                      float totalW=0;
                     /* OLD
                      for(int k=0;k<ps->njoints;k++)
                      {
                          if(joints[k]==1)
                              totalW+=ps->joints[k].bez_wei[i][j];
                      }
                      for(int k=0;k<ps->njoints;k++)
                      {
                          if(joints[k]==1)
                              ps->joints[k].bez_wei[i][j]/=totalW;
                      }*/

                      //NORMALIZE BONE
                      for(int k=0;k<ps->nbones;k++)
                      {
                          if(bones[k]==1)
                              totalW+=ps->bones[k].weights[i][j];
                      }
                      for(int k=0;k<ps->nbones;k++)
                      {
                          if(bones[k]==1)
                              ps->bones[k].weights[i][j]/=totalW;
                      }

                  }
                  #endif
                  //OLD free(joints);
                  free(bones);
              }
          }

}

static int graphics_loadSkeleton(lua_State *state)
{
    //loadSkeleton
}

static int graphics_saveSkeleton(lua_State *state)
{
    //saveSkeleton
}

//Add polygon or shape to shape
static int graphics_addItem(lua_State *state)
{
    ProteoComponent* shape=toProteoComponent(state,1);
    ProteoComponent* item=toProteoComponent(state,2);

    if(debug) printf("graphics.addItem %s -> %s\n",item->id,shape->id);

    item->parent=shape;
    //child->child_next=NULL;

    if(shape==NULL){
        printf("addPolygon ERROR parent==NULL\n");
        return 0;
    }

    if(shape->component.shape.childs==NULL)
    {
        shape->component.shape.childs=malloc(sizeof(ProteoComponent*));
        shape->component.shape.childs[0]=item;
        shape->component.shape.nchilds=1;
    }
    else
    {
        shape->component.shape.childs=realloc(shape->component.shape.childs,(shape->component.shape.nchilds+1)*sizeof(ProteoComponent*));
        shape->component.shape.childs[shape->component.shape.nchilds]=item;
        shape->component.shape.nchilds+=1;
    }

    return 0;

}
//==============================================================================
//   ELLIPSE
//==============================================================================

static int graphics_drawEllipse (SDL_Renderer* renderer,ProteoComponent* ellipse)
{
    SDL_Rect ellipse_rect=ellipse->rect;

    if(ellipse->parent!=NULL)
    {
        ellipse_rect.x+=ellipse->parent->rect.x;
        ellipse_rect.y+=ellipse->parent->rect.y;
    }

	aaellipseRGBA(renderer, ellipse_rect.x, ellipse_rect.y, ellipse_rect.w, ellipse_rect.h,
		ellipse->color.r, ellipse->color.g, ellipse->color.b, ellipse->color.a);
	filledEllipseRGBA (renderer, ellipse_rect.x, ellipse_rect.y, ellipse_rect.w, ellipse_rect.h,
		ellipse->component.ellipse.color.r, ellipse->component.ellipse.color.g, ellipse->component.ellipse.color.b, ellipse->component.ellipse.color.a);


	return 1;
}

/*static int graphics_eventEllipse (lua_State *state,ProteoComponent* ellipse,SDL_Event e,SDL_Renderer* renderer)
{
	return FALSE;
}*/

static int graphics_newEllipse (lua_State *state) {
	const char* id=luaL_checkstring(state,1);
	const char* color=luaL_checkstring(state,2);
	const char* border_color=luaL_checkstring(state,3);
	const int pos_x=luaL_checkint(state,4);
	const int pos_y=luaL_checkint(state,5);
	const int width=luaL_checkint(state,6);
	const int height=luaL_checkint(state,7);

	ProteoComponent* pc=pushProteoComponent(state);

	//strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
	pc->rect.x=pos_x;
	pc->rect.y=pos_y;
	pc->rect.w=width;
	pc->rect.h=height;
    pc->component.ellipse.color=hex2color(color);
	pc->color=hex2color(border_color);
	//pc->next=NULL;
	pc->surface=NULL;
	pc->texture=NULL;
	pc->hidden=FALSE;
	pc->type=Ellips;
	pc->layer=100;
	pc->font=NULL;
    pc->txt=NULL;

	/*if(components==NULL)
	{
		components=pc;
	}
	else
	{
		ProteoComponent* current=components;
		while(current->next!=NULL) current=(ProteoComponent*)current->next;
		current->next=pc;
	}*/
    addComponent(pc,&components);

  	return 1;
}

//==============================================================================
//   RECT
//==============================================================================

static int graphics_drawRect (SDL_Renderer* renderer,ProteoComponent* rect)
{
    SDL_Rect rect_rect=rect->rect;

    if(rect->parent!=NULL)
    {
        rect_rect.x+=rect->parent->rect.x;
        rect_rect.y+=rect->parent->rect.y;
    }

    rectangleRGBA(renderer, rect_rect.x, rect_rect.y, rect_rect.x+rect_rect.w, rect_rect.y+rect_rect.h, rect->color.r, rect->color.g, rect->color.b, rect->color.a);

    boxRGBA(renderer, rect_rect.x, rect_rect.y, rect_rect.x+rect_rect.w, rect_rect.y+rect_rect.h,rect->component.rect.color.r,
            rect->component.rect.color.g, rect->component.rect.color.b, rect->component.rect.color.a);

    /*SDL_SetRenderDrawColor( renderer, rect->component.rect.color.r,
		rect->component.rect.color.g, rect->component.rect.color.b, rect->component.rect.color.a );
	SDL_RenderFillRect( renderer, & rect_rect );*/

	return 1;
}

/*static int graphics_eventRect (lua_State *state,ProteoComponent* rect,SDL_Event e,SDL_Renderer* renderer)
{
	return FALSE;
}*/

//-----------------------------------------------------------------------------
//-- newRect
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param color           rect color (string, standard name or #rrggbb)
//-- @param
//-- @param pos_x           position x on the screen
//-- @param pos_y           position y on the screen
//-- @param width           component width
//-- @param height          component height
//--
//-----------------------------------------------------------------------------

static int graphics_newRect (lua_State *state) {
	const char* id=luaL_checkstring(state,1);
	const char* color=luaL_checkstring(state,2);
	const char* border_color=luaL_checkstring(state,3); //TODO
	const int pos_x=luaL_checkint(state,4);
	const int pos_y=luaL_checkint(state,5);
	const int width=luaL_checkint(state,6);
	const int height=luaL_checkint(state,7);

	ProteoComponent* pc=pushProteoComponent(state);

	//strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
	pc->rect.x=pos_x;
	pc->rect.y=pos_y;
	pc->rect.w=width;
	pc->rect.h=height;
	pc->component.rect.color=hex2color(color);
	pc->color=hex2color(border_color);
	//pc->next=NULL;
	pc->surface=NULL;
	pc->texture=NULL;
	pc->hidden=FALSE;
	pc->type=Rect;
	pc->layer=100;
	pc->font=NULL;
    pc->txt=NULL;
    pc->parent=NULL;

    /*if(components==NULL)
	{
		components=pc;
	}
	else
	{
		ProteoComponent* current=components;
		while(current->next!=NULL) current=(ProteoComponent*)current->next;
		current->next=pc;
	}*/
    addComponent(pc,&components);

  	return 1;
}

//==============================================================================
//   IMAGE
//==============================================================================

static int graphics_drawImage (SDL_Renderer* renderer,ProteoComponent* image)
{
	SDL_Rect image_rect=image->rect;

    if(image->parent!=NULL)
    {
        image_rect.x+=image->parent->rect.x;
        image_rect.y+=image->parent->rect.y;
    }

    if(image->component.image.image->texture!=NULL)
        SDL_RenderCopy(renderer, image->component.image.image->texture, NULL, & image_rect );

	return 1;
}

/*static int graphics_eventImage (lua_State *state,ProteoComponent* image,SDL_Event e,SDL_Renderer* renderer)
{
	return FALSE;
}*/

static int graphics_newImage (lua_State *state) {
	const char* id=luaL_checkstring(state,1);
	const char* file=luaL_checkstring(state,2);
	const int pos_x=luaL_checkint(state,3);
	const int pos_y=luaL_checkint(state,4);
	const int width=luaL_checkint(state,5);
	const int height=luaL_checkint(state,6);

	ProteoComponent* pc=pushProteoComponent(state);

	//strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
	pc->rect.x=pos_x-width/2;
	pc->rect.y=pos_y-height/2;
	pc->rect.w=width;
	pc->rect.h=height;
	//pc->maincolor=hex2color(color);
	//pc->color=hex2color(border_color);
	//pc->next=NULL;
	pc->surface=NULL;
    pc->texture=NULL;
    pc->component.image.image=newTexture(gRenderer,file); //TODO da testare IMG_LoadTexture(gRenderer,file);
    if(pc->component.image.image==NULL) printf("Error Image newTexture: %s\n",file);
    pc->parent=NULL;
	pc->hidden=FALSE;
	pc->type=Image;
	pc->layer=100;
	pc->font=NULL;
    pc->txt=NULL;

	/*if(components==NULL)
	{
		components=pc;
	}
	else
	{
		ProteoComponent* current=components;
		while(current->next!=NULL) current=(ProteoComponent*)current->next;
		current->next=pc;
	}*/
    addComponent(pc,&components);

  	return 1;
}

//==============================================================================
//   SPRITE
//==============================================================================

static int graphics_drawSprite (SDL_Renderer* renderer,ProteoComponent* sprite)
{
    if(sprite->component.sprite.image->texture!=NULL && sprite->nFrame>0 && sprite->nFrame>sprite->currentFrame)
    {
        if(sprite->framesDestination[sprite->currentFrame].x==0 || sprite->framesDestination[sprite->currentFrame].y==0)
           SDL_RenderCopy(renderer, sprite->component.sprite.image->texture, &sprite->framesSource[sprite->currentFrame], &sprite->rect );
        else
        {
            SDL_Rect location={sprite->rect.x,sprite->rect.y,
            sprite->framesDestination[sprite->currentFrame].x,sprite->framesDestination[sprite->currentFrame].y};

            SDL_RenderCopy(renderer, sprite->component.sprite.image->texture, &sprite->framesSource[sprite->currentFrame], &location );
        }
    }

    return 1;
}

static int graphics_newSprite (lua_State *state) {
    const char* id=luaL_checkstring(state,1);
    const char* file=luaL_checkstring(state,2);
    const int pos_x=luaL_checkint(state,3);
    const int pos_y=luaL_checkint(state,4);
    const int width=luaL_checkint(state,5);
    const int height=luaL_checkint(state,6);

    ProteoComponent* pc=pushProteoComponent(state);

    //strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
    pc->rect.x=pos_x;//-width/2;
    pc->rect.y=pos_y;//-height/2;
    pc->rect.w=width;
    pc->rect.h=height;
    //pc->maincolor=hex2color(color);
    //pc->color=hex2color(border_color);
    //pc->next=NULL;
    pc->surface=NULL;
    //pc->texture=newTexture(gRenderer,file);
    pc->texture=NULL;
    pc->component.sprite.image=newTexture(gRenderer,file);
    if(pc->component.sprite.image==NULL) printf("Error newSprite: %s\n",file);

    pc->currentFrame=0;
    pc->nFrame=0;
    pc->parent=NULL;
    pc->hidden=FALSE;
    pc->type=Sprite;
    pc->layer=100;
    pc->font=NULL;
    pc->txt=NULL;

    /*if(components==NULL)
    {
        components=pc;
    }
    else
    {
        ProteoComponent* current=components;
        while(current->next!=NULL) current=(ProteoComponent*)current->next;
        current->next=pc;
    }*/
    addComponent(pc,&components);
    return 1;
}

//==============================================================================
//   ICON
//==============================================================================

static int graphics_drawIcon (SDL_Renderer* renderer,ProteoComponent* icon)
{
    /*if(sprite->texture!=NULL && sprite->nFrame>0 && sprite->nFrame>sprite->currentFrame)
    {
        if(sprite->framesDestination[sprite->currentFrame].x==0 || sprite->framesDestination[sprite->currentFrame].y==0)
           SDL_RenderCopy(renderer, sprite->texture, &sprite->framesSource[sprite->currentFrame], &sprite->rect );
        else
        {
            SDL_Rect location={sprite->rect.x,sprite->rect.y,
            sprite->framesDestination[sprite->currentFrame].x,sprite->framesDestination[sprite->currentFrame].y};

            SDL_RenderCopy(renderer, sprite->texture, &sprite->framesSource[sprite->currentFrame], &location );
        }
    }*/

    SDL_Rect icon_rect=icon->rect;

    if(icon->parent!=NULL)
    {
        icon_rect.x+=icon->parent->rect.x;
        icon_rect.y+=icon->parent->rect.y;
    }

    if(icon->component.icon.icon->texture!=NULL)
        SDL_RenderCopy(renderer, icon->component.icon.icon->texture, NULL, & icon_rect );

    return 1;
}

static int graphics_newIcon (lua_State *state) {
    const char* id=luaL_checkstring(state,1);
    const char* icon_name=luaL_checkstring(state,2);
    const char* color=luaL_checkstring(state,3);
    const int pos_x=luaL_checkint(state,4);
    const int pos_y=luaL_checkint(state,5);
    const int width=luaL_checkint(state,6);
    const int height=luaL_checkint(state,7);

    ProteoComponent* pc=pushProteoComponent(state);

    //strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
    pc->rect.x=pos_x;//-width/2;
    pc->rect.y=pos_y;//-height/2;
    pc->rect.w=width;
    pc->rect.h=height;
    //pc->maincolor=hex2color(color);
    //pc->color=hex2color(border_color);
    //pc->next=NULL;
    pc->surface=NULL;
    pc->texture=NULL;
    pc->component.icon.icon=newIcon(gRenderer,icon2path(icon_name),hex2color(color));

    pc->currentFrame=0;
    pc->nFrame=0;
    pc->parent=NULL;
    pc->hidden=FALSE;
    pc->type=Icon;
    pc->layer=100;
    pc->font=NULL;
    pc->txt=NULL;

    addComponent(pc,&components);
    return 1;
}

//==============================================================================
//   POLYGON
//==============================================================================

static int graphics_drawPolygon (SDL_Renderer* renderer,ProteoComponent* polygon)
{

    SDL_Rect polygon_rect=polygon->rect;

    /*if(polygon->parent!=NULL && polygon->parent->type!=Shape)
    {
        polygon_rect.x+=polygon->parent->rect.x;
        polygon_rect.y+=polygon->parent->rect.y;

        for(int i=0;i<polygon->component.polygon.npoints;i++)
        {
            polygon->component.polygon.tvx[i]=polygon->component.polygon.vx[i]+polygon_rect.x;
            polygon->component.polygon.tvy[i]=polygon->component.polygon.vy[i]+polygon_rect.y;
        }
    }*/

    if(polygon->parent!=NULL)
    {
        polygon_rect.x+=polygon->parent->rect.x;
        polygon_rect.y+=polygon->parent->rect.y;
    }

    if(polygon->parent!=NULL && polygon->parent->type==Shape && polygon->parent->component.shape.hasSkeleton==FALSE)
    {
        for(int i=0;i<polygon->component.polygon.npoints;i++)
        {
            polygon->component.polygon.tvx[i]=polygon->component.polygon.vx[i]+polygon_rect.x;
            polygon->component.polygon.tvy[i]=polygon->component.polygon.vy[i]+polygon_rect.y;
        }
    }

    if(polygon->component.polygon.npoints>1)
        aapolygonRGBA(renderer, polygon->component.polygon.tvx, polygon->component.polygon.tvy,
                      polygon->component.polygon.npoints,
                      polygon->component.polygon.color.r, polygon->component.polygon.color.g, polygon->component.polygon.color.b, polygon->component.polygon.color.a);

    if(polygon->component.polygon.npoints>2)
        filledPolygonRGBA(renderer, polygon->component.polygon.tvx,
                          polygon->component.polygon.tvy, polygon->component.polygon.npoints, polygon->color.r, polygon->color.g, polygon->color.b, polygon->color.a);
    #ifdef DEBUG_BEZIER
    for(int i=0;i<polygon->component.polygon.nbeziers;i++)
    {
        filledEllipseRGBA (renderer, polygon->component.polygon.beziers[i].T0.x,
                           polygon->component.polygon.beziers[i].T0.y, 3, 3,
                           128, 255, 0, 255);

        filledEllipseRGBA (renderer, polygon->component.polygon.beziers[i].T1.x,
                           polygon->component.polygon.beziers[i].T1.y, 3, 3,
                           128, 255, 0, 255);

        filledEllipseRGBA (renderer, polygon->component.polygon.beziers[i].T2.x,
                           polygon->component.polygon.beziers[i].T2.y, 3, 3,
                           128, 255, 0, 255);

        filledEllipseRGBA (renderer, polygon->component.polygon.beziers[i].T3.x,
                           polygon->component.polygon.beziers[i].T3.y, 3, 3,
                           128, 255, 0, 255);

        thickLineRGBA(renderer,
         polygon->component.polygon.beziers[i].T0.x,polygon->component.polygon.beziers[i].T0.y
        ,polygon->component.polygon.beziers[i].T3.x,polygon->component.polygon.beziers[i].T3.y
        ,2,0,0,255,255);

        thickLineRGBA(renderer,
         polygon->component.polygon.beziers[i].T1.x,polygon->component.polygon.beziers[i].T1.y
                      ,polygon->component.polygon.beziers[i].T2.x,polygon->component.polygon.beziers[i].T2.y
        ,2,0,0,255,255);
    }
    #endif

    return 1;
}

//-----------------------------------------------------------------------------
//-- newPolygon
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param color           polygon color (string, standard name or #rrggbb)
//-- @param border          border color (string, standard name or #rrggbb)
//-- @param pos_x           position x on the screen
//-- @param pos_y           position y on the screen
//--
//-----------------------------------------------------------------------------

static int graphics_newPolygon(lua_State *state) {

    const char* id=luaL_checkstring(state,1);
    const char* color=luaL_checkstring(state,2);
    const char* border_color=luaL_checkstring(state,3);
    const int pos_x=luaL_checkint(state,4);
    const int pos_y=luaL_checkint(state,5);

    ProteoComponent* pc=pushProteoComponent(state);

    //strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
    pc->rect.x=pos_x;//-width/2;
    pc->rect.y=pos_y;//-height/2;
    pc->surface=NULL;
    //pc->texture=newTexture(gRenderer,file);
    pc->texture=NULL;
    pc->component.polygon.color=hex2color(color);
    pc->color=hex2color(border_color);

    pc->component.polygon.npoints=0;
    pc->component.polygon.vx=NULL;
    pc->component.polygon.vy=NULL;

    #ifdef BEZIER
    pc->component.polygon.nbeziers=0;
    pc->component.polygon.beziers=NULL;
    #endif

    pc->currentFrame=0;
    pc->nFrame=0;
    pc->parent=NULL;
    pc->hidden=FALSE;
    pc->type=Polyg;
    pc->layer=100;
    pc->font=NULL;
    pc->txt=NULL;

    addComponent(pc,&components);

    return 1;
}

//==============================================================================
//   BEZIER
//==============================================================================

/*static int graphics_drawBezier (SDL_Renderer* renderer,ProteoComponent* bezier)
{

    return 1;
}*/

//-----------------------------------------------------------------------------
//-- newBezier
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param color           polygon color (string, standard name or #rrggbb)
//-- @param border          border color (string, standard name or #rrggbb)
//-- @param x0
//-- @param y0
//-- @param x1
//-- @param y1
//-- @param x2
//-- @param y2
//-- @param x3
//-- @param y3
//-- @param pos_x           position x on the screen
//-- @param pos_y           position y on the screen
//--
//-----------------------------------------------------------------------------

/*static int graphics_newBezier(lua_State *state) {

    const char* id=luaL_checkstring(state,1);
    const char* color=luaL_checkstring(state,2);
    const char* border_color=luaL_checkstring(state,3);

    int x0=luaL_checkinteger(state, 4);
    int y0=luaL_checkinteger(state, 5);

    int x1=luaL_checkinteger(state, 6);
    int y1=luaL_checkinteger(state, 7);

    int x2=luaL_checkinteger(state, 8);
    int y2=luaL_checkinteger(state, 9);

    int x3=luaL_checkinteger(state, 10);
    int y3=luaL_checkinteger(state, 11);

    const int pos_x=luaL_checkint(state,12);
    const int pos_y=luaL_checkint(state,13);

    ProteoComponent* pc=pushProteoComponent(state);

    //strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
    pc->rect.x=pos_x;//-width/2;
    pc->rect.y=pos_y;//-height/2;
    pc->surface=NULL;
    //pc->texture=newTexture(gRenderer,file);
    pc->texture=NULL;
    pc->component.bezier.color=hex2color(color);
    pc->color=hex2color(border_color);

    pc->component.bezier.npoints=0;
    pc->component.bezier.vx=NULL;
    pc->component.bezier.vy=NULL;

    pc->currentFrame=0;
    pc->nFrame=0;
    pc->parent=NULL;
    pc->hidden=FALSE;
    pc->type=Bezier;
    pc->layer=100;
    pc->font=NULL;
    pc->txt=NULL;

    addComponent(pc,&components);

    return 1;
}*/

//==============================================================================
//   SHAPE
//==============================================================================

static int graphics_drawShape (SDL_Renderer* renderer,ProteoComponent* shape)
{
    SDL_Rect shape_rect=shape->rect;

    if(shape->parent!=NULL)
    {
        shape->rect.x+=shape->parent->rect.x;
        shape->rect.y+=shape->parent->rect.y;
    }

    for(int i=0;i<shape->component.shape.nchilds;i++)
       {
           ProteoComponent* current=shape->component.shape.childs[i];
           if(current->type!=Deleted && current->hidden==FALSE)
           {
               SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
               if(current->type==Polyg) graphics_drawPolygon (renderer,current);
               //else if(current->type==Shape) graphics_drawShape (renderer,current);
           }
           //current=current->component.form.child_next;
       }

    shape->rect.x=shape_rect.x;
    shape->rect.y=shape_rect.y;

    return 1;
}

//-----------------------------------------------------------------------------
//-- newShape
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param pos_x           position x on the screen
//-- @param pos_y           position y on the screen
//--
//-----------------------------------------------------------------------------

static int graphics_newShape(lua_State *state)
{
    const char* id=luaL_checkstring(state,1);
    const int pos_x=luaL_checkint(state,2);
    const int pos_y=luaL_checkint(state,3);

    ProteoComponent* pc=pushProteoComponent(state);

    strlcpy(pc->id,id,PROTEO_MAX_ID);
    pc->rect.x=pos_x;//-width/2;
    pc->rect.y=pos_y;//-height/2;
    pc->surface=NULL;

    pc->texture=NULL;
    pc->component.shape.childs=NULL;
    pc->component.shape.nchilds=0;
    pc->component.shape.hasSkeleton=FALSE;

       pc->currentFrame=0;
       pc->nFrame=0;
       pc->parent=NULL;
       pc->hidden=FALSE;
       pc->type=Shape;
       pc->layer=100;
       pc->font=NULL;
       pc->txt=NULL;

       addComponent(pc,&components);

       return 1;
}

//==============================================================================
//   LUA
//==============================================================================
void add_graphics_proteo(lua_State* state)
{

    addFunction_proteo(state,"graphics","newImage",graphics_newImage);
    addFunction_proteo(state,"graphics","newRect",graphics_newRect);
    addFunction_proteo(state,"graphics","newEllipse",graphics_newEllipse);
    addFunction_proteo(state,"graphics","newSprite",graphics_newSprite);
    addFunction_proteo(state,"graphics","newIcon",graphics_newIcon);
    addFunction_proteo(state,"graphics","newPolygon",graphics_newPolygon);
    addFunction_proteo(state,"graphics","newShape",graphics_newShape);
    //addFunction_proteo(state,"graphics","newBezier",graphics_newBezier);
    //addFunction_proteo(state,"graphics","newRoundRect",graphics_newRoundRect); //TODO
    //addFunction_proteo(state,"graphics","newLine",graphics_newLine); //TODO
    //addFunction_proteo(state,"graphics","newText",graphics_newText); //TODO
    addFunction_proteo(state,"graphics","newSkeleton",graphics_newSkeleton);

	addFunction_proteo(state,"graphics","setBackground",graphics_setbackground);
	addFunction_proteo(state,"graphics","setLayer",graphics_setLayer);
	addFunction_proteo(state,"graphics","isInside",graphics_isInside);
	addFunction_proteo(state,"graphics","changeImage",graphics_changeImage);
	addFunction_proteo(state,"graphics","setHidden",graphics_setHidden);
	addFunction_proteo(state,"graphics","setPosition",graphics_setPosition);
    addFunction_proteo(state,"graphics","getPosition",graphics_getPosition);
    addFunction_proteo(state,"graphics","addFrame",graphics_addFrame);
    addFunction_proteo(state,"graphics","addFrameSource",graphics_addFrameSource);
    addFunction_proteo(state,"graphics","setFrame",graphics_setFrame);
    addFunction_proteo(state,"graphics","flipH",graphics_flipH); //TODO
    addFunction_proteo(state,"graphics","flipV",graphics_flipV); //TODO
    addFunction_proteo(state,"graphics","infoComponent",graphics_infoComponent); //TODO
    addFunction_proteo(state,"graphics","setColor",graphics_setColor);
    //addFunction_proteo(state,"graphics","collide",graphics_collide); //TODO
#ifndef BEZIER
    addFunction_proteo(state,"graphics","addPoint",graphics_addPoint);
#endif
    addFunction_proteo(state,"graphics","addBezier",graphics_addBezier);
    addFunction_proteo(state,"graphics","loadSVG",graphics_loadSVG);
    addFunction_proteo(state,"graphics","addItem",graphics_addItem);
    addFunction_proteo(state,"graphics","addJoint",graphics_addJoint);
    addFunction_proteo(state,"graphics","moveJoint",graphics_moveJoint);
    #ifdef AFFINE
    addFunction_proteo(state,"graphics","resetJoint",graphics_resetJoint);
    addFunction_proteo(state,"graphics","rotateJoint",graphics_rotateJoint);
    #endif
    addFunction_proteo(state,"graphics","setJoint",graphics_setJoint);
    addFunction_proteo(state,"graphics","getJoint",graphics_getJoint);
    addFunction_proteo(state,"graphics","addBone",graphics_addBone);
    addFunction_proteo(state,"graphics","bindSkeleton",graphics_bindSkeleton);
    addFunction_proteo(state,"graphics","loadSkeleton",graphics_loadSkeleton);
    addFunction_proteo(state,"graphics","saveSkeleton",graphics_saveSkeleton);
    addFunction_proteo(state,"graphics","updateSkeleton",graphics_updateSkeleton);
    addFunction_proteo(state,"graphics","hideSkeleton",graphics_hideSkeleton);
}
