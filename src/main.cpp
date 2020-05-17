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
                            cout << "dd\n"; 
                            GS.PlayerState = "RIGHT";
                            break;
                        }
                    case SDLK_a:
                        {
                            cout << "aa\n"; 
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
                            GS.manSheet.mUpdateInterval += 10;
                            break;
                        }
                    case SDLK_2:
                        {

                            GS.manSheet.mUpdateInterval -= 10;
                            break;
                        }
                    case SDLK_3:
                        {
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
                SDL_GetMouseState(&mouseX, &mouseY);
                cout << "mouseX: " << mouseX << " mouseY: " << mouseY << "\n";

                break;
            case SDL_QUIT:
                exit(0);
                break;
        }
    }


    if(GS.PlayerState == "LEFT")
    {
        GS.manSheet.mDstRect.x -= 1;
    }
    else if(GS.PlayerState == "RIGHT")
    {
        GS.manSheet.mDstRect.x += 1;
    }


    if(GS.curTime > GS.manSheet.mLastUpdate + GS.manSheet.mUpdateInterval)
    {
        GS.manSheet.mCurFrame = (GS.manSheet.mCurFrame + 1) % GS.manSheet.mNumFrames;
        GS.manSheet.mLastUpdate = GS.curTime;
    }
    //Render Code
    RenderSpriteSheet(GS.renderer, GS.manSheet);

    //Render textBox
    RenderTextBox(GS.renderer, GS.startButton);


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
    SDL_Texture *manTex = GetSDLTexture(GS.renderer, GS.window, "./res/png/manwalk.png");
    //RemoveTextureWhiteSpace(GS.man);
    GS.State = "START";
    GS.PlayerState = "IDLE";
    GS.screenColor.r = 200;
    GS.screenColor.g = 200;
    GS.screenColor.b = 200;
    GS.screenColor.a = 255;

    GS.manSheet = InitSpriteSheet(manTex,
            50,
            100,
            8); 

    GS.manSheet.mUpdateInterval = 200;

    GS.fontTexture = GetSDLTexture(GS.renderer, GS.window, "./res/png/mainText.png");
    GS.mainBoxTexture = GetSDLTexture(GS.renderer, GS.window, "./res/png/textBox.png");

    GS.startButton = InitTextBox(GS.fontTexture,
                                 20,
                                 20,
                                 GS.mainBoxTexture,
                                 3,
                                 "Start",
                                 10,
                                 10,
                                 5,
                                 -1);
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

