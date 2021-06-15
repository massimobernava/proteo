//==============================================================================
//   UTILITY
//==============================================================================

ProteoColor brightness(ProteoColor color,float bright)
{
	ProteoColor col={255,
		MIN(255,color.r*bright),
		MIN(255,color.g*bright),
		MIN(255,color.b*bright)};

		return col;
}

ProteoColor saturation(ProteoColor color, float sat)
{
	const float Pr=0.299;
	const float Pg=0.587;
	const float Pb=0.114;

	float P=sqrt(color.r*color.r*Pr+color.g*color.g*Pg+color.b*color.b*Pb);

	ProteoColor col={255,
		P+(color.r-P)*sat,
		P+(color.g-P)*sat,
		P+(color.b-P)*sat};

		return col;
}

ProteoColor ColorInterpolation(ProteoColor startColor, float t, ProteoColor endColor)
{
    //printf("ColorInterpolation: %f\n",t);
    
    ProteoColor finalColor;

    finalColor.r = startColor.r + (endColor.r - startColor.r) * t;
    finalColor.g = startColor.g + (endColor.g - startColor.g) * t;
    finalColor.b = startColor.b + (endColor.b - startColor.b) * t;
    finalColor.a = startColor.a + (endColor.a - startColor.a) * t;

    return finalColor;
}

float interpolation(float start, float t, float end)
{
    return start + (end - start) * t;
}

char font_path[256];
char* font2path(const char* font)
{
	//#ifdef __EMSCRIPTEN__
	//strcpy(font_path,"");
	//#else

	//#endif

	size_t n = sizeof(nfonts)/sizeof(nfonts[0]);
	for(int i=0;i<n;i++)
	{
		if(strcasecmp(font,nfonts[i].name)==0)
		{
            if(nfonts[i].base==TRUE)
                strcpy(font_path,config.basedir);
            else
            {
#if TARGET_OS_MAC
                wordexp_t exp_result;
                wordexp("~/Library/Preferences/Proteo/", &exp_result, 0);
                strcpy(font_path,exp_result.we_wordv[0]);
                wordfree(&exp_result);
#else
                strcpy(font_path,config.basedir);
#endif
            }
			return strcat(font_path,nfonts[i].path);
		}
	}
	return NULL;
}

char icon_path[256];
char* icon2path(const char* icon)
{
    strcpy(icon_path,config.basedir);
    //#endif

    size_t n = sizeof(nicons)/sizeof(nicons[0]);
    for(int i=0;i<n;i++)
    {
        if(strcasecmp(icon,nicons[i].name)==0)
        {
            return strcat(icon_path,nicons[i].path);
        }
    }
    return NULL;
}

ProteoColor hex2color(const char* input) {

	size_t n = sizeof(colors)/sizeof(colors[0]);
	for(int i=0;i<n;i++)
	{
		if(strcasecmp(input,colors[i].name)==0) return colors[i].color;
	}

    ProteoColor col;
	char color[10];

    if(verbose) printf("hex2color: %s\n",input);
	if (input[0] == '#')
	{
        if(strlen(input)==7)
        {
            strcpy(color+1,input);
            color[0]='f';
            color[1]='f';
        }
        else
        {
            strcpy(color,input);
            color[0]='0';
        }
		
            
	}
	else
	{
		return col;
	}

    unsigned long value = strtoul(color, NULL, 16);

    col.a = (value >> 24) & 0xff;
    col.r = (value >> 16) & 0xff;
    col.g = (value >> 8) & 0xff;
    col.b = (value >> 0) & 0xff;
    //return color;

    return col;
}

void mkpath(const char* dir) {
        char tmp[256];
        char *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp),"%s",dir);
        dirname(tmp);
        len = strlen(tmp);
        if(tmp[len - 1] == '/')
                tmp[len - 1] = 0;
        for(p = tmp + 1; *p; p++)
                if(*p == '/') {
                        *p = 0;
                        #if TARGET_OS_WINDOWS //defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__MINGW64__)
                        _mkdir(tmp);
                        #else
                        int ret=mkdir(tmp, S_IRWXU);
                        #endif
                        *p = '/';
                }
        #if TARGET_OS_WINDOWS //defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__MINGW64__)
        _mkdir(tmp);
        #else
        int ret=mkdir(tmp, S_IRWXU);

        #endif
}

int writefile(char* filename,char* data)
{
    FILE *handler=fopen(filename,"w");
    if (handler)
    {
        fprintf(handler,"%s",data);
        fclose(handler);
        return 0;
    }
    else printf("Write file %s error: %s\n",filename,strerror(errno));

    return 1;
}

int writedatafile(char* filename,char* data,int data_size)
{
	int write_size;
	FILE *handler=fopen(filename,"wb");
    if (handler)
    {
        write_size = fwrite(data, sizeof(char), data_size, handler);
        fclose(handler);
        if(debug) printf("Write data file: %s,%d,%d\n",filename,data_size,write_size);
        return write_size;
    }
    else printf("Write data %s error: %s\n",filename,strerror(errno));

    return 0;
}

char* loadfile(char *filename)
{
   char *buffer = NULL;
   int string_size, read_size;
   FILE *handler = fopen(filename, "r");

   if (handler)
   {
       fseek(handler, 0, SEEK_END);
       string_size = ftell(handler);
       rewind(handler);

       buffer = (char*) malloc(sizeof(char) * (string_size + 1) );
       if(buffer!=NULL)
       {
           read_size = fread(buffer, sizeof(char), string_size, handler);
           buffer[string_size] = '\0';

           if (string_size != read_size)
           {
               free(buffer);
               buffer = NULL;
           }
       }

      fclose(handler);
    }
    else printf("Load file %s error: %s\n",filename,strerror(errno));

    return buffer;
}

int loaddatafile(char *filename,char **buffer)
{
  	int data_size, read_size;
	FILE *handler = fopen(filename, "r");

	if (handler)
	{
    	fseek(handler, 0, SEEK_END);
    	data_size = ftell(handler);
    	rewind(handler);

    	*buffer = (char*) malloc(sizeof(char) * (data_size) );
    	if(buffer!=NULL)
    	{
    		read_size = fread(*buffer, sizeof(char), data_size, handler);

    		if (data_size != read_size)
    		{
           		free(*buffer);
           		*buffer = NULL;
           		return 0;
       		}
			//if(debug) printf("Load data file: %s,%d,%d\n",filename,data_size,read_size);
            if(debug) printf("Load data file: %s\n",filename);
		}
      fclose(handler);
    }
    else printf("Load data %s error: %s\n",filename,strerror(errno));

    return read_size;
}

void hexDump (const char * desc, const void * addr, const int len) {
    int i;
    unsigned char buff[17];
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.

    if (desc != NULL)
        printf ("%s:\n", desc);

    // Length checks.

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    else if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.

    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Don't print ASCII buffer for the "zeroth" line.

            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.

            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.

    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII buffer.

    printf ("  %s\n", buff);
}

char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1);

    if(result!=NULL)
    {
        memcpy(result, s1, len1);
        memcpy(result + len1, s2, len2 + 1);
    }
    result[len1 + len2]=0;

    return result;
}

char* concat3(const char *s1, const char *s2, const char *s3)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    const size_t len3 = strlen(s3);

    char *result = malloc(len1 + len2 + len3 + 1);

    if(result!=NULL)
    {
        memcpy(result, s1, len1);
        memcpy(result + len1, s2, len2);
        memcpy(result + len1 + len2, s3, len3 + 1); // +1 to copy the null-terminator
    }
    result[len1 + len2 + len3]=0;

    return result;
}

int c_quote(const char* src, char* dest, int maxlen) {
    int count = 0;
    if(maxlen < 0) {
        maxlen = strlen(src)+1;      /* add 1 for NULL-terminator */
    }

    while(src  && maxlen > 0) {
        switch(*src) {

            /* these normal, printable chars just need a slash appended */
            case '\\':
            case '\"':
            case '\'':
                if(dest) {
                    *dest++ = '\\';
                    *dest++ = *src;
                }
                count += 2;
                break;

            /* newlines/tabs and unprintable characters need a special code.
             * Use the macro CASE_CHAR defined below.
             * The first arg for the macro is the char to compare to,
             * the 2nd arg is the char to put in the result string, after the '\' */
#define CASE_CHAR(c, d) case c:\
    if(dest) {\
        *dest++ = '\\'; *dest++ = (d);\
        }\
count += 2;\
break;
            /* --------------  */
            CASE_CHAR('\n', 'n');
            CASE_CHAR('\t', 't');
            CASE_CHAR('\b', 'b');
            /*  ------------- */

#undef CASE_CHAR


            /* by default, just copy the char over */
            default:
                if(dest) {
                    *dest++ = *src;
                }
                count++;
        }

        ++src;
        --maxlen;
    }
    return count;
}

//simple encrypt-decrypt function
void encryptDecrypt(char inpString[])
{
    char xorKey[] = "Key";
    int len = strlen(inpString);

    for (int i = 0; i < len; i++)
    {
        inpString[i] = inpString[i] ^ xorKey[i%strlen(xorKey)];
        printf("%c",inpString[i]);
    }
}

#ifdef AFFINE
SDL_Point affineTrasformation(SDL_Point point,float affine[3][3])
{
    SDL_Point ret;

    ret.x=point.x*affine[0][0]+point.y*affine[1][0]+affine[2][0];
    ret.y=point.x*affine[0][1]+point.y*affine[1][1]+affine[2][1];

    return ret;
}
#endif

void mult_matrices(float a[3][3], float b[3][3], float result[3][3])
{
    int i, j, k;
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            for(k = 0; k < 3; k++)
            {
                result[i][j] +=  a[i][k] *  b[k][j];
            }
        }
    }
}


void laderman_mul(const float a[3][3],const float b[3][3],float c[3][3]) {

   float m[24]; // not off by one, just wanted to match the index from the paper

   m[1 ]= (a[0][0]+a[0][1]+a[0][2]-a[1][0]-a[1][1]-a[2][1]-a[2][2])*b[1][1];
   m[2 ]= (a[0][0]-a[1][0])*(-b[0][1]+b[1][1]);
   m[3 ]= a[1][1]*(-b[0][0]+b[0][1]+b[1][0]-b[1][1]-b[1][2]-b[2][0]+b[2][2]);
   m[4 ]= (-a[0][0]+a[1][0]+a[1][1])*(b[0][0]-b[0][1]+b[1][1]);
   m[5 ]= (a[1][0]+a[1][1])*(-b[0][0]+b[0][1]);
   m[6 ]= a[0][0]*b[0][0];
   m[7 ]= (-a[0][0]+a[2][0]+a[2][1])*(b[0][0]-b[0][2]+b[1][2]);
   m[8 ]= (-a[0][0]+a[2][0])*(b[0][2]-b[1][2]);
   m[9 ]= (a[2][0]+a[2][1])*(-b[0][0]+b[0][2]);
   m[10]= (a[0][0]+a[0][1]+a[0][2]-a[1][1]-a[1][2]-a[2][0]-a[2][1])*b[1][2];
   m[11]= a[2][1]*(-b[0][0]+b[0][2]+b[1][0]-b[1][1]-b[1][2]-b[2][0]+b[2][1]);
   m[12]= (-a[0][2]+a[2][1]+a[2][2])*(b[1][1]+b[2][0]-b[2][1]);
   m[13]= (a[0][2]-a[2][2])*(b[1][1]-b[2][1]);
   m[14]= a[0][2]*b[2][0];
   m[15]= (a[2][1]+a[2][2])*(-b[2][0]+b[2][1]);
   m[16]= (-a[0][2]+a[1][1]+a[1][2])*(b[1][2]+b[2][0]-b[2][2]);
   m[17]= (a[0][2]-a[1][2])*(b[1][2]-b[2][2]);
   m[18]= (a[1][1]+a[1][2])*(-b[2][0]+b[2][2]);
   m[19]= a[0][1]*b[1][0];
   m[20]= a[1][2]*b[2][1];
   m[21]= a[1][0]*b[0][2];
   m[22]= a[2][0]*b[0][1];
   m[23]= a[2][2]*b[2][2];

  c[0][0] = m[6]+m[14]+m[19];
  c[0][1] = m[1]+m[4]+m[5]+m[6]+m[12]+m[14]+m[15];
  c[0][2] = m[6]+m[7]+m[9]+m[10]+m[14]+m[16]+m[18];
  c[1][0] = m[2]+m[3]+m[4]+m[6]+m[14]+m[16]+m[17];
  c[1][1] = m[2]+m[4]+m[5]+m[6]+m[20];
  c[1][2] = m[14]+m[16]+m[17]+m[18]+m[21];
  c[2][0] = m[6]+m[7]+m[8]+m[11]+m[12]+m[13]+m[14];
  c[2][1] = m[12]+m[13]+m[14]+m[15]+m[22];
  c[2][2] = m[6]+m[7]+m[8]+m[9]+m[23];
}

SDL_FPoint I2FPoint(SDL_Point p)
{
    SDL_FPoint ret={p.x,p.y};
    return ret;
}
SDL_Point F2IPoint(SDL_FPoint p)
{
    SDL_Point ret={p.x,p.y};
    return ret;
}
SDL_FPoint subtract(SDL_FPoint p0, SDL_FPoint p1)
{
    SDL_FPoint ret={p0.x-p1.x,p0.y-p1.y};

    return ret;
}

SDL_FPoint sum(SDL_FPoint p0, SDL_FPoint p1)
{
    SDL_FPoint ret={p0.x+p1.x,p0.y+p1.y};

    return ret;
}

SDL_FPoint multiply(SDL_FPoint p, float f)
{
    SDL_FPoint ret={p.x*f,p.y*f};

    return ret;
}

float dot(SDL_FPoint p0, SDL_FPoint p1)
{
    float ret=(p0.x*p1.x)+(p0.y*p1.y);

    return ret;
}

float distance(SDL_FPoint p0,SDL_FPoint p1)
{
    return hypotf(p0.x-p1.x,p0.y-p1.y);
}

SDL_FPoint normalize(SDL_FPoint p)
{
    float mag=hypotf(p.x,p.y);
    SDL_FPoint ret={p.x/mag,p.y/mag};

    //problema di normalizzazione? vettore spesso 0
    return ret;
}

float magnitude(SDL_FPoint p)
{
    float len=hypotf(p.x,p.y);

    return len;
}

SDL_FPoint getClosestPointOnSegment(SDL_FPoint p0, SDL_FPoint p1, SDL_FPoint p)
{
    SDL_FPoint d=subtract(p1,p0);
    float c=dot(subtract(p,p0),d)/dot(d,d);
    if(c>=1) return p1;
    else if(c<=0) return p0;
    else return sum(p0,multiply(d,c));
}

/*static getClosestPointOnSegment(p0, p1, p) {
    let d = p1.subtract(p0);
    let c = p.subtract(p0).dot(d) / (d.dot(d));
    if (c >= 1) {
        return p1.clone();
    } else if (c <= 0) {
        return p0.clone();
    } else {
        return p0.add(d.multiply(c));
    }
}
*/
