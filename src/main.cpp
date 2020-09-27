#include "main.h"


GameState GS;


#ifndef EMSCRIPTEN
const int FPS = 60;
//How many miliseconds per frame
const int FrameDelay = 1000 / FPS;
#endif 

bool ShowDebug = true;
SDL_AudioDeviceID audioDevice;

void gameloop() 
{
    Uint32 frameStart;

    frameStart = SDL_GetTicks();  

    GS.curTime = frameStart - GS.startTime;

    //The color at which the screen will be if alpha = 0 on all textures
    SDL_SetRenderDrawColor(GS.renderer, GS.screenColor.r, GS.screenColor.g, GS.screenColor.b, GS.screenColor.a);


    SDL_RenderClear(GS.renderer);


    ////////////////////////////////////////////////////////////////////////
    //Main Game Code
    ////////////////////////////////////////////////////////////////////////

    //Update game state


    //Spritesheet collisions
    int spriteCol = -1;
    for(int i = 0; i < NUM_SPRITESHEET; i++)
    {
        if(SpriteToSpriteCollision(GS.ssPlayer, GS.ssArray[i]))
        {
            spriteCol = i;

            //cout << "col: " << spriteCol << "\n";
        }
    }


    if(spriteCol != -1)
    {
        TextBox textureTB;
        string buttonText;
        string type;

        buttonText = GS.ssArray[spriteCol].mButtonText;
        type = GS.ssArray[spriteCol].mType;

        textureTB = InitTextBox(GS.fontTexture,
                20,
                20,
                GS.mainBoxTexture,
                buttonText, 
                400,
                10,
                5,
                0);           

        //the button text is a hack to redraw when moving player from
        //one interactable object to another with no empty space inbetween
        if(type != TTYPE_NORMAL && GS.tbArray[TB_ACTION_BUTTON].mText != buttonText)
        {
            GS.tbArray[TB_ACTION_BUTTON] = textureTB;
        }
    }
    else
    {
        //removes the text so that it can redraw due to the above check
        GS.tbArray[TB_ACTION_BUTTON].mText = "";
        GS.tbArray[TB_ACTION_BUTTON].mActive = false;
    }

    //Handle user input
    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);

    //Depending on where mouse is, first handle the render states of buttons
    for(int i = 0; i < NUM_TEXTBOX; i++)
    {
        if(MouseTextBoxCol(mouseX, mouseY, GS.tbArray[i]))
        {
            if(GS.tbArray[i].mState == 0)
            {
                GS.tbArray[i].mState = 1;
            }
        }
        else
        {
            if(GS.tbArray[i].mState == 1)
            {
                GS.tbArray[i].mState = 0;
            }
        }
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) 
    {
        switch (event.type) 
        {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_0:
                        {
                            if(ShowDebug)
                            {
                                ShowDebug = false;
                            }
                            else
                            {
                                ShowDebug = true;
                            }
                            break;
                        }
                    case SDLK_d:
                        {
                            ChangePlayerState(&GS, STATE_RIGHT);
                            break;
                        }
                    case SDLK_a:
                        {
                            ChangePlayerState(&GS, STATE_LEFT);
                            break;
                        }
                        //Debug keysym
                    case SDLK_p:
                        {
                            break;
                        }
                    case SDLK_1:
                        {
                            LoadScene(&GS, SCENE_BEDROOM); 
                            break;
                        }
                    case SDLK_2:
                        {
                            //GS.tbArray[0].mIsActive = !GS.tbArray[0].mIsActive;
                            break;
                        }
                    case SDLK_3:
                        {
                            GS.tbArray[0].mLastUpdate = GS.curTime;
                            GS.tbArray[0].mNumRendered = 0;
                            break;
                        }
                    case SDLK_4:
                        {
                            break;
                        }
                }
                break;
            case SDL_KEYUP:
                //cout << "pressed\n"; 
                {
                    ChangePlayerState(&GS, STATE_IDLE);
                    break;
                }
            case SDL_MOUSEBUTTONDOWN:
                for(int i = 0; i < NUM_TEXTBOX; i++)
                {
                    if(GS.tbArray[i].mState == 1)
                    {
                        GS.tbArray[i].mState = 2; 

                        if(GS.tbArray[i].mType == TBTYPE_INPUT)
                        {
                            ChangePlayerState(&GS, GS.tbArray[i].mText);
                        }


                    }
                }

                cout << "X: " << mouseX << " Y: " << mouseY << "\n";
                break;
            case SDL_MOUSEBUTTONUP:
                //cout <<  "MOUSE_UP\n";

                if(GS.SceneCurrent == SCENE_INTRO)
                {
                    if(GS.tbArray[0].mState == 2)
                    {
                        GS.SceneCurrent = SCENE_TRAN;
                        GS.SceneNext = SCENE_BEDROOM;
                        cout << "changestate\n";
                    }
                }
                else
                {
                    ChangePlayerState(&GS, STATE_IDLE);
                    if(GS.tbArray[TB_ACTION_BUTTON].mState == 2)
                    {

                        //Handle texture collisions
                        if(spriteCol != -1)
                        {
                            if(GS.ssArray[spriteCol].mType == TTYPE_NARRATION)
                            {
                                cout << "narration\n";
                                LoadNarration(&GS, GS.ssArray[spriteCol].mNarration);
                            }
                            else if(GS.ssArray[spriteCol].mType == TTYPE_TRANSIT)
                            {
                                cout << "loading: " << GS.ssArray[spriteCol].mName << "\n";
                                GS.SceneCurrent = SCENE_TRAN;
                                GS.SceneNext = GS.ssArray[spriteCol].mName;
                            }
                            else if(GS.ssArray[spriteCol].mType == TTYPE_ACTION)
                            {
                                cout << "actionnnn\n";
                                LoadAction(&GS, GS.ssArray[spriteCol].mName, spriteCol);
                            }
                        }


                    }

                }

                //Default behavior after state specific proccesses is to revert the textbox state back to 1
                for(int i = 0; i < NUM_TEXTBOX; i++)
                {
                    if(GS.tbArray[i].mState == 2)
                    {
                        GS.tbArray[i].mState = 1; 
                    }
                }
                break;
            case SDL_QUIT:
                exit(0);
                break;
        }
    }


    if(GS.SceneCurrent == SCENE_TRAN)
    {
        if(DarkenTexture(&GS.blackTex, GS.curTime))
        {
            LoadScene(&GS, GS.SceneNext); 
            GS.SceneCurrent = GS.SceneNext;
        }
    }
    else
    {
        //At the start of every state except transition state it will lighten the window
        LightenTexture(&GS.blackTex, GS.curTime);
    }

    if(GS.PlayerState == STATE_LEFT)
    {
        //level boundary check, with the collision box offset
        if((GS.ssPlayer.mX + GS.ssPlayer.mColBoxOffset.x)- GS.playerSpeed < GS.lInfo.mLevelTex.mX)
        {
            ChangePlayerState(&GS, STATE_IDLE);
        }
        else
        {
            GS.ssPlayer.mX -= GS.playerSpeed;
            GS.ssPlayer.mUpdate = true;
            GS.ssPlayer.mFlip = SDL_FLIP_HORIZONTAL;
        }
    }
    else if(GS.PlayerState == STATE_RIGHT)
    {
        //level boundary check, with the collision box offset
        if((GS.ssPlayer.mX + GS.ssPlayer.mColBoxOffset.x + GS.ssPlayer.mColBoxOffset.w) 
                + GS.playerSpeed > (GS.lInfo.mLevelTex.mX + GS.lInfo.mLevelTex.mW))
        {
            ChangePlayerState(&GS, STATE_IDLE);
        }
        else
        {
            GS.ssPlayer.mX += GS.playerSpeed;
            GS.ssPlayer.mUpdate = true;
            GS.ssPlayer.mFlip = SDL_FLIP_NONE;
        }
    }
    else if(GS.PlayerState == STATE_IDLE)
    {
        GS.ssPlayer.mCurFrame = PLAYER_IDLE_FRAME; 
        GS.ssPlayer.mUpdate = false;
    }

    if(GS.PlayerState == STATE_ACTION)
    {
        if(GS.ssArray[SS_PLAYER_ACTION].mCurFrame == (GS.ssArray[SS_PLAYER_ACTION].mNumFrames - 1))
        {
            GS.ssPlayer.mActive = true;
            GS.ssArray[SS_PLAYER_ACTION].mActive = false;
            //Not using function Change Player state here because when its state action only 
            //this part can change it back to idle
            GS.PlayerState = STATE_IDLE;
        }
    }
    else if(GS.PlayerState == STATE_NARRATION)
    {

        // +1 due to size > 1 to index of last element
        //cout << "size: " << GS.tbArray[TB_NARRATION_BOX].mText.size()<< " num: " << GS.tbArray[TB_NARRATION_BOX].mNumRendered<< "\n";
        if(GS.tbArray[TB_NARRATION_BOX].mNumRendered + 1 == GS.tbArray[TB_NARRATION_BOX].mText.size())
        {
            GS.PlayerState = STATE_IDLE;
        }
    }

    for(int i = 0; i < NUM_SPRITESHEET; i++)
    {
        UpdateSpriteSheet(&GS.ssArray[i], GS.curTime);
    }

    UpdateSpriteSheet(&GS.ssPlayer, GS.curTime);
    //
    //Render Area
    //

    RenderTexture(GS.renderer, GS.lInfo.mLevelTex);

    for(int i = 0; i < NUM_SPRITESHEET; i++)
    {
        RenderSpriteSheet(GS.renderer, GS.ssArray[i]);
    }

    for(int i = 0; i < NUM_TEXTBOX; i++)
    {
        RenderTextBox(GS.renderer, GS.curTime, &GS.tbArray[i]);
    }

    if(GS.SceneCurrent == SCENE_ENDDOOR)
    {
        for(int i = 0; i < GS.curStory; i++)
        {
            if(GS.curTime > GS.sStory[i].mStartTime + GS.sStory[i].mDelay)
            {
                RenderTextLine(GS.renderer, GS.sStory[i]);
            }
        }
    }

    //renderPlayer
    RenderSpriteSheet(GS.renderer, GS.ssPlayer);

    //Render the fadeout textures
    RenderTexture(GS.renderer, GS.blackTex);
    ////////////////////////////////////////////////////////////////////////
    //End of main game code
    ////////////////////////////////////////////////////////////////////////

#ifndef EMSCRIPTEN
    Uint32 frameTime;
    frameTime = SDL_GetTicks() - frameStart;

    if (FrameDelay > frameTime)
    {
        SDL_Delay(FrameDelay - frameTime);
    }
#endif 
    //emscripten_cancel_main_loop();
}

//Need to use this main signature (int argv, char** args)
//Because SDL alters default program entry point resolving
//SDL calls your entry point expecting a specific signature,
//which is no longer subject to compiler interpretation
int main(int argv, char **args)
{
    //cout << "Starting Game\n";

    GS.startTime = SDL_GetTicks();
    GS.curTime = SDL_GetTicks();
    //Initiate SDL
    StartSDL(&(GS.window), &(GS.renderer));

    GS.NarrativeCounter = 0;

    //RemoveTextureWhiteSpace(GS.man);
    GS.SceneCurrent = SCENE_INTRO; 
    GS.SceneNext = SCENE_INTRO; 
    GS.PlayerState = STATE_IDLE;
    GS.playerSpeed = 0.5;
    GS.screenColor.r = 200;
    GS.screenColor.g = 200;
    GS.screenColor.b = 200;
    GS.screenColor.a = 255;


    //Initial state variables
    GS.blackTex = InitTexture(GetSDLTexture(GS.renderer, GS.window, "./res/png/black.png"), 0 ,0);
    GS.blackTex.mW = GAMEWIDTH;
    GS.blackTex.mH = GAMEHEIGHT;

    GS.fontTexture = GetSDLTexture(GS.renderer, GS.window, "./res/png/mainText.png");
    RemoveTextureWhiteSpace(GS.fontTexture);

    GS.mainBoxTexture = GetSDLTexture(GS.renderer, GS.window, "./res/png/textBox.png");
    RemoveTextureWhiteSpace(GS.mainBoxTexture);

    LoadScene(&GS, SCENE_INTRO);

    //GS.blackTex.mAlpha = 0;
    /*

       audioDevice = SDL_OpenAudioDevice(NULL, 0, &ToddlerMus.wavSpec, NULL, 0);
       if (audioDevice == 0) {
       printf("Failed to open audio: %s", SDL_GetError());
       } 
     */

    //    SDL_PauseAudioDevice(audioDevice, 0);


#ifdef EMSCRIPTEN
    emscripten_set_main_loop(gameloop, 0, 0);
#else
    while(1)
    {
        gameloop();
    }
#endif 
    return 0;

}

