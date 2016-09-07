#pragma once


class OutputCapture
{
public:
        void setup(Ogre::Camera * camera, Ogre::RenderWindow *window, bool color, bool depth);
        void capture();
protected:
        void captureColor();
        void captureDepth();
        Ogre::RenderWindow *window = 0;
        int mode = 0;
        MultiRenderTarget * mrt;

};

// NOTE: Ogre is NOT efficient in getting BOTH color AND depth
void OutputCapture::setup(Ogre::Camera * camera, Ogre::RenderWindow *window, bool color, bool depth)
{
        this->window = window;
        mode = (color ? 1:0)+(depth?2:0);
        if(mode == 3)
        {
                // MultiRenderTarget* mrt = 
                //                        Root::getSingleton().getRenderSystem()->createMultiRenderTarget(MRTbaseName);
                /*
                tex = TextureManager::getSingleton().createManual(
                                                        texname, 
                                                        ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
                                                        (uint)def->width, (uint)def->height, 0, *p, TU_RENDERTARGET, 0, 
                                                        def->hwGammaWrite && !PixelUtil::isFloatingPoint(*p), def->fsaa); 
                                                                                                RenderTexture* rt = tex->getBuffer()->getRenderTarget();
                                        rt->setAutoUpdated(false);
                                        mrt->bindSurface(atch, rt);

                                rendTarget->setDepthBufferPool( def->depthBufferId );

                */
        }
        else if(color)
        {
                  Ogre::TexturePtr Texture = Ogre::TextureManager::getSingleton().createManual("rtf", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,Ogre::TEX_TYPE_2D,
                                                                                               window->getWidth(),window->getHeight(), 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET);


                  Ogre::RenderTexture* RTarget = Texture->getBuffer()->getRenderTarget();
                  /*Ogre::Viewport* Viewport =*/ RTarget->addViewport(camera);
                  RTarget->getViewport(0)->setClearEveryFrame(true);
                  RTarget->getViewport(0)->setOverlaysEnabled(false);
        }
        else if(depth)
        {
                  Ogre::TexturePtr Texture = Ogre::TextureManager::getSingleton().createManual("rtfd", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,Ogre::TEX_TYPE_2D,
                                                                                               window->getWidth(),window->getHeight(), 0, Ogre::PF_FLOAT32_R, Ogre::TU_RENDERTARGET);


                  Ogre::RenderTexture* RTarget = Texture->getBuffer()->getRenderTarget();
                  /*Ogre::Viewport* Viewport =*/ RTarget->addViewport(camera);
                  RTarget->getViewport(0)->setClearEveryFrame(true);
                  RTarget->getViewport(0)->setOverlaysEnabled(false);
        }
}

void OutputCapture::capture()
{
        switch(mode)
        {
                case 1: captureColor(); break;
                case 2: captureDepth(); break;
                case 0: return;
        }
        // MRT
}

void OutputCapture::captureColor()
{
    Ogre::TexturePtr dynTexPtr = Ogre::TextureManager::getSingleton().getByName("rtf");

    Ogre::RenderTexture* RTarget = dynTexPtr->getBuffer()->getRenderTarget();
    window->update();
    RTarget->update();
    /*
    if(I.getHeight() != window->getHeight() || I.getWidth() != window->getWidth()){
       I.resize(window->getHeight(), window->getWidth());
    }
    */
    Ogre::HardwarePixelBufferSharedPtr mPixelBuffer = dynTexPtr->getBuffer();
    mPixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    const Ogre::PixelBox& pixelBox = mPixelBuffer->getCurrentLock();
    dynTexPtr->getBuffer()->blitToMemory(pixelBox);

#if 0
    Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
#if 1 // if texture in BGRa format
    for(unsigned int i=0; i<I.getHeight(); i++){
      for(unsigned int j=0; j<I.getWidth(); j++){
        // Color Image
        I[i][j].B = *pDest++; // Blue component
        I[i][j].G = *pDest++; // Green component
        I[i][j].R = *pDest++; // Red component
        I[i][j].A = *pDest++; // Alpha component
      }
    }
#else // if texture in RGBa format which is the format of the input image
    memcpy(I.bitmap, pDest, I.getHeight()*I.getWidth()*sizeof(vpRGBa));
#endif
#endif
    // Unlock the pixel buffer
    mPixelBuffer->unlock();        
}

void OutputCapture::captureDepth()
{
    Ogre::TexturePtr dynTexPtr = Ogre::TextureManager::getSingleton().getByName("rtfd");

    Ogre::RenderTexture* RTarget = dynTexPtr->getBuffer()->getRenderTarget();
    window->update();
    RTarget->update();
    /*
    if(I.getHeight() != window->getHeight() || I.getWidth() != window->getWidth()){
       I.resize(window->getHeight(), window->getWidth());
    }
    */
    Ogre::HardwarePixelBufferSharedPtr mPixelBuffer = dynTexPtr->getBuffer();
    mPixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    const Ogre::PixelBox& pixelBox = mPixelBuffer->getCurrentLock();
    dynTexPtr->getBuffer()->blitToMemory(pixelBox);

#if 0
    Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
#if 1 // if texture in BGRa format
    for(unsigned int i=0; i<I.getHeight(); i++){
      for(unsigned int j=0; j<I.getWidth(); j++){
        // Color Image
        I[i][j].B = *pDest++; // Blue component
        I[i][j].G = *pDest++; // Green component
        I[i][j].R = *pDest++; // Red component
        I[i][j].A = *pDest++; // Alpha component
      }
    }
#else // if texture in RGBa format which is the format of the input image
    memcpy(I.bitmap, pDest, I.getHeight()*I.getWidth()*sizeof(vpRGBa));
#endif
#endif
    // Unlock the pixel buffer
    mPixelBuffer->unlock();        
}