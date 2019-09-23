#ifndef VIDEORECEIVERRENDERER
#define VIDEORECEIVERRENDERER

#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLProgramTextureExt.h>
#include <GLProgramVC.h>
#include <android/surface_texture.h>
#include <GLProgramSpherical.h>
#include "../General/IPositionable.hpp"
#include "../General/IDrawable.hpp"
#include "../General/PositionDebug.hpp"

class VideoRenderer : public IPositionable{ //Does not inherit from IDrawable !
public:
    //Normal: Render a rectangle
    //Stereo: One decoded frame contains images for left and right eye
    //Render left frame into a rectangle with u->{0,0,5} and right frame int a rectangle with u->{0.5,1.0}
    //Degree360: 360 degree video, rendered onto a sphere instead of a quad
    //The daydream renderer handles external surfaces (like video) itself, but requires the application to
    //'punch a hole' into the scene by rendering a quad with alpha=0.0f
    enum VIDEO_RENDERING_MODE{NORMAL,STEREO,Degree360,PunchHole};
    VideoRenderer(VIDEO_RENDERING_MODE mode,const GLProgramVC& glRenderGeometry,GLProgramTextureExt *glRenderTexEx=nullptr,GLProgramSpherical *glPSpherical=nullptr);
    void initUpdateTexImageJAVA(JNIEnv * env,jobject obj,jobject surfaceTexture);
    void deleteUpdateTexImageJAVA(JNIEnv* env,jobject obj); //frees the global reference so java does not complain
    void updateTexImageJAVA(JNIEnv* env);
    void punchHole(glm::mat4x4 ViewM,glm::mat4x4 ProjM);
    void punchHole2(glm::mat4x4 ViewM,glm::mat4x4 ProjM);
    void drawVideoCanvas(glm::mat4x4 ViewM, glm::mat4x4 ProjM, bool leftEye);
    void drawVideoCanvas360(glm::mat4x4 ViewM, glm::mat4x4 ProjM);
private:
    void setupPosition() override;
    PositionDebug mPositionDebug;
    GLuint mIndexBuffer;
    GLuint mGLBuffVidLeft; //left side of the video frame (u.v coordinates)
    GLuint mGLBuffVidRight; //right side of the video frame (u.v coordinates)
    GLuint mGLBuffVid; //whole video frame (u.v coordinates)
    GLuint mGLBuffVidPunchHole;
    int nIndicesVideoCanvas;
    const GLProgramVC& mGLRenderGeometry;
    GLProgramTextureExt* mGLRenderTexEx= nullptr;
    GLProgramSpherical* mGLProgramSpherical=nullptr;

    const int TESSELATION_FACTOR=10;
    jobject localRefSurfaceTexture;
    jmethodID updateTexImageMethodId;
    jmethodID getTimestampMethodId;
    const VIDEO_RENDERING_MODE mMode;
    ASurfaceTexture* mSurfaceTexture;
};

#endif