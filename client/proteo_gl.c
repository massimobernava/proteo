
unsigned int power_two_floor(unsigned int val) {
    unsigned int power = 2, nextVal = power*2;

    while((nextVal *= 2) <= val)
      power*=2;

    return power*2;
  }

void GL_SetRenderDrawColor(Uint8 r,Uint8 g,Uint8 b,Uint8 a)
{
    glColor4ub(r,g,b,a );
}

void GL_RenderFillRect(const SDL_Rect * rect)
{
    float x1=(2.0f*(float)rect->x/(float)gWidth)-1.0f;
    float y1=(-2.0f*(float)rect->y/(float)gHeight)+1.0f;
    float x2=(2.0f*(float)(rect->x+rect->w)/(float)gWidth)-1.0f;
    float y2=(-2.0f*(float)(rect->y+rect->h)/(float)gHeight)+1.0f;

    glRectf(x1,y1,x2,y2);
}

void GL_RenderCopy(SDL_Surface * surface,const SDL_Rect * srcrect,const SDL_Rect * texture_rect)
{
    float x1=(2.0f*(float)texture_rect->x/(float)gWidth)-1.0f;
    float y1=(-2.0f*(float)texture_rect->y/(float)gHeight)+1.0f;
    float x2=(2.0f*(float)(texture_rect->x+texture_rect->w)/(float)gWidth)-1.0f;
    float y2=(-2.0f*(float)(texture_rect->y+texture_rect->h)/(float)gHeight)+1.0f;

    //Draw the SDL_Texture * as a Quad
    glEnable(GL_TEXTURE_2D);
    //glActiveTexture(GL_TEXTURE0);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Bind the SDL_Texture in OpenGL
    /*if(SDL_GL_BindTexture(form->texture, NULL, NULL)!=0)
    {
        printf("Error binding texture form %s\n",SDL_GetError());
    }*/
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_rect->w, texture_rect->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);

            glBegin(GL_QUADS); {
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                glTexCoord2f(0, 0); glVertex3f(x1, y1, 0);
                glTexCoord2f(1, 0); glVertex3f(x2, y1, 0);
                glTexCoord2f(1, 1); glVertex3f(x2, y2, 0);
                glTexCoord2f(0, 1); glVertex3f(x1, y2, 0);
            } glEnd();
            glDisable(GL_TEXTURE_2D);
    
    glDisable(GL_TEXTURE_2D);
}


#define ROUNDING_POINT_COUNT 8      // Larger values makes circle smoother.
void GL_RenderFillRoundRect(const SDL_Rect * rect, float r)
{
    float x=(2.0f*(float)rect->x/(float)gWidth)-1.0f;
    float y=(-2.0f*(float)rect->y/(float)gHeight)+1.0f;
    //float x2=(2.0f*(float)(rect->x+rect->w)/(float)gWidth)-1.0f;
    //float y2=(-2.0f*(float)(rect->y+rect->h)/(float)gHeight)+1.0f;
    float w=2.0f*(float)(rect->w)/(float)gWidth;
    float h=2.0f*(float)(rect->h)/(float)gHeight;
    
    SDL_FPoint top_left[ROUNDING_POINT_COUNT];
    SDL_FPoint bottom_left[ROUNDING_POINT_COUNT];
    SDL_FPoint top_right[ROUNDING_POINT_COUNT];
    SDL_FPoint bottom_right[ROUNDING_POINT_COUNT];


    float radius = fmin(w, h);
    radius *= r;


    int i = 0;
    float x_offset, y_offset;
    float step = ( 2.0f * M_PI ) / (ROUNDING_POINT_COUNT * 4),
          angle = 0.0f;

    unsigned int index = 0, segment_count = ROUNDING_POINT_COUNT;
    SDL_FPoint bottom_left_corner = { x + radius, y - h + radius };


    while( i != segment_count )
    {
        x_offset = cosf( angle );
        y_offset = sinf( angle );


        top_left[ index ].x = bottom_left_corner.x -
                              ( x_offset * radius );
        top_left[ index ].y = ( h - ( radius * 2.0f ) ) +
                                bottom_left_corner.y -
                              ( y_offset * radius );


        top_right[ index ].x = ( w - ( radius * 2.0f ) ) +
                                 bottom_left_corner.x +
                               ( x_offset * radius );
        top_right[ index ].y = ( h - ( radius * 2.0f ) ) +
                                 bottom_left_corner.y -
                               ( y_offset * radius );


        bottom_right[ index ].x = ( w - ( radius * 2.0f ) ) +
                                    bottom_left_corner.x +
                                  ( x_offset * radius );
        bottom_right[ index ].y = bottom_left_corner.y +
                                  ( y_offset * radius );


        bottom_left[ index ].x = bottom_left_corner.x -
                                 ( x_offset * radius );
        bottom_left[ index ].y = bottom_left_corner.y +
                                 ( y_offset * radius );


        /*top_left[ index ].x = top_left[ index ].x;
        top_left[ index ].y = top_left[ index ].y;


        top_right[ index ].x = top_right[ index ].x;
        top_right[ index ].y = top_right[ index ].y;


        bottom_right[ index ].x = bottom_right[ index ].x ;
        bottom_right[ index ].y = bottom_right[ index ].y;


        bottom_left[ index ].x =  bottom_left[ index ].x ;
        bottom_left[ index ].y =  bottom_left[ index ].y ;*/

        angle -= step;

        ++index;

        ++i;
    }

    glBegin( GL_TRIANGLE_STRIP );
    {
        // Top
        for( i = segment_count - 1 ; i >= 0 ; i--)
        {
            glVertex2f( top_left[ i ].x, top_left[ i ].y );
            glVertex2f( top_right[ i ].x, top_right[ i ].y );
        }

        // In order to stop and restart the strip.
        glVertex2f( top_right[ 0 ].x, top_right[ 0 ].y );
        glVertex2f( top_right[ 0 ].x, top_right[ 0 ].y );

        // Center
        glVertex2f( top_right[ 0 ].x, top_right[ 0 ].y );
        glVertex2f( top_left[ 0 ].x, top_left[ 0 ].y );
        glVertex2f( bottom_right[ 0 ].x, bottom_right[ 0 ].y );
        glVertex2f( bottom_left[ 0 ].x, bottom_left[ 0 ].y );

        // Bottom
        for( i = 0; i != segment_count ; i++ )
        {
            glVertex2f( bottom_right[ i ].x, bottom_right[ i ].y );
            glVertex2f( bottom_left[ i ].x, bottom_left[ i ].y );
        }
    }
    glEnd();
} //DrawRoundRect

void GL_RenderSetClipRect(const SDL_Rect * rect)
{
    if(rect==NULL)
    {
        glDisable(GL_SCISSOR_TEST);
        return;
    }
    
    float x=(2.0f*(float)rect->x/(float)gWidth)-1.0f;
    float y=(-2.0f*(float)rect->y/(float)gHeight)+1.0f;
    //float x2=(2.0f*(float)(rect->x+rect->w)/(float)gWidth)-1.0f;
    //float y2=(-2.0f*(float)(rect->y+rect->h)/(float)gHeight)+1.0f;
    float w=2.0f*(float)(rect->w)/(float)gWidth;
    float h=2.0f*(float)(rect->h)/(float)gHeight;
    
    glEnable(GL_SCISSOR_TEST);
    //glScissor(x, y, w, h);
    glScissor(rect->x,rect->y,rect->w,rect->h);
}

void GL_ResizeSurface(ProteoComponent* pc)
{
    pc->texture_rect.w = power_two_floor(pc->texture_rect.w)*2;
    pc->texture_rect.h = power_two_floor(pc->texture_rect.h)*2;

    SDL_Surface *tmp_surface = SDL_CreateRGBSurface(0, pc->texture_rect.w, pc->texture_rect.h, 32, 0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
    SDL_BlitSurface(pc->surface, NULL, tmp_surface, NULL);
    SDL_FreeSurface(pc->surface);
    pc->surface=tmp_surface;
}
