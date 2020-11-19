#include "main.h"


GameState GS;


#ifndef EMSCRIPTEN
const int FPS = 60;
//How many miliseconds per frame
const int FrameDelay = 1000 / FPS;
#endif 

bool ShowDebug = true;

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
                MAIN_TEXT_W,
                MAIN_TEXT_H,
                GS.mainBoxTexture,
                buttonText, 
                GAMEWIDTH/2 - (buttonText.length() * MAIN_TEXT_W + 4)/2,
                370,
                buttonText.length(),
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
                    case SDLK_i:
                        {
                            GS.playerSpeed += 0.1;
                            cout << "playerSpeed: " << GS.playerSpeed << "\n";
                            break;
                        }
                        //Debug keysym
                    case SDLK_o:
                        {
                            GS.playerSpeed -= 0.1;
                            cout << "playerSpeed: " << GS.playerSpeed << "\n";
                            break;
                        }
                    case SDLK_j:
                        {
                            GS.ssPlayer.mUpdateInterval += 1;
                            cout << "playerSpeed: " << GS.ssPlayer.mUpdateInterval << "\n";
                            break;
                        }
                        //Debug keysym
                    case SDLK_k:
                        {
                            GS.ssPlayer.mUpdateInterval -= 1;
                            break;
                        }
                    case SDLK_n:
                        {
                            //PlayAudio(&GS, GS.WalkMus);
                            break;
                        }
                    case SDLK_m:
                        {
                            //PlayAudio(&GS, GS.chordDm);
                            cout << "playerAudio: " << GS.chordDm.wavPath << "\n";
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
        if(DarkenTexture(&GS))
        {
            LoadScene(&GS, GS.SceneNext); 
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
        if(GS.ssPlayerAction.mCurFrame == (GS.ssPlayerAction.mNumFrames - 1))
        {
            GS.ssPlayer.mActive = true;
            GS.ssPlayerAction.mActive = false;
            if(GS.actionCol> 0)
            {
                GS.ssArray[GS.actionCol].mActive = true;
                cout << "finiACtion : " << GS.actionCol << "\n";
            }
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

    UpdateSpriteSheet(&GS.ssPlayerAction, GS.curTime);
    UpdateSpriteSheet(&GS.ssPlayer, GS.curTime);


    //Audio portion
    if(GS.curSound.mActive)
    {
        if(SDL_GetQueuedAudioSize(GS.audioDevice) == 0)
        {
            cout << "audio finished\n";
            GS.curSound.mActive = false;
        }
    }



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

    //renderPlayersStuff
    RenderSpriteSheet(GS.renderer, GS.ssPlayer);

    RenderSpriteSheet(GS.renderer, GS.ssPlayerAction);
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



    //GS.blackTex.mAlpha = 0;

    GS.acArray[0] = InitAudio(AUDIO_Walk);
    GS.acArray[1] = InitAudio(AUDIO_Dm);
    GS.acArray[2] = InitAudio(AUDIO_D);
    GS.acArray[3] = InitAudio(AUDIO_G);

    GS.curSound.mActive = false;

    GS.audioDevice = SDL_OpenAudioDevice(NULL, 0, &GS.chordDm.wavSpec, NULL, 0);
    if (GS.audioDevice == 0) 
    {
        printf("Failed to open audio: %s", SDL_GetError());
    } 


    SDL_PauseAudioDevice(GS.audioDevice, 0);

    LoadScene(&GS, SCENE_INTRO);


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

