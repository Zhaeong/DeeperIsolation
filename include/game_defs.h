#ifndef GAMEDEFS_H
#define GAMEDEFS_H

#include <string>
#include <SDL.h>
#include <SDL_image.h>


using namespace std;



#define GAMEWIDTH  700 
#define GAMEHEIGHT 500


#define PI 3.14159265
#define AUDIO_FORMAT AUDIO_S16LSB

/*
A man wakes up for work


*/
const Uint32 TEXTUREFORMAT = SDL_PIXELFORMAT_RGBA8888;

const int NUM_SPRITESHEET = 7;
const int SS_CHILD = 3;

//Actions in the form of sprite sheets
const string PLAYER_WALK = "./res/png/manwalk.png";
const string PLAYER_WAKE = "./res/png/manwake.png";
const string PLAYER_WASH = "./res/png/manwash.png";
const string PLAYER_WEAT = "./res/png/manweat.png";
const string MAN_SLEEP = "./res/png/mansleep.png";
const string CHILD_WAKE = "./res/png/childwake.png";

//specific frame in player walk animation png which contains the idle frame
const int PLAYER_IDLE_FRAME = 18;

//player char states
//some textboxes use state strings as text display as well as state changes
//might be an issue with data coupling
const string STATE_ACTION = "ACTION";
const string STATE_NARRATION = "STATE_NARRATION";
const string STATE_IDLE = "IDLE";
const string STATE_LEFT = "Left";
const string STATE_RIGHT = "Right";



const int NUM_TEXTBOX = 5;
const int MAIN_TEXT_W = 12;
const int MAIN_TEXT_H = 15;
const int TB_NARRATION_BOX = 2;
const int TB_ACTION_BUTTON = 4;

//const int NUM_TEXTURE = 6;
const int NUM_LINES = 10;

//Game Scenes
const string SCENE_INTRO = "INTRO";
const string SCENE_LIGHTEN = "LIGHT";
const string SCENE_BEDROOM = "./res/png/bedroom.png";
const string SCENE_LIVINGROOM = "./res/png/livingroom.png";
const string SCENE_ENDDOOR = "./res/png/enddoor.png";
const string SCENE_ENDROOM = "ENDROOM";


//Textures
const string TEX_DOOR = "./res/png/door.png";
const string TEX_SINK = "./res/png/sink.png";
const string TEX_CHILDSLEEP = "./res/png/childsleep.png";
const string TEX_WINDOW = "./res/png/basementwindow.png";
const string TEX_FRIDGE = "./res/png/fridge.png";
const string TEX_MANBED = "./res/png/manbed.png";
const string TEX_TOILET = "./res/png/toilet.png";
const string TEX_TABLE = "./res/png/table.png";


//Texture Types
const string TTYPE_NORMAL = "NORM";
const string TTYPE_TRANSIT = "TRANSIT";
const string TTYPE_ACTION = "ACTION";
const string TTYPE_NARRATION = "NARRATION";

//Texture Actions
const string TACTION_END = "END";

//TextBox types
const string TBTYPE_NORM = "NORM";
const string TBTYPE_INPUT = "INPUT";

//Storylines

const string LINE_0 = "A man wakes up for work";
const string LINE_1 = "His child still sleeps";
const string LINE_2 = "It is necessary to be presentable";
const string LINE_3 = "Breakfast is the most important part of the day";
const string LINE_4 = "The weather is nice outside";
const string LINE_5 = "He goes to bed not thinking of work";
const string LINE_6 = "As he leaves for work his child wakes in an empty home";

//AudioClips
const int NUM_AUDIO_CLIPS = 7;
const string AUDIO_Am = "./res/music/am.wav";
const string AUDIO_C = "./res/music/c.wav";
const string AUDIO_D = "./res/music/d.wav";
const string AUDIO_Dm = "./res/music/dm.wav";
const string AUDIO_Em = "./res/music/em.wav";
const string AUDIO_F = "./res/music/f.wav";
const string AUDIO_G = "./res/music/g.wav";

//Declared as extern so that it's shared between files
extern bool ShowDebug;

struct Texture
{
    bool mActive;
    int mX;
    int mY;
    int mW;
    int mH;
    int mRotation;
    int mAlpha;
    SDL_Texture *mTexture;
    SDL_Point *mCenter;
    SDL_RendererFlip mFlip;

    SDL_Rect mColBoxOffset;

    string mType;
    string mName;
    string mButtonText;
    string mNarration;

    Uint32 mLastUpdate;
    bool mInteract;
};

struct SpriteSheet 
{
    bool mActive;
    bool mInteract;
    bool mUpdate;
    SDL_Texture *mTexture;

    //Where to render the SpriteSheet
    //The w, and h
    SDL_Rect mDstRect;

    SDL_Rect mColBoxOffset;
    float mX;
    float mY;

    int mXOffset;
    int mWoffset;
    
    int mNumFrames;
    int mCurFrame;

    double mRotation;
    int mAlpha;
    SDL_Point *mCenter;
    SDL_RendererFlip mFlip;


    string mType;
    string mName;
    string mButtonText;
    string mNarration;

    bool mHideNarration;

    //handling frame updates
    Uint32 mLastUpdate;
    int mUpdateInterval;
    
};

struct vect2
{
    float x;
    float y;
};

struct AudioClip
{
    bool mActive;
    string wavPath;
    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8 *wavBuffer;
};

struct TextBox
{
    bool mActive;
    string mType;
    SDL_Texture *mFontTex;
    int mFontW;
    int mFontH;

    SDL_Texture *mBoxTex;
    int mBoxDim;
    int mBoxMargin;

    //0 for inactive
    //1 for hovered over
    //2 for pressed
    int mState;

    string mText;

    int mX;
    int mY;


    //Denotes the number of characters that should be displayed 
    int mColumn;

    //Depends on mColumn, set when InitTextBox called
    int mRow;

    Uint32 mStartTime;
    //If it's not set to 0, then after a duration set to inactive;
    Uint32 mDuration;

    Uint32 mLastUpdate;



    //if speed == -1, then Don't need to stagger render
    int mSpeed;
    int mNumRendered;
};

struct TextLine
{
    SDL_Texture *mFontTex;
    int mFontW;
    int mFontH;
    string mText;
    int mX, mY;
    int mAlpha;

    string mAudioPath;
    bool mPlayedAudio;

    Uint32 mStartTime;
    Uint32 mDelay;
};


struct LevelInfo
{
    bool mActive;
    Texture mLevelTex;
    SDL_Point mInitPlayerPos;

};


struct GameState
{
    string SceneCurrent;
    string SceneNext;

    bool darken;
    SDL_Color screenColor;
    SDL_Window *window;
    SDL_Renderer *renderer;


    //Audio
    SDL_AudioDeviceID audioDevice;

    //Used for when two sounds are played at once
    AudioClip curSound;

    AudioClip acArray[NUM_AUDIO_CLIPS];
    AudioClip chordDm;
    AudioClip chordD;
    AudioClip chordG;
    AudioClip WalkMus;
    //SpriteSheet manSheet;


    string startButtonText;
    string PlayerState;
    float playerSpeed;
    
    //Time controls
    Uint32 startTime;
    Uint32 curTime;

    //SDL Textures
    SDL_Texture *fontTexture;
    SDL_Texture *mainBoxTexture;

    //Level specific content
    Texture blackTex;
    LevelInfo lInfo;

    SpriteSheet ssPlayer;
    SpriteSheet ssPlayerAction;
    int actionCol;

    SpriteSheet ssArray[NUM_SPRITESHEET];
    TextBox tbArray[NUM_TEXTBOX];

    int NarrativeCounter;

    int curStory;
    TextLine sStory[NUM_LINES];
};


#endif // GAMEDEFS_H

