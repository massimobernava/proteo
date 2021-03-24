
//#ifdef PROTEO_OPENCV

//#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/types_c.h>

#define OPENCVMAT "OpenCVMat"
#define OPENCVVC "OpenCVVideoCapture"
#define OPENCVNET "OpenCVNet"
#define OPENCVVW "OpenCVVideoWriter"

using namespace std;
using namespace cv;
using namespace cv::dnn;
//using namespace cv::cuda;

extern "C" {
//#endif
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

int Base64encode(char *encoded, const char *string, int len);
int Base64encode_len(int len);
int Base64decode(char *bufplain, const char *bufcoded);
int Base64decode_len(const char *bufcoded);


typedef struct OCVImage
{
    int  channels;
    unsigned long size;
    int  depth;
    int  width;
    int  height;
    
    unsigned char *data;
    unsigned long  step;
    
} OCVImage;

//==============================================================================
//   CHECK
//==============================================================================

static UMat *checkMat (lua_State *L, int index)
{
    UMat *mat;
    luaL_checktype(L, index, LUA_TUSERDATA);
    mat = (UMat *)luaL_checkudata(L, index, OPENCVMAT);
    if (mat == NULL) luaL_typerror(L, index, OPENCVMAT);
    return mat;
}

static VideoCapture *checkCap (lua_State *L, int index)
{
    VideoCapture *cap;
    luaL_checktype(L, index, LUA_TUSERDATA);
    cap = (VideoCapture *)luaL_checkudata(L, index, OPENCVVC);
    if (cap == NULL) luaL_typerror(L, index, OPENCVVC);
    return cap;
}

static Net *checkNet (lua_State *L, int index)
{
    Net *net;
    luaL_checktype(L, index, LUA_TUSERDATA);
    net = (Net *)luaL_checkudata(L, index, OPENCVNET);
    if (net == NULL) luaL_typerror(L, index, OPENCVNET);
    return net;
}

static VideoWriter *checkWriter (lua_State *L, int index)
{
    VideoWriter *vw;
    luaL_checktype(L, index, LUA_TUSERDATA);
    vw = (VideoWriter *)luaL_checkudata(L, index, OPENCVVW);
    if (vw == NULL) luaL_typerror(L, index, OPENCVVW);
    return vw;
}

//==============================================================================
//   GC
//==============================================================================

static int mat_gc(lua_State *l) {
    
    UMat* mat = checkMat(l, 1);
    
    if(mat!=NULL)
    {
        printf("UMat(%d,%d) release\n",mat->rows,mat->cols);
        mat->release();
    }
    
    return 0;
}

static int vc_gc(lua_State *l) {
    
    VideoCapture* cap = checkCap(l, 1);
    
    if(cap!=NULL)
    {
        printf("VideoCapture release\n");
        cap->release();
    }
    
    return 0;
}

static int net_gc(lua_State *l) {
    
    Net* net = checkNet(l, 1);
    
    if(net!=NULL)
    {
        printf("Net release\n");
        net->~Net();
    }
    
    return 0;
}

static int vw_gc(lua_State *l) {
    
    VideoWriter* vw = checkWriter(l, 1);
    
    if(vw!=NULL)
    {
        printf("VideoWriter release\n");
        vw->release();
    }
    
    return 0;
}

//==============================================================================
//   PUSH
//==============================================================================

/*static Mat * pushMat (lua_State *state,Mat mat)
{
    //Mat *mat = (Mat *)lua_newuserdata(L, sizeof(Mat));
    //*(Mat**)lua_newuserdata(state, sizeof(void*)) = mat;
    Mat *ret = new (lua_newuserdata(state, sizeof(Mat))) Mat(mat);
    
    if(luaL_newmetatable(state, OPENCVMAT)){
          lua_pushcfunction(state, mat_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return ret;
}*/

static UMat * pushMat (lua_State *state)
{
    //Mat *mat = (Mat *)lua_newuserdata(L, sizeof(Mat));
    UMat *ret = new (lua_newuserdata(state, sizeof(UMat))) UMat();
    
    if(luaL_newmetatable(state, OPENCVMAT)){
          lua_pushcfunction(state, mat_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return ret;
}

/*static UMat * pushMat (lua_State *state,int _rows, int _cols, int _type)
{
    UMat *ret = new (lua_newuserdata(state, sizeof(UMat))) UMat(_rows,_cols,_type);
    
    if(luaL_newmetatable(state, OPENCVMAT)){
          lua_pushcfunction(state, mat_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return ret;
}*/



static Net *pushNet (lua_State *state)
{
    Net *net = new (lua_newuserdata(state, sizeof(Net))) Net();
    
    net->setPreferableBackend(DNN_BACKEND_CUDA);
    net->setPreferableTarget(DNN_TARGET_CUDA);
    
   /* if(cv::cuda::getCudaEnabledDeviceCount()>0)
    {
        cv::cuda::printShortCudaDeviceInfo(cv::cuda::getDevice());
    }*/
    
    if(luaL_newmetatable(state, OPENCVNET)){
          lua_pushcfunction(state, net_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return net;
}

static VideoWriter *pushWriter (lua_State *state)
{
    VideoWriter *vw = new (lua_newuserdata(state, sizeof(VideoWriter))) VideoWriter();
    
    if(luaL_newmetatable(state, OPENCVVW)){
          lua_pushcfunction(state, vw_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    return vw;
}

static VideoCapture * pushCap (lua_State *state)
{
#if TARGET_OS_IPHONE
    AVCaptureSession *captureSession;
    errore per ricordarmi dove devo continuare
#else
    
    //Mat *mat = (Mat *)lua_newuserdata(L, sizeof(Mat));
    VideoCapture *ret = new (lua_newuserdata(state, sizeof(VideoCapture))) VideoCapture();
    
    if(luaL_newmetatable(state, OPENCVVC)){
          lua_pushcfunction(state, vc_gc);
          lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);
#endif
    return ret;
}

//==============================================================================
//   OPENCV
//==============================================================================

int opencv_img(lua_State *state)
{
    UMat *ret=pushMat(state);//state,100,100,CV_8UC3);
    //ret->setTo(Scalar(255,0,0));
    
    return 1;
}
int opencv_imread(lua_State *state)
{
    const char* file=luaL_checkstring(state,1);
    
    Mat tmp = imread( file, 1 );
    UMat *ret=pushMat(state);
    tmp.copyTo(*ret);
    
    return 1;
}

int opencv_imwrite(lua_State* state)
{
    const char* file=luaL_checkstring(state,1);
    UMat* frame=checkMat(state, 2);
    if (!frame->empty()) imwrite(file, *frame);
    
    return 0;
}

int opencv_imencode(lua_State* state)
{
    UMat *img=checkMat(state,1);
    
    if(img->rows+img->cols==0)
    {
        lua_pushnil(state);
        return 1;
    }
    
    vector<uchar> buf;
    imencode(".jpg",*img,buf);
    
    char *b64 =(char*)malloc(1+Base64encode_len(buf.size()));
    Base64encode(b64,(char*)buf.data(),buf.size());
    //const char* b64=base64encode(digest,digest_lenght);
    //printf("login HMAC B64: %s\n",b64);
    b64[Base64encode_len(buf.size())]=0;
    
    lua_pushstring(state, b64);
    free(b64);
    
    return 1;
}

int opencv_imdecode(lua_State* state)
{
    const char* img=luaL_checkstring(state, 1);
    UMat *out=checkMat(state,2);
    
    char *tmp=(char *)malloc(Base64decode_len(img));
    int len = Base64decode(tmp, img);
    
    //printf("imdecode len=%d\n",len);
    if(len==0) printf("imdecode error: decode len is 0");
    
    vector<uchar> buf(tmp,tmp+len);
    
    imdecode(buf, 1).copyTo(*out);
    
    free(tmp);
    
    return 0;
}

int opencv_videowriter(lua_State* state)
{
    VideoWriter *vw=pushWriter(state);
    
    const char *filename = lua_tostring(state, 1);
    int fourcc=lua_tointeger(state, 2);
    double fps=lua_tonumber(state,3);
    int w=luaL_checkinteger(state, 4);
    int h=luaL_checkinteger(state, 5);
    
    vw->open(filename, fourcc, fps, Size(w,h));
    
    return 1;
}

int opencv_videocapture(lua_State* state)
{
    VideoCapture *cap=pushCap(state);

    if (lua_isnumber(state, 1)==1)
    {
        double value = lua_tonumber(state, 1);
        if(cap->open(value,0))
        {
            printf("VideoCapture open \n");
        }
        else
        {
            printf("VideoCapture open error\n");
        }
        
/*cap->set(CAP_PROP_FRAME_WIDTH, 640);
cap->set(CAP_PROP_FRAME_HEIGHT, 480);
cap->set(CAP_PROP_BUFFERSIZE, 3); */
    }
    else
    {
        const char *value = lua_tostring(state, 1);
        if(cap->open(value,0))
        {
            printf("VideoCapture open \n");
        }
        else
        {
            printf("VideoCapture open error\n");
        }
    }
    
    return 1;
}

int opencv_getFrameSize(lua_State* state)
{
	VideoCapture *cap=checkCap(state,1);
	
	return 1;
}

int opencv_setFrameSize(lua_State* state)
{
	VideoCapture *cap=checkCap(state,1);
	int w=luaL_checkinteger(state, 2);
    int h=luaL_checkinteger(state, 3);
    
    cap->set(CAP_PROP_FRAME_WIDTH, w);
	cap->set(CAP_PROP_FRAME_HEIGHT, h);
	
	return 0;
}

int opencv_setBufferSize(lua_State* state)
{
	VideoCapture *cap=checkCap(state,1);
	int s=luaL_checkinteger(state, 2);
    
    cap->set(CAP_PROP_BUFFERSIZE, s);
	
	return 0;
}

int opencv_write(lua_State* state)
{
    //UMat *ret=pushMat(state);
    VideoWriter *vw=checkWriter(state,1);
    UMat *ret=checkMat(state,2);
    if(vw!=NULL) vw->write(*ret);
    // check if we succeeded
    int success=1;
    if (ret->empty()) {
        success=0;
    }
    lua_pushinteger(state, success);
    return 1;
}

int opencv_frame(lua_State* state)
{
    //UMat *ret=pushMat(state);
    VideoCapture *cap=checkCap(state,1);
    UMat *ret=checkMat(state,2);
    if(cap!=NULL) cap->read(*ret);
    
    int success=1;
    if (ret->empty()) {
        success=0;
    }
    lua_pushinteger(state, success);
    return 1;
}

int opencv_setImg(lua_State* state)
{
    UMat *ret=checkMat(state,1);
    int rows=luaL_checkinteger(state, 2);
    int cols=luaL_checkinteger(state, 3);
    const char* color=luaL_checkstring(state,4);
    int type=luaL_optinteger(state, 5, -1);
    
    char col[8];
    
    if (color[0] == '#')
    {
        strcpy(col,color);
        col[0]='0';
    }
    
    unsigned long value = strtoul(col, NULL, 16);
    
    if(type==-1)
        ret->create(rows, cols, CV_8UC3);
    else
        ret->create(rows, cols, type);
    ret->setTo(Scalar((value >> 0) & 0xff,(value >> 8) & 0xff,(value >> 16) & 0xff));
    
    return 0;
}

int opencv_resize(lua_State* state)
{
    UMat *in=checkMat(state,1);
    UMat *out=checkMat(state,2);
    int rows=luaL_optinteger(state, 3, -1);
    int cols=luaL_optinteger(state, 4, -1);

    if(in->empty()) return 0;
    
    if(rows==-1 || cols==-1)
        resize(*in, *out,out->size());
    else
        resize(*in, *out,Size(rows,cols));
    
    return 0;
}

int opencv_setSize(lua_State* state)
{
    UMat *ret=checkMat(state,1);
    int rows=luaL_checkinteger(state, 2);
    int cols=luaL_checkinteger(state, 3);
    int type=luaL_optinteger(state, 5, -1);
    
    if(type==-1)
        ret->create(rows, cols, CV_8UC3);
    else
        ret->create(rows, cols, type);
     
    return 0;
}

int opencv_fill(lua_State* state)
{
    UMat *ret=checkMat(state,1);
    const char* color=luaL_checkstring(state,2);
    
    char col[8];
    
    if (color[0] == '#')
    {
        strcpy(col,color);
        col[0]='0';
    }
    
    unsigned long value = strtoul(col, NULL, 16);

    ret->setTo(Scalar((value >> 0) & 0xff,(value >> 8) & 0xff,(value >> 16) & 0xff));
    
    return 0;
}

int opencv_mul(lua_State* state)
{
    UMat *in=checkMat(state,1);
    if(lua_isnumber(state, 2))
    {
        float num=luaL_checknumber(state, 2);
        UMat *out=checkMat(state,3);
        //*ret=ret->mul(num);
        
        cv::multiply(*in, num, *out);
    }
    
    return 0;
}

int opencv_add(lua_State* state)
{
    UMat *in=checkMat(state,1);
    UMat *add=checkMat(state,2);
    UMat *out=checkMat(state,3);
    
    if(in->size!=add->size) return 0;
    
    cv::add(*in, *add, *out);
    
    return 0;
}

int opencv_addWeighted(lua_State* state)
{
    UMat *in=checkMat(state,1);
    double alpha=luaL_checknumber(state, 2);
    UMat *add=checkMat(state,3);
    double beta=luaL_checknumber(state, 4);
    UMat *out=checkMat(state,5);
    double gamma=luaL_checknumber(state, 6);

    if(in->size!=add->size) return 0;
    
    cv::addWeighted(*in, alpha, *add, beta, gamma, *out);
    
    return 0;
}

int opencv_flip(lua_State* state)
{
    UMat *in=checkMat(state,1);
    UMat *out=checkMat(state,2);
    int mode=luaL_checkinteger(state, 3);
    
    cv::flip(*in, *out, mode);
    
    return 0;
}

int opencv_threshold(lua_State* state)
{
    UMat *in=checkMat(state,1);
    UMat *out=checkMat(state,2);
    double thresold=luaL_checknumber(state, 3);
    double maxval=luaL_checknumber(state, 4);
    int type=luaL_checkinteger(state, 5);
    
    cv::threshold(*in, *out, thresold, maxval, type);
    
    return 0;
}

int opencv_print(lua_State* state)
{
    UMat *ret=checkMat(state,1);
    Mat mat=ret->getMat(ACCESS_READ);
    for (int r = 0; r < mat.rows; r++) {
        for (int c = 0; c < mat.cols; c++) {

            switch (mat.depth())
            {
            case CV_8U:
            {
                printf("%*u ", 3, mat.at<uchar>(r, c));
                break;
            }
            case CV_8S:
            {
                printf("%*hhd ", 4, mat.at<schar>(r, c));
                break;
            }
            case CV_16U:
            {
                printf("%*hu ", 5, mat.at<ushort>(r, c));
                break;
            }
            case CV_16S:
            {
                printf("%*hd ", 6, mat.at<short>(r, c));
                break;
            }
            case CV_32S:
            {
                printf("%*d ", 6, mat.at<int>(r, c));
                break;
            }
            case CV_32F:
            {
                printf("%*.4f ", 10, mat.at<float>(r, c));
                break;
            }
            case CV_64F:
            {
                printf("%*.4f ", 10, mat.at<double>(r, c));
                break;
            }
            }
        } printf("\n");
    } printf("\n");
    
    return 0;
    
}

int opencv_readnet(lua_State* state)
{
    const char* model=luaL_checkstring(state,1);
    const char* config=luaL_checkstring(state,2);
    Net* net=pushNet(state);
    *net = readNet(model, config);
    //readNet
    //Net net = readNetFromCaffe("pose/mpi/pose_deploy_linevec_faster_4_stages.prototxt", "pose/mpi/pose_iter_160000.caffemodel");
    
    return 1;
}

int opencv_forward(lua_State* state)
{
    Net* net=checkNet(state,1);
    UMat* img=checkMat(state,2);
    float scale=luaL_checknumber(state, 3);
    UMat* ret=checkMat(state,4);
    
    //bool cudaEnabled=cv::cuda::getCudaEnabledDeviceCount()>0;
    
    if(img!=NULL && img->dims==2)
    {
        int inWidth = img->cols*scale;// 368;
        int inHeight = img->rows*scale;//368;
    
        UMat inpBlob = blobFromImage(img->getMat(ACCESS_READ), 1.0 / 255, Size(inWidth, inHeight), Scalar(0, 0, 0), false, false).getUMat(ACCESS_READ);

        /*GpuMat test;
         Mat test2 = blobFromImage(img, 1.0 / 255, Size(inWidth, inHeight), Scalar(0, 0, 0), false, false);*/
        
        /*if(cudaEnabled)
        {
            cv::cuda::GpuMat inGpu;
            inGpu.upload(inpBlob);
            net->setInput(inGpu);
        }
        else*/
            net->setInput(inpBlob);

        //std::vector<std::string> v = { "xyzzy", "plugh", "abracadabra" };
        //std::vector<std::vector<Mat>> out;
        Mat output = net->forward();
    
        //UMat *ret=pushMat(state);
        output.copyTo(*ret);
    }
    
    
    return 0;
}

int opencv_forwardTable(lua_State* state)
{
    Net* net=checkNet(state,1);
    UMat* img=checkMat(state,2);
    float scale=luaL_checknumber(state, 3);
    
   // bool cudaEnabled=cv::cuda::getCudaEnabledDeviceCount()>0;
    
    if(img!=NULL && img->dims==2)
    {
        int inWidth = img->cols*scale;// 368;
        int inHeight = img->rows*scale;//368;
    
        UMat inpBlob = blobFromImage(img->getMat(ACCESS_READ), 1.0 / 255, Size(inWidth, inHeight), Scalar(0, 0, 0), false, false).getUMat(ACCESS_READ);
    
        /*if(cudaEnabled)
        {
            cv::cuda::printShortCudaDeviceInfo(cv::cuda::getDevice());
            
            cv::cuda::GpuMat inGpu;
            inGpu.upload(inpBlob);
            net->setInput(inGpu);
        }
        else*/
            net->setInput(inpBlob);
        
    }else return 0;
    
    std::vector<String> out_names;
    std::vector<Mat> out_mats;
    std::vector<UMat*> ret_mats;
    
    lua_pushvalue(state, 4);
    lua_pushnil(state);
    while (lua_next(state, -2))
    {
        lua_pushvalue(state, -2);
        const char *out_name = lua_tostring(state, -1);
        UMat* ret_mat=checkMat(state,-2);

        //printf("%s\n",out_name);
        out_names.push_back(out_name);
        out_mats.push_back(Mat());//*out_mat);
        ret_mats.push_back(ret_mat);
        
        lua_pop(state, 2);
    }
    lua_pop(state, 1);
    
    net->forward(out_mats,out_names);
    
    for(std::vector<int>::size_type i = 0; i != out_mats.size(); i++) {

        out_mats[i].copyTo(*ret_mats[i]);
        //out_mats[i].download(*ret_mats[i]);
    }
    /*if(img!=NULL && img->dims==2)
    {
        int inWidth = img->cols*scale;// 368;
        int inHeight = img->rows*scale;//368;
    
        UMat inpBlob = blobFromImage(img->getMat(ACCESS_READ), 1.0 / 255, Size(inWidth, inHeight), Scalar(0, 0, 0), false, false).getUMat(ACCESS_READ);
    
        net->setInput(inpBlob);

        //std::vector<std::string> v = { "xyzzy", "plugh", "abracadabra" };
        //std::vector<std::vector<Mat>> out;
        Mat output = net->forward(out_name);
    
        //UMat *ret=pushMat(state);
        output.copyTo(*ret);
    }*/
    
    
    return 0;
}

int opencv_sliceImg(lua_State* state)
{
    UMat* img=checkMat(state,1);
    UMat* out=checkMat(state,2);
    int row=luaL_checkinteger(state, 3);
    int col=luaL_checkinteger(state, 4);
    Mat in=img->getMat(ACCESS_READ);
    
    int H = img->size[2];
    int W = img->size[3];
    
    Mat s(H, W, CV_32F, in.ptr(row,col));
    s.copyTo(*out);
    
    return 0;
}

int opencv_cropImg(lua_State* state)
{
    UMat* img=checkMat(state,1);
    UMat* out=checkMat(state,2);
    if(img!=NULL)
    {
        Mat in=img->getMat(ACCESS_READ);
    
        Rect roi;
        roi.x=luaL_checkinteger(state, 3);
        roi.y=luaL_checkinteger(state, 4);
        roi.width=luaL_checkinteger(state, 5);
        roi.height=luaL_checkinteger(state, 6);
    
        Mat crop = in(roi);
        crop.copyTo(*out);
    }
    return 0;
}

int opencv_minMaxLoc(lua_State* state)
{
    UMat* img=checkMat(state,1);
    
    Point2f p(-1,-1);
    
    Point maxLoc;
    double max;
    Point minLoc;
    double min;

    minMaxLoc(img->getMat(ACCESS_READ),&min, &max, &minLoc, &maxLoc);
    
    p=maxLoc;
    
    lua_newtable(state);
    lua_pushstring(state, "max");
    lua_newtable(state);
    
    lua_pushnumber(state, p.x);
    lua_setfield(state, -2, "x");
    
    lua_pushnumber(state, p.y);
    lua_setfield(state, -2, "y");
    
    lua_pushnumber(state, max);
    lua_setfield(state, -2, "v");
    
    lua_settable(state,-3);
    
    return 1;
}
/*int opencv_getpoints(lua_State* state)
{
    UMat* img=checkMat(state,1);
    UMat* out=checkMat(state,2);
    int nPoints = luaL_checkint(state, 3);
    
    //Mat frameCopy=img->getMat(ACCESS_READ);
    Mat output=out->getMat(ACCESS_READ);
    
    //int nPoints = 15;
    float thresh = 0.1;
    int frameWidth = img->cols;
    int frameHeight = img->rows;
    
    int H = output.size[2];
    int W = output.size[3];

   // printf("fH: %d fW: %d \n",frameHeight,frameWidth);
    // find the position of the body parts
    //vector<Point> points(nPoints);
    lua_newtable(state);
    for (int n=0; n < nPoints; n++)
    {
        // Probability map of corresponding body's part.
        Mat probMap(H, W, CV_32F, output.ptr(0,n));

        Point2f p(-1,-1);
        Point maxLoc;
        double prob;
        minMaxLoc(probMap, 0, &prob, 0, &maxLoc);
        if (prob > thresh)
        {
            p = maxLoc;
            p.x *= (float)frameWidth / W ;
            p.y *= (float)frameHeight / H ;

            //circle(frameCopy, cv::Point((int)p.x, (int)p.y), 8, Scalar(0,255,255), -1);
            //cv::putText(frameCopy, cv::format("%d", n), cv::Point((int)p.x, (int)p.y), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255), 2);

        }
        lua_pushinteger(state, n+1);
        lua_newtable(state);
        
        lua_pushnumber(state, p.x);
        lua_setfield(state, -2, "x");
        
        lua_pushnumber(state, p.y);
        lua_setfield(state, -2, "y");
        
        lua_settable(state,-3);
        //points[n] = p;
    }
    
    //UMat *ret=pushMat(state);
    //frameCopy.copyTo(*ret);
    
    return 1;
}*/

int opencv_putText(lua_State* state)
{
    
    return 0;
}

int opencv_line(lua_State* state)
{
    UMat* img=checkMat(state,1);
    int ax = luaL_checkint(state, 2);
    int ay = luaL_checkint(state, 3);
    int bx = luaL_checkint(state, 4);
    int by = luaL_checkint(state, 5);
    int w = luaL_checkint(state, 6);
    const char* color=luaL_checkstring(state,7);
    
    char col[8];
    
    if (color[0] == '#')
    {
        strcpy(col,color);
        col[0]='0';
    }
    
    unsigned long value = strtoul(col, NULL, 16);
    
    line(*img, cv::Point((int)ax, (int)ay), cv::Point((int)bx, (int)by), Scalar((value >> 0) & 0xff,(value >> 8) & 0xff,(value >> 16) & 0xff), w);
    return 0;
}

int opencv_circle(lua_State* state)
{
    UMat* img=checkMat(state,1);
    int x = luaL_checkint(state, 2);
    int y = luaL_checkint(state, 3);
    int r = luaL_checkint(state, 4);
    const char* color=luaL_checkstring(state,5);
    
    char col[8];
    
    if (color[0] == '#')
    {
        strcpy(col,color);
        col[0]='0';
    }
    
    unsigned long value = strtoul(col, NULL, 16);

    /*col.a = 255;//(value >> 24) & 0xff;
    col.r = (value >> 16) & 0xff;
    col.g = (value >> 8) & 0xff;
    col.b = (value >> 0) & 0xff;*/
    
    circle(*img, cv::Point((int)x, (int)y), r, Scalar((value >> 0) & 0xff,(value >> 8) & 0xff,(value >> 16) & 0xff), -1);
    
    return 0;
}

const char* type2str(int type) {
  const char* r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  //uchar chans = 1 + (type >> CV_CN_SHIFT);

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

  return r;
}

int opencv_infoImg(lua_State* state)
{
    UMat * mat = (UMat *)luaL_checkudata(state, 1, OPENCVMAT);
    if(mat!=NULL)
    {
        printf("MAT %s (",type2str(mat->type()));
    
        for(int i=0;i<mat->dims;i++)
        {
            printf("%dx",mat->size[i]);
        }
        printf("%d)\n",mat->dims);
    }

    return 0;
}
int opencv_infoNet(lua_State* state)
{
     Net * net = (Net *)luaL_checkudata(state, 1, OPENCVNET);
    if(net!=NULL)
    {
        printf("NET:\n");
        printf("\tinput layer:\n");
        /*for(auto layer : net->get) {
            printf("\t\t%s\n",layer.c_str());
        }*/
        for(auto layer : net->getLayerNames()) {
            printf("\t%s\n",layer.c_str());
        }
        printf("\toutput layer:\n");
        for(auto layer : net->getUnconnectedOutLayersNames()) {
            printf("\t\t%s\n",layer.c_str());
        }
    }
    
    return 0;
}

int opencv_size(lua_State* state)
{
    UMat * mat = (UMat *)luaL_checkudata(state, 1, OPENCVMAT);
    if(mat!=NULL)
    {
        lua_newtable(state);
        for(int i=0;i<mat->dims;i++)
        {
            lua_pushinteger(state, i+1);
            lua_pushnumber(state, mat->size[i]);
            lua_settable(state,-3);
        }
    }
    return 1;
}

OCVImage getImage(void* mat)
{
    //IplImage  img = (IplImage)(*(UMat*)mat).getMat(ACCESS_READ);
    Mat img=((UMat*)mat)->getMat(ACCESS_READ);
    
    OCVImage ret;
    if(img.depth()==CV_8U)
        ret.depth=8;
    ret.size=img.total()*img.elemSize();
    ret.data=img.data;//imageData;
    ret.height=img.size().height;//height;
    ret.channels=img.channels();//nChannels;
    ret.width=img.size().width;//width;
    ret.step=img.step;//img.widthStep;
    
    return ret;
}

/*
 void fillTexture(SDL_Texture * texture, cv::Mat const &mat)
 {
     IplImage * img = &(IplImage)mat;

     unsigned char * texture_data = NULL;
     int texture_pitch = 0;

     SDL_LockTexture(texture, 0, (void **)&texture_data, &texture_pitch);
     memcpy(texture_data, (void *)img->imageData, img->width * img->height * img->nChannels);
     SDL_UnlockTexture(texture);
 }
 */

/*
 SDL_Surface *ipl_to_surface (IplImage *opencvimg)
 {
     int pitch = opencvimg->nChannels*opencvimg->width;
     printf("Depth %d, nChannels %d, pitch %d\n", opencvimg->depth,
                     opencvimg->nChannels, pitch);
     SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)opencvimg->imageData,
     opencvimg->width,
     opencvimg->height,
     opencvimg->depth*opencvimg->nChannels,
     opencvimg->widthStep,
     0xff0000, 0x00ff00, 0x0000ff, 0 ); //single channel: 0xffffff, 0xffffff, 0xffffff,0);
 
     return surface;
 }
 */
void imread_c(const char* path)
{
	const int POSE_PAIRS[14][2] = 
	{   
    {0,1}, {1,2}, {2,3},
    {3,4}, {1,5}, {5,6},
    {6,7}, {1,14}, {14,8}, {8,9},
    {9,10}, {14,11}, {11,12}, {12,13}
	};

	Mat frame;
    frame = imread( path, 1 );
    Mat frameCopy = frame.clone();
    int frameWidth = frame.cols;
    int frameHeight = frame.rows;

	int nPoints = 15;
	float thresh = 0.1;

    int inWidth = 368;
    int inHeight = 368;

    if ( !frame.data )
    {
        printf("No image data \n");
    }
    else
    {
    	Net net = readNetFromCaffe("pose/mpi/pose_deploy_linevec_faster_4_stages.prototxt", "pose/mpi/pose_iter_160000.caffemodel");

    	Mat inpBlob = blobFromImage(frame, 1.0 / 255, Size(inWidth, inHeight), Scalar(0, 0, 0), false, false);

    	net.setInput(inpBlob);

    	Mat output = net.forward();

    	int H = output.size[2];
    	int W = output.size[3];

    	// find the position of the body parts
    	vector<Point> points(nPoints);
    	for (int n=0; n < nPoints; n++)
   		{
        	// Probability map of corresponding body's part.
        	Mat probMap(H, W, CV_32F, output.ptr(0,n));

        	Point2f p(-1,-1);
        	Point maxLoc;
        	double prob;
        	minMaxLoc(probMap, 0, &prob, 0, &maxLoc);
        	if (prob > thresh)
        	{
            	p = maxLoc;
            	p.x *= (float)frameWidth / W ;
            	p.y *= (float)frameHeight / H ;

            	circle(frameCopy, cv::Point((int)p.x, (int)p.y), 8, Scalar(0,255,255), -1);
            	cv::putText(frameCopy, cv::format("%d", n), cv::Point((int)p.x, (int)p.y), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255), 2);

        	}
        	points[n] = p;
    	}

    	int nPairs = sizeof(POSE_PAIRS)/sizeof(POSE_PAIRS[0]);

    	for (int n = 0; n < nPairs; n++)
    	{
        	// lookup 2 connected body/hand parts
        	Point2f partA = points[POSE_PAIRS[n][0]];
        	Point2f partB = points[POSE_PAIRS[n][1]];

        	if (partA.x<=0 || partA.y<=0 || partB.x<=0 || partB.y<=0)
            	continue;

        	line(frame, partA, partB, Scalar(0,255,255), 8);
        	circle(frame, partA, 8, Scalar(0,0,255), -1);
        	circle(frame, partB, 8, Scalar(0,0,255), -1);
   		}

   		imwrite("Output-Skeleton.jpg", frame);

    }
}


//#ifdef __cplusplus
}
//#endif

