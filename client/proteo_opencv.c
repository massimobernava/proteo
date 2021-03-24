
//#ifdef PROTEO_OPENCV

#define OPENCVMAT "OpenCVMat"
#define OPENCVVC "OpenCVVideoCapture"
#define OPENCVNET "OpenCVNet"

#include <emscripten.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/*typedef struct OCVImage
{
    int  channels;
    unsigned long size;
    int  depth;
    int  width;
    int  height;
    
    unsigned char *data;
    unsigned long  step;
    
} OCVImage;*/


//==============================================================================
//   OPENCV
//==============================================================================

int opencv_img(lua_State *state)
{
   void *mat=(void*)EM_ASM_INT(
		let ret = new cv.Mat();
		return ret;
	);
	
    lua_pushlightuserdata(state, mat);
    
    return 1;
}
int opencv_imread(lua_State *state)
{
    /*const char* file=luaL_checkstring(state,1);
    
    Mat tmp = imread( file, 1 );
    UMat *ret=pushMat(state);
    tmp.copyTo(*ret);
    */
    return 1;
}

int opencv_imwrite(lua_State* state)
{
    /*const char* file=luaL_checkstring(state,1);
    UMat* frame=checkMat(state, 2);
    if (!frame->empty()) imwrite(file, *frame);
    */
    return 0;
}

int opencv_imencode(lua_State* state)
{
    /*UMat *img=checkMat(state,1);
    
    vector<uchar> buf;
    imencode(".jpg",*img,buf);
    
    char *b64 =(char*)malloc(1+Base64encode_len(buf.size()));
    Base64encode(b64,(char*)buf.data(),buf.size());
    //const char* b64=base64encode(digest,digest_lenght);
    //printf("login HMAC B64: %s\n",b64);
    b64[Base64encode_len(buf.size())]=0;
    
    lua_pushstring(state, b64);
    free(b64);*/
    
    return 1;
}

int opencv_imdecode(lua_State* state)
{
    /*const char* img=luaL_checkstring(state, 1);
    UMat *out=checkMat(state,2);
    
    char *tmp=(char *)malloc(Base64decode_len(img));
    int len = Base64decode(tmp, img);
    
    vector<uchar> buf(tmp,tmp+len);
    
    imdecode(buf, 1).copyTo(*out);
    
    free(tmp);*/
    
    return 0;
}

int opencv_videocapture(lua_State* state)
{

    void *cap=(void*)EM_ASM_INT(
					let video = document.getElementById('videoInput');
					let ret = new cv.VideoCapture(video);
					return ret;
				);
	lua_pushlightuserdata(state, cap);	
	return 1;
	
}

int opencv_getFrameSize(lua_State* state)
{
	//VideoCapture *cap=checkCap(state,1);
	
	return 1;
}

int opencv_setFrameSize(lua_State* state)
{
	/*VideoCapture *cap=checkCap(state,1);
	int w=luaL_checkinteger(state, 2);
    int h=luaL_checkinteger(state, 3);
    
    cap->set(CAP_PROP_FRAME_WIDTH, w);
	cap->set(CAP_PROP_FRAME_HEIGHT, h);*/
	
	return 0;
}

int opencv_setBufferSize(lua_State* state)
{
	/*VideoCapture *cap=checkCap(state,1);
	int s=luaL_checkinteger(state, 2);
    
    cap->set(CAP_PROP_BUFFERSIZE, s);*/
	
	return 0;
}


int opencv_frame(lua_State* state)
{

    /*VideoCapture *cap=checkCap(state,1);
    UMat *ret=checkMat(state,2);
    if(cap!=NULL) cap->read(*ret);*/

    return 0;
}

int opencv_setImg(lua_State* state)
{
    /*UMat *ret=checkMat(state,1);
    int rows=luaL_checkinteger(state, 2);
    int cols=luaL_checkinteger(state, 3);
    const char* color=luaL_checkstring(state,4);
    
    char col[8];
    
    if (color[0] == '#')
    {
        strcpy(col,color);
        col[0]='0';
    }
    
    unsigned long value = strtoul(col, NULL, 16);
    
    ret->create(rows, cols, CV_8UC3);
    ret->setTo(Scalar((value >> 0) & 0xff,(value >> 8) & 0xff,(value >> 16) & 0xff));
    */
    return 0;
}

int opencv_resize(lua_State* state)
{
	/*UMat *in=checkMat(state,1);
	UMat *out=checkMat(state,2);

    if(in->empty()) return 0;
    
	resize(*in, *out,out->size());
	*/
	return 0;
}

int opencv_setSize(lua_State* state)
{
    /*UMat *ret=checkMat(state,1);
    int rows=luaL_checkinteger(state, 2);
    int cols=luaL_checkinteger(state, 3);
    
    ret->create(rows, cols, CV_8UC3);
     */
    return 0;
}


const char* type2str(int type) {
  const char* r;

  /*uchar depth = type & CV_MAT_DEPTH_MASK;

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }
*/
  return r;
}


OCVImage getImage(void* mat)
{
    //IplImage  img = (IplImage)(*(UMat*)mat).getMat(ACCESS_READ);
    //Mat img=((UMat*)mat)->getMat(ACCESS_READ);
    
    OCVImage ret;
    /*if(img.depth()==CV_8U)
        ret.depth=8;
    ret.size=img.total()*img.elemSize();
    ret.data=img.data;//imageData;
    ret.height=img.size().height;//height;
    ret.channels=img.channels();//nChannels;
    ret.width=img.size().width;//width;
    ret.step=img.step;//img.widthStep;
    */
    return ret;
}



/*void addFunction_proteo(lua_State *state,const char *lib,const char *fname,lua_CFunction f)
{
	lua_getglobal(state,"proteo");
	lua_getfield(state,-1,lib);
	lua_pushcfunction(state,f);
	lua_setfield(state, -2, fname);
	lua_pop(state,2);
}

int main(int argc,char **argv)
{
	lua_State *state = luaL_newstate();//lua_open();

    luaL_openlibs(state);
    
    lua_newtable(state);

    lua_newtable(state);
    lua_setfield(state, -2, "opencv");

	lua_setglobal(state,"proteo");
	
	addFunction_proteo(state,"opencv","img",opencv_img);
	addFunction_proteo(state,"opencv","videocapture",opencv_videocapture);
	
	const char* test= "opencv_frame=proteo.opencv.img()"
	"opencv_cap=proteo.opencv.videocapture(0)"
	"print('TEST!!!') ";
	
	luaL_dostring(state, test);
	
	lua_close(state);
}*/
