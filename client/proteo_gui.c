void freecomponents()
{
    if(debug)printf("Free components\n");
  while(components!=NULL)
  {
    ProteoComponent* tmp=(ProteoComponent*)components->next;

    if(components->surface!=NULL) SDL_FreeSurface(components->surface);
    if(components->txt!=NULL) free(components->txt);
    //if(components->texture!=NULL) SDL_DestroyTexture(components->texture);
    //if(components->font!=NULL) TTF_CloseFont(components->font);
      if(components->type==Form && components->component.form.childs!=NULL) free(components->component.form.childs);
      else if(components->type==List && components->component.list.childs!=NULL) free(components->component.list.childs);
      else if(components->type==DropDown && components->component.dropdown.childs!=NULL) free(components->component.dropdown.childs);
      else if(components->type==Shape && components->component.shape.childs!=NULL) free(components->component.shape.childs);
      else if(components->type==Polyg)
      {
          #ifndef BEZIER
          if(components->component.polygon.vx!=NULL) free(components->component.polygon.vx);
          if(components->component.polygon.vy!=NULL) free(components->component.polygon.vy);
        #endif
          if(components->component.polygon.tvx!=NULL) free(components->component.polygon.tvx);
          if(components->component.polygon.tvy!=NULL) free(components->component.polygon.tvy);
      }

#ifdef LIGHTPROTEOCOMPONENT
      free(components);
#endif
    components=tmp;
  }
}

void freecomponent(ProteoComponent* prev,ProteoComponent* comp)
{
    prev->next=comp->next;
#ifdef DLINK_LIST_COMPONENT
    comp->next->prev=prev;
#endif

    if(debug)printf("Free component %s\n",comp->id);
    if(comp->surface!=NULL) SDL_FreeSurface(comp->surface);
    if(comp->txt!=NULL) free(comp->txt);
    if(comp->type==Form && comp->component.form.childs!=NULL) free(comp->component.form.childs);
    else if(comp->type==List && comp->component.list.childs!=NULL) free(comp->component.list.childs);
    else if(comp->type==DropDown && comp->component.dropdown.childs!=NULL) free(comp->component.dropdown.childs);
    else if(comp->type==Shape && comp->component.shape.childs!=NULL) free(comp->component.shape.childs);
    else if(comp->type==Polyg)
    {
        #ifndef BEZIER
        if(comp->component.polygon.vx!=NULL) free(comp->component.polygon.vx);
        if(comp->component.polygon.vy!=NULL) free(comp->component.polygon.vy);
        #endif
        if(comp->component.polygon.tvx!=NULL) free(comp->component.polygon.tvx);
        if(comp->component.polygon.tvy!=NULL) free(comp->component.polygon.tvy);
    }
    #ifdef LIGHTPROTEOCOMPONENT
          free(comp);
    #endif
}

static int gui_suspendDrawing (lua_State *state) {

    return 0;
}

SDL_Rect getTextureRect(ProteoComponent* pc)
{
    SDL_Rect rect;
    int w,h;
    TTF_SizeText(pc->font->font,pc->txt,&w,&h);
    rect.x=pc->rect.x+(pc->rect.w-w)/2;
    rect.y=pc->rect.y+(pc->rect.h-h)/2;
    rect.w=w;
    rect.h=h;

    if(pc->type==Label)
    {
        if(pc->component.label.align==1) rect.x=pc->rect.x;
        else if(pc->component.label.align==2) rect.x=pc->rect.x+(pc->rect.w-w);

        if(rect.w>pc->rect.w)
        {
            rect.h=(ceilf((float)rect.w/(float)pc->rect.w))*h;
            rect.w=pc->rect.w;
        }
    }
    else if(pc->type==TextField)
    {
        rect.x=pc->rect.x;
    }
    else if(pc->type==DropDown)
    {
        rect.x=pc->rect.x;
    }
    else if(pc->type==ListItem)
    {
        rect.x=10;//pc->rect.x;
    }

    return rect;
}

SDL_Surface* getTxtSurface(ProteoComponent* pc)
{

    if(pc->type==TextField)
    {
        SDL_Color Col = {pc->component.textfield.font_color.r, pc->component.textfield.font_color.g, pc->component.textfield.font_color.b};

        if(pc->component.textfield.mode==0)
            return TTF_RenderText_Blended(pc->font->font, pc->txt, Col);
        else if(pc->component.textfield.mode==1) //Password
        {
            char data[PROTEO_MAX_LENGTH];
            for(int i=0;i<strlen(pc->txt);i++) data[i]='*';
            data[strlen(pc->txt)]=0;
            return TTF_RenderText_Blended(pc->font->font, data, Col);

        }
    }
    else if(pc->type==Button)
    {
        SDL_Color Col = {pc->component.button.font_color.r, pc->component.button.font_color.g, pc->component.button.font_color.b};
        return TTF_RenderText_Blended(pc->font->font, pc->txt, Col);

    }
    else if(pc->type==Label)
    {
        SDL_Color Col = {pc->component.label.font_color.r, pc->component.label.font_color.g, pc->component.label.font_color.b};
        return TTF_RenderText_Blended_Wrapped(pc->font->font, pc->txt, Col,pc->rect.w);
        //return TTF_RenderText_Blended(pc->font->font, pc->txt, Col);
    }
    else if(pc->type==DropDown)
    {
        SDL_Color Col = {pc->component.dropdown.font_color.r, pc->component.dropdown.font_color.g, pc->component.dropdown.font_color.b};
        return TTF_RenderText_Blended(pc->font->font, pc->txt, Col);
    }
    else if(pc->type==DropDownItem)
    {
        SDL_Color Col = {pc->component.dropdownitem.font_color.r, pc->component.dropdownitem.font_color.g, pc->component.dropdownitem.font_color.b};
        return TTF_RenderText_Blended(pc->font->font, pc->txt, Col);
    }
    else if(pc->type==ListItem)
    {
        SDL_Color Col = {pc->component.listitem.font_color.r, pc->component.listitem.font_color.g, pc->component.listitem.font_color.b};
        return TTF_RenderText_Blended(pc->font->font, pc->txt, Col);
    }

}
void updatetxt(ProteoComponent* pc,SDL_Renderer* renderer)
{
    if(pc->font->font==NULL || pc->txt==NULL) return;

    if(pc->surface!=NULL) SDL_FreeSurface(pc->surface);
    if(pc->texture!=NULL) SDL_DestroyTexture(pc->texture);

    pc->texture_rect=getTextureRect(pc);

	/*int w,h;
	TTF_SizeText(pc->font->font,pc->txt,&w,&h);
    pc->texture_rect.x=pc->rect.x;
	pc->texture_rect.y=pc->rect.y+(pc->rect.h-h)/2;
	pc->texture_rect.w=w;
	pc->texture_rect.h=h;*/

    /*SDL_Color Col = {pc->component.textfield.font_color.r, pc->component.textfield.font_color.g, pc->component.textfield.font_color.b};

    if(pc->component.textfield.mode==0)
        pc->surface = TTF_RenderText_Blended(pc->font->font, pc->txt, Col);
    else if(pc->component.textfield.mode==1) //Password
    {
        char data[PROTEO_MAX_LENGTH];
        for(int i=0;i<strlen(pc->txt);i++) data[i]='*';
        data[strlen(pc->txt)]=0;
        pc->surface = TTF_RenderText_Blended(pc->font->font, data, Col);

    }*/
    pc->surface = getTxtSurface(pc);
	pc->texture = SDL_CreateTextureFromSurface(renderer, pc->surface);
}

static int gui_getText (lua_State *state) {

  ProteoComponent* pc=toProteoComponent(state,1);
  if(verbose) printf("gui.getText\n");

    if(pc->type==DropDown)
    {
        //TODO per il dropdown deve tornare il testo del controller o quello del child selezionato?
        lua_pushlstring(state,pc->txt,strlen(pc->txt));
    }
    else lua_pushlstring(state,pc->txt,strlen(pc->txt));

  return 1;
}

static int gui_getId (lua_State *state) {

  ProteoComponent* pc=toProteoComponent(state,1);
  if(verbose) printf("gui.getId\n");

  lua_pushlstring(state,pc->id,strlen(pc->id));

  return 1;
}

static int gui_getValue (lua_State *state) {

  ProteoComponent* pc=toProteoComponent(state,1);
  if(verbose) printf("gui.getValue\n");

    if(pc->type==DropDown)
    {
        if(pc->component.dropdown.selected_child==NULL)
            lua_pushnil(state);
        else
            lua_pushlightuserdata(state, pc->component.dropdown.selected_child);
    }
    else return 0;//TODO

  return 1;
}

static int gui_setText (lua_State *state) {

    ProteoComponent* pc=toProteoComponent(state,1);
    const char* txt=luaL_checkstring(state,2);
    if(debug) printf("gui.setText\n");

    if(pc->txt!=NULL) free(pc->txt);

    if(txt!=NULL && strlen(txt)>=0)
    {
        pc->txt=malloc((strlen(txt)+1)*sizeof(char));
        strlcpy(pc->txt,txt,strlen(txt)+1);

        if(pc->type==TextField) pc->component.textfield.cursor_position=strlen(txt);
    }



  updatetxt(pc,gRenderer);

  return 0;
}

static int gui_addItem (lua_State *state)
{
    ProteoComponent* parent=toProteoComponent(state,1);
    ProteoComponent* child=toProteoComponent(state,2);

    if(debug) printf("gui.addItem %s -> %s\n",child->id,parent->id);

    child->parent=parent;
    //child->child_next=NULL;

    if(parent==NULL){
        printf("addItem ERROR parent==NULL\n");
        return 0;
    }

    ProteoComponent*** childs=NULL;
    int *nchilds=NULL;
    if(parent->type==Form)
    {
        nchilds=&parent->component.form.nchilds;
        childs=&parent->component.form.childs;
    }
    else if(parent->type==Container)
    {
        nchilds=&parent->component.container.nchilds;
        childs=&parent->component.container.childs;
    }
    else if(parent->type==List)
    {
        nchilds=&parent->component.list.nchilds;
        childs=&parent->component.list.childs;
    }
    else if(parent->type==DropDown)
    {
        nchilds=&parent->component.dropdown.nchilds;
        childs=&parent->component.dropdown.childs;
    }
    else return 0;

    *nchilds=*nchilds+1;
    ProteoComponent** tmp;
    int newsize=*nchilds;
    tmp=malloc(*nchilds*sizeof(ProteoComponent*));
   /* for(int i=0;i<(newsize-1);i++)
    {
        ProteoComponent* pc=(*childs)[i];
        tmp[i]=pc;
    }
    tmp[newsize-1]=child;*/
    int pos=newsize-1;
    for(int i=0,j=0;i<(newsize-1);j++,i++)
    {
        ProteoComponent* pc=(*childs)[i];

        if(pos==(newsize-1) && pc->layer>child->layer) //Aggiungo il nuovo elemento nella posizione corretta rispetto al layer
        {
            pos=j;
            j++;
        }

        tmp[j]=pc;
    }
    tmp[pos]=child;

    free(*childs);
    *childs=tmp;

    /*if(parent->child_next==NULL)
    {
        parent->child_next=child;
    }
    else
    {
        child->child_next=parent->child_next;
        parent->child_next=child;
    }*/

    return 0;
}

static int gui_setHidden (lua_State *state) {

  ProteoComponent* pc=toProteoComponent(state,1);

  if(pc!=NULL  && pc->type==Deleted) return 0;

  const int hidden=lua_toboolean(state,2);
  pc->hidden=hidden;
  if(debug) printf("gui.setHidden: %s\n",pc->id);

  return 0;
}

static int gui_setColor (lua_State *state) {

  ProteoComponent* pc=toProteoComponent(state,1);
  const char* color=luaL_checkstring(state,2);

  pc->color=hex2color(color);
  if(debug) printf("gui.setColor: %s\n",pc->id);

  return 0;
}

static int gui_setEnabled (lua_State *state) {

  ProteoComponent* pc=toProteoComponent(state,1);
  const int enabled=lua_toboolean(state,2);
  pc->enabled=enabled;

    if(selected==pc) selected=NULL;

  if(debug) printf("gui.setEnabled: %s\n",pc->id);

  return 0;
}

static int gui_emptyList(lua_State *state) {

    ProteoComponent* pc=toProteoComponent(state,1);

    for(int i=0;i<pc->component.list.nchilds;i++)
    {
        ProteoComponent* current=pc->component.list.childs[i];

        if(current->surface!=NULL) { SDL_FreeSurface(current->surface); current->surface=NULL; }
        if(current->texture!=NULL) { SDL_DestroyTexture(current->texture); current->texture=NULL; }
        //if(current->font->font!=NULL) { TTF_CloseFont(current->font->font); current->font->font=NULL; }
        current->type=Deleted;
        current->parent=NULL;
    }
    free(pc->component.list.childs);
    pc->component.list.nchilds=0;
    pc->component.list.scroll_y=0;
    pc->component.list.childs=NULL;

  /*ProteoComponent* current=components;
    while(current!=NULL)
    {
      if(current->type==ListItem && current->parent==pc)
      {
		if(current->surface!=NULL) { SDL_FreeSurface(current->surface); current->surface=NULL; }
		if(current->texture!=NULL) { SDL_DestroyTexture(current->texture); current->texture=NULL; }
		if(current->font->font!=NULL) { TTF_CloseFont(current->font->font); current->font->font=NULL; }
		current->type=Deleted;
		current->parent=NULL;
      }
      current=current->next;
    }
  */
  return 0;
}

static int gui_linkCheckbox (lua_State *state) {
  ProteoComponent* obj1=toProteoComponent(state,1);
  ProteoComponent* obj2=toProteoComponent(state,2);

  if(debug) printf("gui.linkCheckbox\n");

    obj1->component.checkbox.next_link=obj2;
  obj2->component.checkbox.prev_link=obj1;

  return 0;
}


static int gui_checkState (lua_State *state) {

  ProteoComponent* pc=toProteoComponent(state,1);
    if(pc->type==Checkbox)
        lua_pushinteger(state,pc->component.checkbox.state);
    else if(pc->type==DropDown)
        lua_pushinteger(state,pc->component.dropdown.state);
    else if(pc->type==TextField)
        lua_pushinteger(state,pc->component.textfield.mode);
    else if(pc->type==Form)
        lua_pushinteger(state,pc->component.form.state);
    else return 0;
    
  return 1;
}

static int gui_setState (lua_State *state) {

  ProteoComponent* pc=toProteoComponent(state,1);
  const int state_value=luaL_checkint(state,2);

    if(pc->type==Checkbox)
        pc->component.checkbox.state=state_value;
    else if(pc->type==TextField)
        pc->component.textfield.mode=state_value;
    else if(pc->type==Form)
        pc->component.form.state=state_value;

    if(pc->type==TextField)
        updatetxt(pc, gRenderer);

  return 0;
}

static int gui_setPosition (lua_State *state) {

    ProteoComponent* pc=toProteoComponent(state,1);
    const int x=luaL_checkint(state,2);
    const int y=luaL_checkint(state,3);

    if(debug) printf("gui.setPosition : %s\n",pc->id);

    pc->rect.x=x;
    pc->rect.y=y;

    return 0;
}

static int gui_infoComponent (lua_State *state) {

    return 0;
}

//==============================================================================
//   LIST
//==============================================================================
static int gui_drawList (SDL_Renderer* renderer,ProteoComponent* list)
{
    SDL_Rect list_rect=list->rect;

    if(list->parent!=NULL)
    {
        list_rect.x+=list->parent->rect.x;
        list_rect.y+=list->parent->rect.y;
    }

  SDL_SetRenderDrawColor( renderer, list->color.r,
    list->color.g, list->color.b, list->color.a );
  SDL_RenderFillRect( renderer, & list_rect );

    //ProteoComponent* current=list->component.list.childs[0];
  //int i=0;
  //while(i<list->component.list.nchilds)
      for(int i=0;i<list->component.list.nchilds;i++)
    {
        ProteoComponent* current=list->component.list.childs[i];
      if(current->type==ListItem)
      {
        if(((i*current->rect.h)+list->component.list.scroll_y)>list_rect.h) break;
        if((((i+1)*current->rect.h)+list->component.list.scroll_y)<0)
        {
          //current=current->component.listitem.child_next;
            current=list->component.list.childs[i];
          continue;
        }
          SDL_RenderSetClipRect(renderer,&list_rect);

        current->rect.x=list_rect.x;
        current->rect.y=list_rect.y+(i*current->rect.h)+list->component.list.scroll_y;

        current->texture_rect.x=current->rect.x;
        current->texture_rect.y=current->rect.y+(current->rect.h-current->texture_rect.h)/2;

          ProteoColor current_color=current->color;

        if(current==list->component.list.selected_item)
          current_color=brightness(current->color,1.2f);

        SDL_SetRenderDrawColor( renderer, current_color.r,
        current_color.g, current_color.b, current_color.a );

        SDL_RenderFillRect( renderer, & current->rect );
        SDL_RenderCopy(renderer, current->texture, NULL, & current->texture_rect );

          SDL_RenderSetClipRect(renderer,NULL);
      }
      //current=current->component.listitem.child_next;
    }

  return 1;
}

static int gui_eventList (lua_State *state,ProteoComponent* list,SDL_Event e,SDL_Renderer* renderer)
{
    SDL_Rect list_rect=list->rect;

    if(list->parent!=NULL)
    {
        list_rect.x+=list->parent->rect.x;
        list_rect.y+=list->parent->rect.y;
    }

    if(e.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_Point point={e.button.x/gScale,e.button.y/gScale};
        //ProteoComponent* current=list->component.list.child_next;
        //while(current!=NULL)
        for(int i=0;i<list->component.list.nchilds;i++)
        {
            ProteoComponent* current=list->component.list.childs[i];
            if(SDL_PointInRect(&point,&current->rect))
            {
                //printf("select: %s\n",current->id);
                list->component.list.selected_item=current;
                //printf("call: %s\n",current->parent->callback);
                bool callback_valid=false;
                if(current->parent->callback!=NULL)
                {
                    callback_valid=true;
                    lua_getglobal(state,current->parent->callback);
                }
                else if(current->parent->ref_callback!=-1)
                {
                    callback_valid=true;
                    lua_getref(state,current->parent->ref_callback);
                }
                    
                
                if(callback_valid)
                {
                    //lua_getglobal(state,current->parent->callback);
                    lua_pushlightuserdata(state, current);
                    int error = lua_pcall(state, 1, 0, 0);

                    if (error) {
                        fprintf(stderr, "ERROR pcall(%s): %s\n",current->parent->callback, lua_tostring(state, -1));
                        lua_pop(state, 1);
                    }
                }
                return TRUE;
            }
        }
    }
    else if(e.type == SDL_MOUSEWHEEL)
    {
        int x,y;
        SDL_GetMouseState(&x,&y);
        SDL_Point point={x/gScale,y/gScale};
        if(SDL_PointInRect(&point,&list_rect))
        {
            list->component.list.scroll_y+=e.wheel.y;
            if(list->component.list.scroll_y>0) list->component.list.scroll_y=0;
            return TRUE;
        }
    }
    else if(e.type == SDL_FINGERMOTION)
    {
        SDL_Point point={e.tfinger.x*gWidth,e.tfinger.y*gHeight};
        if(SDL_PointInRect(&point,&list_rect))
        {
            list->component.list.scroll_y+=e.tfinger.dy*gHeight;
            if(list->component.list.scroll_y>0) list->component.list.scroll_y=0;
            return TRUE;
        }
    }
  return FALSE;
}

//-----------------------------------------------------------------------------
//-- newList
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param title           list title (string)
//-- @param font            font name (string)
//-- @param font_size       font size (int)
//-- @param font_color      font color (string, standard name or #rrggbb)
//-- @param back_color      background color (string, standard name or #rrggbb)
//-- @param pos_x           position x on the screen
//-- @param pos_y           position y on the screen
//-- @param width           component width
//-- @param height          component height
//-- @param callback        callback txt update
//--
//-----------------------------------------------------------------------------


static int gui_newList (lua_State *state) {

  const char* id=luaL_checkstring(state,1);
  const char* title=luaL_checkstring(state,2);
  const char* font=luaL_checkstring(state,3);
  const int font_size=luaL_checkint(state,4);
  const char* font_color=luaL_checkstring(state,5);
  const char* back_color=luaL_checkstring(state,6);
  const int pos_x=luaL_checkint(state,7);
  const int pos_y=luaL_checkint(state,8);
  const int width=luaL_checkint(state,9);
  const int height=luaL_checkint(state,10);

    char* callback=NULL;
    int ref_callback=-1;
    if (lua_isstring(state,11)==1)
        callback=(char*)luaL_checkstring(state,11);
    else
        ref_callback=lua_ref(state, TRUE);
    
  ProteoComponent* pc=pushProteoComponent(state);

  //strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
  pc->rect.x=pos_x;
  pc->rect.y=pos_y;
  pc->rect.w=width;
  pc->rect.h=height;
    pc->component.list.font_color=hex2color(font_color);
    pc->component.list.scroll_y=0;
    pc->component.list.selected_item=NULL;
  pc->color=hex2color(back_color);
  //pc->next=NULL;
  pc->surface=NULL;
  pc->texture=NULL;
  pc->hidden=FALSE;
  pc->enabled=TRUE;
  pc->type=List;
  pc->layer=100;
  pc->callback=callback;
    pc->ref_callback=ref_callback;
    
    pc->txt=NULL;
    if(title!=NULL && strlen(title)>0)
    {
        pc->txt=malloc(strlen(title)+1);
        strcpy(pc->txt,title);
    }
  //strcpy(pc->txt,title);
    pc->component.list.nchilds=0;
  pc->component.list.childs=NULL;
  pc->font=NULL;
    pc->parent=NULL;

    addComponent(pc,&components);

  return 1;
}

//-----------------------------------------------------------------------------
//-- newListItem
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param txt             listitem text value (string)
//-- @param font            font name (string)
//-- @param font_size       font size (int)
//-- @param font_color      font color (string, standard name or #rrggbb)
//-- @param back_color      background color (string, standard name or #rrggbb)
//-- @param icon_name       name of icon (string, if empy or null not icon)
//-- @param icon_color      color of icon (string)
//-- @param width           component width
//-- @param height          component height
//--
//-----------------------------------------------------------------------------

static int gui_newListItem (lua_State *state) {
    const char* id=luaL_checkstring(state,1);
    const char* txt=luaL_checkstring(state,2);
    const char* font=luaL_checkstring(state,3);
    const int font_size=luaL_checkint(state,4);
    const char* font_color=luaL_checkstring(state,5);
    const char* back_color=luaL_checkstring(state,6);
    const char* icon_name=luaL_checkstring(state,7);    //TODO
    const char* icon_color=luaL_checkstring(state,8);   //TODO
    const int width=luaL_checkint(state,9);
    const int height=luaL_checkint(state,10);

    ProteoComponent* pc=pushProteoComponent(state);
    if(debug) printf("newListItem: %s\n",id);

    //strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
    pc->rect.x=0;
    pc->rect.y=0;
    pc->rect.w=width;
    pc->rect.h=height;
    pc->component.listitem.font_color=hex2color(font_color);
    pc->color=hex2color(back_color);
    //pc->next=NULL;
    pc->surface=NULL;
    pc->texture=NULL;
    pc->hidden=FALSE;
    pc->enabled=TRUE;
    pc->type=ListItem;
    pc->layer=100;
    pc->font=newFont(font2path(font), font_size);
    //strcpy(pc->txt,txt);
    if(txt!=NULL && strlen(txt)>0)
    {
        pc->txt=malloc(strlen(txt)+1);
        strcpy(pc->txt,txt);
    }
    //pc->component.listitem.child_next=NULL;
    pc->parent=NULL;

    addComponent(pc,&components);

    if(!pc->font->font) {
        if(verbose) printf("Font newListItem error: %s\n", TTF_GetError());
    }
    else
    {
        /*SDL_Color Col = {pc->component.listitem.font_color.r, pc->component.listitem.font_color.g, pc->component.listitem.font_color.b};
        int w,h;
        TTF_SizeText(pc->font->font,txt,&w,&h);
        pc->texture_rect.x=10;//(width-w)/2;
        pc->texture_rect.y=(height-h)/2;
        pc->texture_rect.w=w;
        pc->texture_rect.h=h;
        pc->surface = TTF_RenderText_Blended(pc->font->font, txt, Col);*/

        pc->texture_rect=getTextureRect(pc);
        pc->surface = getTxtSurface(pc);

        pc->texture = SDL_CreateTextureFromSurface(gRenderer, pc->surface);
    }
  return 1;
}
//==============================================================================
//   LABEL
//==============================================================================

static int gui_eventLabel (lua_State *state,ProteoComponent* label,SDL_Event e,SDL_Renderer* renderer)
{
  return FALSE;
}

static int gui_drawLabel  (SDL_Renderer* renderer,ProteoComponent* label)
{
    SDL_Rect label_rect=label->rect;
    SDL_Rect texture_rect=label->texture_rect;

    if(label->parent!=NULL)
    {
        label_rect.x+=label->parent->rect.x;
        label_rect.y+=label->parent->rect.y;

        texture_rect.x+=label->parent->rect.x;
        texture_rect.y+=label->parent->rect.y;
    }

    SDL_SetRenderDrawColor( renderer, label->color.r,
              label->color.g, label->color.b, label->color.a );
    SDL_RenderFillRect( renderer, & label_rect );

    if(label->texture)
        SDL_RenderCopy(renderer, label->texture, NULL, & texture_rect );

    return 1;
}

//-----------------------------------------------------------------------------
//-- newLabel
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param txt             label text value (string)
//-- @param font            font name (string)
//-- @param font_size       font size (int)
//-- @param font_color      font color (string, standard name or #rrggbb)
//-- @param back_color      background color (string, standard name or #rrggbb)
//-- @param alignment       string alignment (center,left,right)
//-- @param pos_x           position x on the screen
//-- @param pos_y           position y on the screen
//-- @param width           component width
//-- @param height          component height
//--
//-----------------------------------------------------------------------------

static int gui_newLabel (lua_State *state) {
  const char* id=luaL_checkstring(state,1);
  const char* txt=luaL_checkstring(state,2);
  const char* font=luaL_checkstring(state,3);
  const int font_size=luaL_checkint(state,4);
  const char* font_color=luaL_checkstring(state,5);
  const char* back_color=luaL_checkstring(state,6);
  const int alignment=luaL_checkint(state,7);
  const int pos_x=luaL_checkint(state,8);
  const int pos_y=luaL_checkint(state,9);
  const int width=luaL_checkint(state,10);
  const int height=luaL_checkint(state,11);

  ProteoComponent* pc=pushProteoComponent(state);

  //strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
    pc->rect.x=pos_x;
    pc->rect.y=pos_y;
    pc->rect.w=width;
    pc->rect.h=height;
    pc->component.label.font_color=hex2color(font_color);
    pc->color=hex2color(back_color);
    //pc->next=NULL;
    pc->surface=NULL;
    pc->texture=NULL;
    pc->hidden=FALSE;
    pc->enabled=TRUE;
    pc->component.label.align=alignment;
    pc->type=Label;
    pc->layer=100;
    pc->font=newFont(font2path(font), font_size);
    //strcpy(pc->txt,txt);
    if(txt!=NULL && strlen(txt)>0)
    {
        pc->txt=malloc(strlen(txt)+1);
        strcpy(pc->txt,txt);
    }
    else pc->txt=NULL;
    //pc->child_next=NULL;
    pc->parent=NULL;

    addComponent(pc,&components);

  //TTF_Font* ttf = TTF_OpenFont("./Helvetica 400.ttf", font_size);

  if(!pc->font->font) {
      if(verbose) printf("Font newLabel(%d) error: %s\n",currentline(state), TTF_GetError());

  }
    else
    {

    /*int w,h;
    TTF_SizeText(pc->font->font,txt,&w,&h);
    pc->texture_rect.x=pos_x;
    pc->texture_rect.y=pos_y;
    pc->texture_rect.w=w;
    pc->texture_rect.h=h;*/
        pc->texture_rect=getTextureRect(pc);
        /*SDL_Color Col = {pc->component.label.font_color.r, pc->component.label.font_color.g, pc->component.label.font_color.b};
        pc->surface = TTF_RenderText_Blended(pc->font->font, txt, Col);*/
        pc->surface = getTxtSurface(pc);
        pc->texture = SDL_CreateTextureFromSurface(gRenderer, pc->surface);
    }

    nComponents++;
    if(verbose) printf("Component label n:%d\n",nComponents);

    return 1;
}

//==============================================================================
//   TEXTFIELD
//==============================================================================

static int gui_eventTextField (lua_State *state,ProteoComponent* textfield,SDL_Event e,SDL_Renderer* renderer)
{
    
    SDL_Rect textfield_rect=textfield->rect;

    if(textfield->parent!=NULL)
    {
        textfield_rect.x+=textfield->parent->rect.x;
        textfield_rect.y+=textfield->parent->rect.y;
    }
    
    
  if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_FINGERDOWN)
  {
    
      SDL_Point point;
      if(e.type == SDL_MOUSEBUTTONDOWN)
      {
          point.x=e.button.x/gScale;
          point.y=e.button.y/gScale;
          
      }else if(e.type == SDL_FINGERDOWN)
      {
          point.x=e.tfinger.x*SCREEN_WIDTH;
          point.y=e.tfinger.y*SCREEN_HEIGHT;
      }
      
      if(SDL_PointInRect(&point,&textfield_rect))
      {
        selected=textfield;
        textfield->component.textfield.cursor_position=strlen(textfield->txt);
        if(debug) printf("Select: %s\n",textfield->id);
#if TARGET_OS_IPHONE
        SDL_StartTextInput();
#endif
        return TRUE;
    }
  }
    
  else if(e.type == SDL_TEXTINPUT)
  {
	if(selected==textfield)
    {
        if(textfield->txt==NULL) return TRUE;

        int input_size=strlen(e.text.text);
        int txt_size=strlen(textfield->txt);
        int new_size=txt_size+input_size+1;
        char* tmp=malloc(new_size);

        for(int i=0;i<textfield->component.textfield.cursor_position;i++)
            tmp[i]=textfield->txt[i];
        for(int i=0;i<input_size;i++)
            tmp[textfield->component.textfield.cursor_position+i]=e.text.text[i];
        for(int i=0;i<txt_size-textfield->component.textfield.cursor_position;i++)
            tmp[textfield->component.textfield.cursor_position+input_size+i]=textfield->txt[textfield->component.textfield.cursor_position+i];

        tmp[new_size-1]=0;
        free(textfield->txt);
        textfield->txt=tmp;

        textfield->component.textfield.cursor_position+=strlen(e.text.text);

		updatetxt(textfield,renderer);
        return TRUE;
	}
  }
  /*                case SDL_TEXTEDITING:

                    Update the composition text.
                    Update the cursor position.
                    Update the selection length (if any).
                    */
  /*                  composition = event.edit.text;
                    cursor = event.edit.start;
                    selection_len = event.edit.length;
                    break;
*/
  else if(e.type == SDL_KEYDOWN)
  {
    if(selected==textfield)
    {
      int update=FALSE;
      if(e.key.keysym.sym==SDLK_BACKSPACE)
      {
        update=TRUE;
        int pos=strlen(textfield->txt);
        //if(pos>0) textfield->txt[pos-1]=0;
          if(textfield->component.textfield.cursor_position>0)
          {
              for(int i=textfield->component.textfield.cursor_position;i<pos;i++)
                  textfield->txt[i-1]=textfield->txt[i];
              textfield->txt[pos-1]=0;
              textfield->component.textfield.cursor_position--;
          }
        //if(textfield->component.textfield.cursor_position>0)
          //  textfield->component.textfield.cursor_position--;
      }
        else if(e.key.keysym.sym==SDLK_LEFT)
        {
            if(textfield->component.textfield.cursor_position>0)
            textfield->component.textfield.cursor_position--;
        }
        else if(e.key.keysym.sym==SDLK_RIGHT)
        {
            int pos=strlen(textfield->txt);
            if(textfield->component.textfield.cursor_position<pos)
            textfield->component.textfield.cursor_position++;
        }
#if TARGET_OS_IPHONE
      else  if(e.key.keysym.sym==SDLK_RETURN)
      {
          SDL_StopTextInput();
      }
#endif
      /*else if( e.key.keysym.sym < 0x80 && e.key.keysym.sym > 0 ){

        update=TRUE;
        int pos=strlen(textfield->txt);
        textfield->txt[pos]=(char)e.key.keysym.sym;
        textfield->txt[pos+1]=0;
        if (e.key.keysym.mod & KMOD_SHIFT)
        {


			if(e.key.keysym.sym>='0' && e.key.keysym.sym<='9')
				textfield->txt[pos]='*';
			else textfield->txt[pos]=toupper(textfield->txt[pos]);
        }

      }*/

      if(update==TRUE)
      {
        updatetxt(textfield,renderer);
        return TRUE;
      }
    }
  }
  return FALSE;
}

int proteo_makecursor(SDL_Rect *rect,ProteoComponent* textfield)
{
    if (!rect) return -1;
    if (textfield->font->font==NULL) return -1;

    SDL_Rect textfield_rect=textfield->rect;

    if(textfield->parent!=NULL)
    {
        textfield_rect.x+=textfield->parent->rect.x;
        textfield_rect.y+=textfield->parent->rect.y;
    }

  int w,h;
    char tmp=textfield->txt[textfield->component.textfield.cursor_position];
    textfield->txt[textfield->component.textfield.cursor_position]=0;
  TTF_SizeText(textfield->font->font,textfield->txt,&w,&h);
    textfield->txt[textfield->component.textfield.cursor_position]=tmp;
  rect->x = textfield_rect.x+w;
  rect->y = textfield_rect.y+(textfield_rect.h-h)/2;
  rect->w = 3;
  rect->h = h;
  return 0;
}

static int gui_drawTextField (SDL_Renderer* renderer,ProteoComponent* textfield)
{
    SDL_Rect textfield_rect=textfield->rect;
    SDL_Rect texture_rect=textfield->texture_rect;

    if(textfield->parent!=NULL)
    {
        textfield_rect.x+=textfield->parent->rect.x;
        textfield_rect.y+=textfield->parent->rect.y;

        texture_rect.x+=textfield->parent->rect.x;
        texture_rect.y+=textfield->parent->rect.y;
    }

  SDL_SetRenderDrawColor( renderer, textfield->color.r,
            textfield->color.g, textfield->color.b, textfield->color.a );
  SDL_RenderFillRect( renderer, & textfield_rect );

    SDL_RenderSetClipRect(renderer,&textfield_rect);
  if(textfield->texture!=NULL) SDL_RenderCopy(renderer, textfield->texture, NULL, & texture_rect );

  if(textfield==selected)
  {
      blinkCursor++;
      if(blinkCursor<30)
      {
          SDL_SetRenderDrawColor( renderer,
                                 textfield->component.textfield.font_color.r,
                                 textfield->component.textfield.font_color.g,
                                 textfield->component.textfield.font_color.b,
                                 255 );
          SDL_Rect cursor;
          proteo_makecursor(&cursor,textfield);
          SDL_RenderFillRect( renderer, & cursor );
      }
      else if (blinkCursor>60) blinkCursor=0;
  }
    SDL_RenderSetClipRect(renderer,NULL);

  return TRUE;
}

//-----------------------------------------------------------------------------
//-- newTextField
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param txt             textfiel initial text value (string)
//-- @param font            font name (string)
//-- @param font_size       font size (int)
//-- @param font_color      font color (string, standard name or #rrggbb)
//-- @param back_color      background color (string, standard name or #rrggbb)
//-- @param pos_x           position x on the screen
//-- @param pos_y           position y on the screen
//-- @param width           component width
//-- @param height          component height
//-- @param callback        callback txt update
//--
//-----------------------------------------------------------------------------

static int gui_newTextField (lua_State *state) {
  const char* id=luaL_checkstring(state,1);
  const char* txt=luaL_checkstring(state,2);
  const char* font=luaL_checkstring(state,3);
  const int font_size=luaL_checkint(state,4);
  const char* font_color=luaL_checkstring(state,5);
  const char* back_color=luaL_checkstring(state,6);
  const int pos_x=luaL_checkint(state,7);
  const int pos_y=luaL_checkint(state,8);
  const int width=luaL_checkint(state,9);
  const int height=luaL_checkint(state,10);

    char* callback=NULL;
    int ref_callback=-1;
    if (lua_isstring(state,11)==1)
        callback=(char*)luaL_checkstring(state,11);
    else
        ref_callback=lua_ref(state, TRUE);

  ProteoComponent* pc=pushProteoComponent(state);

  //strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
  pc->rect.x=pos_x;
  pc->rect.y=pos_y;
  pc->rect.w=width;
  pc->rect.h=height;
  pc->font=newFont(font2path(font), font_size); ;
  //pc->font_size=font_size;
    pc->component.textfield.font_color=hex2color(font_color);
    pc->component.textfield.cursor_position=0;
  pc->color=hex2color(back_color);
  //pc->next=NULL;
  pc->surface=NULL;
  pc->texture=NULL;
  pc->hidden=FALSE;
  pc->enabled=TRUE;
  pc->type=TextField;
    pc->component.textfield.mode=0; //0=normal, 1=password
    pc->callback=callback;
    pc->ref_callback=ref_callback;

    //aamalloc(PROTEO_MAX_LENGTH*sizeof(char));
    if(txt!=NULL )//&& strlen(txt)>0)
    {
        pc->txt=malloc(strlen(txt)+1);
        pc->component.textfield.cursor_position=strlen(txt);
        strlcpy(pc->txt,txt,strlen(txt)+1);
    }
    else pc->txt[0]=0; //TODO da verificare, forse è corretto pc->txt=NULL?
    //pc->child_next=NULL;
    pc->parent=NULL;
    pc->layer=100;

    updatetxt(pc,gRenderer);
    //if(strlen(txt)>0) updatetxt(pc,gRenderer);

    addComponent(pc,&components);

    nComponents++;
    if(verbose) printf("Component textfield n:%d\n",nComponents);
    return 1;
}

//==============================================================================
//   BUTTON
//==============================================================================

static int gui_eventButton (lua_State *state,ProteoComponent* button,SDL_Event e,SDL_Renderer* renderer)
{
    SDL_Rect button_rect=button->rect;
    
    if(button->parent!=NULL)
    {
        button_rect.x+=button->parent->rect.x;
        button_rect.y+=button->parent->rect.y;
    }
    
    SDL_Point point;
    if(e.type == SDL_MOUSEBUTTONDOWN)
    {
        point.x=e.button.x/gScale;
        point.y=e.button.y/gScale;
    }else if(e.type == SDL_FINGERDOWN)
    {
        point.x=e.tfinger.x*SCREEN_WIDTH;
        point.y=e.tfinger.y*SCREEN_HEIGHT;
    }
    else return FALSE;
    
    if(SDL_PointInRect(&point,&button_rect))
    {
      //printf("callback: %s",current->callback);
        if(button->callback!=NULL)
            lua_getglobal(state,button->callback);
        else if(button->ref_callback!=-1)
            lua_getref(state,button->ref_callback);
      lua_pushlightuserdata(state, button);
      int error = lua_pcall(state, 1, 0, 0);

      if (error) {
        fprintf(stderr, "ERROR pcall(%s): %s\n",button->callback, lua_tostring(state, -1));
        lua_pop(state, 1);
      }

      return TRUE;
    }
  
  return FALSE;
}

static int gui_drawButton (SDL_Renderer* renderer,ProteoComponent* button)
{
    SDL_Rect button_rect=button->rect;
    SDL_Rect texture_rect=button->texture_rect;

    if(button->parent!=NULL)
    {
        button_rect.x+=button->parent->rect.x;
        button_rect.y+=button->parent->rect.y;

        texture_rect.x+=button->parent->rect.x;
        texture_rect.y+=button->parent->rect.y;
    }
  ProteoColor bcol={button->color.a,button->color.r,button->color.g,button->color.b};
  if(button->enabled)
  {
    int x,y;

    SDL_GetMouseState(&x,&y);
    SDL_Point point={x/gScale,y/gScale};


    if(SDL_PointInRect(&point,&button_rect))
    {
      bcol=brightness(button->color,1.1f);
      //SDL_SetRenderDrawColor( renderer, bright.r,
            //bright.g, bright.b, button->color.a );
    }
    //else
      //SDL_SetRenderDrawColor( renderer, button->color.r,
            //button->color.g, button->color.b, button->color.a );
  }
  else
  {
    bcol=saturation(button->color,0.3f);
    //SDL_SetRenderDrawColor( renderer, desat.r,
            //desat.g, desat.b, button->color.a );
  }
    roundedBoxRGBA(renderer,button_rect.x-button->component.button.border_size,button_rect.y-button->component.button.border_size,
       button_rect.x+button_rect.w+button->component.button.border_size,button_rect.y+button_rect.h+button->component.button.border_size,
                   button->component.button.rounded?5:0,
       button->colorB.r, button->colorB.g, button->colorB.b, button->colorB.a);
    roundedBoxRGBA(renderer,button_rect.x,button_rect.y,
    button_rect.x+button_rect.w,button_rect.y+button_rect.h,
    button->component.button.rounded?5:0,
    bcol.r, bcol.g, bcol.b, bcol.a);

  //SDL_RenderFillRect( renderer, & button->rect );
  if(button->texture!=NULL) SDL_RenderCopy(renderer, button->texture, NULL, & texture_rect );

  return 1;
}

//-----------------------------------------------------------------------------
//-- newButton
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param txt             button text value (string)
//-- @param font            font name (string)
//-- @param font_size       font size (int)
//-- @param font_color      font color (string, standard name or #rrggbb)
//-- @param back_color      background color (string, standard name or #rrggbb)
//-- @param border_size     border_size (int)
//-- @param border_color    border color (string, standard name or #rrggbb)
//-- @param rounded         rounded (bool)
//-- @param pos_x           position x on the screen
//-- @param pos_y           position y on the screen
//-- @param width           component width
//-- @param height          component height
//-- @param callback        callback function (string or function)
//--
//-----------------------------------------------------------------------------

static int gui_newButton (lua_State *state) {
  const char* id=luaL_checkstring(state,1);
  const char* txt=luaL_checkstring(state,2);
  const char* font=luaL_checkstring(state,3);
  const int font_size=luaL_checkint(state,4);
  const char* font_color=luaL_checkstring(state,5);
  const char* back_color=luaL_checkstring(state,6);
  const int border_size=luaL_checkint(state,7);
  const char* border_color=luaL_checkstring(state,8);
  const int rounded=lua_toboolean(state,9);   //TODO lo trasformerei in ButtonType
  const int pos_x=luaL_checkint(state,10);
  const int pos_y=luaL_checkint(state,11);
  const int width=luaL_checkint(state,12);
  const int height=luaL_checkint(state,13);
    char* callback=NULL;
    int ref_callback=-1;
    if (lua_isstring(state,14)==1)
        callback=(char*)luaL_checkstring(state,14);
    else
        ref_callback=lua_ref(state, TRUE);

  ProteoComponent* pc=pushProteoComponent(state);

  //strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
  pc->rect.x=pos_x;
  pc->rect.y=pos_y;
  pc->rect.w=width;
  pc->rect.h=height;
    pc->component.button.font_color=hex2color(font_color);
    pc->component.button.border_size=border_size;
    pc->component.button.rounded=rounded;
  pc->color=hex2color(back_color);
  pc->colorB=hex2color(border_color);
  //pc->next=NULL;
  pc->surface=NULL;
  pc->texture=NULL;
  pc->hidden=FALSE;
  pc->enabled=TRUE;
  pc->callback=callback;
  pc->ref_callback=ref_callback;
  pc->type=Button;
  pc->font=newFont(font2path(font), font_size);
  if(txt!=NULL && strlen(txt)>0)
  {
      pc->txt=malloc(strlen(txt)+1);
      strcpy(pc->txt,txt);
  }
  else pc->txt=NULL;
    //pc->child_next=NULL;
    pc->parent=NULL;
    pc->layer=100;

    addComponent(pc,&components);
  //TTF_Font* ttf = TTF_OpenFont("./Helvetica 400.ttf", font_size);

  if(!pc->font->font) {
      if(verbose) printf("Font newButton(%d) error: %s\n",currentline(state), TTF_GetError());

  }
  else
  {

    /*int w,h;
    TTF_SizeText(pc->font->font,txt,&w,&h);
    pc->texture_rect.x=pos_x+(width-w)/2;
    pc->texture_rect.y=pos_y+(height-h)/2;
    pc->texture_rect.w=w;
    pc->texture_rect.h=h;*/
      pc->texture_rect=getTextureRect(pc);
      /*SDL_Color Col = {pc->component.button.font_color.r, pc->component.button.font_color.g, pc->component.button.font_color.b};
      pc->surface = TTF_RenderText_Blended(pc->font->font, txt, Col);*/
      pc->surface = getTxtSurface(pc);
      pc->texture = SDL_CreateTextureFromSurface(gRenderer, pc->surface);
  }

    nComponents++;
    if(verbose) printf("Component button n:%d\n",nComponents);

    return 1;
}

//==============================================================================
//   CHECKBOX
//==============================================================================

static int gui_eventCheckbox (lua_State *state,ProteoComponent* checkbox,SDL_Event e,SDL_Renderer* renderer)
{
    SDL_Rect checkbox_rect=checkbox->rect;

  //printf("eventCheck: %s",checkbox->id);
  if(e.type == SDL_MOUSEBUTTONDOWN)
  {
      if(checkbox->parent!=NULL)
      {
          checkbox_rect.x+=checkbox->parent->rect.x;
          checkbox_rect.y+=checkbox->parent->rect.y;
      }

    SDL_Point point={e.button.x/gScale,e.button.y/gScale};
    if(SDL_PointInRect(&point,&checkbox_rect))
    {
      //printf("eventCheck pointInRect: %s\n",checkbox->id);

      if(checkbox->component.checkbox.state==1) checkbox->component.checkbox.state=2;
      else if(checkbox->component.checkbox.state==2)
      {
        checkbox->component.checkbox.state=1;
        ProteoComponent* current=checkbox->component.checkbox.next_link;
        while(current!=NULL && current!=checkbox) {current->component.checkbox.state=2; current=current->component.checkbox.next_link;}
        current=checkbox->component.checkbox.prev_link;
        while(current!=NULL && current!=checkbox) {current->component.checkbox.state=2; current=current->component.checkbox.prev_link;}
      }

      if(checkbox->callback!=NULL && checkbox->callback[0] != '\0')
      {
        lua_getglobal(state,checkbox->callback);
        lua_pushlightuserdata(state, checkbox);
        int error = lua_pcall(state, 1, 0, 0);

        if (error) {
          fprintf(stderr, "ERROR pcall(%s): %s\n",checkbox->callback, lua_tostring(state, -1));
          lua_pop(state, 1);
        }
      }
      return TRUE;
    }
  }
  return FALSE;
}

static int gui_drawCheckbox (SDL_Renderer* renderer,ProteoComponent* checkbox)
{
    SDL_Rect checkbox_rect=checkbox->rect;

    if(checkbox->parent!=NULL)
    {
        checkbox_rect.x+=checkbox->parent->rect.x;
        checkbox_rect.y+=checkbox->parent->rect.y;
    }

    ProteoColor checkbox_color=checkbox->color;
  if(!checkbox->enabled)
  {
    //ProteoColor dark=brightness(button->color,0.5f);
    checkbox_color=saturation(checkbox->color,0.3f);
  }
    if(checkbox->component.checkbox.type==0 || checkbox->component.checkbox.type==1 || checkbox->component.checkbox.type==2)
    {
        SDL_SetRenderDrawColor( renderer, checkbox_color.r,
        checkbox_color.g, checkbox_color.b, checkbox_color.a );
        SDL_RenderFillRect( renderer, & checkbox_rect );
    }
    else
    {
        /*aaellipseRGBA(renderer, checkbox_rect.x+checkbox_rect.w/2, checkbox_rect.y+checkbox_rect.h/2,
                      checkbox_rect.w/2, checkbox_rect.h/2,
        checkbox_color.r, checkbox_color.g, checkbox_color.b, checkbox_color.a);*/

        filledEllipseRGBA (renderer, checkbox_rect.x+checkbox_rect.w/2, checkbox_rect.y+checkbox_rect.h/2, checkbox_rect.w/2, checkbox_rect.h/2,
                           checkbox_color.r, checkbox_color.g, checkbox_color.b, checkbox_color.a);

    }
  /*if(checkbox->texture!=NULL && checkbox->component.checkbox.state==1) SDL_RenderCopy(renderer, checkbox->texture, NULL, & checkbox->texture_rect );*/
    if(checkbox->component.checkbox.state==1)
    {
        /*SDL_SetRenderDrawColor( renderer, checkbox->component.checkbox.mark_color.r,
        checkbox->component.checkbox.mark_color.g, checkbox->component.checkbox.mark_color.b, checkbox->component.checkbox.mark_color.a );
        SDL_RenderDrawLine(renderer,checkbox->rect.x,checkbox->rect.y,checkbox->rect.x+checkbox->rect.w,checkbox->rect.y+checkbox->rect.h);*/
        if(checkbox->component.checkbox.type==0) //SimpleSquare
        {
            SDL_Rect mark;
            mark.x=checkbox_rect.x+5;
            mark.y=checkbox_rect.y+5;
            mark.w=checkbox_rect.w-10;
            mark.h=checkbox_rect.h-10;
            SDL_SetRenderDrawColor( renderer, checkbox->component.checkbox.mark_color.r,
            checkbox->component.checkbox.mark_color.g, checkbox->component.checkbox.mark_color.b, checkbox->component.checkbox.mark_color.a );
            SDL_RenderFillRect( renderer, & mark );
        }
        else if(checkbox->component.checkbox.type==1 || checkbox->component.checkbox.type==4) //CrossSquare CrossRound
        {
            thickLineRGBA(renderer,
                      checkbox_rect.x+5,checkbox_rect.y+5,
                      checkbox_rect.x+checkbox_rect.w-5,checkbox_rect.y+checkbox_rect.h-5,
                      5,
                      checkbox->component.checkbox.mark_color.r,
                      checkbox->component.checkbox.mark_color.g, checkbox->component.checkbox.mark_color.b, checkbox->component.checkbox.mark_color.a );
            thickLineRGBA(renderer,
                        checkbox_rect.x+checkbox_rect.w-5,checkbox_rect.y+5,
                          checkbox_rect.x+5,checkbox_rect.y+checkbox_rect.h-5,
                          5,
                          checkbox->component.checkbox.mark_color.r,
                          checkbox->component.checkbox.mark_color.g, checkbox->component.checkbox.mark_color.b, checkbox->component.checkbox.mark_color.a );
        }
        else if(checkbox->component.checkbox.type==3) //SimpleRound
        {
            SDL_Rect mark;
            mark.x=checkbox_rect.x+(checkbox_rect.w/2);
            mark.y=checkbox_rect.y+(checkbox_rect.h/2);
            mark.w=(checkbox_rect.w/2)-4;
            mark.h=(checkbox_rect.h/2)-4;

            filledEllipseRGBA (renderer, mark.x, mark.y, mark.w, mark.h,
            checkbox->component.checkbox.mark_color.r, checkbox->component.checkbox.mark_color.g, checkbox->component.checkbox.mark_color.b, checkbox->component.checkbox.mark_color.a);
        }
    }
  return 1;
}

//-----------------------------------------------------------------------------
//-- newCheckbox
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param mark_color      mark color (string, standard name or #rrggbb)
//-- @param back_color      background color (string, standard name or #rrggbb)
//-- @param box_type        box type (int)
//-- @param pos_x           position x on the screen
//-- @param pos_y           position y on the screen
//-- @param width           component width
//-- @param height          component height
//-- @param callback        check state change callback (string)
//--
//-----------------------------------------------------------------------------
static int gui_newCheckbox (lua_State *state) {
  const char* id=luaL_checkstring(state,1);
  const char* mark_color=luaL_checkstring(state,2);
  const char* back_color=luaL_checkstring(state,3);
  const int box_type=luaL_checkint(state,4);
  const int pos_x=luaL_checkint(state,5);
  const int pos_y=luaL_checkint(state,6);
  const int width=luaL_checkint(state,7);
  const int height=luaL_checkint(state,8);
  const char* callback=luaL_checkstring(state,9);

  ProteoComponent* pc=pushProteoComponent(state);

  //strcpy(pc->id,id);
    strlcpy(pc->id,id,PROTEO_MAX_ID);
  pc->rect.x=pos_x;
  pc->rect.y=pos_y;
  pc->rect.w=width;
  pc->rect.h=height;
    pc->component.checkbox.mark_color=hex2color(mark_color);
  pc->color=hex2color(back_color);
  //pc->next=NULL;
  pc->surface=NULL;
  pc->texture=NULL;
  pc->hidden=FALSE;
  pc->enabled=TRUE;
  pc->type=Checkbox;
  pc->font=NULL;
    pc->component.checkbox.state=2;//unchecked
    pc->component.checkbox.type=box_type;
    pc->component.checkbox.next_link=NULL;
    pc->component.checkbox.prev_link=NULL;
  //pc->child_next=NULL;
  pc->parent=NULL;
  pc->callback=(char*)callback;
    pc->txt=NULL;
    pc->layer=100;

    addComponent(pc,&components);
  /*pc->font = newFont(font2path("Helvetica"), 30);

  if(!pc->font->font) {
      printf("Font newCheckbox(%d) error: %s\n",currentline(state), TTF_GetError());

  }
  else
  {
      SDL_Color Col = {pc->component.checkbox.mark_color.r, pc->component.checkbox.mark_color.g, pc->component.checkbox.mark_color.b};
    int w,h;
    TTF_SizeText(pc->font->font,"X",&w,&h);
    pc->texture_rect.x=pos_x+(width-w)/2;
    pc->texture_rect.y=pos_y+(height-h)/2;
    pc->texture_rect.w=w;
    pc->texture_rect.h=h;
    pc->surface = TTF_RenderText_Blended(pc->font->font, "X", Col);
    pc->texture = SDL_CreateTextureFromSurface(gRenderer, pc->surface);
  }*/

    nComponents++;
    if(verbose) printf("Component checkbox n:%d\n",nComponents);

    return 1;
}

//==============================================================================
//   DropDown
//==============================================================================
static int gui_drawDropDown (SDL_Renderer* renderer,ProteoComponent* dropdown)
{
    ProteoColor back_color={dropdown->color.a,dropdown->color.r,dropdown->color.g,dropdown->color.b};
    SDL_Rect dropdown_rect=dropdown->rect;
    SDL_Rect texture_rect=dropdown->texture_rect;

    if(dropdown->parent!=NULL)
    {
        dropdown_rect.x+=dropdown->parent->rect.x;
        dropdown_rect.y+=dropdown->parent->rect.y;

        texture_rect.x+=dropdown->parent->rect.x;
        texture_rect.y+=dropdown->parent->rect.y;
    }

    int x,y;
    SDL_GetMouseState(&x,&y);
    SDL_Point point={x/gScale,y/gScale};

    if(dropdown->enabled)
    {
      if(SDL_PointInRect(&point,&dropdown_rect))
      {
        back_color=brightness(dropdown->color,1.1f);
      }
    }
    else
    {
      back_color=saturation(dropdown->color,0.3f);
    }

    SDL_SetRenderDrawColor( renderer, back_color.r, back_color.g, back_color.b, back_color.a );
    SDL_RenderFillRect( renderer, & dropdown_rect );

    if(dropdown->component.dropdown.selected_child!=NULL)
    {
        texture_rect.w=dropdown->component.dropdown.selected_child->texture_rect.w;
        SDL_RenderCopy(renderer, dropdown->component.dropdown.selected_child->texture, NULL, & texture_rect );
    }
    else if(dropdown->texture!=NULL) SDL_RenderCopy(renderer, dropdown->texture, NULL, & texture_rect );

    SDL_Rect open;
    open.x=dropdown_rect.x+dropdown_rect.w-dropdown_rect.h;
    open.y=dropdown_rect.y+5;
    open.w=dropdown_rect.h-10;
    open.h=dropdown_rect.h-10;

    ProteoColor button_color=brightness(back_color,0.9f);

    SDL_SetRenderDrawColor( renderer, button_color.r,
                               button_color.g, button_color.b, button_color.a );
    SDL_RenderFillRect( renderer, & open );

    if(dropdown->component.dropdown.state==0)
        filledTrigonRGBA(renderer,
            open.x+open.w/2,open.y+open.h-5,
            open.x+5,open.y+5,
            open.x+open.w-5,open.y+5,
            back_color.r,back_color.g, back_color.b, back_color.a );
    else
    {
        filledTrigonRGBA(renderer,
            open.x+open.w/2,open.y+5,
            open.x+5,open.y+open.h-5,
            open.x+open.w-5,open.y+open.h-5,
            back_color.r,back_color.g, back_color.b, back_color.a );

        //ProteoComponent* current=dropdown->component.dropdown.child_next;
        int hPos=0;
        //while(current!=NULL)
        for(int i=0;i<dropdown->component.dropdown.nchilds;i++)
        {
            ProteoComponent* current=dropdown->component.dropdown.childs[i];
            SDL_Rect item_rect=current->rect;
            item_rect.x+=dropdown_rect.x;
            item_rect.y+=dropdown_rect.y+dropdown_rect.h+hPos;

            SDL_Rect item_texture_rect=current->texture_rect;
            item_texture_rect.x+=dropdown_rect.x;
            item_texture_rect.y+=dropdown_rect.y+dropdown_rect.h+hPos;

            ProteoColor item_back_color={current->color.a,current->color.r,current->color.g,current->color.b};

            if(SDL_PointInRect(&point,&item_rect))
            {
              item_back_color=brightness(current->color,1.1f);
            }

            SDL_SetRenderDrawColor( renderer, item_back_color.r, item_back_color.g, item_back_color.b, item_back_color.a );
            SDL_RenderFillRect( renderer, & item_rect );

            if(current->texture!=NULL)
                SDL_RenderCopy(renderer, current->texture, NULL, & item_texture_rect );

            hPos+=item_rect.h;
          /*if(((i*current->rect.h)+list->component.list.scroll_y)>list->rect.h) break;
          if((((i+1)*current->rect.h)+list->component.list.scroll_y)<0)
          {
            current=current->child_next;
            i+=1;
            continue;
          }
            SDL_RenderSetClipRect(renderer,&list->rect);

          current->rect.x=list->rect.x;
          current->rect.y=list->rect.y+(i*current->rect.h)+list->component.list.scroll_y;

          current->texture_rect.x=current->rect.x;
          current->texture_rect.y=current->rect.y+(current->rect.h-current->texture_rect.h)/2;

            ProteoColor current_color=current->color;

          if(current==selected)
            current_color=brightness(current->color,1.2f);

          SDL_SetRenderDrawColor( renderer, current_color.r,
          current_color.g, current_color.b, current_color.a );

          SDL_RenderFillRect( renderer, & current->rect );
          SDL_RenderCopy(renderer, current->texture, NULL, & current->texture_rect );
          i+=1;

            SDL_RenderSetClipRect(renderer,NULL);*/
      }
    }
    /*roundedBoxRGBA(renderer,dropdown->rect.x,dropdown->rect.y,
      dropdown->rect.x+dropdown->rect.w,dropdown->rect.y+dropdown->rect.h,
      5,
      bcol.r, bcol.g, bcol.b, bcol.a);
    roundedRectangleRGBA(renderer,dropdown->rect.x,dropdown->rect.y,
      dropdown->rect.x+dropdown->rect.w,dropdown->rect.y+dropdown->rect.h,
      5,
      dropdown->colorB.r, dropdown->colorB.g, dropdown->colorB.b, dropdown->colorB.a);
    //SDL_RenderFillRect( renderer, & button->rect );
    if(dropdown->texture!=NULL) SDL_RenderCopy(renderer, dropdown->texture, NULL, & dropdown->texture_rect );*/

    return 1;

}

static int gui_eventDropDown (lua_State *state,ProteoComponent* dropdown,SDL_Event e,SDL_Renderer* renderer)
{
    if(e.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_Rect dropdown_rect=dropdown->rect;

        if(dropdown->parent!=NULL)
        {
            dropdown_rect.x+=dropdown->parent->rect.x;
            dropdown_rect.y+=dropdown->parent->rect.y;
        }

        SDL_Point point={e.button.x/gScale,e.button.y/gScale};
        if(SDL_PointInRect(&point,&dropdown_rect))
        {
            dropdown->component.dropdown.state=!dropdown->component.dropdown.state;
            return TRUE;
        }

        if(dropdown->component.dropdown.state==0) return FALSE;

        int hPos=0;
        //while(current!=NULL)
        for(int i=0;i<dropdown->component.dropdown.nchilds;i++)
        {
            ProteoComponent* current=dropdown->component.dropdown.childs[i];
            SDL_Rect item_rect=current->rect;
            item_rect.x+=dropdown_rect.x;
            item_rect.y+=dropdown_rect.y+dropdown_rect.h+hPos;

            if(SDL_PointInRect(&point,&item_rect))
            {

                if(debug) printf("Selected item: %s\n",current->txt);
    
                if(dropdown->callback!=NULL)
                    lua_getglobal(state,dropdown->callback);
                else if(dropdown->ref_callback!=-1)
                    lua_getref(state,dropdown->ref_callback);
              lua_pushlightuserdata(state, dropdown);
              int error = lua_pcall(state, 1, 0, 0);

              if (error) {
                fprintf(stderr, "ERROR pcall(%s): %s\n",dropdown->callback, lua_tostring(state, -1));
                lua_pop(state, 1);
              }
                
                dropdown->component.dropdown.selected_child=current;
                dropdown->component.dropdown.state=0;
                return TRUE;
            }

            hPos+=item_rect.h;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
//-- newDropDown
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param txt             dropdown text value (string)
//-- @param font            font name (string)
//-- @param font_size       font size (int)
//-- @param font_color      font color (string, standard name or #rrggbb)
//-- @param icon_name       icon name (string)
//-- @param icon_color      icon color (string, standard name or #rrggbb)
//-- @param type            dropdown type (int)
//-- @param back_color      background color (string, standard name or #rrggbb)
//-- @param pos_x           position x on the screen
//-- @param pos_y           position y on the screen
//-- @param width           component width
//-- @param height          component height
//-- @param callback        callback
//--
//-----------------------------------------------------------------------------

static int gui_newDropDown (lua_State *state)
{
    const char* id=luaL_checkstring(state,1);
    const char* txt=luaL_checkstring(state,2);
    const char* font=luaL_checkstring(state,3);
    const int font_size=luaL_checkint(state,4);
    const char* font_color=luaL_checkstring(state,5);

    const char* icon_name=luaL_checkstring(state,6);
    const char* icon_color=luaL_checkstring(state,7);
    const int type=luaL_checkint(state,8);

    const char* back_color=luaL_checkstring(state,9);

    const int pos_x=luaL_checkint(state,10);
    const int pos_y=luaL_checkint(state,11);
    const int width=luaL_checkint(state,12);
    const int height=luaL_checkint(state,13);

    char* callback=NULL;
    int ref_callback=-1;
    if (lua_isstring(state,14)==1)
        callback=(char*)luaL_checkstring(state,14);
    else
        ref_callback=lua_ref(state, TRUE);

    ProteoComponent* pc=pushProteoComponent(state);

    strlcpy(pc->id,id,PROTEO_MAX_ID);
    pc->rect.x=pos_x;
    pc->rect.y=pos_y;
    pc->rect.w=width;
    pc->rect.h=height;
    pc->component.dropdown.font_color=hex2color(font_color);
    pc->color=hex2color(back_color);
    //pc->colorB=hex2color(icon_color);
    //pc->next=NULL;
    pc->surface=NULL;
    pc->texture=NULL;
    pc->hidden=FALSE;
    pc->enabled=TRUE;
    pc->type=DropDown;
    pc->font=NULL;
    pc->component.dropdown.state=0;//closed
    pc->component.dropdown.nchilds=0;
    pc->component.dropdown.childs=NULL;
    pc->component.dropdown.selected_child=NULL;
    pc->callback=callback;
    pc->ref_callback=ref_callback;
    pc->parent=NULL;
    pc->layer=100;
    pc->font=newFont(font2path(font), font_size);
    if(txt!=NULL && strlen(txt)>0)
    {
        pc->txt=malloc(strlen(txt)+1);
        strcpy(pc->txt,txt);
    }
    else pc->txt=NULL;

    addComponent(pc,&components);

    if(!pc->font->font) {
        if(verbose) printf("Font newDropDown(%d) error: %s\n",currentline(state), TTF_GetError());
    }
    else
    {
        pc->texture_rect=getTextureRect(pc);
        pc->surface = getTxtSurface(pc);
        pc->texture = SDL_CreateTextureFromSurface(gRenderer, pc->surface);
    }

    nComponents++;
    if(verbose) printf("Component dropdown n:%d\n",nComponents);

    return 1;
}
//-----------------------------------------------------------------------------
//-- newDropDownItem
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param txt             dropdown text value (string)
//-- @param font            font name (string)
//-- @param font_size       font size (int)
//-- @param font_color      font color (string, standard name or #rrggbb)
//-- @param back_color      background color (string, standard name or #rrggbb)
//-- @param icon_name       icon name (string)
//-- @param icon_color      icon color (string, standard name or #rrggbb)
//-- @param width           component width
//-- @param height          component height
//--
//-----------------------------------------------------------------------------

static int gui_newDropDownItem (lua_State *state)
{
    const char* id=luaL_checkstring(state,1);
    const char* txt=luaL_checkstring(state,2);
    const char* font=luaL_checkstring(state,3);
    const int font_size=luaL_checkint(state,4);
    const char* font_color=luaL_checkstring(state,5);

    const char* back_color=luaL_checkstring(state,6);
    const char* icon_name=luaL_checkstring(state,7);
    const char* icon_color=luaL_checkstring(state,8);

    const int width=luaL_checkint(state,9);
    const int height=luaL_checkint(state,10);

    ProteoComponent* pc=pushProteoComponent(state);
    if(debug) printf("newDropDropItem: %s\n",id);

    strlcpy(pc->id,id,PROTEO_MAX_ID);
    pc->rect.x=0;
    pc->rect.y=0;
    pc->rect.w=width;
    pc->rect.h=height;
    pc->component.dropdownitem.font_color=hex2color(font_color);
    pc->color=hex2color(back_color);
    //pc->next=NULL;
    pc->surface=NULL;
    pc->texture=NULL;
    pc->hidden=FALSE;
    pc->enabled=TRUE;
    pc->type=DropDownItem;
    pc->layer=100;
    pc->font=newFont(font2path(font), font_size);
      //strcpy(pc->txt,txt);
    if(txt!=NULL && strlen(txt)>0)
    {
        pc->txt=malloc(strlen(txt)+1);
        strcpy(pc->txt,txt);
    }
    //pc->component.dropdownitem.child_next=NULL;
    pc->parent=NULL;

    addComponent(pc,&components);

    if(!pc->font->font) {
        printf("Font newDropBoxItem error: %s\n", TTF_GetError());
    }
    else
    {
        SDL_Color Col = {pc->component.dropdownitem.font_color.r, pc->component.dropdownitem.font_color.g, pc->component.dropdownitem.font_color.b};
        int w,h;
        TTF_SizeText(pc->font->font,txt,&w,&h);
        pc->texture_rect.x=10;//(width-w)/2;
        pc->texture_rect.y=(height-h)/2;
        pc->texture_rect.w=w;
        pc->texture_rect.h=h;
        pc->surface = TTF_RenderText_Blended(pc->font->font, txt, Col);

        /*pc->texture_rect=getTextureRect(pc);
        pc->surface = getTxtSurface(pc);*/

        pc->texture = SDL_CreateTextureFromSurface(gRenderer, pc->surface);
    }
    return 1;
}

//==============================================================================
//   Form
//==============================================================================
static int gui_drawForm (SDL_Renderer* renderer,ProteoComponent* form)
{
    ProteoColor back_color={form->component.form.back_color.a,form->component.form.back_color.r,form->component.form.back_color.g,form->component.form.back_color.b};
    ProteoColor bar_color={form->component.form.bar_color.a,form->component.form.bar_color.r,form->component.form.bar_color.g,form->component.form.bar_color.b};

    if(form->enabled)
    {
      int x,y;
      SDL_GetMouseState(&x,&y);
      SDL_Point point={x/gScale,y/gScale};


      if(SDL_PointInRect(&point,&form->rect))
      {
          //back_color=brightness(form->component.form.back_color,1.1f);
          bar_color=brightness(form->component.form.bar_color,1.1f);
      }
    }
    else
    {
        back_color=saturation(form->component.form.back_color,0.3f);
        bar_color=saturation(form->component.form.bar_color,0.3f);
    }

    if(form->component.form.state==2)
    {
        SDL_SetRenderDrawColor( renderer, back_color.r,
        back_color.g, back_color.b, back_color.a );
        SDL_RenderFillRect( renderer, & form->rect );
    }

    if((form->component.form.mode & 2) == 0)
    {

        SDL_SetRenderDrawColor( renderer, bar_color.r,
                               bar_color.g, bar_color.b, bar_color.a );
        SDL_Rect bar;
        bar.x=form->rect.x;
        bar.y=form->rect.y;
        bar.w=form->rect.w;
        bar.h=form->component.form.bar_size;
        SDL_RenderFillRect( renderer, & bar);

        form->texture_rect.x=10+form->rect.x;
        form->texture_rect.y=form->rect.y+(form->component.form.bar_size-form->texture_rect.h)/2;
        if(form->texture!=NULL) SDL_RenderCopy(renderer, form->texture, NULL, & form->texture_rect );

        ProteoColor button_color=brightness(form->component.form.bar_color,0.9f);

        int resizePosition=1;
        if((form->component.form.mode & 4) == 0)
        {
            resizePosition++;

            SDL_Rect close;
            close.x=form->rect.x+form->rect.w-form->component.form.bar_size;
            close.y=form->rect.y+5;
            close.w=form->component.form.bar_size-10;
            close.h=form->component.form.bar_size-10;

            SDL_SetRenderDrawColor( renderer, button_color.r,
                               button_color.g, button_color.b, button_color.a );
            SDL_RenderFillRect( renderer, & close );

            thickLineRGBA(renderer,
                      close.x+5,close.y+5,
                      close.x+close.w-5,close.y+close.h-5,
                      5,
                      back_color.r,
                      back_color.g, back_color.b, back_color.a );
            thickLineRGBA(renderer,
                        close.x+close.w-5,close.y+5,
                          close.x+5,close.y+close.h-5,
                          5,
                          back_color.r,
                          back_color.g, back_color.b, back_color.a );
        }

        if((form->component.form.mode & 8) == 0)
        {
            SDL_Rect resize;
            resize.x=form->rect.x+form->rect.w-resizePosition*form->component.form.bar_size;
            resize.y=form->rect.y+5;
            resize.w=form->component.form.bar_size-10;
            resize.h=form->component.form.bar_size-10;

            SDL_SetRenderDrawColor( renderer, button_color.r,
                               button_color.g, button_color.b, button_color.a );
            SDL_RenderFillRect( renderer, & resize );

            if(form->component.form.state==2)
                filledTrigonRGBA(renderer,
                        resize.x+resize.w/2,resize.y+5,
                        resize.x+5,resize.y+resize.h-5,
                        resize.x+resize.w-5,resize.y+resize.h-5,
                             back_color.r,back_color.g, back_color.b, back_color.a );
            else
                filledTrigonRGBA(renderer,
                resize.x+resize.w/2,resize.y+resize.h-5,
                resize.x+5,resize.y+5,
                resize.x+resize.w-5,resize.y+5,
                     back_color.r,back_color.g, back_color.b, back_color.a );
        }


    }

    if(form->component.form.state==2)
    {
        //ProteoComponent* current=form->component.form.child_next;
       // while(current!=NULL)
            for(int i=0;i<form->component.form.nchilds;i++)
        {
            ProteoComponent* current=form->component.form.childs[i];
            if(current->type!=Deleted && current->hidden==FALSE)
            {
                SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
                if(current->type==Rect) graphics_drawRect (renderer,current);
                else if(current->type==Ellips) graphics_drawEllipse (renderer,current);
                else if(current->type==Label)  gui_drawLabel (renderer,current);
                else if(current->type==Image) graphics_drawImage (renderer,current);
                else if(current->type==Icon) graphics_drawIcon (renderer,current);
                else if(current->type==Polyg) graphics_drawPolygon (renderer,current);
                else if(current->type==Button) gui_drawButton (renderer,current);
                else if(current->type==TextField) gui_drawTextField (renderer,current);
                else if(current->type==List) gui_drawList (renderer,current);
                else if(current->type==Checkbox) gui_drawCheckbox (renderer,current);
                else if(current->type==Sprite) graphics_drawSprite (renderer,current);
                else if(current->type==DropDown) gui_drawDropDown (renderer,current);
                else if(current->type==Form) gui_drawForm (renderer,current);
            }
            //current=current->component.form.child_next;
        }
    }
    return 1;
}

static int gui_eventForm (lua_State *state,ProteoComponent* form,SDL_Event e,SDL_Renderer* renderer)
{
    if(e.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_Point point={e.button.x/gScale,e.button.y/gScale};
        if((form->component.form.mode & 8) == 0 && (form->component.form.mode & 2) == 0)
        {
            int resizePosition=1;
            if((form->component.form.mode & 4) == 0) resizePosition++;

            SDL_Rect resize;
            resize.x=form->rect.x+form->rect.w-resizePosition*form->component.form.bar_size;
            resize.y=form->rect.y+5;
            resize.w=form->component.form.bar_size-10;
            resize.h=form->component.form.bar_size-10;

            if(SDL_PointInRect(&point,&resize))
            {
                //TODO callback
                if(form->component.form.state==2) form->component.form.state=3;
                else if(form->component.form.state==3) form->component.form.state=2;
                return TRUE;
            }

        }

        if((form->component.form.mode & 1) == 0)
        {
            SDL_Rect bar;
            bar.x=form->rect.x;
            bar.y=form->rect.y;
            bar.w=form->rect.w;
            bar.h=form->component.form.bar_size;
            if(SDL_PointInRect(&point,&bar))
            {
                selected=form;
                return TRUE;
            }
        }
    }
    else if(e.type == SDL_MOUSEBUTTONUP)
    {
        if(selected==form)
        {
            selected=NULL;
            return TRUE;
        }

    }
    else if(e.type == SDL_MOUSEMOTION)
    {
        if(selected==form)
        {
            form->rect.x=e.button.x;
            form->rect.y=e.button.y;
            return TRUE;
        }
    }
    //ProteoComponent* current=form->component.form.child_next;
    //while(current!=NULL)
    for(int i=form->component.form.nchilds-1;i>=0;i--)
    {
        ProteoComponent* current=form->component.form.childs[i];
        if(current->type!=Deleted && current->hidden==FALSE && current->enabled==TRUE )
        {
            if(current->type==Button) { if(gui_eventButton (state,current,e,gRenderer)) break; }
            else if(current->type==TextField) { if(gui_eventTextField (state,current,e,gRenderer)) break; }
            else if(current->type==List) { if(gui_eventList (state,current,e,gRenderer)) break; }
            //else if(current->type==Image) { if(graphics_eventImage (L,current,e,gRenderer)) break; }
            else if(current->type==Checkbox) { if(gui_eventCheckbox (state,current,e,gRenderer)) break; }
            else if(current->type==DropDown) { if(gui_eventDropDown(state,current,e,gRenderer)) break; }
            else if(current->type==Form) { if(gui_eventForm(state,current,e,gRenderer)) break; }
        }
        //current=current->component.form.child_next;
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
//-- newForm
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param title           form title (string)
//-- @param font            font name (string)
//-- @param font_size       font size (int)
//-- @param font_color      font color (string, standard name or #rrggbb)
//-- @param bar_color       bar color (string, standard name or #rrggbb)
//-- @param back_color      background color (string, standard name or #rrggbb)
//-- @param form_type       form type (int)
//-- @param icon_name       name of icon (string, if empy or null not icon)
//-- @param icon_color      color of icon (string, standard name or #rrggbb)
//-- @param pos_x           position x on the screen
//-- @param pos_y           position y on the screen
//-- @param width           component width
//-- @param height          component height
//-- @param callback        state change callback (string)
//--
//-----------------------------------------------------------------------------
static int gui_newForm (lua_State *state) {

    const char* id=luaL_checkstring(state,1);
    const char* title=luaL_checkstring(state,2);
    const char* font=luaL_checkstring(state,3);
    const int font_size=luaL_checkint(state,4);
    const char* font_color=luaL_checkstring(state,5);
    const char* bar_color=luaL_checkstring(state,6);
    const char* back_color=luaL_checkstring(state,7);
    const int bar_size=luaL_checkint(state,8);
    const int form_type=luaL_checkint(state,9);
    const char* icon_name=luaL_checkstring(state,10);    //TODO
    const char* icon_color=luaL_checkstring(state,11);  //TODO
    const int pos_x=luaL_checkint(state,12);
    const int pos_y=luaL_checkint(state,13);
    const int width=luaL_checkint(state,14);
    const int height=luaL_checkint(state,15);
      char* callback=NULL;
      int ref_callback=-1;
      if (lua_isstring(state,16)==1)
          callback=(char*)luaL_checkstring(state,16);
      else
          ref_callback=lua_ref(state, TRUE);

    ProteoComponent* pc=pushProteoComponent(state);
    if(debug) printf("newForm: %s\n",id);

    strlcpy(pc->id,id,PROTEO_MAX_ID);
    pc->rect.x=pos_x;
    pc->rect.y=pos_y;
    pc->rect.w=width;
    pc->rect.h=height;
    pc->component.form.font_color=hex2color(font_color);
    pc->component.form.back_color=hex2color(back_color);
    pc->component.form.bar_color=hex2color(bar_color);
    pc->component.form.mode=form_type;
    pc->component.form.bar_size=bar_size;
    pc->component.form.state=2;
    //pc->next=NULL;
    pc->surface=NULL;
    pc->texture=NULL;
    pc->hidden=FALSE;
    pc->enabled=TRUE;
    pc->type=Form;
    pc->layer=100;
    pc->callback=callback;
    pc->ref_callback=ref_callback;
    pc->font=newFont(font2path(font), font_size);
      //strcpy(pc->txt,txt);
    if(title!=NULL && strlen(title)>0)
    {
        pc->txt=malloc(strlen(title)+1);
        strcpy(pc->txt,title);
    }
    pc->component.form.nchilds=0;
    pc->component.form.childs=NULL;
    pc->parent=NULL;

    addComponent(pc,&components);

    if(!pc->font->font) {
        if(verbose) printf("Font newForm(%d) error: %s\n",currentline(state), TTF_GetError());
    }
    else
    {
        SDL_Color Col = {pc->component.form.font_color.r, pc->component.form.font_color.g, pc->component.form.font_color.b};
        int w,h;
        TTF_SizeText(pc->font->font,title,&w,&h);
        pc->texture_rect.x=10;//(width-w)/2;
        pc->texture_rect.y=(bar_size-h)/2;
        pc->texture_rect.w=w;
        pc->texture_rect.h=h;
        pc->surface = TTF_RenderText_Blended(pc->font->font, title, Col);
        pc->texture = SDL_CreateTextureFromSurface(gRenderer, pc->surface);
    }

    return 1;
}

static int gui_eventContainer (lua_State *state,ProteoComponent* container,SDL_Event e,SDL_Renderer* renderer)
{

    return FALSE;
}

static int gui_drawContainer (SDL_Renderer* renderer,ProteoComponent* container)
{
    ProteoColor back_color={container->component.container.back_color.a,container->component.container.back_color.r,container->component.container.back_color.g,container->component.container.back_color.b};

    if(container->enabled)
    {
        int x,y;
        SDL_GetMouseState(&x,&y);
        SDL_Point point={x/gScale,y/gScale};

        if(SDL_PointInRect(&point,&container->rect))
        {
            //Show scroll
        }
    }
    else
    {
        back_color=saturation(container->component.container.back_color,0.3f);
    }

    SDL_SetRenderDrawColor( renderer, back_color.r,
        back_color.g, back_color.b, back_color.a );
        SDL_RenderFillRect( renderer, & container->rect );

    return FALSE;
}

//-----------------------------------------------------------------------------
//-- newContainer
//--
//-- @param id              component id (string, max size PROTEO_MAX_ID)
//-- @param back_color      background color (string, standard name or #rrggbb)
//-- @param pos_x           position x on the screen
//-- @param pos_y           position y on the screen
//-- @param width           component width
//-- @param height          component height
//--
//-----------------------------------------------------------------------------
static int gui_newContainer (lua_State *state)
{
    const char* id=luaL_checkstring(state,1);
    const char* back_color=luaL_checkstring(state,2);
    const int pos_x=luaL_checkint(state,3);
    const int pos_y=luaL_checkint(state,4);
    const int width=luaL_checkint(state,5);
    const int height=luaL_checkint(state,6);

    ProteoComponent* pc=pushProteoComponent(state);
    if(debug) printf("newContainer: %s\n",id);

    strlcpy(pc->id,id,PROTEO_MAX_ID);
    pc->rect.x=pos_x;
    pc->rect.y=pos_y;
    pc->rect.w=width;
    pc->rect.h=height;
    pc->component.container.back_color=hex2color(back_color);
    //pc->next=NULL;
    pc->surface=NULL;
    pc->texture=NULL;
    pc->hidden=FALSE;
    pc->enabled=TRUE;
    pc->type=Container;
    pc->layer=100;
    pc->callback=NULL;
    pc->ref_callback=-1;
    pc->font=NULL;
    pc->txt=NULL;

    pc->component.container.nchilds=0;
    pc->component.container.childs=NULL;
    pc->parent=NULL;

    addComponent(pc,&components);

    return 1;
}

//==============================================================================
//   LUA
//==============================================================================
void add_gui_proteo(lua_State* state)
{
	addFunction_proteo(state,"gui","newLabel",gui_newLabel);
	addFunction_proteo(state,"gui","newTextField",gui_newTextField);
	addFunction_proteo(state,"gui","newCheckbox",gui_newCheckbox);
	addFunction_proteo(state,"gui","newButton",gui_newButton);
    addFunction_proteo(state,"gui","newList",gui_newList);
    addFunction_proteo(state,"gui","newListItem",gui_newListItem);
    addFunction_proteo(state,"gui","newDropDown",gui_newDropDown);
    addFunction_proteo(state,"gui","newDropDownItem",gui_newDropDownItem);
    addFunction_proteo(state,"gui","newForm",gui_newForm);
    addFunction_proteo(state,"gui","newContainer",gui_newContainer);
    //addFunction_proteo(state,"gui","newProgressBar",gui_newProgressBar); //TODO

	addFunction_proteo(state,"gui","linkCheckbox",gui_linkCheckbox);
	addFunction_proteo(state,"gui","suspendDrawing",gui_suspendDrawing);
	addFunction_proteo(state,"gui","setHidden",gui_setHidden);
	addFunction_proteo(state,"gui","setEnabled",gui_setEnabled);
	addFunction_proteo(state,"gui","getText",gui_getText);
	addFunction_proteo(state,"gui","setText",gui_setText);
	addFunction_proteo(state,"gui","checkState",gui_checkState);
	addFunction_proteo(state,"gui","setState",gui_setState);
	addFunction_proteo(state,"gui","emptyList",gui_emptyList);
	addFunction_proteo(state,"gui","addItem",gui_addItem);
	addFunction_proteo(state,"gui","setColor",gui_setColor);
    addFunction_proteo(state,"gui","infoComponent",gui_infoComponent); //TODO
    addFunction_proteo(state,"gui","setPosition",gui_setPosition);
    //addFunction_proteo(state,"gui","getPosition",gui_getPosition); //TODO
    addFunction_proteo(state,"gui","getValue",gui_getValue);
    addFunction_proteo(state,"gui","getId",gui_getId);
    //addFunction_proteo(state,"gui","setFontColor",gui_setFontColor); //TODO

	addTable_proteo(state,"gui","LabelAlignment");
	addEnum_proteo(state,"gui","LabelAlignment","Center",0);
	addEnum_proteo(state,"gui","LabelAlignment","Left",1);
	addEnum_proteo(state,"gui","LabelAlignment","Right",2);

	addTable_proteo(state,"gui","CheckboxType");
	addEnum_proteo(state,"gui","CheckboxType","SimpleSquare",0);
	addEnum_proteo(state,"gui","CheckboxType","CrossSquare",1);
    addEnum_proteo(state,"gui","CheckboxType","CheckSquare",2); //TODO
    addEnum_proteo(state,"gui","CheckboxType","SimpleRound",3);
    addEnum_proteo(state,"gui","CheckboxType","CrossRound",4);
    addEnum_proteo(state,"gui","CheckboxType","CheckRound",5); //TODO

	addTable_proteo(state,"gui","CheckboxState");
	addEnum_proteo(state,"gui","CheckboxState","Checked",1);
	addEnum_proteo(state,"gui","CheckboxState","Unchecked",2);
	addEnum_proteo(state,"gui","CheckboxState","Mixed",3);

    addTable_proteo(state,"gui","FormState");
    addEnum_proteo(state,"gui","FormState","Fullscreen",1);
    addEnum_proteo(state,"gui","FormState","WindowOpen",2);
    addEnum_proteo(state,"gui","FormState","WindowBar",3);

    addTable_proteo(state,"gui","FormType");
    addEnum_proteo(state,"gui","FormType","Normal",0);
    addEnum_proteo(state,"gui","FormType","Fixed",1);
    addEnum_proteo(state,"gui","FormType","HideBar",2);
    addEnum_proteo(state,"gui","FormType","HideClose",4);
    addEnum_proteo(state,"gui","FormType","HideResize",8);


    addTable_proteo(state,"gui","TextFieldType");
    addEnum_proteo(state,"gui","TextFieldType","Normal",0);
    addEnum_proteo(state,"gui","TextFieldType","Password",1);

    addTable_proteo(state,"gui","DropDownType"); //TODO da semplificare
    addEnum_proteo(state,"gui","DropDownType","Normal",0);
    addEnum_proteo(state,"gui","DropDownType","Stack",1);
    addEnum_proteo(state,"gui","DropDownType","SlidingInBoth",2);
    addEnum_proteo(state,"gui","DropDownType","SlidingInFromLeft",3);
    addEnum_proteo(state,"gui","DropDownType","SlidingInFromRight",4);
    addEnum_proteo(state,"gui","DropDownType","FlipVertical",5);
    addEnum_proteo(state,"gui","DropDownType","FlipFromLeft",6);
    addEnum_proteo(state,"gui","DropDownType","FlipFromRight",7);

    addEnum_proteo(state,"gui","DropDownType","NormalUp",8);
    addEnum_proteo(state,"gui","DropDownType","StackUp",9);
    addEnum_proteo(state,"gui","DropDownType","SlidingInBothUp",10);
    addEnum_proteo(state,"gui","DropDownType","SlidingInFromLeftUp",11);
    addEnum_proteo(state,"gui","DropDownType","SlidingInFromRightUp",12);
    addEnum_proteo(state,"gui","DropDownType","FlipVerticalUp",13);
    addEnum_proteo(state,"gui","DropDownType","FlipFromLeftUp",14);
    addEnum_proteo(state,"gui","DropDownType","FlipFromRightUp",15);
}
