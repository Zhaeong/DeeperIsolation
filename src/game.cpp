#include "game.h"

int StartSDL(SDL_Window **window, SDL_Renderer **renderer)
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "Couldn't initialize SDL: %s",
                SDL_GetError());
    }

    //Init Audio
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "Couldn't initialize SDL: %s",
                SDL_GetError());
    }

    //Uint32 windowType = SDL_WINDOW_FULLSCREEN;
    Uint32 windowType = SDL_WINDOW_RESIZABLE;

    //SDL_WINDOW_FULLSCREEN
    //SDL_WINDOW_RESIZABLE
    if (SDL_CreateWindowAndRenderer(GAMEWIDTH,
                GAMEHEIGHT,
                windowType,
                window,
                renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "Couldn't create window and renderer: %s",
                SDL_GetError());
    }

    return 0;
}

SDL_Texture *GetSDLTexture(SDL_Renderer *renderer, SDL_Window *window, string textureLocation)
{
    //Make sure to initialize texture to null or else SDL_DestroyTexture will crash program
    SDL_Texture *texture = NULL;

    SDL_Surface *loadedSurface = IMG_Load(textureLocation.c_str());

    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n",
                textureLocation.c_str(),
                IMG_GetError());
    }
    else
    {
        //Convert surface to display format

        //Note have to use SDL_PIXELFORMAT_ARGB8888 due to window using SDL_PIXELFORMAT_RGB888
        //Which doesn't have alpha channel

        //SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface,
        //                                                          SDL_GetWindowPixelFormat( window ),
        //                                                          0 );

        //cout << "Loading Texture: " << textureLocation << "\n";
        SDL_Surface *formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface,
                TEXTUREFORMAT,
                0);

        if (formattedSurface == NULL)
        {
            printf("Unable to convert loaded surface to display format! SDL Error: %s\n",
                    SDL_GetError());
        }
        else
        {
            //Create blank streamable texture

            //texture = SDL_CreateTexture( renderer,
            //                             SDL_GetWindowPixelFormat( window ),
            //                             SDL_TEXTUREACCESS_STREAMING,
            //                             formattedSurface->w,
            //                             formattedSurface->h );

            texture = SDL_CreateTexture(renderer,
                    TEXTUREFORMAT,
                    SDL_TEXTUREACCESS_STREAMING,
                    formattedSurface->w,
                    formattedSurface->h);

            //texture = SDL_CreateTextureFromSurface( renderer, formattedSurface );
            if (texture == NULL)
            {
                printf("Unable to create blank texture! SDL Error: %s\n",
                        SDL_GetError());
            }
            else
            {
                void *mPixels;
                int mPitch;

                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

                //Lock texture for manipulation
                if (SDL_LockTexture(texture, NULL, &mPixels, &mPitch) != 0)
                {
                    printf("Unable to lock texture! %s\n", SDL_GetError());
                }

                //Copy loaded/formatted surface pixels
                memcpy(mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

                //Unlock texture to update
                SDL_UnlockTexture(texture);

                mPixels = NULL;

                //Get image dimensions
                //mWidth = formattedSurface->w;
                //mHeight = formattedSurface->h;
            }

            //Get rid of old formatted surface
            SDL_FreeSurface(formattedSurface);
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }
    return texture;
}

void RemoveTextureWhiteSpace(SDL_Texture *texture)
{
    void *mPixels;
    int mPitch;

    if (texture == NULL)
    {
        printf("Input Texture is null in RemoveTextureWhiteSpace! SDL Error: %s\n",
                SDL_GetError());
    }

    if (SDL_LockTexture(texture, NULL, &mPixels, &mPitch) != 0)
    {
        printf("Unable to lock texture! %s\n", SDL_GetError());
    }
    else
    {
        //Allocate format from window
        //Uint32 format = SDL_GetWindowPixelFormat( window );

        SDL_PixelFormat *mappingFormat = SDL_AllocFormat(TEXTUREFORMAT);

        int texWidth = 0;
        int texHeight = 0;

        Uint32 texFormat;
        SDL_QueryTexture(texture, &texFormat, NULL, &texWidth, &texHeight);

        //Get pixel data
        Uint32 *pixels = (Uint32 *)mPixels;
        int pixelCount = (mPitch / 4) * texHeight;

        //cout << "texformat:" << texFormat << "\n";
        //cout << "texWidth:" << texWidth << "\n";
        //cout << "texHeight:" << texHeight << "\n";
        //cout << "mPitch:" << mPitch << "\n";
        //cout << "pixelCount:" << pixelCount << "\n";

        //Map colors

        Uint32 colorKeyWhite = SDL_MapRGBA(mappingFormat, 0xFF, 0xFF, 0xFF, 0xFF);
        Uint32 colorKeyGray = SDL_MapRGBA(mappingFormat, 182, 182, 182, 0xFF);

        Uint32 transparent = SDL_MapRGBA(mappingFormat, 0xFF, 0xFF, 0xFF, 0);

        //Color key pixels
        for (int i = 0; i < pixelCount; ++i)
        {
            //cout << "col:" << pixels[i] << "\n";
            //cout << "key:" << colorKey << "\n";

            if (pixels[i] == colorKeyWhite || pixels[i] == colorKeyGray)
            {
                pixels[i] = transparent;
            }
        }

        SDL_UnlockTexture(texture);
        //mPixels = NULL;
        //Free format
        SDL_FreeFormat(mappingFormat);
    }
}

Texture InitTexture(SDL_Texture *sdlTexture, int x, int y)
{
    Texture outTex;
    outTex.mX = x;
    outTex.mY = y;
    SDL_QueryTexture(sdlTexture, NULL, NULL, &outTex.mW, &outTex.mH);
    outTex.mRotation = 0;
    outTex.mAlpha = 255;
    outTex.mTexture = sdlTexture;
    outTex.mCenter = NULL;
    outTex.mFlip = SDL_FLIP_NONE;
    return outTex;
}

SpriteSheet InitSpriteSheet(SDL_Texture *sdlTexture,
        int w,
        int h,
        int numFrames)
{
    SpriteSheet sSheet;

    sSheet.mTexture = sdlTexture;

    sSheet.mDstRect.x = 0;
    sSheet.mDstRect.y = 0;

    sSheet.mDstRect.w = w;
    sSheet.mDstRect.h = h;
    sSheet.mNumFrames = numFrames;

    sSheet.mCurFrame = 0;

    sSheet.mRotation = 0;
    sSheet.mAlpha = 255;

    sSheet.mCenter = NULL;

    sSheet.mFlip = SDL_FLIP_NONE;


    return sSheet;
}


void RenderTexture(SDL_Renderer *renderer, Texture tex)
{
    //Don't render if the alpha is 0
    if(tex.mAlpha > 0)
    {
        SDL_SetTextureBlendMode(tex.mTexture, SDL_BLENDMODE_BLEND);

        SDL_SetTextureAlphaMod(tex.mTexture, tex.mAlpha);

        SDL_Rect srcRect;
        SDL_Rect dstRect;

        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.h = tex.mH;
        srcRect.w = tex.mW;

        int xPos = tex.mX;
        int yPos = tex.mY;

        dstRect.x = xPos;
        dstRect.y = yPos; 
        dstRect.h = tex.mH;
        dstRect.w = tex.mW;

        SDL_RenderCopyEx(renderer, tex.mTexture, &srcRect, &dstRect, tex.mRotation, tex.mCenter, tex.mFlip);
    }
}



void RenderSpriteSheet(SDL_Renderer *renderer, SpriteSheet sSheet)
{
    //Don't render if the alpha is 0
    if(sSheet.mAlpha > 0)
    {
        //SDL_SetTextureBlendMode(sSheet.mTexture, SDL_BLENDMODE_BLEND);

        //SDL_SetTextureAlphaMod(sSheet.mTexture, sSheet.mAlpha);

        SDL_Rect srcRect;
        srcRect.x = sSheet.mDstRect.w * sSheet.mCurFrame;
        srcRect.y = 0;
        srcRect.w = sSheet.mDstRect.w;
        srcRect.h = sSheet.mDstRect.h;

        SDL_RenderCopyEx(renderer, sSheet.mTexture, &srcRect, &sSheet.mDstRect, sSheet.mRotation, sSheet.mCenter, sSheet.mFlip);
#ifdef DEBUG 
        if(ShowDebug)
        {

        }

#endif
    }
}

void SetTextureColor(SDL_Texture *texture, int R, int G, int B, int A)
{
    void *mPixels;
    int mPitch;

    if (texture == NULL)
    {
        printf("Input Texture is null in RemoveTextureWhiteSpace! SDL Error: %s\n",
                SDL_GetError());
    }

    if (SDL_LockTexture(texture, NULL, &mPixels, &mPitch) != 0)
    {
        printf("Unable to lock texture! %s\n", SDL_GetError());
    }
    else
    {
        SDL_PixelFormat *mappingFormat = SDL_AllocFormat(TEXTUREFORMAT);

        int texWidth = 0;
        int texHeight = 0;

        Uint32 texFormat;
        SDL_QueryTexture(texture, &texFormat, NULL, &texWidth, &texHeight);

        //Get pixel data
        Uint32 *pixels = (Uint32 *)mPixels;
        int pixelCount = (mPitch / 4) * texHeight;

        //Map colors

        Uint32 colorKey = SDL_MapRGBA(mappingFormat, R, G, B, A);

        Uint32 transparent = SDL_MapRGBA(mappingFormat, 0xFF, 0xFF, 0xFF, 0);

        //Color key pixels
        for (int i = 0; i < pixelCount; ++i)
        {

            if (pixels[i] != transparent)
            {
                pixels[i] = colorKey;
            }
        }

        SDL_UnlockTexture(texture);

        //Free format
        SDL_FreeFormat(mappingFormat);
    }
}

AudioClip InitAudio(string filepath)
{
    AudioClip clip;
    clip.wavPath = filepath;
    if (SDL_LoadWAV(clip.wavPath.c_str(), &clip.wavSpec, &clip.wavBuffer, &clip.wavLength) == NULL) 
    {
        fprintf(stderr, "Could not open wav %s: %s\n", filepath.c_str(), SDL_GetError());
    } 
    return clip;
}

void PlayAudio(SDL_AudioDeviceID audioDevice, AudioClip clip)
{
    int success = SDL_QueueAudio(audioDevice, clip.wavBuffer, clip.wavLength);
    if(success < 0)
    {
        printf("SDL_QueueAudio failed %s, err: %s", clip.wavPath.c_str(), SDL_GetError()); 
    }

}


bool TextureMouseCollisionSingle(Texture mTexture, int xPos, int yPos)
{

    if (xPos >= mTexture.mX 
            && xPos <= (mTexture.mX + mTexture.mW) 
            && yPos >= mTexture.mY
            && yPos <= (mTexture.mY + mTexture.mH) 
       )
    {
        return true;
    }

    return false;
}

SDL_Point RotatePointByOtherPoint(int inX,
        int inY,
        int centerX,
        int centerY,
        int degrees)
{
    SDL_Point retPoint;
    double rad = degrees * PI / (double)180.0;
    double cosVal = cos(rad);
    double sinVal = sin(rad);

    //Change offset points back to origin
    int startX = inX - centerX;
    int startY = inY - centerY;

    double rotateX = startX * cosVal - startY * sinVal;
    double rotateY = startX * sinVal + startY * cosVal;

    retPoint.x = rotateX + centerX; 
    retPoint.y = rotateY + centerY;

    return retPoint;
}

TextBox InitTextBox(SDL_Texture *fontTex,
        int fontW,
        int fontH,
        SDL_Texture *boxTex,
        int boxDim,
        string text,
        int x,
        int y,
        int column,
        int speed)
{
    TextBox outTextBox;
    outTextBox.mFontTex = fontTex;
    outTextBox.mFontW = fontW;
    outTextBox.mFontH = fontH;
    outTextBox.mBoxTex = boxTex;
    outTextBox.mBoxDim = boxDim;
    outTextBox.mText = text;
    outTextBox.mX = x;
    outTextBox.mY = y;
    outTextBox.mColumn = column;
    outTextBox.mSpeed = speed;
    outTextBox.mNumRendered = 0;
    outTextBox.mLastUpdate = 0;
    return outTextBox;
}

void RenderTextBox(SDL_Renderer *renderer, Uint32 curTime, TextBox *tBox)
{

    SDL_Rect srcRect;
    SDL_Rect dstRect;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = tBox->mFontW;
    srcRect.h = tBox->mFontH;

    dstRect.x = tBox->mX;
    dstRect.y = tBox->mY;
    dstRect.w = tBox->mFontW;
    dstRect.h = tBox->mFontH;

    int curXPos = 0;
    int curYPos = 0;
    int renderNum = tBox->mText.size();

    //This is to ensure that the first char isn't rendered at start
    if(tBox->mLastUpdate == 0)
    {
        tBox->mLastUpdate = curTime;
    }
    if(tBox->mSpeed > 0 && tBox->mNumRendered < tBox->mText.size())
    {
        renderNum = tBox->mNumRendered;

        if(curTime > tBox->mLastUpdate + tBox->mSpeed)
        {
            tBox->mNumRendered += 1;
            tBox->mLastUpdate = curTime;
        }
    }

    for(int i = 0; i < renderNum; i++)
    {
        char curChar = tBox->mText[i];
        int xTextPos = 0;
        int yTextPos = 0;
        //Capitals

        if ((int)curChar >= 65 && (int)curChar <= 90)
        {
            xTextPos = (int)curChar - 65;
        }
        //lower case
        else if ((int)curChar >= 97 && (int)curChar <= 122)
        {
            xTextPos = (int)curChar - 97;
            yTextPos = 1;
        }
        //numbers
        else if ((int)curChar >= 48 && (int)curChar <= 57)
        {
            xTextPos = (int)curChar - 48;
            yTextPos = 2;
        }

        srcRect.x = xTextPos * tBox->mFontW;
        srcRect.y = yTextPos * tBox->mFontH;

        dstRect.x = tBox->mX + tBox->mFontW * curXPos;
        dstRect.y = tBox->mY + tBox->mFontH * curYPos;
        SDL_RenderCopyEx(renderer, tBox->mFontTex, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);
        if(curXPos + 1 > tBox->mColumn)
        {
            curXPos = 0;
            curYPos += 1;
        }
        else
        {
            curXPos += 1;
        }
    }
}
