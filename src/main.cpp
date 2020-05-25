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
    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);

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
                            GS.PlayerState = "RIGHT";
                            break;
                        }
                    case SDLK_a:
                        {
                            GS.PlayerState = "LEFT";
                            break;
                        }
                        //Debug keysym
                    case SDLK_p:
                        {
                            break;
                        }
                    case SDLK_1:
                        {
                            LoadStartScene(&GS); 
                            break;
                        }
                    case SDLK_2:
                        {
                            GS.tbArray[0].mIsActive = !GS.tbArray[0].mIsActive;
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
                    GS.PlayerState = "IDLE";
                    break;
                }
            case SDL_MOUSEBUTTONDOWN:
                //cout << "MOUSE_DOWN \n";

                break;
            case SDL_MOUSEBUTTONUP:
                //cout <<  "MOUSE_UP\n";

                cout << "mouseX: " << mouseX << " mouseY: " << mouseY << "\n";

                break;
            case SDL_QUIT:
                exit(0);
                break;
        }
    }
    if(MouseTextBoxCol(mouseX, mouseY, GS.tbArray[0]))
    {
        GS.tbArray[0].mIsActive = true;
    }
    else
    {
        GS.tbArray[0].mIsActive = false; 
    }

    if(GS.PlayerState == "LEFT")
    {
        GS.ssArray[0].mDstRect.x -= 1;
    }
    else if(GS.PlayerState == "RIGHT")
    {
        GS.ssArray[0].mDstRect.x += 1;
    }

    LightenTexture(&GS.blackTex, GS.curTime);

    UpdateSpriteSheet(GS.ssArray, GS.curTime);
    //
    //Render Area
    //
    for(int i = 0; i < NUM_SPRITESHEET; i++)
    {
        RenderSpriteSheet(GS.renderer, GS.ssArray[i]);
    }

    //Render textBox
    RenderTextBox(GS.renderer, GS.curTime, &GS.tbArray[0]);

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

    //RemoveTextureWhiteSpace(GS.man);
    GS.State = "MENU";
    GS.PlayerState = "IDLE";
    GS.screenColor.r = 200;
    GS.screenColor.g = 200;
    GS.screenColor.b = 200;
    GS.screenColor.a = 255;

    //So unused array members are not worked upon
    for(int i = 0; i < NUM_SPRITESHEET; i++)
    {
        GS.ssArray[i].mActive = false;
    }


    SDL_Texture *titleTex = GetSDLTexture(GS.renderer, GS.window, "./res/png/title.png");
    SpriteSheet titleSheet = InitSpriteSheet(titleTex,
            GAMEWIDTH,
            GAMEHEIGHT,
            7); 


    titleSheet.mUpdateInterval = 200;

    GS.ssArray[0] = titleSheet;

/*
    SDL_Texture *manTex = GetSDLTexture(GS.renderer, GS.window, "./res/png/manwalk.png");
    SpriteSheet manSheet = InitSpriteSheet(manTex,
            50,
            100,
            8); 

    manSheet.mUpdateInterval = 200;

    GS.ssArray[1] = manSheet;

*/

    GS.fontTexture = GetSDLTexture(GS.renderer, GS.window, "./res/png/mainText.png");
    RemoveTextureWhiteSpace(GS.fontTexture);
    GS.mainBoxTexture = GetSDLTexture(GS.renderer, GS.window, "./res/png/textBox.png");
    RemoveTextureWhiteSpace(GS.mainBoxTexture);

    GS.tbArray[0] = InitTextBox(GS.fontTexture,
            20,
            20,
            GS.mainBoxTexture,
            3,
            "Start",
            10,
            10,
            5,
            200);

    GS.blackTex = InitTexture(GetSDLTexture(GS.renderer, GS.window, "./res/png/black.png"), 0 ,0);
    GS.blackTex.mW = GAMEWIDTH;
    GS.blackTex.mH = GAMEHEIGHT;
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

