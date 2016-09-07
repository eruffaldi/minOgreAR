/**
 Minimal AR, see also VISP vpAROgre

 1) pick named monitor fullscreen
 2) background image
 3) camera from intrinsics
 4) mesh rendered flat
 */
#include <Ogre.h> 
#include <Eigen/Dense>

using namespace Ogre; 

void setVisionCameraProjection(Ogre::Camera *camera , int w, int h, float n, float f, float px, float py, float u0, float v0)
{
      Ogre::Real f_m_n,f_p_n;
      f_m_n = (Ogre::Real)(f-n);
      f_p_n = (Ogre::Real)(f+n);
      Ogre::Matrix4 Projection
        = Ogre::Matrix4( (Ogre::Real)(2.0*px/w), 0,  (Ogre::Real)(1.0 - 2.0*(u0/w)), 0,
                 0, (Ogre::Real)(2.0*py/h), (Ogre::Real)(-1.0 + 2.0*(v0/h)),0,
                 0, 0, (Ogre::Real)(-1.0*f_p_n/f_m_n), (Ogre::Real)(-2.0*f*n/f_m_n),
                 0, 0, -1.0, 0);
      camera->setCustomProjectionMatrix(true, Projection);
}

void setVisionCameraProjection(Ogre::Camera *camera , Eigen::Vector2i size, Eigen::Vector2f nearfar, Eigen::Matrix3f K)
{
        setVisionCameraProjection(camera,size.x(),size.y(),nearfar.x(),nearfar.y(),
                K(0,0),K(1,1),K(0,2),K(1,2));
}


void setCameraPose(Ogre::Camera *camera , Eigen::Matrix4f cMw)
{
          Ogre::Matrix4 ModelView
            = Ogre::Matrix4( (Ogre::Real)cMw(0,0),  (Ogre::Real)cMw(0,1),  (Ogre::Real)cMw(0,2),  (Ogre::Real)cMw(0,3),
                 (Ogre::Real)-cMw(1,0), (Ogre::Real)-cMw(1,1), (Ogre::Real)-cMw(1,2), (Ogre::Real)-cMw(1,3),
                 (Ogre::Real)-cMw(2,0), (Ogre::Real)-cMw(2,1), (Ogre::Real)-cMw(2,2), (Ogre::Real)-cMw(2,3),
                             (Ogre::Real)0,          (Ogre::Real)0,          (Ogre::Real)0,          (Ogre::Real)1);
          camera->setCustomViewMatrix(true, ModelView);

}


class ImageBackground
{
public:
        void setup(SceneManager * scene, int w, int h);

        // get pixel buffer / update
protected:
        int mBackgroundWidth, mBackgroundHeight;
        Ogre::Rectangle2D *     mBackground;
        Ogre::SceneNode *       mBackgroundNode;

        Ogre::HardwarePixelBufferSharedPtr mPixelBuffer;        /** Pointer to the pixel buffer */
};

void ImageBackground::setup(SceneManager * sceneMgr, int w, int h)
{       
        mBackgroundWidth = w;
        mBackgroundHeight = h;
        // Create a rectangle to show the incoming images from the camera
        mBackground = new Ogre::Rectangle2D(true); // true = textured
        mBackground->setCorners(-1.0, 1.0, 1.0, -1.0); // Spread all over the window
        mBackground->setBoundingBox(Ogre::AxisAlignedBox(-100000.0*Ogre::Vector3::UNIT_SCALE, 100000.0*Ogre::Vector3::UNIT_SCALE)); // To be shown everywhere

        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_NONE);
        Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(1);

            Ogre::TextureManager::getSingleton().createManual("BackgroundTexture",
              Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
              Ogre::TEX_TYPE_2D,
              mBackgroundWidth,//width
              mBackgroundHeight,//height
              0,  // num of mip maps
                          //Ogre::PF_BYTE_RGBA,
          Ogre::PF_BYTE_BGRA,
          Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

        Ogre::TexturePtr dynTexPtr = Ogre::TextureManager::getSingleton().getByName("BackgroundTexture");
        mPixelBuffer = dynTexPtr->getBuffer();
        
        Ogre::MaterialPtr Backgroundmaterial = Ogre::MaterialManager::getSingleton().create("BackgroundMaterial",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        Ogre::Technique *Backgroundtechnique = Backgroundmaterial->createTechnique();
        Backgroundtechnique->createPass();
        Backgroundmaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        Backgroundmaterial->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false); // Background
        Backgroundmaterial->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false); // Background
        Backgroundmaterial->getTechnique(0)->getPass(0)->createTextureUnitState("BackgroundTexture");
        mBackground->setMaterial("BackgroundMaterial"); // Attach the material to the rectangle
        mBackground->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND); // To be rendered in Background

        // Add the background to the Scene Graph so it will be rendered
        mBackgroundNode = sceneMgr->getRootSceneNode()->createChildSceneNode("BackgoundNode");
        mBackgroundNode->attachObject(mBackground);
}

class MinOgre
{
public:
        MinOgre();
        ~MinOgre();

        void setCameraPose();
        void setBackground();

        Ogre::Root *root;
protected:
        void createCamera();


        RenderWindow* window;
        SceneManager* sceneMgr;
        Camera* camera;
        //OutputCapture capture;
        ImageBackground background;

};

MinOgre::~MinOgre()
{
        delete root;
}

MinOgre::MinOgre()
{
        // MULTIPLE: Ogre::Root::getSingletonPtr()
        root = new Root(); // pluginsOGRE_BUILD_SUFFIX.cfg, "ogre.cfg", "Ogre.log");

        #if defined(_DEBUG)
                root->loadPlugin("C:/OGRESDK/bin/debug/RenderSystem_GL_d.dll");
                //Linux
                //root->loadPlugin("/usr/local/lib/OGRE/RenderSystem_GL_d");  
        #else
                root->loadPlugin("C:/OGRESDK/bin/release/RenderSystem_GL.dll");
                //Linux
                //root->loadPlugin("/usr/local/lib/OGRE/RenderSystem_GL");  
        #endif



        const Ogre::RenderSystemList& rs = root->getAvailableRenderers();
        if(&rs && rs.size()&&rs.at(0)->getName().compare("RenderSystem_GL")){ 
                 RenderSystem * r=rs.at(0); 
                root->setRenderSystem(r); 
                 r->setConfigOption("Full Screen","No");  
                 r->setConfigOption("Video Mode","800 x 600 @ 16-bit colour"); 
         }
         else{ 
         exit(1); 
        }         
        ResourceGroupManager &resources=ResourceGroupManager::getSingleton(); 
        resources.addResourceLocation("data","FileSystem"); 
        resources.initialiseAllResourceGroups();         

        window = root->initialise(true, "Simple Ogre App"); 
          Ogre::Viewport* viewPort = window->addViewport(camera);
        //   Ogre::Viewport* viewPort = mCamera->getViewport();
          viewPort->setClearEveryFrame(true);

        sceneMgr = root->createSceneManager(ST_GENERIC); 
        createCamera(); 
        background.setup(sceneMgr,128,128);


        sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(camera); 
}

void MinOgre::setCameraPose()
{

}

void MinOgre::setBackground()
{
  //mPixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD); // Lock the buffer
  //const Ogre::PixelBox& pixelBox = mPixelBuffer->getCurrentLock();
  // TODO copy to buffer
  //mPixelBuffer->unlock();
}


void MinOgre::createCamera()
{ 
 Camera* cam = sceneMgr->createCamera("SimpleCamera"); 
 cam->setPosition(Vector3(0.0f,0.0f,500.0f)); 
 cam->lookAt(Vector3(0.0f,0.0f,0.0f)); 
 cam->setNearClipDistance(5.0f); 
 cam->setFarClipDistance(5000.0f); 

 Viewport* v = window->addViewport(cam); 
 v->setBackgroundColour(ColourValue(0.5,0.5,0.5)); 

 cam->setAspectRatio(Real(v->getActualWidth())/v->getActualHeight()); 

 camera = cam;
} 


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
#define WIN32_LEAN_AND_MEAN 
#include "windows.h" 
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT){ 
#else 
int main(int argc, char **argv)
{ 
#endif
        MinOgre mo;

/*
    Ogre::Light * light = ogre.getSceneManager()->createLight();
    light->setDiffuseColour(1, 1, 1); // scaled RGB values
    light->setSpecularColour(1, 1, 1); // scaled RGB values
    light->setPosition(-5, -5, 10);
    light->setType(Ogre::Light::LT_POINT);
*/
         int running=1000; 
         while(running--) 
         { 
                 WindowEventUtilities::messagePump(); 
                 mo.root->renderOneFrame(); 
                 printf("%d\n",running); 
         } 

 return 0; 
}