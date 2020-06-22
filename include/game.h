#ifndef GAME_H
#define GAME_H
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <cstring>
#include <iostream>

#ifdef EMSCRIPTEN
    #include <emscripten.h>
#endif 

#include "game_defs.h"


using namespace std;


int StartSDL(SDL_Window **window, SDL_Renderer **renderer);

SDL_Texture* GetSDLTexture(SDL_Renderer *renderer, SDL_Window *window, string textureLocation);

Texture InitTexture(SDL_Texture *sdlTexture, int x, int y);
void RenderTexture(SDL_Renderer *renderer, Texture tex);

SpriteSheet InitSpriteSheet(SDL_Texture *sdlTexture,
        int w,
        int h,
        int numFrames);

void UpdateSpriteSheet(SpriteSheet *ssArray, Uint32 curTime);
void RenderSpriteSheet(SDL_Renderer *renderer, SpriteSheet sSheet);
void RemoveTextureWhiteSpace(SDL_Texture *texture);

AudioClip InitAudio(string filepath);

void PlayAudio(SDL_AudioDeviceID audioDevice, AudioClip clip);

SDL_Point RotatePointByOtherPoint(int inX,
                                  int inY,
                                  int centerX,
                                  int centerY,
                                  int degrees);

TextBox InitTextBox(SDL_Texture *fontTex,
        int fontW,
        int fontH,
        SDL_Texture *boxTex,
        string text,
        int x,
        int y,
        int column,
        int speed);

void RenderTextBox(SDL_Renderer *renderer, Uint32 curTime, TextBox *tBox);

bool DarkenTexture(Texture *tex, Uint32 curTime);

bool LightenTexture(Texture *tex, Uint32 curTime);

bool MouseTextBoxCol(int mouseX, int mouseY, TextBox tb);

LevelInfo InitLevelInfo(GameState *GS, string texturePath);

void ChangePlayerState(GameState *GS, string newState);

void RefreshState(GameState *GS);

void SpawnPlayer(GameState *GS, float x, float y);

void SpawnControls(GameState *GS);

void LoadScene(GameState *GS, string sceneName);

bool SpriteTextureCollision(SpriteSheet ss, Texture tex);



#endif // GAME_H
 
