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
        Uint32 colorKeyGray = SDL_MapRGBA(mappingFormat, 153, 153, 153, 0xFF);

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
    outTex.mActive = true;
    outTex.mX = x;
    outTex.mY = y;
    SDL_QueryTexture(sdlTexture, NULL, NULL, &outTex.mW, &outTex.mH);
    outTex.mRotation = 0;
    outTex.mAlpha = 255;
    outTex.mTexture = sdlTexture;
    outTex.mCenter = NULL;
    outTex.mFlip = SDL_FLIP_NONE;
    outTex.mLastUpdate = 0;

    outTex.mType = TTYPE_NORMAL;
    outTex.mName = "";
    outTex.mButtonText = "";
    outTex.mNarration = "";

    outTex.mInteract = true;
    SDL_Rect colOffset;
    colOffset.x = 0;
    colOffset.y = 0;
    colOffset.w = outTex.mW;
    colOffset.h = outTex.mH;

    outTex.mColBoxOffset = colOffset;

    return outTex;
}

void RenderTexture(SDL_Renderer *renderer, Texture tex)
{
    //Don't render if the alpha is 0
    if(tex.mActive)
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

#ifdef DEBUG 
        if(ShowDebug)
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(renderer, &dstRect);

            //Coloffset
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, SDL_ALPHA_OPAQUE);

            SDL_Rect offSetRect;

            offSetRect.x = dstRect.x + tex.mColBoxOffset.x;
            offSetRect.y = dstRect.y + tex.mColBoxOffset.y;
            offSetRect.w = tex.mColBoxOffset.w;
            offSetRect.h = tex.mColBoxOffset.h;
            SDL_RenderDrawRect(renderer, &offSetRect);


        }

#endif
    }
}

SpriteSheet InitSpriteSheet(SDL_Texture *sdlTexture,
        int w,
        int h,
        int numFrames)
{
    SpriteSheet sSheet;

    sSheet.mActive = true;
    sSheet.mInteract = true;
    sSheet.mUpdate = true;

    sSheet.mTexture = sdlTexture;

    sSheet.mX = 0;
    sSheet.mY = 0;
    sSheet.mDstRect.x = 0;
    sSheet.mDstRect.y = 0;

    //    sSheet.mXOffset = 0;
    //    sSheet.mWoffset = 0;

    SDL_Rect colOffset;
    colOffset.x = 0;
    colOffset.y = 0;
    colOffset.w = w;
    colOffset.h = h;

    sSheet.mDstRect.w = w;
    sSheet.mDstRect.h = h;

    //if numFrames == 1, then the w and h are from texture
    if(numFrames == 1)
    {
        int textureW, textureH;
        SDL_QueryTexture(sdlTexture, NULL, NULL, &textureW, &textureH);

        sSheet.mDstRect.w = textureW;
        sSheet.mDstRect.h = textureH;

        colOffset.w = textureW;
        colOffset.h = textureH;
    }


    sSheet.mColBoxOffset = colOffset;
    sSheet.mNumFrames = numFrames;

    sSheet.mType = TTYPE_NORMAL;
    sSheet.mButtonText = "";
    sSheet.mNarration = "";
    sSheet.mName = "";

    sSheet.mHideNarration = false;


    sSheet.mLastUpdate = 0;
    sSheet.mUpdateInterval = 1000;
    sSheet.mCurFrame = 0;

    sSheet.mRotation = 0;
    sSheet.mAlpha = 255;

    sSheet.mCenter = NULL;

    sSheet.mFlip = SDL_FLIP_NONE;




    return sSheet;
}

void UpdateSpriteSheet(SpriteSheet *ssheet, Uint32 curTime)
{
    if(ssheet->mActive && ssheet->mUpdate)
    {
        if(curTime > ssheet->mLastUpdate + ssheet->mUpdateInterval)
        {
            ssheet->mCurFrame = (ssheet->mCurFrame + 1) % ssheet->mNumFrames;
            ssheet->mLastUpdate = curTime;
        }
    }
}

void RenderSpriteSheet(SDL_Renderer *renderer, SpriteSheet sSheet)
{
    //Don't render if the alpha is 0
    if(sSheet.mActive)
    {
        //SDL_SetTextureBlendMode(sSheet.mTexture, SDL_BLENDMODE_BLEND);

        //SDL_SetTextureAlphaMod(sSheet.mTexture, sSheet.mAlpha);

        SDL_Rect srcRect;
        srcRect.x = sSheet.mDstRect.w * sSheet.mCurFrame;
        srcRect.y = 0;
        srcRect.w = sSheet.mDstRect.w;
        srcRect.h = sSheet.mDstRect.h;

        sSheet.mDstRect.x = sSheet.mX;
        sSheet.mDstRect.y = sSheet.mY;

        SDL_RenderCopyEx(renderer, sSheet.mTexture, &srcRect, &sSheet.mDstRect, sSheet.mRotation, sSheet.mCenter, sSheet.mFlip);
#ifdef DEBUG 
        if(ShowDebug)
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(renderer, &sSheet.mDstRect);

            //Coloffset
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, SDL_ALPHA_OPAQUE);

            SDL_Rect offSetRect;

            offSetRect.x = sSheet.mDstRect.x + sSheet.mColBoxOffset.x;
            offSetRect.y = sSheet.mDstRect.y + sSheet.mColBoxOffset.y;
            offSetRect.w = sSheet.mColBoxOffset.w;
            offSetRect.h = sSheet.mColBoxOffset.h;
            SDL_RenderDrawRect(renderer, &offSetRect);
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
    clip.mActive = true;
    clip.wavPath = filepath;
    if (SDL_LoadWAV(clip.wavPath.c_str(), &clip.wavSpec, &clip.wavBuffer, &clip.wavLength) == NULL) 
    {
        fprintf(stderr, "Could not open wav %s: %s\n", filepath.c_str(), SDL_GetError());
    } 
    return clip;
}

void PlayAudio(GameState *GS, AudioClip clip)
{

    //using malloc and memcpy because we don't want to touch the 
    //original audio
    Uint8 *clipBuffer = (Uint8 *) malloc(clip.wavLength);
    memcpy(clipBuffer, clip.wavBuffer, clip.wavLength);

    int success = -1;

    if(ShowDebug)
    {
        cout << "playerAudio: " << clip.wavPath << "\n";
    }
    //Layering sounds 
    if(GS->curSound.mActive)
    {
        //Buffer which stores the remaining data in queue 
        Uint8 *wavBuffer;

        //need to account for currently played audio 
        Uint32 sizeCurAudio = SDL_GetQueuedAudioSize(GS->audioDevice);

        wavBuffer = (Uint8 *)malloc(sizeCurAudio);

        //printf("ori:  %p\n", GS->curSound.wavBuffer);

        //This new pointer is the offsetted pointer location of the current sound that's already been played
        //We can simply clear queue and queue this pointer and it will seem like nothing happened and 
        //The track will play still as normal, but we want to mix the remaining portion with the new audio
        Uint8 *curAudioOffset = GS->curSound.wavBuffer + (GS->curSound.wavLength - sizeCurAudio);

        //printf("new:  %p\n", curAudioOffset);

        memcpy(wavBuffer, curAudioOffset, sizeCurAudio);

        //Two conditions, if the new audio that wants to be played is shorter than the remaining audio left 
        //to be played in queue, then mix the new audio with the remaining audio in queue
        if(sizeCurAudio > clip.wavLength)
        {
            SDL_MixAudioFormat(wavBuffer, clipBuffer, AUDIO_FORMAT, clip.wavLength, SDL_MIX_MAXVOLUME);

            SDL_ClearQueuedAudio(GS->audioDevice);
            success = SDL_QueueAudio(GS->audioDevice, wavBuffer, sizeCurAudio);
        }
        //else if the new audio is greater in length than the remaining audio in queue
        //then the new audio becomes mainaudio and mix the remaining audio with the new audio
        else
        {
            //there
            GS->curSound = clip;
            SDL_MixAudioFormat(clipBuffer, wavBuffer, AUDIO_FORMAT, sizeCurAudio, SDL_MIX_MAXVOLUME);
            SDL_ClearQueuedAudio(GS->audioDevice);
            success = SDL_QueueAudio(GS->audioDevice, clipBuffer, clip.wavLength);
        }

        free(wavBuffer);


    }
    else
    {
        GS->curSound = clip;
        success = SDL_QueueAudio(GS->audioDevice, clipBuffer, clip.wavLength);
    }

    free(clipBuffer);
    if(success < 0)
    {
        printf("SDL_QueueAudio failed %s, err: %s", clip.wavPath.c_str(), SDL_GetError()); 
    }

}

void PlayAudioFromPath(GameState *GS, string clipPath)
{
    for(int i = 0; i < NUM_AUDIO_CLIPS; i++)
    {
        if(clipPath == GS->acArray[i].wavPath)
        {
            PlayAudio(GS, GS->acArray[i]);
        }
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
        string text,
        int x,
        int y,
        int column,
        int speed)
{
    TextBox outTextBox;

    outTextBox.mActive = true;
    outTextBox.mType = TBTYPE_NORM;
    outTextBox.mFontTex = fontTex;
    outTextBox.mFontW = fontW;
    outTextBox.mFontH = fontH;

    outTextBox.mBoxTex = boxTex;

    int boxW, boxH;
    SDL_QueryTexture(boxTex, NULL, NULL, &boxW, &boxH);
    outTextBox.mBoxDim = boxW/3;
    outTextBox.mBoxMargin = 2;
    outTextBox.mState = 0;

    outTextBox.mText = text;
    outTextBox.mX = x;
    outTextBox.mY = y;

    outTextBox.mColumn = column;

    //The addition of 1 or 0 will make this always take the ceiling of a division
    //e.g. 10/5 = 2, and 11/2 = 3
    outTextBox.mRow = text.size()/column + (text.size() % column != 0);

    outTextBox.mSpeed = speed;

    outTextBox.mDuration = 0;
    outTextBox.mStartTime = 0;

    outTextBox.mNumRendered = 0;
    outTextBox.mLastUpdate = 0;

    return outTextBox;
}



void RenderTextBox(SDL_Renderer *renderer, Uint32 curTime, TextBox *tBox)
{

    if(!tBox->mActive)
    {
        return;
    }

    if(tBox->mStartTime == 0)
    {
        tBox->mStartTime = curTime;
    }

    if(tBox->mDuration != 0 && curTime - tBox->mStartTime > tBox->mDuration)
    {
        tBox->mActive = false;
        return;
    }
    SDL_Rect srcRect;
    SDL_Rect dstRect;

    //////////////////////////////
    //First render the background
    //////////////////////////////

    //9 points represents the start locations
    //This indicates the srcRect where it will render from

    SDL_Point tl, t, tr,
              ml, m, mr,
              bl, b, br;

    tl.x = 0;
    tl.y = 0;

    t.x = tBox->mBoxDim * 1;
    t.y = 0;

    tr.x = tBox->mBoxDim * 2;
    tr.y = 0;

    ml.x = 0; 
    ml.y = tBox->mBoxDim * 1;

    m.x = tBox->mBoxDim * 1;
    m.y = tBox->mBoxDim * 1;

    mr.x = tBox->mBoxDim * 2;
    mr.y = tBox->mBoxDim * 1;

    bl.x = 0;
    bl.y = tBox->mBoxDim * 2;

    b.x = tBox->mBoxDim * 1;
    b.y = tBox->mBoxDim * 2;

    br.x = tBox->mBoxDim * 2;
    br.y = tBox->mBoxDim * 2;

    //When the button is active offset by 3 * mBoxDim in y axis

    tl.y += tBox->mBoxDim * (3 * tBox->mState);
    t.y  += tBox->mBoxDim * (3 * tBox->mState);
    tr.y += tBox->mBoxDim * (3 * tBox->mState);
    ml.y += tBox->mBoxDim * (3 * tBox->mState);
    m.y  += tBox->mBoxDim * (3 * tBox->mState);
    mr.y += tBox->mBoxDim * (3 * tBox->mState);
    bl.y += tBox->mBoxDim * (3 * tBox->mState);
    b.y  += tBox->mBoxDim * (3 * tBox->mState);
    br.y += tBox->mBoxDim * (3 * tBox->mState);


    //Render mid first, and others can be imposed upon
    //m
    srcRect.x = m.x;
    srcRect.y = m.y;
    srcRect.w = tBox->mBoxDim;
    srcRect.h = tBox->mBoxDim;

    dstRect.x = tBox->mX - tBox->mBoxMargin;
    dstRect.y = tBox->mY - tBox->mBoxMargin;
    dstRect.w = (tBox->mColumn * tBox->mFontW) + (tBox->mBoxMargin * 2);
    dstRect.h = (tBox->mRow * tBox->mFontH) + (tBox->mBoxMargin * 2);

    SDL_RenderCopyEx(renderer, tBox->mBoxTex, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);

    //tl
    srcRect.x = tl.x;
    srcRect.y = tl.y;
    srcRect.w = tBox->mBoxDim;
    srcRect.h = tBox->mBoxDim;

    dstRect.x = tBox->mX - tBox->mBoxMargin - tBox->mBoxDim;
    dstRect.y = tBox->mY - tBox->mBoxMargin - tBox->mBoxDim;
    dstRect.w = tBox->mBoxDim;
    dstRect.h = tBox->mBoxDim;

    SDL_RenderCopyEx(renderer, tBox->mBoxTex, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);

    //t
    srcRect.x = t.x;
    srcRect.y = t.y;
    srcRect.w = tBox->mBoxDim;
    srcRect.h = tBox->mBoxDim;

    dstRect.x = tBox->mX - tBox->mBoxMargin;
    dstRect.y = tBox->mY - tBox->mBoxMargin - tBox->mBoxDim;
    dstRect.w = (tBox->mColumn * tBox->mFontW) + (tBox->mBoxMargin * 2);
    dstRect.h = tBox->mBoxDim;

    SDL_RenderCopyEx(renderer, tBox->mBoxTex, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);

    //tr
    srcRect.x = tr.x;
    srcRect.y = tr.y;
    srcRect.w = tBox->mBoxDim;
    srcRect.h = tBox->mBoxDim;

    dstRect.x = tBox->mX + (tBox->mColumn * tBox->mFontW) + tBox->mBoxMargin;
    dstRect.y = tBox->mY - tBox->mBoxMargin - tBox->mBoxDim;
    dstRect.w = tBox->mBoxDim;
    dstRect.h = tBox->mBoxDim;

    SDL_RenderCopyEx(renderer, tBox->mBoxTex, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);


    //ml
    srcRect.x = ml.x;
    srcRect.y = ml.y;
    srcRect.w = tBox->mBoxDim;
    srcRect.h = tBox->mBoxDim;

    dstRect.x = tBox->mX - tBox->mBoxMargin - tBox->mBoxDim;
    dstRect.y = tBox->mY - tBox->mBoxMargin;
    dstRect.w = tBox->mBoxDim;
    dstRect.h = (tBox->mRow * tBox->mFontH) + (tBox->mBoxMargin * 2);

    SDL_RenderCopyEx(renderer, tBox->mBoxTex, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);

    //mr
    srcRect.x = mr.x;
    srcRect.y = mr.y;
    srcRect.w = tBox->mBoxDim;
    srcRect.h = tBox->mBoxDim;

    dstRect.x = tBox->mX + (tBox->mColumn * tBox->mFontW) + tBox->mBoxMargin;
    dstRect.y = tBox->mY - tBox->mBoxMargin;
    dstRect.w = tBox->mBoxDim;
    dstRect.h = (tBox->mRow * tBox->mFontH) + (tBox->mBoxMargin * 2);

    SDL_RenderCopyEx(renderer, tBox->mBoxTex, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);

    //bl
    srcRect.x = bl.x;
    srcRect.y = bl.y;
    srcRect.w = tBox->mBoxDim;
    srcRect.h = tBox->mBoxDim;

    dstRect.x = tBox->mX - tBox->mBoxMargin - tBox->mBoxDim;    
    dstRect.y = tBox->mY + (tBox->mRow * tBox->mFontH) + tBox->mBoxMargin;
    dstRect.w = tBox->mBoxDim;
    dstRect.h = tBox->mBoxDim;

    SDL_RenderCopyEx(renderer, tBox->mBoxTex, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);

    //b
    srcRect.x = b.x;
    srcRect.y = b.y;
    srcRect.w = tBox->mBoxDim;
    srcRect.h = tBox->mBoxDim;

    dstRect.x = tBox->mX - tBox->mBoxMargin;    
    dstRect.y = tBox->mY + (tBox->mRow * tBox->mFontH) + tBox->mBoxMargin;
    dstRect.w = (tBox->mColumn * tBox->mFontW) + (tBox->mBoxMargin * 2);
    dstRect.h = tBox->mBoxDim;

    SDL_RenderCopyEx(renderer, tBox->mBoxTex, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);

    //br
    srcRect.x = br.x;
    srcRect.y = br.y;
    srcRect.w = tBox->mBoxDim;
    srcRect.h = tBox->mBoxDim;

    dstRect.x = tBox->mX + (tBox->mColumn * tBox->mFontW) + tBox->mBoxMargin;
    dstRect.y = tBox->mY + (tBox->mRow * tBox->mFontH) + tBox->mBoxMargin;
    dstRect.w = tBox->mBoxDim;
    dstRect.h = tBox->mBoxDim;

    SDL_RenderCopyEx(renderer, tBox->mBoxTex, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);



    //////////////////////
    //Rendering Text
    //////////////////////
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
    if(tBox->mSpeed > 0 && tBox->mNumRendered <= tBox->mText.size())
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
        //space
        else if((int)curChar == 32)
        {
            xTextPos = 3;
            yTextPos = 25;
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
        if(curXPos + 1 >= tBox->mColumn)
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

TextLine InitTextLine(SDL_Texture *fontTex,
        int fontW,
        int fontH,
        string text,
        int x,
        int y
        )
{
    TextLine outTextLine;

    outTextLine.mFontTex = fontTex;
    outTextLine.mFontW = fontW;
    outTextLine.mFontH = fontH;

    outTextLine.mText = text;
    outTextLine.mX = x;
    outTextLine.mY = y;

    outTextLine.mAlpha = 255;
    outTextLine.mAudioPath = "";
    outTextLine.mPlayedAudio = false;

    return outTextLine;
}

void RenderTextLine(SDL_Renderer *renderer, TextLine tLine)
{
    SDL_Rect srcRect, dstRect;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = tLine.mFontW;
    srcRect.h = tLine.mFontH;

    dstRect.x = tLine.mX;
    dstRect.y = tLine.mY;
    dstRect.w = tLine.mFontW;
    dstRect.h = tLine.mFontH;

    int curXPos = 0;
    int curYPos = 0;

    int numLetters = tLine.mText.size();

    for(int i = 0; i < numLetters; i++)
    {
        char curChar = tLine.mText[i];
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
        //space
        else if((int)curChar == 32)
        {
            xTextPos = 3;
            yTextPos = 25;
        }
        //numbers
        else if ((int)curChar >= 48 && (int)curChar <= 57)
        {
            xTextPos = (int)curChar - 48;
            yTextPos = 2;
        }

        srcRect.x = xTextPos * tLine.mFontW;
        srcRect.y = yTextPos * tLine.mFontH;

        dstRect.x = tLine.mX + tLine.mFontW * curXPos;
        dstRect.y = tLine.mY + tLine.mFontH * curYPos;

        Uint8 originalAlpha;
        SDL_GetTextureAlphaMod(tLine.mFontTex, &originalAlpha);

        SDL_SetTextureBlendMode(tLine.mFontTex, SDL_BLENDMODE_BLEND);

        SDL_SetTextureAlphaMod(tLine.mFontTex, tLine.mAlpha);
        SDL_RenderCopyEx(renderer, tLine.mFontTex, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);

        //Revert back to original alpha

        SDL_SetTextureAlphaMod(tLine.mFontTex, originalAlpha);
        curXPos += 1;
    }

}

bool DarkenTexture(GameState *GS)
{
    int updateIncrement = 5;
    if(GS->blackTex.mAlpha < 255)
    {
        if(GS->blackTex.mAlpha + updateIncrement >= 255)
        {
            GS->blackTex.mAlpha = 255;
            return true;
        }
        else
        {
            GS->blackTex.mAlpha += updateIncrement;
            GS->blackTex.mLastUpdate = GS->curTime;
            return false;
        }
    }
    return true;
}

bool LightenTexture(Texture *tex, Uint32 curTime)
{
    int updateIncrement = 5;
    if(tex->mAlpha > 0)
    {
        if(tex->mAlpha - updateIncrement <= 0)
        {
            tex->mAlpha = 0;
            return true;
        }
        else
        {
            tex->mAlpha -= updateIncrement;
            tex->mLastUpdate = curTime;
            return false;
        }
    }
    return true;
}

bool MouseTextBoxCol(int mouseX, int mouseY, TextBox tb)
{
    if(!tb.mActive)
    {
        return false;
    }

    bool xCol = false;
    bool yCol = false;
    if(mouseX >= tb.mX &&
            mouseX <= tb.mX + (tb.mColumn * tb.mFontW))
    {
        xCol = true;
    }
    if(mouseY >= tb.mY &&
            mouseY <= tb.mY + (tb.mRow* tb.mFontH))
    {
        yCol = true;
    }

    if(xCol && yCol)
    {
        return true;
    }
    else
    {
        return false;
    }
}

LevelInfo InitLevelInfo(GameState *GS, string texturePath)
{
    LevelInfo outLevel;
    outLevel.mActive = true;
    SDL_Texture *levelSDLTex = GetSDLTexture(GS->renderer, GS->window, texturePath);
    outLevel.mLevelTex = InitTexture(levelSDLTex, 0, 0);
    outLevel.mLevelTex.mX = GAMEWIDTH/2 - (outLevel.mLevelTex.mW/2);
    outLevel.mLevelTex.mY = GAMEHEIGHT/2 - (outLevel.mLevelTex.mH/2);
    return outLevel;
}

void LoadNarration(GameState *GS, string sNarration, int spriteCol)
{
    if(spriteCol != -1)
    {
        if(GS->ssArray[spriteCol].mInteract)
        {
            GS->ssArray[spriteCol].mInteract = false;




        }
        else
        {
            return;
        }
    }

    AddStoryLine(GS, sNarration);
    if(ShowDebug)
    {
        cout << "loaded narration: " << sNarration << " spritecol: " << spriteCol << "\n";
    }

    //Some actions won't need narration, however we'll still want the narration recorded via story
    if(spriteCol >= 0 && GS->ssArray[spriteCol].mHideNarration)
    {
        return;
    }

    //2 is the margin of text box, really should change it so that the box begins
    //at left instead of marging going past
    int y = GS->lInfo.mLevelTex.mY + GS->lInfo.mLevelTex.mH + 10;
    int column = sNarration.size();

    if(sNarration == LINE_3)
    {
        column = 37;
    }

    //Mid point of the level minus half the width of the textbox
    //So that the textbox is centered according to the level texture
    int x = (GS->lInfo.mLevelTex.mX + GS->lInfo.mLevelTex.mW/2) - (column * MAIN_TEXT_W)/2;


    GS->PlayerState = STATE_NARRATION;
    GS->tbArray[TB_NARRATION_BOX] = InitTextBox(GS->fontTexture,
            MAIN_TEXT_W,
            MAIN_TEXT_H,
            GS->mainBoxTexture,
            sNarration,
            x,
            y,
            column,
            100);

    GS->tbArray[TB_NARRATION_BOX].mDuration = sNarration.size() * 200 + 1000;

}

void LoadAction(GameState *GS, string action, int spriteCol)
{
    if(spriteCol != -1 && !GS->ssArray[spriteCol].mInteract)
    {
        return;
    }

    if(GS->ssArray[spriteCol].mNarration != "" && GS->ssArray[spriteCol].mNarration != "NONE")
    {
        LoadNarration(GS, GS->ssArray[spriteCol].mNarration, spriteCol);
    }

    //make player sprite invisible
    GS->ssPlayer.mActive = false;
    GS->ssArray[spriteCol].mActive = false;
    GS->actionCol = spriteCol;


    if(ShowDebug)
    {
        cout << "loaded action: " << action << " spritecol: " << spriteCol << "\n";
    }

    SDL_Texture *actionTex = GetSDLTexture(GS->renderer, GS->window, action);
    RemoveTextureWhiteSpace(actionTex);

    SpriteSheet actionSheet;
    if(action == PLAYER_WAKE)
    {
        actionSheet = InitSpriteSheet(actionTex,
                100,
                100,
                15); 

        actionSheet.mUpdateInterval = 200;
    }
    else if(action == PLAYER_WASH)
    {
        actionSheet = InitSpriteSheet(actionTex,
                100,
                100,
                32); 

        actionSheet.mUpdateInterval = 200;
    }
    else if(action == PLAYER_WEAT)
    {
        actionSheet = InitSpriteSheet(actionTex,
                100,
                100,
                67); 

        actionSheet.mUpdateInterval = 100;

    }
    else if(action == CHILD_WAKE)
    {
        actionSheet = InitSpriteSheet(actionTex,
                100,
                100,
                16); 

        actionSheet.mUpdateInterval = 500;
    }
    else if(action == MAN_SLEEP)
    {
        actionSheet = InitSpriteSheet(actionTex,
                100,
                100,
                26); 

        actionSheet.mUpdateInterval = 300;
    }

    if(spriteCol != -1)
    {
        GS->ssPlayer.mX = GS->ssArray[spriteCol].mX;

        GS->ssPlayer.mY = GS->ssArray[spriteCol].mY;

        actionSheet.mX = GS->ssArray[spriteCol].mX;
        actionSheet.mY = GS->ssArray[spriteCol].mY;
    }
    else
    {
        actionSheet.mX = GS->ssPlayer.mX;
        actionSheet.mY = GS->ssPlayer.mY;

    }

    GS->ssPlayerAction = actionSheet;

    GS->PlayerState = STATE_ACTION;

}

void ChangePlayerState(GameState *GS, string newState)
{
    if(GS->PlayerState == STATE_ACTION || GS->PlayerState == STATE_NARRATION)
    {
        return;
    }

    GS->PlayerState = newState;
}
void RefreshState(GameState *GS)
{
    for(int i = 0; i < NUM_SPRITESHEET; i++)
    {
        GS->ssArray[i].mActive = false;
    }

    for(int i = 0; i < NUM_TEXTBOX; i++)
    {
        GS->tbArray[i].mActive = false;
    }
}


void SpawnPlayer(GameState *GS, float x, float y)
{
    SDL_Texture *manTex = GetSDLTexture(GS->renderer, GS->window, "./res/png/manwalk.png");
    RemoveTextureWhiteSpace(manTex);
    SpriteSheet manSheet = InitSpriteSheet(manTex,
            100,
            100,
            18); 

    //Need fine tuning to match sprite sheet updating with transforming
    manSheet.mUpdateInterval = 127;
    GS->playerSpeed = 0.6;

    manSheet.mX = x;

    manSheet.mY = GS->lInfo.mLevelTex.mY + GS->lInfo.mLevelTex.mH - manSheet.mDstRect.h;

    manSheet.mColBoxOffset.x = 30;
    manSheet.mColBoxOffset.w = 40;

    GS->ssPlayer = manSheet;
    GS->ssPlayer.mCurFrame = PLAYER_IDLE_FRAME;
    GS->ssPlayer.mUpdate = false;
}

void SpawnControls(GameState *GS)
{
    int controlWidth = 5;

    //Width of the textbox is calculated by column * fontW + tbox->mBoxMargin * 2
    int boxWidthInPixels = controlWidth * MAIN_TEXT_W + (2*2);

    int yLocation = 410; 
    GS->tbArray[0] = InitTextBox(GS->fontTexture,
            MAIN_TEXT_W,
            MAIN_TEXT_H,
            GS->mainBoxTexture,
            STATE_LEFT,
            GAMEWIDTH/2 - boxWidthInPixels - 10, 
            yLocation,
            5,
            200);

    GS->tbArray[0].mType = TBTYPE_INPUT;

    GS->tbArray[1] = InitTextBox(GS->fontTexture,
            MAIN_TEXT_W,
            MAIN_TEXT_H,
            GS->mainBoxTexture,
            STATE_RIGHT,
            GAMEWIDTH/2 + 10, 
            yLocation,
            5,
            200);

    GS->tbArray[1].mType = TBTYPE_INPUT;
}

void AddStoryLine(GameState *GS, string line)
{
    GS->sStory[GS->curStory] = InitTextLine(GS->fontTexture,
            MAIN_TEXT_W,
            MAIN_TEXT_H,
            line,
            20,
            20 + (GS->curStory * 20));
    //GS->sStory[GS->curStory].mAlpha = 100;

    //Adds a delay of X ms by each line
    GS->sStory[GS->curStory].mDelay = GS->curStory * 3000;

    string audioPath = "";
    //Handle adding audio here
    if(line == LINE_0)
    {
        audioPath = AUDIO_G;
    }
    else if(line == LINE_1)
    {
        audioPath = AUDIO_Em;
    }
    else if(line == LINE_2)
    {
        audioPath = AUDIO_Am;
    }
    else if(line == LINE_3)
    {
        audioPath = AUDIO_F;
    }
    else if(line == LINE_4)
    {
        audioPath = AUDIO_C;
    }
    else if(line == LINE_5)
    {
        audioPath = AUDIO_D;
    }
    else if(line == LINE_6)
    {
        audioPath = AUDIO_Dm;
    }
    else 
    {
        if(ShowDebug)
        {
            cout << "Warning, No Audio set:" << line << "\n";
        }
    }

    GS->sStory[GS->curStory].mAudioPath = audioPath;

    GS->curStory += 1;

    //Dont play the audio from last two because they are the endings
    if(audioPath != "" && line != LINE_5 && line != LINE_6)
    {
        PlayAudioFromPath(GS, audioPath);

    }
}

void LoadScene(GameState *GS, string sceneName)
{

    if(ShowDebug)
    {
        cout << "Loading Scene: " << sceneName << "\n";
    }

    //kind of an issue because the action take doesnt persist after
    //transitioning back to each room, all variables gets reset
    RefreshState(GS);
    GS->SceneCurrent = sceneName;

    if(sceneName == SCENE_INTRO)
    {

        //So unused array members are not worked upon
        SDL_Texture *titleTex = GetSDLTexture(GS->renderer, GS->window, "./res/png/title.png");
        SpriteSheet titleSheet = InitSpriteSheet(titleTex,
                GAMEWIDTH,
                GAMEHEIGHT,
                15); 

        titleSheet.mUpdateInterval = 200;

        GS->ssArray[1] = titleSheet;

        GS->tbArray[0] = InitTextBox(GS->fontTexture,
                MAIN_TEXT_W,
                MAIN_TEXT_H,
                GS->mainBoxTexture,
                GS->startButtonText,
                226,
                351,
                GS->startButtonText.size(),
                200);

        GS->curStory = 0;
        for(int i = 0; i < NUM_LINES; i++)
        {
            GS->sStory[i].mText = "";
        }

        //SceneSpefic stuff
        GS->curStory = 0;

        //RemoveTextureWhiteSpace(GS.man);
        GS->SceneCurrent = SCENE_INTRO; 
        GS->SceneNext = SCENE_INTRO; 
        GS->PlayerState = STATE_IDLE;

        GS->screenColor.r = 200;
        GS->screenColor.g = 200;
        GS->screenColor.b = 200;
        GS->screenColor.a = 255;

        GS->lInfo.mActive = false;
        GS->ssPlayer.mActive = false;
        GS->ssPlayerAction.mActive = false; 


    }
    else if(sceneName == SCENE_BEDROOM)
    {
        GS->lInfo = InitLevelInfo(GS, SCENE_BEDROOM);


        GS->lInfo.mInitPlayerPos.x = 180;
        GS->lInfo.mInitPlayerPos.y = 280;

        SpawnPlayer(GS, GS->lInfo.mInitPlayerPos.x, GS->lInfo.mInitPlayerPos.y);

        SpawnControls(GS);

        //Door
        SDL_Texture *doorTex = GetSDLTexture(GS->renderer, GS->window, TEX_DOOR);

        SpriteSheet door = InitSpriteSheet(doorTex, 0, 0, 1); 
        door.mX = GS->lInfo.mInitPlayerPos.x + 25;
        door.mY = GS->lInfo.mLevelTex.mY + GS->lInfo.mLevelTex.mH - door.mDstRect.h;
        door.mType = TTYPE_TRANSIT;
        door.mName = SCENE_LIVINGROOM;
        door.mButtonText = "Enter Living room";
        GS->ssArray[0] = door;

        //ChildSleep

        SDL_Texture *childsleepTex = GetSDLTexture(GS->renderer, GS->window, TEX_CHILDSLEEP);

        SpriteSheet childSleepSS = InitSpriteSheet(childsleepTex, 100, 100, 4);

        childSleepSS.mType = TTYPE_NARRATION;
        childSleepSS.mButtonText = "Look";
        childSleepSS.mNarration = LINE_1 ;

        childSleepSS.mX = 300;
        childSleepSS.mY = GS->ssPlayer.mY;
        childSleepSS.mUpdateInterval = 1500;
        GS->ssArray[SS_CHILD] = childSleepSS;


        //manbed

        SDL_Texture *manbedTex = GetSDLTexture(GS->renderer, GS->window, TEX_MANBED);

        SpriteSheet manbedSS = InitSpriteSheet(manbedTex, 106, 100, 1);

        manbedSS.mType = TTYPE_NORMAL;

        manbedSS.mX = 410;
        manbedSS.mY = GS->ssPlayer.mY;
        GS->ssArray[4] = manbedSS;

        //Special case for start spawn of the level where the player is in bed instead of door.
        if(GS->curStory == 0)
        {
            LoadNarration(GS, LINE_0, -1);

            GS->ssPlayer.mX = 420;

            GS->curStory = 1;
            LoadAction(GS, PLAYER_WAKE, -1);
        }


    }
    else if(sceneName == SCENE_LIVINGROOM)
    {
        GS->lInfo = InitLevelInfo(GS, SCENE_LIVINGROOM);

        GS->lInfo.mInitPlayerPos.x = 380;
        GS->lInfo.mInitPlayerPos.y = 300;
        SpawnPlayer(GS, GS->lInfo.mInitPlayerPos.x, GS->lInfo.mInitPlayerPos.y);
        SpawnControls(GS);
        SDL_Texture *doorTex = GetSDLTexture(GS->renderer, GS->window, TEX_DOOR);

        //bedroom door
        SpriteSheet door = InitSpriteSheet(doorTex, 0, 0 ,1);
        door.mX = GS->lInfo.mInitPlayerPos.x + 25;
        door.mY = GS->lInfo.mLevelTex.mY + GS->lInfo.mLevelTex.mH - door.mDstRect.h;
        door.mType = TTYPE_TRANSIT;
        door.mName = SCENE_BEDROOM;
        door.mButtonText = "Enter Bedroom";
        GS->ssArray[0] = door;

        //Enddoor
        SpriteSheet endDoor = InitSpriteSheet(doorTex, 0, 0 , 1);

        endDoor.mX = GS->lInfo.mLevelTex.mX + 5;
        endDoor.mY = GS->lInfo.mLevelTex.mY + GS->lInfo.mLevelTex.mH - endDoor.mDstRect.h;

        endDoor.mType = TTYPE_TRANSIT;
        endDoor.mName = SCENE_ENDDOOR;
        endDoor.mButtonText = "Go to Work";
        GS->ssArray[1] = endDoor;


        //window
        SDL_Texture *windowTex= GetSDLTexture(GS->renderer, GS->window, TEX_WINDOW);

        SpriteSheet windowSS = InitSpriteSheet(windowTex, 60, 29, 6);

        windowSS.mType = TTYPE_NARRATION;
        windowSS.mButtonText = "Look";
        windowSS.mNarration = LINE_4;

        windowSS.mX = 290;
        windowSS.mY = 195; 
        windowSS.mUpdateInterval = 1500;
        GS->ssArray[2] = windowSS;

        //Table
        SDL_Texture *tableTex = GetSDLTexture(GS->renderer, GS->window, TEX_TABLE);
        RemoveTextureWhiteSpace(tableTex);
        SpriteSheet table = InitSpriteSheet(tableTex, 0, 0, 1); //action same height player
        table.mX = 280; 
        table.mY = GS->lInfo.mLevelTex.mY + GS->lInfo.mLevelTex.mH - table.mDstRect.h;;
        table.mType = TTYPE_NORMAL;
        GS->ssArray[3] = table;

        //fridge
        SDL_Texture *fridgeTex = GetSDLTexture(GS->renderer, GS->window, TEX_FRIDGE);

        SpriteSheet fridge = InitSpriteSheet(fridgeTex, 0, 0, 1); //action same height player
        fridge.mX = 170;
        fridge.mY = GS->lInfo.mLevelTex.mY + GS->lInfo.mLevelTex.mH - fridge.mDstRect.h;;
        fridge.mType = TTYPE_ACTION;
        fridge.mName = PLAYER_WEAT; 
        fridge.mButtonText = "Eat";
        fridge.mNarration = LINE_3;
        fridge.mColBoxOffset.x = 43;
        fridge.mColBoxOffset.w = 14;

        fridge.mHideNarration = true;
        GS->ssArray[4] = fridge;

        //toilet
        SDL_Texture *toiletTex = GetSDLTexture(GS->renderer, GS->window, TEX_TOILET);
        RemoveTextureWhiteSpace(toiletTex);
        SpriteSheet toilet = InitSpriteSheet(toiletTex, 0, 0, 1); //action same height player
        toilet.mX = GS->lInfo.mLevelTex.mX + GS->lInfo.mLevelTex.mW - toilet.mDstRect.w;
        toilet.mY = GS->lInfo.mLevelTex.mY + GS->lInfo.mLevelTex.mH - toilet.mDstRect.h;;
        toilet.mType = TTYPE_NORMAL;
        GS->ssArray[5] = toilet;


        //Sink
        SDL_Texture *sinkTex = GetSDLTexture(GS->renderer, GS->window, TEX_SINK);
        RemoveTextureWhiteSpace(sinkTex);
        SpriteSheet sink = InitSpriteSheet(sinkTex, 0, 0, 1); //action same height player
        sink.mX = GS->lInfo.mLevelTex.mX + GS->lInfo.mLevelTex.mW - sink.mDstRect.w;
        sink.mY = GS->lInfo.mLevelTex.mY + GS->lInfo.mLevelTex.mH - sink.mDstRect.h;;
        sink.mType = TTYPE_ACTION;
        sink.mName = PLAYER_WASH; 
        sink.mButtonText = "Wash";
        sink.mNarration = LINE_2;
        sink.mColBoxOffset.x = 60;
        sink.mColBoxOffset.w = 40;
        sink.mHideNarration = true;
        GS->ssArray[6] = sink;

    }
    else if(sceneName == SCENE_ENDDOOR)
    {
        GS->lInfo = InitLevelInfo(GS, SCENE_BEDROOM);
        AddStoryLine(GS, LINE_6);

        //50 because the sprite anim is 100x100
        //And i idiotically implemented action initiation by passing in string
        //So there's no way of me finding out the dimensions through the image
        GS->ssPlayer.mX = GAMEWIDTH/2 - 50;
        GS->ssPlayer.mY = GAMEHEIGHT/2 - 50;
        //init text start renderings
        LoadAction(GS, CHILD_WAKE, -1);


        SDL_Texture *manbedTex = GetSDLTexture(GS->renderer, GS->window, TEX_MANBED);

        SpriteSheet manbedSS = InitSpriteSheet(manbedTex, 106, 100, 1);

        manbedSS.mType = TTYPE_NORMAL;

        manbedSS.mX = 410;
        manbedSS.mY = GS->ssPlayer.mY;
        GS->ssArray[4] = manbedSS;

        GS->startButtonText = "Try Again";

        for(int i = 0; i < GS->curStory; i++)
        {
            if(ShowDebug)
            {
                cout << GS->sStory[i].mText << "\n";
            }
            GS->sStory[i].mStartTime = GS->curTime;
        }
    }
    else if(sceneName == SCENE_ENDROOM)
    {
        GS->lInfo = InitLevelInfo(GS, SCENE_BEDROOM);
        AddStoryLine(GS, LINE_5);

        GS->ssPlayer.mX = 415;
        GS->ssPlayer.mY = GAMEHEIGHT/2 - 50;


        //ChildSleep

        SDL_Texture *childsleepTex = GetSDLTexture(GS->renderer, GS->window, TEX_CHILDSLEEP);

        SpriteSheet childSleepSS = InitSpriteSheet(childsleepTex, 100, 100, 4);

        childSleepSS.mType = TTYPE_NARRATION;
        childSleepSS.mButtonText = "Look";
        childSleepSS.mNarration = LINE_1 ;

        childSleepSS.mX = 300;
        childSleepSS.mY = GS->ssPlayer.mY;
        childSleepSS.mUpdateInterval = 1500;
        GS->ssArray[SS_CHILD] = childSleepSS;


        //manbed

        SDL_Texture *manbedTex = GetSDLTexture(GS->renderer, GS->window, TEX_MANBED);

        SpriteSheet manbedSS = InitSpriteSheet(manbedTex, 106, 100, 1);

        manbedSS.mType = TTYPE_NORMAL;

        manbedSS.mX = 410;
        manbedSS.mY = GS->ssPlayer.mY;
        GS->ssArray[4] = manbedSS;

        GS->startButtonText = "Again";
        //init text start renderings
        LoadAction(GS, MAN_SLEEP, -1);
        for(int i = 0; i < GS->curStory; i++)
        {
            if(ShowDebug)
            {
                cout << GS->sStory[i].mText << "\n";
            }
            GS->sStory[i].mStartTime = GS->curTime;
        }
    }


}


bool SpriteTextureCollision(SpriteSheet ss, Texture tex)
{
    //if either is inactive, return false;
    if(!ss.mActive || !tex.mActive)
    {
        return false; 
    }
    bool horizCol = false;
    bool vertCol = false;

    /*native texture col
      if(ss.mX + ss.mDstRect.w >= tex.mX 
      && ss.mX <= tex.mX + tex.mW)
      {
      horizCol = true;
      }

      if(ss.mY + ss.mDstRect.h >= tex.mY
      && ss.mY <= tex.mY + tex.mH)
      {
      vertCol = true;
      }
     */

    //new texture col
    if(ss.mX + ss.mColBoxOffset.x + ss.mColBoxOffset.w >= tex.mX + tex.mColBoxOffset.x 
            && ss.mX + ss.mColBoxOffset.x <= tex.mX + tex.mColBoxOffset.x + tex.mColBoxOffset.w)
    {
        horizCol = true;
    }

    if(ss.mY + ss.mColBoxOffset.y + ss.mColBoxOffset.h >= tex.mY + tex.mColBoxOffset.y
            && ss.mY + ss.mColBoxOffset.y <= tex.mY + ss.mColBoxOffset.y + tex.mColBoxOffset.h)
    {
        vertCol = true;
    }

    return horizCol && vertCol;


}

bool SpriteToSpriteCollision(SpriteSheet ss, SpriteSheet ss2)
{
    if(ss2.mType == TTYPE_NORMAL)
    {
        return false;
    }
    //if either is inactive, return false;
    if(!ss.mActive || !ss2.mActive)
    {
        return false; 
    }
    bool horizCol = false;
    bool vertCol = false;

    //new texture col
    if(ss.mX + ss.mColBoxOffset.x + ss.mColBoxOffset.w >= ss2.mX + ss2.mColBoxOffset.x 
            && ss.mX + ss.mColBoxOffset.x <= ss2.mX + ss2.mColBoxOffset.x + ss2.mColBoxOffset.w)
    {
        horizCol = true;
    }

    if(ss.mY + ss.mColBoxOffset.y + ss.mColBoxOffset.h >= ss2.mY + ss2.mColBoxOffset.y
            && ss.mY + ss.mColBoxOffset.y <= ss2.mY + ss.mColBoxOffset.y + ss2.mColBoxOffset.h)
    {
        vertCol = true;
    }

    return horizCol && vertCol;

}

