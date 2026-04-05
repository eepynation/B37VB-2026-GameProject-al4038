/**
 * Main game source for ESCAPE EVIL LEARY.
 * Keeps the menu flow, map logic, dialogue, movement,
 * combat, and final fight in one file.
 */

#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

const int screenWidth = 1920;
const int screenHeight = 1080;

/* Tile and sprite scale */
#define TILE_SIZE 16
#define TILE_SCALE 4.0f
#define DRAW_TILE_SIZE ((int)(TILE_SIZE * TILE_SCALE))

/* Shared counts */
#define DIRT_ROWS 1
#define PLAYER_IDLE_FRAMES 9
#define PLAYER_WALK_FRAMES 12
#define PLAYER_ATTACK_FRAMES 8
#define PLAYER_BOOK_FRAMES 10
#define MAP_THEME_COUNT 3

/* World layout */
#define VISIBLE_COLUMNS (1920 / DRAW_TILE_SIZE)
#define HOLE_WIDTH 10
#define GROUND_RAISE_PIXELS 3
#define DECOR_SCALE 4.0f
#define DECOR_BASE_OFFSET_Y 65.0f
#define TREE_CENTER_X_RATIO 0.8f
#define TREE_CENTER_X_OFFSET -20.0f
#define FLOATING_PLATFORM_WIDTH_FACTOR 0.90f
#define FLOATING_PLATFORM_WIDTH_OFFSET 340.0f
#define FLOATING_PLATFORM_BOTTOM_OFFSET_TILES 0.20f
#define MAP_TRANSITION_SPEED 3200.0f
#define MAP_TRANSITION_SLICE_HEIGHT 6
/* Arena and final fight flow */
#define FIRST_ARENA_TRIGGER_X_RATIO 0.4f
#define FIRST_ARENA_KIERAN_START_X -220.0f
#define FIRST_ARENA_KIERAN_STOP_X 140.0f
#define FINAL_FIGHT_STAGE_1 1
#define FINAL_FIGHT_STAGE_2 2
#define FINAL_FIGHT_STAGE1_KIERAN_START_X -220.0f
#define FINAL_FIGHT_STAGE1_KIERAN_STOP_X 260.0f
#define FINAL_FIGHT_PLAYER_START_X_RATIO 0.960f
#define FINAL_FIGHT_DIALOG_PHASE_INTRO 0
#define FINAL_FIGHT_DIALOG_PHASE_FURY 1
#define FINAL_FIGHT_DIALOG_PHASE_SAFETY 2
#define FINAL_FIGHT_DIALOG_PHASE_TRAP 3
/* Final fight platform phase */
#define FINAL_FIGHT_LIFT_SPEED 180.0f
#define FINAL_FIGHT_LIFT_HEIGHT 420.0f
#define FINAL_FIGHT_FLOOR_DROP_RATIO 0.65f
#define FINAL_FIGHT_SAFETY_PLATFORM_SCALE 3.0f
#define FINAL_FIGHT_SAFETY_PLATFORM_DROP_SPEED 260.0f
#define FINAL_FIGHT_SAFETY_PLATFORM_PULL_SPEED 220.0f
#define FINAL_FIGHT_SAFETY_PLATFORM_TARGET_OFFSET 180.0f
#define FINAL_FIGHT_SAFETY_PLATFORM_PULL_MARGIN 24.0f
#define FINAL_FIGHT_FALLING_PLATFORM_COUNT 9
#define FINAL_FIGHT_FALLING_PLATFORM_SCALE 3.0f
#define FINAL_FIGHT_FALLING_PLATFORM_SPEED 210.0f
#define FINAL_FIGHT_PLATFORM_FREEZE_TIME 1.5f
#define FINAL_FIGHT_FALLING_PLATFORM_RESPAWN_MIN_GAP 90.0f
#define FINAL_FIGHT_FALLING_PLATFORM_RESPAWN_MAX_GAP 160.0f
#define FINAL_FIGHT_KIERAN_RESPAWN_MARGIN 280.0f
#define FINAL_FIGHT_STAGE2_KIERAN_SPEED 165.0f
#define FINAL_FIGHT_STAGE2_KIERAN_JUMP 780.0f
#define FINAL_FIGHT_STAGE2_PLAYER_JUMP 1220.0f
/* Dialogue and combat timing */
#define FIRST_ARENA_DIALOG_SPEED 42.0f
#define FINAL_FIGHT_DIALOG_SPEED 42.0f
#define FIRST_ARENA_OUTRO_TARGET_MARGIN 48.0f
#define PLAYER_ATTACK_SPEED 14.0f
#define PLAYER_ATTACK_COOLDOWN 2.5f
#define PLAYER_BOOK_SPEED 760.0f
#define PLAYER_BOOK_LIFT 140.0f
#define PLAYER_BOOK_GRAVITY 900.0f
#define PLAYER_BOOK_SCALE 3.0f
#define PLAYER_BOOK_THROW_FRAME 3
#define PLAYER_BOOK_DESPAWN_MARGIN 220.0f
#define KIERAN_BOOK_FREEZE_DURATION 2.0f
#define OPENING_INTRO_DURATION 1.6f
#define KIERAN_MAX_HEALTH 50
#define PLAYER_BOOK_DAMAGE 5
#define FINAL_FIGHT_BOOK_SPEED 1450.0f
#define FINAL_FIGHT_BOOK_LIFT 40.0f
#define FINAL_FIGHT_BOOK_GRAVITY 420.0f
#define FINAL_FIGHT_PLATFORM_BOB_AMPLITUDE 90.0f
#define FINAL_FIGHT_PLATFORM_BOB_SPEED 1.8f
#define OPENING_KIERAN_START_X -220.0f
#define OPENING_KIERAN_STOP_X 180.0f
#define OPENING_KIERAN_WALK_SPEED 180.0f
#define OPENING_ESCAPE_KIERAN_SPEED_SCALE 0.72f
#define OPENING_ESCAPE_MIN_GAP 260.0f

/* Dialog text */
static const char *FIRST_ARENA_DIALOG_KIERAN_LINE_1 =
    "Kieran: You have gotten away this time, but I will find";
static const char *FIRST_ARENA_DIALOG_KIERAN_LINE_2 =
    "you, and you will hand in your project.";
static const char *FIRST_ARENA_DIALOG_PLAYER_LINE =
    "Player: yeah yeah whatever";
static const char *FINAL_FIGHT_DIALOG_KIERAN_LINE_1 =
    "Kieran: I told you I would find you, and this time there is no where to run.";
static const char *FINAL_FIGHT_DIALOG_KIERAN_LINE_2 =
    "You will hand in your work or you will die with a failed grade.";
static const char *FINAL_FIGHT_DIALOG_PLAYER_LINE =
    "Player: How did you find me???? I've told you I didn't do your pathetic assignment, now you will be defeated by your own assignment";
static const char *FINAL_FIGHT_DIALOG_FURY_LINE =
    "Kieran: NOOOOOOO, NOW PERISH YOU FOOL";
static const char *FINAL_FIGHT_DIALOG_SAFETY_LINE =
    "QUICKLY, JUMP TO SAFETY";
static const char *FINAL_FIGHT_DIALOG_TRAP_LINE =
    "Kieran: you insignificent fool, YOU CANT RUN";
static const char *OPENING_DIALOG_KIERAN_LINE_1 =
    "Kieran: There you are at last.";
static const char *OPENING_DIALOG_KIERAN_LINE_2 =
    "Hand in your project right now.";
static const char *OPENING_DIALOG_PLAYER_LINE =
    "Player: I forgot to do it, soz.";
static const char *OPENING_DIALOG_2_KIERAN_LINE_1 =
    "Kieran: WHAT???";
static const char *OPENING_DIALOG_2_KIERAN_LINE_2 =
    "*starts to twitch with rage* NOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO YOU WILL HAND IT IN NOW";
static const char *OPENING_DIALOG_2_PLAYER_LINE =
    "Player: Uhm, no bye.";

/* Texture slots */
typedef enum {
    TEXTURE_GRASS_1 = 0,
    TEXTURE_GRASS_2,
    TEXTURE_GRASS_3,
    TEXTURE_GRASS_4,
    TEXTURE_PLATFORM_TOP,
    TEXTURE_PLATFORM_SECOND,
    TEXTURE_PLATFORM_MIDDLE,
    TEXTURE_PLATFORM_ARENA,
    TEXTURE_BACKGROUND,
    TEXTURE_ARENA_BACKGROUND,
    TEXTURE_TREE,
    TEXTURE_BUSH_1,
    TEXTURE_BUSH_2,
    TEXTURE_FLOWER_1,
    TEXTURE_FLOWER_2,
    TEXTURE_FENCE,
    TEXTURE_STONE_BRICK_1,
    TEXTURE_SIGN_RIGHT,
    TEXTURE_DIALOG_1,
    TEXTURE_KIERAN_IDLE,
    TEXTURE_KIERAN_RUN_1,
    TEXTURE_KIERAN_RUN_2,
    TEXTURE_KIERAN_JUMP,
    TEXTURE_KIERAN_FURY,
    TEXTURE_UNDER_1,
    TEXTURE_UNDER_2,
    TEXTURE_UNDER_3,
    TEXTURE_UNDER_4,
    TEXTURE_CORNER_LEFT,
    TEXTURE_CORNER_RIGHT,
    TEXTURE_CLOSE_CORNER,
    TEXTURE_UNDER_CLOSE_CORNER,
    TEXTURE_UNDER_CORNER_LEFT,
    TEXTURE_UNDER_CORNER_RIGHT,
    TEXTURE_BRICK_LEFT_CORNER,
    TEXTURE_BRICK_TILE,
    TEXTURE_BRICK_RIGHT_CORNER,
    TEXTURE_BRICK_LEFT_CORNER_UNDER,
    TEXTURE_BRICK_TILE_UNDER,
    TEXTURE_BRICK_RIGHT_CORNER_UNDER,
    TEXTURE_COUNT
} texture_asset;

/* Core data */
Texture2D textures[TEXTURE_COUNT];
Texture2D girlPlayerIdleFrames[PLAYER_IDLE_FRAMES];
Texture2D girlPlayerWalkFrames[PLAYER_WALK_FRAMES];
Texture2D girlPlayerAttackFrames[PLAYER_ATTACK_FRAMES];
Texture2D girlPlayerBookFrames[PLAYER_BOOK_FRAMES];
int groundGrassType[VISIBLE_COLUMNS];
int undergroundType[VISIBLE_COLUMNS];

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 spawn;
    float width;
    float height;
    float speed;
    float jumpVelocity;
    float targetDrawHeight;
    bool onGround;
    bool facingLeft;
} Actor;

typedef struct {
    Vector2 center;
    Vector2 velocity;
    float animationTime;
    bool active;
    bool stopped;
    bool facingLeft;
    int stoppedFrame;
} BookProjectile;

typedef struct {
    Vector2 position;
    int textureAsset;
    bool frozen;
    float freezeTimer;
} FallingPlatform;

typedef enum {
    TRANSITION_NONE = 0,
    TRANSITION_CLOSING,
    TRANSITION_HOLD,
    TRANSITION_OPENING
} TransitionState;

typedef enum {
    MENU_TAB_START = 0,
    MENU_TAB_CONTROLS,
    MENU_TAB_GUIDE
} StartMenuTab;

/* Global state */
Actor player;
Actor kieran;
BookProjectile playerBook;

/* Player combat state */
bool chaseStarted = false;
bool playerAttackActive = false;
bool playerAttackThrewBook = false;
float playerAttackTime = 0.0f;
float playerAttackCooldownTimer = 0.0f;

/* First arena state */
bool firstArenaRunInStarted = false;
bool firstArenaRunInFinished = false;
bool firstArenaPlayerReplyVisible = false;
float firstArenaDialogCharCount = 0.0f;
float firstArenaPlayerDialogCharCount = 0.0f;
bool firstArenaOutroWalkStarted = false;
bool firstArenaOutroTransitionStarted = false;

/* Final fight state */
bool finalFightDialogActive = false;
bool finalFightPlayerReplyVisible = false;
float finalFightDialogCharCount = 0.0f;
float finalFightPlayerDialogCharCount = 0.0f;
float finalFightFuryDialogCharCount = 0.0f;
float finalFightSafetyDialogCharCount = 0.0f;
float finalFightTrapDialogCharCount = 0.0f;
int finalFightDialogPhase = FINAL_FIGHT_DIALOG_PHASE_INTRO;
bool finalFightFuryMode = false;
bool finalFightLiftActive = false;
float finalFightLiftOffset = 0.0f;
bool finalFightLiftColumns[64] = {0};
int finalFightLiftPlatformStarts[2] = {0, 0};
int finalFightLiftPlatformCount = 0;
float finalFightLiftPlatformOffsetX[2] = {0.0f, 0.0f};
int finalFightPlayerLiftPlatformIndex = -1;
int finalFightKieranLiftPlatformIndex = -1;
bool finalFightSafetyPlatformActive = false;
bool finalFightSafetyPlatformPulling = false;
bool finalFightSafetyDialogShown = false;
bool finalFightPlayerReachedSafety = false;
Vector2 finalFightSafetyPlatformPosition = {0.0f, 0.0f};
float finalFightSafetyPlatformTargetY = 0.0f;
int finalFightStage = FINAL_FIGHT_STAGE_1;
bool finalFightStage1RunInFinished = false;
FallingPlatform finalFightFallingPlatforms[FINAL_FIGHT_FALLING_PLATFORM_COUNT];
bool finalFightStage2Started = false;
float kieranFreezeTimer = 0.0f;
int kieranHealth = KIERAN_MAX_HEALTH;
float finalFightTimeRemaining = 0.0f;
int playerStage2PlatformIndex = -1;
int kieranStage2PlatformIndex = -1;

/* Start menu and opening scene state */
bool startMenuActive = true;
StartMenuTab startMenuTab = MENU_TAB_START;
bool openingIntroActive = false;
float openingIntroTimer = 0.0f;
bool openingMapRunInStarted = false;
bool openingMapRunInFinished = false;
bool openingMapDialogActive = false;
bool openingMapPlayerReplyVisible = false;
float openingMapDialogCharCount = 0.0f;
float openingMapPlayerDialogCharCount = 0.0f;
int openingMapDialogPage = 0;
bool openingMapEscapeActive = false;
bool openingMapEscapeJumped = false;

/* Shared transition state */
int currentMapIndex = 0;
TransitionState transitionState = TRANSITION_NONE;
float transitionRadius = 0.0f;
bool transitionShowEightHoursLater = false;
bool transitionShowGameOver = false;
bool transitionShowVictory = false;
Vector2 transitionCenter = {0.0f, 0.0f};

/* Forward declarations */
/* Loading and reset helpers */
void LoadGameTextures(void);
void GenerateGround(void);
void InitActors(void);
void LoadNextMap(void);
void RestartCurrentMap(void);
void ResetActor(Actor *actor);
void ResetPlayerCombatState(void);
void ResetPlayerBookState(void);
void ResetOpeningSceneState(bool isOpeningMap);
void ResetFirstArenaState(bool isFirstArena);
void ResetFinalFightState(bool isFinalFightStage1);
void ResetFinalFightPlatforms(void);
void LoadFrameStrip(Texture2D *frames, int frameCount, const char *pattern);
void SetFrameStripFilter(Texture2D *frames, int frameCount);
void UnloadFrameStrip(Texture2D *frames, int frameCount);

/* Layout helpers */
bool IsFirstArenaMap(void);
bool IsFinalFightMap(void);
bool IsFinalFightStage1(void);
bool IsFinalFightStage2(void);
float GetGroundTopY(void);
float GetDecorBaseY(void);
int GetHoleStartColumn(void);
float GetTreeCenterX(void);
float GetLeftBush1X(void);
float GetLeftBush2X(void);
float GetStoneBrickX(void);
float GetSignRightX(void);
void GetHoleBounds(float *left, float *right);
Texture2D GetCurrentPlatformTexture(void);
Rectangle GetFloatingPlatformRect(void);
Rectangle GetFinalFightBrickPlatformRect(void);
Rectangle GetFinalFightLiftPlatformRect(int platformIndex);
Rectangle GetFinalFightSafetyPlatformRect(void);
Texture2D GetFinalFightFallingPlatformTexture(int platformIndex);
Rectangle GetFinalFightFallingPlatformRect(int platformIndex);
int GetActorStage2PlatformIndex(const Actor *actor);
float GetFloatingPlatformSurfaceY(void);
float GetFinalFightSafetyPlatformSurfaceY(void);
Rectangle GetActorBounds(const Actor *actor);
float AbsFloat(float value);

/* Dialogue and transition helpers */
void CopyVisibleText(char *dest, int destSize, const char *source, int visibleChars);
void WrapDialogText(char *line1, int line1Size, char *line2, int line2Size, const char *source,
                    int visibleChars, Font font, float fontSize, float spacing, float maxWidth);
void DrawDialogText(Font font, const char *text, Vector2 position, float fontSize, float spacing,
                    Color color);
float GetMaxTransitionRadius(void);
void StartFinalFightStage2(void);
void ClearFinalFightLiftColumns(void);
bool IsFinalFightLiftColumn(int column);
void MarkFinalFightLiftColumns(float centerX);
bool IsActorStandingOnRect(const Actor *actor, Rectangle rect, float surfaceY);
void InitFinalFightSafetyPlatform(void);
void StartFinalFightLift(void);
void UpdateFinalFightLift(void);
void InitFinalFightStage2Platforms(void);
void StartGameOverTransition(void);
void StartVictoryTransition(void);
void UpdateFinalFightStage2Platforms(void);
void UpdateFinalFightTimer(void);
void StartMapTransition(void);
void UpdateMapTransition(void);

/* Animation and texture lookup */
Texture2D GetGrassTexture(int variant);
Texture2D GetUndergroundTexture(int variant);
Texture2D GetPlayerFrame(void);
Texture2D GetKieranFrame(void);
Texture2D GetPlayerBookTexture(int frameIndex);
int GetPlayerBookFrameIndex(void);
void DrawProp(Texture2D texture, float leftX, float baseY, float scale);
void DrawPropCentered(Texture2D texture, float centerX, float baseY, float scale);
void DrawActorSprite(Texture2D texture, const Actor *actor);
Rectangle GetTreeBounds(void);
Rectangle GetPlayerBookBounds(void);
void LaunchPlayerBook(void);
void FreezePlayerBook(int frameIndex);
void FreezePlayerBookOnSurface(float surfaceY, int frameIndex);

/* Actor updates */
void UpdateActorPhysics(Actor *actor);
void ResolveActorCollisions(Actor *actor, Vector2 previousPosition);
void UpdateFirstArenaDialog(void);
void UpdateFinalFightDialog(void);
void UpdatePlayer(void);
void UpdatePlayerBook(void);
void UpdateKieran(void);

/* Rendering */
void DrawBackgroundFull(void);
void DrawGround(void);
void DrawFloatingPlatform(void);
void DrawBush(void);
void DrawLeftBushes(void);
void DrawFlower(void);
void DrawFence(void);
void DrawStoneBrick(void);
void DrawSignRight(void);
void DrawFinalFightLiftPlatform(int platformIndex);
void DrawFinalFightStage1FloorColumn(int column, float yOffset);
void DrawFinalFightStage1Floor(void);
void DrawFinalFightSafetyPlatform(void);
void DrawFinalFightStage2Platforms(void);
void DrawFinalFightBrickPlatform(void);
void DrawFirstArenaPlatforms(void);
void DrawFirstArenaTree(void);
void DrawFirstArenaBricks(void);
void DrawDialogBox(void);
void DrawFinalFightDialogBox(void);
void DrawPlayer(void);
void DrawPlayerBook(void);
void DrawKieran(void);
void DrawKieranHealthBar(void);
void DrawFinalFightTimer(void);
void DrawTree(void);
void DrawMapTransition(void);
void UpdateStartMenu(void);
void DrawStartMenu(void);
void UpdateOpeningIntro(void);
void UpdateOpeningMapDialog(void);
bool UseOpeningIntro(void);
float GetOpeningIntroProgress(float delay);
Vector2 GetOpeningIntroGroundOffset(int column);
Vector2 GetOpeningIntroTextureOffset(Texture2D texture);
Vector2 GetOpeningIntroActorOffset(const Actor *actor);
void DrawOpeningIntroOverlay(void);
void DrawOpeningDialogBox(void);
void UnloadGameTextures(void);

/* Main loop */
int main(void)
{
    InitWindow(screenWidth, screenHeight, "EVIL LEARY");
    SetTargetFPS(60);

    srand((unsigned int)time(NULL));

    LoadGameTextures();
    GenerateGround();
    finalFightStage = FINAL_FIGHT_STAGE_1;
    InitActors();
    transitionCenter = (Vector2){screenWidth * 0.5f, screenHeight * 0.5f};
    transitionRadius = GetMaxTransitionRadius();

    while (!WindowShouldClose())
    {
        if (startMenuActive)
        {
            UpdateStartMenu();
        }
        else if (openingIntroActive)
        {
            UpdateOpeningIntro();
        }
        else if (transitionState == TRANSITION_NONE)
        {
            UpdatePlayer();
            UpdateKieran();
            UpdatePlayerBook();
            UpdateOpeningMapDialog();
            UpdateFirstArenaDialog();
            UpdateFinalFightDialog();
            UpdateFinalFightLift();
            UpdateFinalFightStage2Platforms();
            UpdateFinalFightTimer();

            if (!firstArenaOutroWalkStarted &&
                player.position.x + player.width >= (float)screenWidth - 2.0f)
            {
                transitionShowEightHoursLater = false;
                StartMapTransition();
            }

            if (transitionState == TRANSITION_NONE &&
                !(currentMapIndex == 0 && openingMapEscapeActive) &&
                CheckCollisionRecs(GetActorBounds(&player), GetActorBounds(&kieran)))
            {
                if (IsFinalFightStage2())
                {
                    StartGameOverTransition();
                }
                else
                {
                    ResetActor(&player);
                    ResetActor(&kieran);
                }
            }
        }
        else
        {
            UpdateMapTransition();
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (startMenuActive)
        {
            DrawStartMenu();
        }
        else
        {
            DrawBackgroundFull();

            if (!IsFinalFightMap())
            {
                DrawGround();

                if (IsFirstArenaMap())
                {
                    DrawFirstArenaPlatforms();
                    DrawFirstArenaTree();
                    DrawFirstArenaBricks();
                }
                else
                {
                    DrawStoneBrick();
                    DrawLeftBushes();
                    DrawBush();
                    DrawFlower();
                    DrawTree();
                    DrawFence();
                    DrawSignRight();
                }
            }
            else
            {
                if (IsFinalFightStage1())
                {
                    DrawFinalFightStage1Floor();
                    DrawFinalFightSafetyPlatform();
                }
                else
                {
                    DrawFinalFightBrickPlatform();
                    DrawFinalFightStage2Platforms();
                }
            }

            if (!IsFirstArenaMap() && !IsFinalFightMap())
            {
                DrawFloatingPlatform();
            }
            DrawPlayer();
            DrawKieran();
            DrawKieranHealthBar();
            DrawFinalFightTimer();
            DrawPlayerBook();
            DrawOpeningDialogBox();
            DrawDialogBox();
            DrawFinalFightDialogBox();
            if (!(IsFirstArenaMap() && firstArenaRunInFinished) &&
                !(currentMapIndex == 0 && !chaseStarted) &&
                !finalFightDialogActive &&
                !IsFinalFightStage1() &&
                !openingIntroActive)
            {
                DrawText("Move: D or Right Arrow   Jump: Space/W/Up   Attack: E", 24, 24, 26, WHITE);
                DrawText("Run from Kieran", 24, 56, 24, (Color){255, 220, 120, 255});
            }
            DrawOpeningIntroOverlay();
        }
        DrawMapTransition();

        EndDrawing();
    }

    UnloadGameTextures();
    CloseWindow();
    return 0;
}

/* Loading textures and initial setup */
void LoadGameTextures(void)
{
    // Tile textures
    textures[TEXTURE_GRASS_1] = LoadTexture("textures/tile_1_grass.png");
    textures[TEXTURE_GRASS_2] = LoadTexture("textures/tile_2_grass.png");
    textures[TEXTURE_GRASS_3] = LoadTexture("textures/tile_3_grass.png");
    textures[TEXTURE_GRASS_4] = LoadTexture("textures/tile_4_grass.png");

    // Platform and background textures
    textures[TEXTURE_PLATFORM_TOP] = LoadTexture("textures/platform_1.png");
    textures[TEXTURE_PLATFORM_SECOND] = LoadTexture("textures/platform_2.png");
    textures[TEXTURE_PLATFORM_MIDDLE] = LoadTexture("textures/platform_2.png");
    textures[TEXTURE_PLATFORM_ARENA] = LoadTexture("textures/platform_4.png");
    textures[TEXTURE_BACKGROUND] = LoadTexture("textures/background.png");
    textures[TEXTURE_ARENA_BACKGROUND] = LoadTexture("textures/arena_background.png");

    // Environment textures
    textures[TEXTURE_TREE] = LoadTexture("textures/tree.png");
    textures[TEXTURE_BUSH_1] = LoadTexture("textures/bush_1.png");
    textures[TEXTURE_BUSH_2] = LoadTexture("textures/bush_2.png");
    textures[TEXTURE_FLOWER_1] = LoadTexture("textures/flower_1_animation.png");
    textures[TEXTURE_FLOWER_2] = LoadTexture("textures/flower_2_animation.png");
    textures[TEXTURE_FENCE] = LoadTexture("textures/fence_1.png");
    textures[TEXTURE_STONE_BRICK_1] = LoadTexture("textures/stone_brick_1.png");
    textures[TEXTURE_SIGN_RIGHT] = LoadTexture("textures/arrow_sign_right.png");
    textures[TEXTURE_DIALOG_1] = LoadTexture("textures/dialog_box_1.png");

    // Kieran textures
    textures[TEXTURE_KIERAN_IDLE] = LoadTexture("textures/kieran/KIERAN_1_IDLE.png");
    textures[TEXTURE_KIERAN_RUN_1] = LoadTexture("textures/kieran/KIERAN_RUN_LEFTFOOTBACK.png");
    textures[TEXTURE_KIERAN_RUN_2] = LoadTexture("textures/kieran/KIERAN_RUN_RIGHTFOOTBACK.png");
    textures[TEXTURE_KIERAN_JUMP] = LoadTexture("textures/kieran/KIERAN_JUMP.png");
    textures[TEXTURE_KIERAN_FURY] = LoadTexture("textures/kieran/KIERAN_FURY.png");

    // Player textures
    LoadFrameStrip(girlPlayerIdleFrames, PLAYER_IDLE_FRAMES,
                   "textures/Girl_Player/idle_%d.png");
    LoadFrameStrip(girlPlayerWalkFrames, PLAYER_WALK_FRAMES,
                   "textures/Girl_Player/walk_%d.png");
    LoadFrameStrip(girlPlayerAttackFrames, PLAYER_ATTACK_FRAMES,
                   "textures/Girl_Player/attack_%d.png");
    LoadFrameStrip(girlPlayerBookFrames, PLAYER_BOOK_FRAMES,
                   "textures/Girl_Player/book_%d.png");

    // Underground and edge textures
    textures[TEXTURE_UNDER_1] = LoadTexture("textures/close_underground_1.png");
    textures[TEXTURE_UNDER_2] = LoadTexture("textures/close_underground_2.png");
    textures[TEXTURE_UNDER_3] = LoadTexture("textures/close_underground_3.png");
    textures[TEXTURE_UNDER_4] = LoadTexture("textures/close_underground_4.png");

    textures[TEXTURE_CORNER_LEFT] = LoadTexture("textures/tile_corner_left.png");
    textures[TEXTURE_CORNER_RIGHT] = LoadTexture("textures/tile_corner_right.png");
    textures[TEXTURE_CLOSE_CORNER] = LoadTexture("textures/tile_close_corner.png");
    textures[TEXTURE_UNDER_CLOSE_CORNER] = LoadTexture("textures/underground_close_corner.png");

    textures[TEXTURE_UNDER_CORNER_LEFT] = LoadTexture("textures/underground_corner_left.png");
    textures[TEXTURE_UNDER_CORNER_RIGHT] = LoadTexture("textures/underground_corner_right.png");
    textures[TEXTURE_BRICK_LEFT_CORNER] = LoadTexture("textures/brick_left_corner.png");
    textures[TEXTURE_BRICK_TILE] = LoadTexture("textures/brick_tile.png");
    textures[TEXTURE_BRICK_RIGHT_CORNER] = LoadTexture("textures/brick_right_corner.png");
    textures[TEXTURE_BRICK_LEFT_CORNER_UNDER] = LoadTexture("textures/brick_left_corner_under.png");
    textures[TEXTURE_BRICK_TILE_UNDER] = LoadTexture("textures/brick_tile_under.png");
    textures[TEXTURE_BRICK_RIGHT_CORNER_UNDER] = LoadTexture("textures/brick_right_corner_under.png");

    // Texture filtering
    for (int i = 0; i < TEXTURE_COUNT; i++)
    {
        if (textures[i].id != 0)
            SetTextureFilter(textures[i], TEXTURE_FILTER_POINT);
    }

    SetFrameStripFilter(girlPlayerIdleFrames, PLAYER_IDLE_FRAMES);
    SetFrameStripFilter(girlPlayerWalkFrames, PLAYER_WALK_FRAMES);
    SetFrameStripFilter(girlPlayerAttackFrames, PLAYER_ATTACK_FRAMES);
    SetFrameStripFilter(girlPlayerBookFrames, PLAYER_BOOK_FRAMES);
}

void GenerateGround(void)
{
    for (int i = 0; i < VISIBLE_COLUMNS; i++)
    {
        groundGrassType[i] = rand() % 4;
        undergroundType[i] = rand() % 4;
    }
}

void LoadFrameStrip(Texture2D *frames, int frameCount, const char *pattern)
{
    int i;

    for (i = 0; i < frameCount; i++)
        frames[i] = LoadTexture(TextFormat(pattern, i + 1));
}

void SetFrameStripFilter(Texture2D *frames, int frameCount)
{
    int i;

    for (i = 0; i < frameCount; i++)
    {
        if (frames[i].id != 0)
            SetTextureFilter(frames[i], TEXTURE_FILTER_POINT);
    }
}

void UnloadFrameStrip(Texture2D *frames, int frameCount)
{
    int i;

    for (i = 0; i < frameCount; i++)
    {
        if (frames[i].id != 0)
            UnloadTexture(frames[i]);
    }
}

/* Shared reset helpers */
void ResetPlayerCombatState(void)
{
    playerAttackActive = false;
    playerAttackThrewBook = false;
    playerAttackTime = 0.0f;
    playerAttackCooldownTimer = 0.0f;
}

void ResetPlayerBookState(void)
{
    playerBook.active = false;
    playerBook.stopped = false;
    playerBook.animationTime = 0.0f;
    playerBook.center = (Vector2){0.0f, 0.0f};
    playerBook.velocity = (Vector2){0.0f, 0.0f};
    playerBook.facingLeft = false;
    playerBook.stoppedFrame = 0;
}

/* Map-specific state resets */
void ResetOpeningSceneState(bool isOpeningMap)
{
    openingMapRunInStarted = !isOpeningMap;
    openingMapRunInFinished = !isOpeningMap;
    openingMapDialogActive = false;
    openingMapPlayerReplyVisible = false;
    openingMapDialogCharCount = 0.0f;
    openingMapPlayerDialogCharCount = 0.0f;
    openingMapDialogPage = 0;
    openingMapEscapeActive = false;
    openingMapEscapeJumped = false;
}

void ResetFirstArenaState(bool isFirstArena)
{
    firstArenaRunInStarted = false;
    firstArenaRunInFinished = !isFirstArena;
    firstArenaPlayerReplyVisible = false;
    firstArenaDialogCharCount = 0.0f;
    firstArenaPlayerDialogCharCount = 0.0f;
    firstArenaOutroWalkStarted = false;
    firstArenaOutroTransitionStarted = false;
}

void ResetFinalFightState(bool isFinalFightStage1)
{
    finalFightDialogActive = false;
    finalFightPlayerReplyVisible = false;
    finalFightDialogCharCount = 0.0f;
    finalFightPlayerDialogCharCount = 0.0f;
    finalFightFuryDialogCharCount = 0.0f;
    finalFightSafetyDialogCharCount = 0.0f;
    finalFightTrapDialogCharCount = 0.0f;
    finalFightDialogPhase = FINAL_FIGHT_DIALOG_PHASE_INTRO;
    finalFightFuryMode = false;
    finalFightLiftActive = false;
    finalFightLiftOffset = 0.0f;
    ClearFinalFightLiftColumns();
    finalFightSafetyPlatformActive = false;
    finalFightSafetyPlatformPulling = false;
    finalFightSafetyDialogShown = false;
    finalFightPlayerReachedSafety = false;
    finalFightSafetyPlatformPosition = (Vector2){0.0f, 0.0f};
    finalFightSafetyPlatformTargetY = 0.0f;
    finalFightStage1RunInFinished = !isFinalFightStage1;
    finalFightStage2Started = false;
    kieranFreezeTimer = 0.0f;
    playerStage2PlatformIndex = -1;
    kieranStage2PlatformIndex = -1;
    finalFightTimeRemaining = 0.0f;
}

void ResetFinalFightPlatforms(void)
{
    for (int i = 0; i < FINAL_FIGHT_FALLING_PLATFORM_COUNT; i++)
    {
        finalFightFallingPlatforms[i].position = (Vector2){0.0f, 0.0f};
        finalFightFallingPlatforms[i].textureAsset = TEXTURE_PLATFORM_TOP;
        finalFightFallingPlatforms[i].frozen = false;
        finalFightFallingPlatforms[i].freezeTimer = 0.0f;
    }
}

/* Actor spawn setup */
void InitActors(void)
{
    float groundTop = GetGroundTopY();
    Rectangle platformRect = GetFloatingPlatformRect();
    Rectangle finalFightBrickRect = GetFinalFightBrickPlatformRect();
    float platformTop = GetFloatingPlatformSurfaceY();
    float kieranStartX = 40.0f;
    float actorSpeed = ((currentMapIndex % MAP_THEME_COUNT) == 0) ? 350.0f : 220.0f;

    player.width = 68.0f;
    player.height = 124.0f;
    player.targetDrawHeight = 136.0f;
    if (IsFirstArenaMap())
    {
        player.spawn = (Vector2){220.0f, groundTop - player.height};
    }
    else if (IsFinalFightStage1())
    {
        player.spawn = (Vector2){
            ((float)screenWidth * FINAL_FIGHT_PLAYER_START_X_RATIO) - player.width * 0.5f,
            groundTop - player.height
        };
    }
    else if (IsFinalFightStage2())
    {
        player.spawn = (Vector2){
            platformRect.x + platformRect.width * 0.72f - player.width * 0.5f,
            platformTop - player.height
        };
    }
    else
    {
        player.spawn = (Vector2){
            platformRect.x + platformRect.width * 0.5f - player.width * 0.5f,
            platformTop - player.height
        };
    }
    player.position = player.spawn;
    player.velocity = (Vector2){0.0f, 0.0f};
    player.speed = actorSpeed;
    player.jumpVelocity = 900.0f;
    player.onGround = true;
    player.facingLeft = IsFinalFightMap();

    kieran.width = 42.0f;
    kieran.height = 72.0f;
    if (IsFirstArenaMap())
    {
        kieran.spawn = (Vector2){FIRST_ARENA_KIERAN_START_X, groundTop - kieran.height};
    }
    else if (IsFinalFightStage1())
    {
        kieran.spawn = (Vector2){FINAL_FIGHT_STAGE1_KIERAN_START_X, groundTop - kieran.height};
    }
    else if (IsFinalFightStage2())
    {
        kieran.spawn = (Vector2){
            finalFightBrickRect.x + finalFightBrickRect.width * 0.5f - kieran.width * 0.5f,
            finalFightBrickRect.y - kieran.height
        };
    }
    else
    {
        kieran.spawn = (Vector2){kieranStartX, groundTop - kieran.height};
    }
    kieran.position = kieran.spawn;
    kieran.velocity = (Vector2){0.0f, 0.0f};
    kieran.speed = actorSpeed;
    kieran.jumpVelocity = 1040.0f;
    kieran.targetDrawHeight = 72.0f;
    kieran.onGround = true;
    kieran.facingLeft = false;
    ResetPlayerCombatState();
    ResetPlayerBookState();
    kieranHealth = KIERAN_MAX_HEALTH;
    chaseStarted = false;
    ResetOpeningSceneState(currentMapIndex == 0);
    ResetFirstArenaState(IsFirstArenaMap());
    ResetFinalFightState(IsFinalFightStage1());
    ResetFinalFightPlatforms();

    if (currentMapIndex == 0)
    {
        kieran.position.x = OPENING_KIERAN_START_X;
        kieran.position.y = groundTop - kieran.height;
    }
}

void LoadNextMap(void)
{
    bool keepChasing = chaseStarted;

    currentMapIndex++;
    if (IsFinalFightMap())
    {
        finalFightStage = FINAL_FIGHT_STAGE_1;
    }
    GenerateGround();
    InitActors();
    chaseStarted = keepChasing;
}

void RestartCurrentMap(void)
{
    if (IsFinalFightMap())
    {
        finalFightStage = FINAL_FIGHT_STAGE_1;
    }

    GenerateGround();
    InitActors();
}

void ResetActor(Actor *actor)
{
    actor->position = actor->spawn;
    actor->velocity = (Vector2){0.0f, 0.0f};
    actor->onGround = false;

    if (actor == &player)
    {
        ResetPlayerCombatState();
        ResetPlayerBookState();
    }
}

/* Map and layout helpers */
bool IsFinalFightMap(void)
{
    return (currentMapIndex % MAP_THEME_COUNT) == 2;
}

bool IsFinalFightStage1(void)
{
    return IsFinalFightMap() && finalFightStage == FINAL_FIGHT_STAGE_1;
}

bool IsFinalFightStage2(void)
{
    return IsFinalFightMap() && finalFightStage == FINAL_FIGHT_STAGE_2;
}

bool IsFirstArenaMap(void)
{
    return (currentMapIndex % MAP_THEME_COUNT) == 1;
}

float GetGroundTopY(void)
{
    return (float)(screenHeight - (DIRT_ROWS + 1) * DRAW_TILE_SIZE - GROUND_RAISE_PIXELS);
}

float GetDecorBaseY(void)
{
    return GetGroundTopY() + DRAW_TILE_SIZE - DECOR_BASE_OFFSET_Y;
}

int GetHoleStartColumn(void)
{
    switch (currentMapIndex % MAP_THEME_COUNT)
    {
        case 1: return 7;
        case 2: return 13;
        default: return (VISIBLE_COLUMNS - HOLE_WIDTH) / 2;
    }
}

float GetTreeCenterX(void)
{
    switch (currentMapIndex % MAP_THEME_COUNT)
    {
        case 1: return 1640.0f;
        case 2: return 1400.0f;
        default: return screenWidth * TREE_CENTER_X_RATIO + TREE_CENTER_X_OFFSET;
    }
}

float GetLeftBush1X(void)
{
    switch (currentMapIndex % MAP_THEME_COUNT)
    {
        case 1: return 170.0f;
        case 2: return 80.0f;
        default: return 120.0f;
    }
}

float GetLeftBush2X(void)
{
    switch (currentMapIndex % MAP_THEME_COUNT)
    {
        case 1: return 340.0f;
        case 2: return 230.0f;
        default: return 280.0f;
    }
}

float GetStoneBrickX(void)
{
    switch (currentMapIndex % MAP_THEME_COUNT)
    {
        case 1: return 40.0f;
        case 2: return 120.0f;
        default: return -8.0f;
    }
}

float GetSignRightX(void)
{
    switch (currentMapIndex % MAP_THEME_COUNT)
    {
        case 1: return 1740.0f;
        case 2: return 1820.0f;
        default: return 1800.0f;
    }
}

void GetHoleBounds(float *left, float *right)
{
    int holeStart = GetHoleStartColumn();

    *left = (float)(holeStart * DRAW_TILE_SIZE);
    *right = *left + (HOLE_WIDTH * DRAW_TILE_SIZE);
}

Texture2D GetCurrentPlatformTexture(void)
{
    if (IsFirstArenaMap() || IsFinalFightMap())
    {
        Texture2D noPlatform = {0};
        return noPlatform;
    }

    return textures[TEXTURE_PLATFORM_TOP];
}

Rectangle GetFloatingPlatformRect(void)
{
    Rectangle rect = {0.0f, 0.0f, 0.0f, 0.0f};
    Texture2D platformTex = GetCurrentPlatformTexture();

    if (platformTex.id == 0)
    {
        return rect;
    }

    float grassY = GetGroundTopY();
    int holeStart = GetHoleStartColumn();
    float holeCenterX = (holeStart * DRAW_TILE_SIZE) + (HOLE_WIDTH * DRAW_TILE_SIZE) / 2.0f;
    float platformWidth = (HOLE_WIDTH * DRAW_TILE_SIZE) * FLOATING_PLATFORM_WIDTH_FACTOR -
                          FLOATING_PLATFORM_WIDTH_OFFSET;
    float aspectRatio = (float)platformTex.height / (float)platformTex.width;
    float platformHeight = (platformWidth * aspectRatio) ;
    float platformBottomY = grassY - (FLOATING_PLATFORM_BOTTOM_OFFSET_TILES * DRAW_TILE_SIZE);

    rect.width = platformWidth;
    rect.height = platformHeight;
    rect.x = holeCenterX - platformWidth / 2.0f;
    rect.y = platformBottomY - platformHeight + 80.0f;
    return rect;
}

Rectangle GetFinalFightBrickPlatformRect(void)
{
    const float brickScale = TILE_SCALE;
    const float brickSeamOverlap = 4.0f;
    float baseY = 470.0f;
    Texture2D leftTex = textures[TEXTURE_BRICK_LEFT_CORNER];
    Texture2D middleTex = textures[TEXTURE_BRICK_TILE];
    Texture2D rightTex = textures[TEXTURE_BRICK_RIGHT_CORNER];
    Rectangle rect = {0.0f, 0.0f, 0.0f, 0.0f};

    if (!IsFinalFightStage2() || leftTex.id == 0 || middleTex.id == 0 || rightTex.id == 0)
    {
        return rect;
    }

    rect.x = 260.0f;
    rect.y = baseY;

    if (kieranHealth <= (KIERAN_MAX_HEALTH / 2))
    {
        rect.y -= sinf(GetTime() * FINAL_FIGHT_PLATFORM_BOB_SPEED) *
                  FINAL_FIGHT_PLATFORM_BOB_AMPLITUDE;
    }

    rect.width = ((leftTex.width + middleTex.width + rightTex.width) * brickScale) -
                 (brickSeamOverlap * 2.0f);
    rect.height = leftTex.height * brickScale;
    return rect;
}

Rectangle GetFinalFightSafetyPlatformRect(void)
{
    Rectangle rect = {0.0f, 0.0f, 0.0f, 0.0f};
    Texture2D platformTex = textures[TEXTURE_PLATFORM_MIDDLE];

    if (!finalFightSafetyPlatformActive || platformTex.id == 0)
    {
        return rect;
    }

    rect.width = platformTex.width * FINAL_FIGHT_SAFETY_PLATFORM_SCALE;
    rect.height = platformTex.height * FINAL_FIGHT_SAFETY_PLATFORM_SCALE;
    rect.x = finalFightSafetyPlatformPosition.x;
    rect.y = finalFightSafetyPlatformPosition.y;
    return rect;
}

Texture2D GetFinalFightFallingPlatformTexture(int platformIndex)
{
    Texture2D emptyTex = {0};
    int textureAsset = TEXTURE_PLATFORM_TOP;

    if (!IsFinalFightStage2() ||
        platformIndex < 0 ||
        platformIndex >= FINAL_FIGHT_FALLING_PLATFORM_COUNT)
    {
        return emptyTex;
    }

    textureAsset = finalFightFallingPlatforms[platformIndex].textureAsset;

    if (textureAsset < 0 || textureAsset >= TEXTURE_COUNT)
    {
        return emptyTex;
    }

    return textures[textureAsset];
}

Rectangle GetFinalFightFallingPlatformRect(int platformIndex)
{
    Rectangle rect = {0.0f, 0.0f, 0.0f, 0.0f};
    Texture2D platformTex = GetFinalFightFallingPlatformTexture(platformIndex);

    if (platformTex.id == 0)
    {
        return rect;
    }

    rect.width = platformTex.width * FINAL_FIGHT_FALLING_PLATFORM_SCALE;
    rect.height = platformTex.height * FINAL_FIGHT_FALLING_PLATFORM_SCALE;
    rect.x = finalFightFallingPlatforms[platformIndex].position.x;
    rect.y = finalFightFallingPlatforms[platformIndex].position.y;
    return rect;
}

int GetActorStage2PlatformIndex(const Actor *actor)
{
    if (!IsFinalFightStage2())
    {
        return -1;
    }

    for (int i = 0; i < FINAL_FIGHT_FALLING_PLATFORM_COUNT; i++)
    {
        Rectangle platformRect = GetFinalFightFallingPlatformRect(i);

        if (IsActorStandingOnRect(actor, platformRect, platformRect.y))
        {
            return i;
        }
    }

    return -1;
}

float GetFloatingPlatformSurfaceY(void)
{
    Rectangle platformRect = GetFloatingPlatformRect();

    return platformRect.y + platformRect.height * 0.04f;
}

float GetFinalFightSafetyPlatformSurfaceY(void)
{
    Rectangle platformRect = GetFinalFightSafetyPlatformRect();

    return platformRect.y + platformRect.height * 0.08f;
}

Rectangle GetActorBounds(const Actor *actor)
{
    return (Rectangle){actor->position.x, actor->position.y, actor->width, actor->height};
}

float AbsFloat(float value)
{
    return (value < 0.0f) ? -value : value;
}

void CopyVisibleText(char *dest, int destSize, const char *source, int visibleChars)
{
    int i = 0;

    if (destSize <= 0)
    {
        return;
    }

    if (visibleChars < 0)
    {
        visibleChars = 0;
    }

    while (i < destSize - 1 && i < visibleChars && source[i] != '\0')
    {
        dest[i] = source[i];
        i++;
    }

    dest[i] = '\0';
}

void WrapDialogText(char *line1, int line1Size, char *line2, int line2Size, const char *source,
                    int visibleChars, Font font, float fontSize, float spacing, float maxWidth)
{
    char visibleText[256] = {0};
    int length = 0;
    int splitIndex = -1;

    if (line1Size > 0) line1[0] = '\0';
    if (line2Size > 0) line2[0] = '\0';

    CopyVisibleText(visibleText, sizeof(visibleText), source, visibleChars);
    length = TextLength(visibleText);

    if (length == 0)
    {
        return;
    }

    if (MeasureTextEx(font, visibleText, fontSize, spacing).x <= maxWidth)
    {
        CopyVisibleText(line1, line1Size, visibleText, length);
        return;
    }

    for (int i = length; i > 0; i--)
    {
        char savedChar = visibleText[i];

        if (visibleText[i] == ' ')
        {
            visibleText[i] = '\0';

            if (MeasureTextEx(font, visibleText, fontSize, spacing).x <= maxWidth)
            {
                splitIndex = i;
                visibleText[i] = savedChar;
                break;
            }
        }

        visibleText[i] = savedChar;
    }

    if (splitIndex < 0)
    {
        for (int i = length; i > 0; i--)
        {
            char savedChar = visibleText[i];

            visibleText[i] = '\0';

            if (MeasureTextEx(font, visibleText, fontSize, spacing).x <= maxWidth)
            {
                splitIndex = i;
                visibleText[i] = savedChar;
                break;
            }

            visibleText[i] = savedChar;
        }
    }

    if (splitIndex < 0)
    {
        splitIndex = length;
    }

    CopyVisibleText(line1, line1Size, visibleText, splitIndex);

    while (visibleText[splitIndex] == ' ')
    {
        splitIndex++;
    }

    CopyVisibleText(line2, line2Size, visibleText + splitIndex, TextLength(visibleText + splitIndex));
}

void DrawDialogText(Font font, const char *text, Vector2 position, float fontSize, float spacing,
                    Color color)
{
    DrawTextEx(font, text, position, fontSize, spacing, color);
}

float GetMaxTransitionRadius(void)
{
    return sqrtf((float)(screenWidth * screenWidth + screenHeight * screenHeight));
}

/* Final fight setup and transitions */
void ClearFinalFightLiftColumns(void)
{
    for (int i = 0; i < 64; i++)
    {
        finalFightLiftColumns[i] = false;
    }

    finalFightLiftPlatformStarts[0] = 0;
    finalFightLiftPlatformStarts[1] = 0;
    finalFightLiftPlatformCount = 0;
    finalFightLiftPlatformOffsetX[0] = 0.0f;
    finalFightLiftPlatformOffsetX[1] = 0.0f;
    finalFightPlayerLiftPlatformIndex = -1;
    finalFightKieranLiftPlatformIndex = -1;
}

bool IsFinalFightLiftColumn(int column)
{
    return column >= 0 && column < VISIBLE_COLUMNS && column < 64 && finalFightLiftColumns[column];
}

void MarkFinalFightLiftColumns(float centerX)
{
    int centerColumn = (int)(centerX / DRAW_TILE_SIZE);
    int startColumn = centerColumn - 1;

    if (startColumn < 0)
    {
        startColumn = 0;
    }

    if (startColumn > VISIBLE_COLUMNS - 3)
    {
        startColumn = VISIBLE_COLUMNS - 3;
    }

    for (int i = 0; i < finalFightLiftPlatformCount; i++)
    {
        if (finalFightLiftPlatformStarts[i] == startColumn)
        {
            return;
        }
    }

    if (finalFightLiftPlatformCount < 2)
    {
        finalFightLiftPlatformStarts[finalFightLiftPlatformCount] = startColumn;
        finalFightLiftPlatformCount++;
    }

    for (int offset = 0; offset < 3; offset++)
    {
        int column = startColumn + offset;

        if (column >= 0 && column < VISIBLE_COLUMNS && column < 64)
        {
            finalFightLiftColumns[column] = true;
        }
    }
}

Rectangle GetFinalFightLiftPlatformRect(int platformIndex)
{
    const float brickScale = TILE_SCALE;
    const float brickSeamOverlap = 4.0f;
    Texture2D leftTex = textures[TEXTURE_BRICK_LEFT_CORNER];
    Texture2D middleTex = textures[TEXTURE_BRICK_TILE];
    Texture2D rightTex = textures[TEXTURE_BRICK_RIGHT_CORNER];
    Rectangle rect = {0.0f, 0.0f, 0.0f, 0.0f};

    if (platformIndex < 0 ||
        platformIndex >= finalFightLiftPlatformCount ||
        leftTex.id == 0 ||
        middleTex.id == 0 ||
        rightTex.id == 0)
    {
        return rect;
    }

    rect.x = (finalFightLiftPlatformStarts[platformIndex] * DRAW_TILE_SIZE) + 4.0f +
             finalFightLiftPlatformOffsetX[platformIndex];
    rect.y = GetGroundTopY() - finalFightLiftOffset;
    rect.width = ((leftTex.width + middleTex.width + rightTex.width) * brickScale) -
                 (brickSeamOverlap * 2.0f);
    rect.height = leftTex.height * brickScale;
    return rect;
}

bool IsActorStandingOnRect(const Actor *actor, Rectangle rect, float surfaceY)
{
    float actorCenterX = actor->position.x + actor->width * 0.5f;
    float actorBottom = actor->position.y + actor->height;

    return rect.width > 0.0f &&
           actorCenterX >= rect.x &&
           actorCenterX <= rect.x + rect.width &&
           AbsFloat(actorBottom - surfaceY) < 10.0f;
}

void InitFinalFightSafetyPlatform(void)
{
    Texture2D platformTex = textures[TEXTURE_PLATFORM_MIDDLE];
    float platformWidth = platformTex.width * FINAL_FIGHT_SAFETY_PLATFORM_SCALE;

    if (platformTex.id == 0)
    {
        finalFightSafetyPlatformActive = false;
        return;
    }

    finalFightSafetyPlatformActive = true;
    finalFightSafetyPlatformPulling = false;
    finalFightSafetyDialogShown = false;
    finalFightPlayerReachedSafety = false;
    finalFightSafetyPlatformPosition.x = ((float)screenWidth - platformWidth) * 0.5f;
    finalFightSafetyPlatformPosition.y = -(platformTex.height * FINAL_FIGHT_SAFETY_PLATFORM_SCALE) - 80.0f;
    finalFightSafetyPlatformTargetY = (GetGroundTopY() - FINAL_FIGHT_LIFT_HEIGHT) +
                                      FINAL_FIGHT_SAFETY_PLATFORM_TARGET_OFFSET;
}

void StartFinalFightLift(void)
{
    if (finalFightLiftActive)
    {
        return;
    }

    finalFightDialogActive = false;
    finalFightLiftActive = true;
    finalFightLiftOffset = 0.0f;
    ClearFinalFightLiftColumns();
    MarkFinalFightLiftColumns(player.position.x + player.width * 0.5f);
    MarkFinalFightLiftColumns(kieran.position.x + kieran.width * 0.5f);
    finalFightPlayerLiftPlatformIndex = (finalFightLiftPlatformCount > 0) ? 0 : -1;
    finalFightKieranLiftPlatformIndex = (finalFightLiftPlatformCount > 1) ? 1 : -1;
    InitFinalFightSafetyPlatform();
}

void UpdateFinalFightLift(void)
{
    Actor *actors[2] = {&player, &kieran};
    Rectangle previousSafetyRect = GetFinalFightSafetyPlatformRect();
    float previousSafetyTop = GetFinalFightSafetyPlatformSurfaceY();
    Rectangle previousLiftRects[2] = {
        GetFinalFightLiftPlatformRect(0),
        GetFinalFightLiftPlatformRect(1)
    };
    float dt = GetFrameTime();

    if (!finalFightLiftActive)
    {
        return;
    }

    finalFightLiftOffset += FINAL_FIGHT_LIFT_SPEED * dt;

    if (finalFightLiftOffset > FINAL_FIGHT_LIFT_HEIGHT)
    {
        finalFightLiftOffset = FINAL_FIGHT_LIFT_HEIGHT;
    }

    if (finalFightSafetyPlatformActive)
    {
        Rectangle playerBounds = GetActorBounds(&player);

        if (!finalFightSafetyPlatformPulling)
        {
            finalFightSafetyPlatformPosition.y += FINAL_FIGHT_SAFETY_PLATFORM_DROP_SPEED * dt;

            if (finalFightSafetyPlatformPosition.y > finalFightSafetyPlatformTargetY)
            {
                finalFightSafetyPlatformPosition.y = finalFightSafetyPlatformTargetY;
            }
        }

        Rectangle currentSafetyRect = GetFinalFightSafetyPlatformRect();
        float currentSafetyTop = GetFinalFightSafetyPlatformSurfaceY();
        float deltaX = currentSafetyRect.x - previousSafetyRect.x;
        float deltaY = currentSafetyRect.y - previousSafetyRect.y;

        if (CheckCollisionRecs(playerBounds, currentSafetyRect) ||
            IsActorStandingOnRect(&player, currentSafetyRect, currentSafetyTop))
        {
            finalFightPlayerReachedSafety = true;
        }

        if (deltaX != 0.0f || deltaY != 0.0f)
        {
            for (int i = 0; i < 2; i++)
            {
                Actor *actor = actors[i];
                if (IsActorStandingOnRect(actor, previousSafetyRect, previousSafetyTop))
                {
                    actor->position.x += deltaX;
                    actor->position.y += deltaY;
                }
            }
        }
    }

    if (finalFightSafetyPlatformPulling &&
        finalFightPlayerLiftPlatformIndex >= 0 &&
        finalFightKieranLiftPlatformIndex >= 0)
    {
        bool playerPlatformReachedTarget = false;
        Rectangle playerLiftRect = GetFinalFightLiftPlatformRect(finalFightPlayerLiftPlatformIndex);
        Rectangle kieranLiftRect = GetFinalFightLiftPlatformRect(finalFightKieranLiftPlatformIndex);
        float playerLiftBaseX = (finalFightLiftPlatformStarts[finalFightPlayerLiftPlatformIndex] *
                                 DRAW_TILE_SIZE) + 4.0f;
        float targetX = kieranLiftRect.x + kieranLiftRect.width;
        float targetOffset = targetX - playerLiftBaseX;

        if (playerLiftRect.x > targetX)
        {
            finalFightLiftPlatformOffsetX[finalFightPlayerLiftPlatformIndex] -=
                FINAL_FIGHT_SAFETY_PLATFORM_PULL_SPEED * dt;

            if (finalFightLiftPlatformOffsetX[finalFightPlayerLiftPlatformIndex] < targetOffset)
            {
                finalFightLiftPlatformOffsetX[finalFightPlayerLiftPlatformIndex] = targetOffset;
            }
        }

        Rectangle currentPlayerLiftRect = GetFinalFightLiftPlatformRect(finalFightPlayerLiftPlatformIndex);
        if (currentPlayerLiftRect.x <= targetX)
        {
            playerPlatformReachedTarget = true;
        }

        if (playerPlatformReachedTarget)
        {
            if (finalFightPlayerReachedSafety)
            {
                finalFightDialogActive = true;
                finalFightDialogPhase = FINAL_FIGHT_DIALOG_PHASE_TRAP;
                finalFightTrapDialogCharCount = 0.0f;
                finalFightSafetyPlatformPulling = false;
                return;
            }

            finalFightSafetyPlatformPulling = false;
        }
    }

    for (int platformIndex = 0; platformIndex < finalFightLiftPlatformCount; platformIndex++)
    {
        Rectangle previousLiftRect = previousLiftRects[platformIndex];
        Rectangle currentLiftRect = GetFinalFightLiftPlatformRect(platformIndex);
        float deltaX = currentLiftRect.x - previousLiftRect.x;
        float deltaY = currentLiftRect.y - previousLiftRect.y;

        if (deltaX == 0.0f && deltaY == 0.0f)
        {
            continue;
        }

        for (int actorIndex = 0; actorIndex < 2; actorIndex++)
        {
            Actor *actor = actors[actorIndex];

            if (IsActorStandingOnRect(actor, previousLiftRect, previousLiftRect.y))
            {
                actor->position.x += deltaX;
                actor->position.y += deltaY;
            }
        }
    }

    if (!finalFightSafetyPlatformPulling &&
        finalFightPlayerLiftPlatformIndex >= 0 &&
        finalFightKieranLiftPlatformIndex >= 0)
    {
        Rectangle playerLiftRect = GetFinalFightLiftPlatformRect(finalFightPlayerLiftPlatformIndex);
        Rectangle kieranLiftRect = GetFinalFightLiftPlatformRect(finalFightKieranLiftPlatformIndex);

        if (playerLiftRect.width > 0.0f &&
            kieranLiftRect.width > 0.0f &&
            playerLiftRect.x <= kieranLiftRect.x + kieranLiftRect.width &&
            IsActorStandingOnRect(&player, playerLiftRect, playerLiftRect.y))
        {
            StartGameOverTransition();
        }
    }

    if (!finalFightSafetyDialogShown &&
        finalFightSafetyPlatformActive &&
        !finalFightSafetyPlatformPulling &&
        finalFightLiftOffset >= FINAL_FIGHT_LIFT_HEIGHT &&
        finalFightSafetyPlatformPosition.y >= finalFightSafetyPlatformTargetY)
    {
        finalFightDialogActive = true;
        finalFightDialogPhase = FINAL_FIGHT_DIALOG_PHASE_SAFETY;
        finalFightSafetyDialogCharCount = 0.0f;
        finalFightSafetyDialogShown = true;
    }
}

void InitFinalFightStage2Platforms(void)
{
    const float laneCenters[FINAL_FIGHT_FALLING_PLATFORM_COUNT] = {
        700.0f, 830.0f, 960.0f, 1090.0f, 1220.0f, 1350.0f, 1480.0f, 1610.0f, 1740.0f
    };
    const float startY[FINAL_FIGHT_FALLING_PLATFORM_COUNT] = {
        -980.0f, -860.0f, -740.0f, -620.0f, 60.0f, -500.0f, -1100.0f, -1240.0f, -1380.0f
    };
    const int textureCycle[3] = {
        TEXTURE_PLATFORM_TOP,
        TEXTURE_PLATFORM_SECOND,
        TEXTURE_PLATFORM_ARENA
    };
    Rectangle brickPlatformRect = GetFinalFightBrickPlatformRect();
    int textureOrder[FINAL_FIGHT_FALLING_PLATFORM_COUNT];

    for (int i = 0; i < FINAL_FIGHT_FALLING_PLATFORM_COUNT; i++)
    {
        textureOrder[i] = textureCycle[i % 3];
    }

    for (int i = FINAL_FIGHT_FALLING_PLATFORM_COUNT - 1; i > 0; i--)
    {
        int swapIndex = GetRandomValue(0, i);
        int tempTexture = textureOrder[i];

        textureOrder[i] = textureOrder[swapIndex];
        textureOrder[swapIndex] = tempTexture;
    }

    for (int i = 0; i < FINAL_FIGHT_FALLING_PLATFORM_COUNT; i++)
    {
        Texture2D platformTex = textures[textureOrder[i]];
        float platformWidth = platformTex.width * FINAL_FIGHT_FALLING_PLATFORM_SCALE;

        finalFightFallingPlatforms[i].textureAsset = textureOrder[i];
        finalFightFallingPlatforms[i].position.x = laneCenters[i] - platformWidth * 0.5f;
        finalFightFallingPlatforms[i].position.y = startY[i];
        finalFightFallingPlatforms[i].frozen = false;
        finalFightFallingPlatforms[i].freezeTimer = 0.0f;
    }

    player.position.x = laneCenters[4] - player.width * 0.5f;
    player.position.y = startY[4] - player.height;
    player.velocity = (Vector2){0.0f, 0.0f};
    player.onGround = true;
    player.facingLeft = false;
    player.spawn = player.position;

    kieran.position.x = brickPlatformRect.x + brickPlatformRect.width * 0.5f - kieran.width * 0.5f;
    kieran.position.y = brickPlatformRect.y - kieran.height;
    kieran.velocity = (Vector2){0.0f, 0.0f};
    kieran.onGround = true;
    kieran.facingLeft = false;
    kieran.spawn = kieran.position;
    playerStage2PlatformIndex = 4;
    kieranStage2PlatformIndex = -1;
}

void UpdateFinalFightStage2Platforms(void)
{
    Actor *actors[2] = {&player, &kieran};
    float dt = GetFrameTime();

    if (!IsFinalFightStage2() || !finalFightStage2Started)
    {
        return;
    }

    for (int platformIndex = 0; platformIndex < FINAL_FIGHT_FALLING_PLATFORM_COUNT; platformIndex++)
    {
        Rectangle previousRect = GetFinalFightFallingPlatformRect(platformIndex);
        float highestY = previousRect.y;

        if (finalFightFallingPlatforms[platformIndex].frozen)
        {
            finalFightFallingPlatforms[platformIndex].freezeTimer -= dt;

            if (finalFightFallingPlatforms[platformIndex].freezeTimer <= 0.0f)
            {
                finalFightFallingPlatforms[platformIndex].freezeTimer = 0.0f;
                finalFightFallingPlatforms[platformIndex].frozen = false;
            }

            continue;
        }

        finalFightFallingPlatforms[platformIndex].position.y += FINAL_FIGHT_FALLING_PLATFORM_SPEED * dt;

        if (previousRect.y > (float)screenHeight + 40.0f)
        {
            for (int otherIndex = 0; otherIndex < FINAL_FIGHT_FALLING_PLATFORM_COUNT; otherIndex++)
            {
                if (otherIndex == platformIndex)
                {
                    continue;
                }

                if (finalFightFallingPlatforms[otherIndex].position.y < highestY)
                {
                    highestY = finalFightFallingPlatforms[otherIndex].position.y;
                }
            }

            finalFightFallingPlatforms[platformIndex].position.y =
                highestY - (float)GetRandomValue(
                    (int)FINAL_FIGHT_FALLING_PLATFORM_RESPAWN_MIN_GAP,
                    (int)FINAL_FIGHT_FALLING_PLATFORM_RESPAWN_MAX_GAP
                );
            finalFightFallingPlatforms[platformIndex].freezeTimer = 0.0f;
        }

        Rectangle currentRect = GetFinalFightFallingPlatformRect(platformIndex);
        float deltaX = currentRect.x - previousRect.x;
        float deltaY = currentRect.y - previousRect.y;

        if (deltaX == 0.0f && deltaY == 0.0f)
        {
            continue;
        }

        for (int actorIndex = 0; actorIndex < 2; actorIndex++)
        {
            Actor *actor = actors[actorIndex];

            if (IsActorStandingOnRect(actor, previousRect, previousRect.y))
            {
                actor->position.x += deltaX;
                actor->position.y += deltaY;
            }
        }
    }
}

void StartFinalFightStage2(void)
{
    finalFightStage = FINAL_FIGHT_STAGE_2;
    ResetFinalFightState(false);
    finalFightStage2Started = true;
    finalFightFuryMode = true;
    kieranHealth = KIERAN_MAX_HEALTH;
    finalFightTimeRemaining = 300.0f;
    kieran.speed = FINAL_FIGHT_STAGE2_KIERAN_SPEED;
    kieran.jumpVelocity = FINAL_FIGHT_STAGE2_KIERAN_JUMP;
    player.jumpVelocity = FINAL_FIGHT_STAGE2_PLAYER_JUMP;
    ResetPlayerCombatState();
    ResetPlayerBookState();
    InitFinalFightStage2Platforms();
    chaseStarted = false;
}

void StartGameOverTransition(void)
{
    if (transitionState != TRANSITION_NONE)
    {
        return;
    }

    transitionShowGameOver = true;
    transitionShowEightHoursLater = false;
    transitionShowVictory = false;
    finalFightTimeRemaining = 0.0f;
    StartMapTransition();
}

void StartVictoryTransition(void)
{
    if (transitionState != TRANSITION_NONE)
    {
        return;
    }

    transitionShowVictory = true;
    transitionShowEightHoursLater = false;
    transitionShowGameOver = false;
    finalFightTimeRemaining = 0.0f;
    StartMapTransition();
}

void UpdateFinalFightTimer(void)
{
    if (!IsFinalFightStage2() || !finalFightStage2Started || transitionState != TRANSITION_NONE)
    {
        return;
    }

    if (kieranHealth <= 0)
    {
        return;
    }

    finalFightTimeRemaining -= GetFrameTime();

    if (finalFightTimeRemaining <= 0.0f)
    {
        finalFightTimeRemaining = 0.0f;
        StartGameOverTransition();
    }
}

void StartMapTransition(void)
{
    if (transitionState != TRANSITION_NONE)
    {
        return;
    }

    transitionState = TRANSITION_CLOSING;
    transitionRadius = GetMaxTransitionRadius();
    transitionCenter = (Vector2){screenWidth * 0.5f, screenHeight * 0.5f};
    player.velocity = (Vector2){0.0f, 0.0f};
    kieran.velocity = (Vector2){0.0f, 0.0f};
}

void UpdateMapTransition(void)
{
    float dt = GetFrameTime();
    float maxRadius = GetMaxTransitionRadius();

    if (transitionState == TRANSITION_CLOSING)
    {
        transitionRadius -= MAP_TRANSITION_SPEED * dt;

        if (transitionRadius <= 0.0f)
        {
            transitionRadius = 0.0f;

            if (transitionShowEightHoursLater || transitionShowGameOver || transitionShowVictory)
            {
                transitionState = TRANSITION_HOLD;
            }
            else
            {
                LoadNextMap();
                transitionState = TRANSITION_OPENING;
            }
        }
    }
    else if (transitionState == TRANSITION_HOLD)
    {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
        {
            if (transitionShowGameOver)
            {
                transitionShowGameOver = false;
                RestartCurrentMap();
            }
            else if (transitionShowVictory)
            {
                transitionShowVictory = false;
                currentMapIndex = 0;
                finalFightStage = FINAL_FIGHT_STAGE_1;
                GenerateGround();
                InitActors();
                startMenuActive = true;
                startMenuTab = MENU_TAB_START;
                openingIntroActive = false;
                openingIntroTimer = 0.0f;
            }
            else
            {
                LoadNextMap();
            }

            transitionState = TRANSITION_OPENING;
            transitionShowEightHoursLater = false;
        }
    }
    else if (transitionState == TRANSITION_OPENING)
    {
        transitionRadius += MAP_TRANSITION_SPEED * dt;

        if (transitionRadius >= maxRadius)
        {
            transitionRadius = maxRadius;
            transitionState = TRANSITION_NONE;
            transitionShowEightHoursLater = false;
            transitionShowGameOver = false;
            transitionShowVictory = false;
        }
    }
}

/* Opening menu and intro flow */
void UpdateStartMenu(void)
{
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
    {
        startMenuTab--;

        if (startMenuTab < MENU_TAB_START)
        {
            startMenuTab = MENU_TAB_GUIDE;
        }
    }
    else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
    {
        startMenuTab++;

        if (startMenuTab > MENU_TAB_GUIDE)
        {
            startMenuTab = MENU_TAB_START;
        }
    }

    if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) &&
        startMenuTab == MENU_TAB_START)
    {
        startMenuActive = false;
        openingIntroActive = true;
        openingIntroTimer = 0.0f;
    }
}

void DrawStartMenu(void)
{
    const char *tabLabels[3] = {"Start", "Controls", "Guide"};
    float tabWidth = 220.0f;
    float tabHeight = 68.0f;
    float tabGap = 26.0f;
    float totalTabWidth = (tabWidth * 3.0f) + (tabGap * 2.0f);
    float tabStartX = ((float)screenWidth - totalTabWidth) * 0.5f;
    float tabY = 210.0f;

    DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);
    DrawText("EVIL LEARY", 710, 92, 72, WHITE);
    DrawText("Choose a tab with A/D or Left/Right", 688, 162, 28, (Color){220, 220, 220, 255});

    for (int i = 0; i < 3; i++)
    {
        Color tabColor = (i == startMenuTab) ? (Color){242, 194, 82, 255} : (Color){58, 82, 110, 240};
        Color textColor = (i == startMenuTab) ? BLACK : WHITE;
        int textWidth = MeasureText(tabLabels[i], 34);
        int textX = (int)(tabStartX + (i * (tabWidth + tabGap)) + (tabWidth - textWidth) * 0.5f);

        DrawRectangle((int)(tabStartX + (i * (tabWidth + tabGap))), (int)tabY,
                      (int)tabWidth, (int)tabHeight, tabColor);
        DrawText(tabLabels[i], textX, (int)tabY + 16, 34, textColor);
    }

    if (startMenuTab == MENU_TAB_START)
    {
        DrawText("Start The Game", 775, 390, 48, WHITE);
        DrawText("Press Enter to start the game.", 700, 490, 34, WHITE);
        DrawText("Escape Kieran, survive the arena, and finish the final fight before time runs out.",
                 360, 560, 30, (Color){210, 210, 210, 255});
        DrawText("Books hurt him for 5 damage each, so keep moving and keep attacking.",
                 470, 610, 30, (Color){210, 210, 210, 255});
    }
    else if (startMenuTab == MENU_TAB_CONTROLS)
    {
        DrawText("Controls", 850, 390, 48, WHITE);
        DrawText("Move Right: D or Right Arrow", 560, 490, 34, WHITE);
        DrawText("Move Left: A or Left Arrow", 560, 540, 34, WHITE);
        DrawText("Jump: Space, W, or Up Arrow", 560, 590, 34, WHITE);
        DrawText("Attack: E", 560, 640, 34, WHITE);
        DrawText("Menu Tabs: A/D or Left/Right", 560, 690, 34, WHITE);
    }
    else
    {
        DrawText("Guide", 885, 390, 48, WHITE);
        DrawText("1. Get away from Kieran in the opening chase.", 500, 490, 34, WHITE);
        DrawText("2. Reach the final fight and survive the platform phase.", 500, 540, 34, WHITE);
        DrawText("3. Once the platforms start falling, beat Kieran within 300 seconds.", 500, 590, 34, WHITE);
        DrawText("4. If Kieran touches you or the timer runs out, it is game over.", 500, 640, 34, WHITE);
    }
}

void UpdateOpeningIntro(void)
{
    openingIntroTimer += GetFrameTime();

    if (openingIntroTimer >= OPENING_INTRO_DURATION)
    {
        openingIntroTimer = OPENING_INTRO_DURATION;
        openingIntroActive = false;
    }
}

bool UseOpeningIntro(void)
{
    return openingIntroActive && currentMapIndex == 0 && !IsFirstArenaMap() && !IsFinalFightMap();
}

float GetOpeningIntroProgress(float delay)
{
    float progress = (openingIntroTimer - delay) / (OPENING_INTRO_DURATION - delay);

    if (!UseOpeningIntro())
    {
        return 1.0f;
    }

    if (progress < 0.0f)
    {
        progress = 0.0f;
    }
    else if (progress > 1.0f)
    {
        progress = 1.0f;
    }

    return 1.0f - powf(1.0f - progress, 3.0f);
}

Vector2 GetOpeningIntroGroundOffset(int column)
{
    float columnDelay = ((float)(column % 8) / 8.0f) * 0.35f;
    float progress = GetOpeningIntroProgress(columnDelay);
    float remaining = 1.0f - progress;
    float horizontalOffset = (column < (VISIBLE_COLUMNS / 2)) ?
                             -(screenWidth + 160.0f) :
                             (screenWidth + 160.0f);

    return (Vector2){horizontalOffset * remaining, 0.0f};
}

Vector2 GetOpeningIntroTextureOffset(Texture2D texture)
{
    float progress = GetOpeningIntroProgress(0.12f);
    float remaining = 1.0f - progress;

    if (!UseOpeningIntro())
    {
        return (Vector2){0.0f, 0.0f};
    }

    if (texture.id == textures[TEXTURE_TREE].id)
    {
        return (Vector2){0.0f, -(screenHeight + 260.0f) * remaining};
    }

    if (texture.id == textures[TEXTURE_BUSH_1].id || texture.id == textures[TEXTURE_STONE_BRICK_1].id ||
        texture.id == textures[TEXTURE_SIGN_RIGHT].id)
    {
        return (Vector2){-(screenWidth + 220.0f) * remaining, 0.0f};
    }

    if (texture.id == textures[TEXTURE_BUSH_2].id || texture.id == textures[TEXTURE_FENCE].id)
    {
        return (Vector2){(screenWidth + 220.0f) * remaining, 0.0f};
    }

    if (texture.id == textures[TEXTURE_FLOWER_1].id || texture.id == textures[TEXTURE_FLOWER_2].id)
    {
        return (Vector2){0.0f, (screenHeight + 220.0f) * remaining};
    }

    return (Vector2){0.0f, 0.0f};
}

Vector2 GetOpeningIntroActorOffset(const Actor *actor)
{
    float progress = GetOpeningIntroProgress(0.2f);
    float remaining = 1.0f - progress;

    if (!UseOpeningIntro())
    {
        return (Vector2){0.0f, 0.0f};
    }

    if (actor == &player)
    {
        return (Vector2){0.0f, (screenHeight + 260.0f) * remaining};
    }

    return (Vector2){-(screenWidth + 260.0f) * remaining, 0.0f};
}

void DrawOpeningIntroOverlay(void)
{
    if (!UseOpeningIntro())
    {
        return;
    }

    float progress = GetOpeningIntroProgress(0.0f);
    unsigned char alpha = (unsigned char)(255.0f * (1.0f - progress));

    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, alpha});
}

/* Texture and animation lookup */
Texture2D GetGrassTexture(int variant)
{
    switch (variant)
    {
        case 1: return textures[TEXTURE_GRASS_2];
        case 2: return textures[TEXTURE_GRASS_3];
        case 3: return textures[TEXTURE_GRASS_4];
        default: return textures[TEXTURE_GRASS_1];
    }
}

Texture2D GetUndergroundTexture(int variant)
{
    switch (variant)
    {
        case 1: return textures[TEXTURE_UNDER_2];
        case 2: return textures[TEXTURE_UNDER_3];
        case 3: return textures[TEXTURE_UNDER_4];
        default: return textures[TEXTURE_UNDER_1];
    }
}

Texture2D GetPlayerFrame(void)
{
    if (IsFirstArenaMap() && firstArenaRunInFinished && !firstArenaOutroWalkStarted)
    {
        if (girlPlayerIdleFrames[0].id != 0)
        {
            return girlPlayerIdleFrames[0];
        }

        for (int i = 0; i < PLAYER_IDLE_FRAMES; i++)
        {
            if (girlPlayerIdleFrames[i].id != 0)
            {
                return girlPlayerIdleFrames[i];
            }
        }
    }

    if (playerAttackActive)
    {
        int attackFrame = (int)(playerAttackTime * PLAYER_ATTACK_SPEED);
        Texture2D playerTex = {0};

        if (attackFrame >= PLAYER_ATTACK_FRAMES)
        {
            attackFrame = PLAYER_ATTACK_FRAMES - 1;
        }

        if (attackFrame < 0)
        {
            attackFrame = 0;
        }

        playerTex = girlPlayerAttackFrames[attackFrame];

        if (playerTex.id != 0)
        {
            return playerTex;
        }

        for (int i = 0; i < PLAYER_ATTACK_FRAMES; i++)
        {
            if (girlPlayerAttackFrames[i].id != 0)
            {
                return girlPlayerAttackFrames[i];
            }
        }
    }

    bool walking = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) ||
                   IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT) ||
                   AbsFloat(player.velocity.x) > 20.0f;
    Texture2D playerTex = {0};

    if (walking)
    {
        playerTex = girlPlayerWalkFrames[((int)(GetTime() * 14.0f)) % PLAYER_WALK_FRAMES];

        if (playerTex.id != 0)
        {
            return playerTex;
        }

        for (int i = 0; i < PLAYER_WALK_FRAMES; i++)
        {
            if (girlPlayerWalkFrames[i].id != 0)
            {
                return girlPlayerWalkFrames[i];
            }
        }
    }

    playerTex = girlPlayerIdleFrames[((int)(GetTime() * 8.0f)) % PLAYER_IDLE_FRAMES];

    if (playerTex.id != 0)
    {
        return playerTex;
    }

    for (int i = 0; i < PLAYER_IDLE_FRAMES; i++)
    {
        if (girlPlayerIdleFrames[i].id != 0)
        {
            return girlPlayerIdleFrames[i];
        }
    }

    return (Texture2D){0};
}

Texture2D GetKieranFrame(void)
{
    if (finalFightFuryMode && textures[TEXTURE_KIERAN_FURY].id != 0)
    {
        return textures[TEXTURE_KIERAN_FURY];
    }

    if (((finalFightDialogActive &&
          finalFightDialogPhase == FINAL_FIGHT_DIALOG_PHASE_FURY) ||
         finalFightLiftActive) &&
        textures[TEXTURE_KIERAN_FURY].id != 0)
    {
        return textures[TEXTURE_KIERAN_FURY];
    }

    if (!kieran.onGround && textures[TEXTURE_KIERAN_JUMP].id != 0)
    {
        return textures[TEXTURE_KIERAN_JUMP];
    }

    if (AbsFloat(kieran.velocity.x) > 20.0f)
    {
        Texture2D runTex = textures[TEXTURE_KIERAN_RUN_1 + (((int)(GetTime() * 10.0f)) % 2)];

        if (runTex.id != 0)
        {
            return runTex;
        }
    }

    if (textures[TEXTURE_KIERAN_IDLE].id != 0) return textures[TEXTURE_KIERAN_IDLE];
    if (textures[TEXTURE_KIERAN_RUN_1].id != 0) return textures[TEXTURE_KIERAN_RUN_1];
    if (textures[TEXTURE_KIERAN_RUN_2].id != 0) return textures[TEXTURE_KIERAN_RUN_2];
    if (textures[TEXTURE_KIERAN_FURY].id != 0) return textures[TEXTURE_KIERAN_FURY];
    return textures[TEXTURE_KIERAN_JUMP];
}

Texture2D GetPlayerBookTexture(int frameIndex)
{
    if (frameIndex < 0)
    {
        frameIndex = 0;
    }

    if (frameIndex >= PLAYER_BOOK_FRAMES)
    {
        frameIndex = PLAYER_BOOK_FRAMES - 1;
    }

    if (girlPlayerBookFrames[frameIndex].id != 0)
    {
        return girlPlayerBookFrames[frameIndex];
    }

    for (int i = 0; i < PLAYER_BOOK_FRAMES; i++)
    {
        if (girlPlayerBookFrames[i].id != 0)
        {
            return girlPlayerBookFrames[i];
        }
    }

    return (Texture2D){0};
}

int GetPlayerBookFrameIndex(void)
{
    if (playerBook.stopped)
    {
        return playerBook.stoppedFrame;
    }

    return ((int)(playerBook.animationTime * PLAYER_ATTACK_SPEED)) % PLAYER_BOOK_FRAMES;
}

/* Shared draw helpers */
void DrawProp(Texture2D texture, float leftX, float baseY, float scale)
{
    if (texture.id == 0) return;
    Vector2 introOffset = GetOpeningIntroTextureOffset(texture);
    float drawY = baseY - (texture.height * scale);

    DrawTextureEx(
        texture,
        (Vector2){leftX + introOffset.x, drawY + introOffset.y},
        0.0f,
        scale,
        WHITE
    );
}

void DrawPropCentered(Texture2D texture, float centerX, float baseY, float scale)
{
    if (texture.id == 0) return;

    DrawProp(texture, centerX - (texture.width * scale * 0.5f), baseY, scale);
}

void DrawActorSprite(Texture2D texture, const Actor *actor)
{
    Vector2 introOffset = GetOpeningIntroActorOffset(actor);
    float drawScale = actor->targetDrawHeight / (float)texture.height;
    float drawWidth = texture.width * drawScale;
    float drawHeight = texture.height * drawScale;
    float drawX = actor->position.x + (actor->width - drawWidth) * 0.5f;
    float drawY = actor->position.y + (actor->height - drawHeight);
    Rectangle source = {0.0f, 0.0f, (float)texture.width, (float)texture.height};

    if (actor->facingLeft)
    {
        source.x = (float)texture.width;
        source.width = -(float)texture.width;
    }

    DrawTexturePro(
        texture,
        source,
        (Rectangle){drawX + introOffset.x, drawY + introOffset.y, drawWidth, drawHeight},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
}

Rectangle GetTreeBounds(void)
{
    Rectangle rect = {0.0f, 0.0f, 0.0f, 0.0f};

    if (textures[TEXTURE_TREE].id == 0)
    {
        return rect;
    }

    rect.width = textures[TEXTURE_TREE].width * DECOR_SCALE;
    rect.height = textures[TEXTURE_TREE].height * DECOR_SCALE;
    rect.x = GetTreeCenterX() - rect.width * 0.5f;
    rect.y = GetDecorBaseY() - rect.height;
    return rect;
}

Rectangle GetPlayerBookBounds(void)
{
    Texture2D bookTex = GetPlayerBookTexture(GetPlayerBookFrameIndex());
    Rectangle rect = {playerBook.center.x, playerBook.center.y, 0.0f, 0.0f};

    if (bookTex.id == 0)
    {
        return rect;
    }

    rect.width = bookTex.width * PLAYER_BOOK_SCALE;
    rect.height = bookTex.height * PLAYER_BOOK_SCALE;
    rect.x = playerBook.center.x - rect.width * 0.5f;
    rect.y = playerBook.center.y - rect.height * 0.5f;
    return rect;
}

void LaunchPlayerBook(void)
{
    Texture2D bookTex = GetPlayerBookTexture(0);
    float throwSpeed = IsFinalFightStage2() ? FINAL_FIGHT_BOOK_SPEED : PLAYER_BOOK_SPEED;
    float throwLift = IsFinalFightStage2() ? FINAL_FIGHT_BOOK_LIFT : PLAYER_BOOK_LIFT;
    float spawnOffsetX = player.facingLeft ? -18.0f : 18.0f;
    float spawnFrontX = player.position.x + player.width * (player.facingLeft ? 0.25f : 0.75f);

    playerBook.active = true;
    playerBook.stopped = false;
    playerBook.animationTime = 0.0f;
    playerBook.stoppedFrame = 0;
    playerBook.facingLeft = player.facingLeft;
    playerBook.velocity.x = player.facingLeft ? -throwSpeed : throwSpeed;
    playerBook.velocity.y = -throwLift;
    playerBook.center.x = spawnFrontX + spawnOffsetX;
    playerBook.center.y = player.position.y + player.height * 0.42f;

    if (bookTex.id != 0)
    {
        playerBook.center.y -= (bookTex.height * PLAYER_BOOK_SCALE) * 0.1f;
    }
}

void FreezePlayerBook(int frameIndex)
{
    playerBook.stopped = true;
    playerBook.velocity = (Vector2){0.0f, 0.0f};
    playerBook.stoppedFrame = frameIndex;
}

void FreezePlayerBookOnSurface(float surfaceY, int frameIndex)
{
    Texture2D stopTex = GetPlayerBookTexture(frameIndex);
    float stopHeight = GetPlayerBookBounds().height;

    if (stopTex.id != 0)
    {
        stopHeight = stopTex.height * PLAYER_BOOK_SCALE;
    }

    playerBook.center.y = surfaceY - stopHeight * 0.5f;
    FreezePlayerBook(frameIndex);
}

/* Actor movement and combat */
void UpdateActorPhysics(Actor *actor)
{
    float dt = GetFrameTime();
    Vector2 previousPosition = actor->position;
    const float gravity = 1900.0f;

    actor->velocity.y += gravity * dt;
    actor->position.x += actor->velocity.x * dt;

    if (actor->position.x < 0.0f)
    {
        actor->position.x = 0.0f;
    }
    else if (actor->position.x + actor->width > (float)screenWidth)
    {
        actor->position.x = (float)screenWidth - actor->width;
    }

    actor->position.y += actor->velocity.y * dt;
    ResolveActorCollisions(actor, previousPosition);
}

/* Collision against the active map layout */
void ResolveActorCollisions(Actor *actor, Vector2 previousPosition)
{
    float holeLeft = 0.0f;
    float holeRight = 0.0f;
    float centerX = actor->position.x + actor->width * 0.5f;
    float previousBottom = previousPosition.y + actor->height;
    float currentBottom = actor->position.y + actor->height;
    float groundTop = GetGroundTopY();
    float platformTop = GetFloatingPlatformSurfaceY();
    Rectangle platformRect = GetFloatingPlatformRect();
    float safetyPlatformTop = GetFinalFightSafetyPlatformSurfaceY();
    Rectangle safetyPlatformRect = GetFinalFightSafetyPlatformRect();

    actor->onGround = false;
    GetHoleBounds(&holeLeft, &holeRight);

    if (GetCurrentPlatformTexture().id != 0 &&
        actor->velocity.y >= 0.0f &&
        centerX >= platformRect.x &&
        centerX <= platformRect.x + platformRect.width &&
        previousBottom <= platformTop &&
        currentBottom >= platformTop)
    {
        actor->position.y = platformTop - actor->height;
        actor->velocity.y = 0.0f;
        actor->onGround = true;
        return;
    }

    if (finalFightSafetyPlatformActive &&
        actor->velocity.y >= 0.0f &&
        centerX >= safetyPlatformRect.x &&
        centerX <= safetyPlatformRect.x + safetyPlatformRect.width &&
        previousBottom <= safetyPlatformTop &&
        currentBottom >= safetyPlatformTop)
    {
        actor->position.y = safetyPlatformTop - actor->height;
        actor->velocity.y = 0.0f;
        actor->onGround = true;
        return;
    }

    if (IsFinalFightStage1())
    {
        float stageGroundTop = groundTop + (finalFightLiftActive ? finalFightLiftOffset : 0.0f);

        if (finalFightLiftActive)
        {
            for (int i = 0; i < finalFightLiftPlatformCount; i++)
            {
                Rectangle liftPlatformRect = GetFinalFightLiftPlatformRect(i);
                float liftPlatformTop = liftPlatformRect.y;

                if (liftPlatformRect.width <= 0.0f)
                {
                    continue;
                }

                if (actor->velocity.y >= 0.0f &&
                    centerX >= liftPlatformRect.x &&
                    centerX <= liftPlatformRect.x + liftPlatformRect.width &&
                    previousBottom <= liftPlatformTop &&
                    currentBottom >= liftPlatformTop)
                {
                    actor->position.y = liftPlatformTop - actor->height;
                    actor->velocity.y = 0.0f;
                    actor->onGround = true;
                    return;
                }
            }

            return;
        }

        if (actor->velocity.y >= 0.0f &&
            previousBottom <= stageGroundTop &&
            currentBottom >= stageGroundTop)
        {
            actor->position.y = stageGroundTop - actor->height;
            actor->velocity.y = 0.0f;
            actor->onGround = true;
        }

        return;
    }

    if (IsFinalFightMap())
    {
        if (IsFinalFightStage2())
        {
            Rectangle brickPlatformRect = GetFinalFightBrickPlatformRect();
            float brickPlatformTop = brickPlatformRect.y;

            if (brickPlatformRect.width > 0.0f &&
                actor->velocity.y >= 0.0f &&
                centerX >= brickPlatformRect.x &&
                centerX <= brickPlatformRect.x + brickPlatformRect.width &&
                previousBottom <= brickPlatformTop &&
                currentBottom >= brickPlatformTop)
            {
                actor->position.y = brickPlatformTop - actor->height;
                actor->velocity.y = 0.0f;
                actor->onGround = true;
                return;
            }

            for (int i = 0; i < FINAL_FIGHT_FALLING_PLATFORM_COUNT; i++)
            {
                Rectangle fallingPlatformRect = GetFinalFightFallingPlatformRect(i);
                float fallingPlatformTop = fallingPlatformRect.y;

                if (fallingPlatformRect.width <= 0.0f)
                {
                    continue;
                }

                if (actor->velocity.y >= 0.0f &&
                    centerX >= fallingPlatformRect.x &&
                    centerX <= fallingPlatformRect.x + fallingPlatformRect.width &&
                    previousBottom <= fallingPlatformTop &&
                    currentBottom >= fallingPlatformTop)
                {
                    actor->position.y = fallingPlatformTop - actor->height;
                    actor->velocity.y = 0.0f;
                    actor->onGround = true;
                    return;
                }
            }
        }

        return;
    }

    if (IsFirstArenaMap())
    {
        if (actor->velocity.y >= 0.0f &&
            previousBottom <= groundTop &&
            currentBottom >= groundTop)
        {
            actor->position.y = groundTop - actor->height;
            actor->velocity.y = 0.0f;
            actor->onGround = true;
        }

        return;
    }

    if (actor->velocity.y >= 0.0f &&
        centerX < holeLeft &&
        previousBottom <= groundTop &&
        currentBottom >= groundTop)
    {
        actor->position.y = groundTop - actor->height;
        actor->velocity.y = 0.0f;
        actor->onGround = true;
    }
    else if (actor->velocity.y >= 0.0f &&
             centerX > holeRight &&
             previousBottom <= groundTop &&
             currentBottom >= groundTop)
    {
        actor->position.y = groundTop - actor->height;
        actor->velocity.y = 0.0f;
        actor->onGround = true;
    }
}

/* Player update and input */
void UpdatePlayer(void)
{
    float dt = GetFrameTime();

    if (playerAttackCooldownTimer > 0.0f)
    {
        playerAttackCooldownTimer -= dt;

        if (playerAttackCooldownTimer < 0.0f)
        {
            playerAttackCooldownTimer = 0.0f;
        }
    }

    if (IsFinalFightStage1() && !finalFightLiftActive)
    {
        player.velocity = (Vector2){0.0f, 0.0f};
        player.facingLeft = true;
        UpdateActorPhysics(&player);

        if (player.position.y > screenHeight + 200.0f)
        {
            ResetActor(&player);
        }

        return;
    }

    if (finalFightDialogActive)
    {
        player.velocity = (Vector2){0.0f, 0.0f};
        player.onGround = true;
        player.facingLeft = true;
        return;
    }

    if (currentMapIndex == 0 && !chaseStarted)
    {
        player.velocity.x = 0.0f;
        player.facingLeft = true;

        UpdateActorPhysics(&player);

        if (player.position.y > screenHeight + 200.0f)
        {
            ResetActor(&player);
        }

        return;
    }

    if (currentMapIndex == 0 && openingMapEscapeActive)
    {
        player.velocity.x = player.speed;
        player.facingLeft = false;

        if (!openingMapEscapeJumped && player.onGround)
        {
            player.velocity.y = -player.jumpVelocity;
            player.onGround = false;
            openingMapEscapeJumped = true;
        }

        UpdateActorPhysics(&player);

        if (player.position.y > screenHeight + 200.0f)
        {
            ResetActor(&player);
        }

        return;
    }

    if (IsFirstArenaMap() && firstArenaRunInFinished)
    {
        if (firstArenaOutroWalkStarted)
        {
            float targetX = (float)screenWidth - player.width - FIRST_ARENA_OUTRO_TARGET_MARGIN;

            player.velocity.x = player.speed;
            player.facingLeft = false;

            UpdateActorPhysics(&player);

            if (player.position.x >= targetX)
            {
                player.position.x = targetX;
                player.velocity.x = 0.0f;

                if (!firstArenaOutroTransitionStarted)
                {
                    firstArenaOutroTransitionStarted = true;
                    transitionShowEightHoursLater = true;
                    StartMapTransition();
                }
            }
        }
        else
        {
            player.velocity.x = 0.0f;
            player.facingLeft = true;

            UpdateActorPhysics(&player);
        }

        if (player.position.y > screenHeight + 200.0f)
        {
            ResetActor(&player);
        }

        return;
    }

    if (IsKeyPressed(KEY_E) && !playerAttackActive && playerAttackCooldownTimer <= 0.0f)
    {
        playerAttackActive = true;
        playerAttackThrewBook = false;
        playerAttackTime = 0.0f;
        playerAttackCooldownTimer = PLAYER_ATTACK_COOLDOWN;
    }

    if (playerAttackActive)
    {
        int attackFrame = (int)(playerAttackTime * PLAYER_ATTACK_SPEED);

        playerAttackTime += dt;

        if (!playerAttackThrewBook && attackFrame >= PLAYER_BOOK_THROW_FRAME)
        {
            LaunchPlayerBook();
            playerAttackThrewBook = true;
        }

        if (playerAttackTime >= ((float)PLAYER_ATTACK_FRAMES / PLAYER_ATTACK_SPEED))
        {
            playerAttackActive = false;
            playerAttackThrewBook = false;
            playerAttackTime = 0.0f;
        }
    }

    float moveInput = 0.0f;

    if (IsFinalFightMap())
    {
        if (!playerAttackActive && (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)))
        {
            moveInput = -1.0f;
        }

        if (!playerAttackActive && (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)))
        {
            moveInput = 1.0f;
        }
    }
    else if (!playerAttackActive && (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)))
    {
        moveInput = 1.0f;
    }

    player.velocity.x = moveInput * player.speed;

    if (moveInput > 0.0f)
    {
        player.facingLeft = false;
    }
    else if (moveInput < 0.0f)
    {
        player.facingLeft = true;
    }

    if (!playerAttackActive &&
        (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) &&
        player.onGround)
    {
        player.velocity.y = -player.jumpVelocity;
        player.onGround = false;
    }

    UpdateActorPhysics(&player);

    if (player.position.y > screenHeight + 200.0f)
    {
        if (IsFinalFightStage2() || (IsFinalFightStage1() && finalFightLiftActive))
        {
            StartGameOverTransition();
        }
        else
        {
            ResetActor(&player);
        }
    }

    if (IsFinalFightStage2())
    {
        int currentPlatformIndex = GetActorStage2PlatformIndex(&player);

        if (playerStage2PlatformIndex >= 0 &&
            currentPlatformIndex != playerStage2PlatformIndex)
        {
            finalFightFallingPlatforms[playerStage2PlatformIndex].frozen = true;
            finalFightFallingPlatforms[playerStage2PlatformIndex].freezeTimer =
                FINAL_FIGHT_PLATFORM_FREEZE_TIME;
        }

        playerStage2PlatformIndex = currentPlatformIndex;
    }
}

void UpdatePlayerBook(void)
{
    if (!playerBook.active || playerBook.stopped)
    {
        return;
    }

    float dt = GetFrameTime();
    float bookGravity = IsFinalFightStage2() ? FINAL_FIGHT_BOOK_GRAVITY : PLAYER_BOOK_GRAVITY;
    Rectangle previousBounds = GetPlayerBookBounds();
    float holeLeft = 0.0f;
    float holeRight = 0.0f;

    playerBook.animationTime += dt;
    playerBook.center.x += playerBook.velocity.x * dt;
    playerBook.center.y += playerBook.velocity.y * dt;
    playerBook.velocity.y += bookGravity * dt;

    Rectangle currentBounds = GetPlayerBookBounds();
    int currentFrame = GetPlayerBookFrameIndex();

    if (CheckCollisionRecs(currentBounds, GetActorBounds(&kieran)))
    {
        if (IsFinalFightStage2())
        {
            kieranHealth -= PLAYER_BOOK_DAMAGE;

            if (kieranHealth < 0)
            {
                kieranHealth = 0;
            }

            kieranFreezeTimer = KIERAN_BOOK_FREEZE_DURATION;
            ResetPlayerBookState();

            if (kieranHealth == 0)
            {
                StartVictoryTransition();
            }

            return;
        }

        kieranFreezeTimer = KIERAN_BOOK_FREEZE_DURATION;
        FreezePlayerBook(currentFrame);
        return;
    }

    if (playerBook.velocity.y >= 0.0f)
    {
        Rectangle platformRect = GetFloatingPlatformRect();
        float platformTop = GetFloatingPlatformSurfaceY();
        float bookCenterX = currentBounds.x + currentBounds.width * 0.5f;
        bool crossedPlatform = previousBounds.y + previousBounds.height <= platformTop &&
                               currentBounds.y + currentBounds.height >= platformTop;

        if (GetCurrentPlatformTexture().id != 0 &&
            bookCenterX >= platformRect.x &&
            bookCenterX <= platformRect.x + platformRect.width &&
            crossedPlatform)
        {
            FreezePlayerBookOnSurface(platformTop, PLAYER_BOOK_FRAMES - 1);
            return;
        }

        if (IsFinalFightStage2())
        {
            for (int i = 0; i < FINAL_FIGHT_FALLING_PLATFORM_COUNT; i++)
            {
                Rectangle fallingPlatformRect = GetFinalFightFallingPlatformRect(i);
                float platformTopY = fallingPlatformRect.y;
                bool crossedPlatformTop = previousBounds.y + previousBounds.height <= platformTopY &&
                                          currentBounds.y + currentBounds.height >= platformTopY;

                if (bookCenterX >= fallingPlatformRect.x &&
                    bookCenterX <= fallingPlatformRect.x + fallingPlatformRect.width &&
                    crossedPlatformTop)
                {
                    FreezePlayerBookOnSurface(platformTopY, PLAYER_BOOK_FRAMES - 1);
                    return;
                }
            }
        }
        else
        {
            float groundTop = GetGroundTopY();
            bool solidGround = IsFirstArenaMap() || IsFinalFightStage1();
            bool crossedGround = previousBounds.y + previousBounds.height <= groundTop &&
                                 currentBounds.y + currentBounds.height >= groundTop;

            GetHoleBounds(&holeLeft, &holeRight);

            if (IsFinalFightStage1())
            {
                groundTop += finalFightLiftOffset;
                crossedGround = previousBounds.y + previousBounds.height <= groundTop &&
                                currentBounds.y + currentBounds.height >= groundTop;

                if (finalFightLiftActive)
                {
                    for (int i = 0; i < finalFightLiftPlatformCount; i++)
                    {
                        Rectangle liftPlatformRect = GetFinalFightLiftPlatformRect(i);
                        float liftedPlatformTop = liftPlatformRect.y;
                        bool crossedLiftedPlatform =
                            previousBounds.y + previousBounds.height <= liftedPlatformTop &&
                            currentBounds.y + currentBounds.height >= liftedPlatformTop;

                        if (bookCenterX >= liftPlatformRect.x &&
                            bookCenterX <= liftPlatformRect.x + liftPlatformRect.width &&
                            crossedLiftedPlatform)
                        {
                            FreezePlayerBookOnSurface(liftedPlatformTop, PLAYER_BOOK_FRAMES - 1);
                            return;
                        }
                    }
                }

                if (finalFightSafetyPlatformActive)
                {
                    Rectangle safetyPlatformRect = GetFinalFightSafetyPlatformRect();
                    float safetyPlatformTop = GetFinalFightSafetyPlatformSurfaceY();
                    bool crossedSafetyPlatform = previousBounds.y + previousBounds.height <= safetyPlatformTop &&
                                                currentBounds.y + currentBounds.height >= safetyPlatformTop;

                    if (bookCenterX >= safetyPlatformRect.x &&
                        bookCenterX <= safetyPlatformRect.x + safetyPlatformRect.width &&
                        crossedSafetyPlatform)
                    {
                        FreezePlayerBookOnSurface(safetyPlatformTop, PLAYER_BOOK_FRAMES - 1);
                        return;
                    }
                }
            }

            if (solidGround ||
                bookCenterX < holeLeft ||
                bookCenterX > holeRight)
            {
                if (crossedGround)
                {
                    FreezePlayerBookOnSurface(groundTop, PLAYER_BOOK_FRAMES - 1);
                    return;
                }
            }
        }
    }

    if (currentBounds.x + currentBounds.width < -PLAYER_BOOK_DESPAWN_MARGIN ||
        currentBounds.x > (float)screenWidth + PLAYER_BOOK_DESPAWN_MARGIN ||
        currentBounds.y > (float)screenHeight + PLAYER_BOOK_DESPAWN_MARGIN)
    {
        ResetPlayerBookState();
    }
}

/* Dialog flow */
void UpdateOpeningMapDialog(void)
{
    const char *kieranLine1 = (openingMapDialogPage == 0) ?
                              OPENING_DIALOG_KIERAN_LINE_1 :
                              OPENING_DIALOG_2_KIERAN_LINE_1;
    const char *kieranLine2 = (openingMapDialogPage == 0) ?
                              OPENING_DIALOG_KIERAN_LINE_2 :
                              OPENING_DIALOG_2_KIERAN_LINE_2;
    const char *playerLine = (openingMapDialogPage == 0) ?
                             OPENING_DIALOG_PLAYER_LINE :
                             OPENING_DIALOG_2_PLAYER_LINE;
    int totalKieranChars = TextLength(kieranLine1) + TextLength(kieranLine2);
    int totalPlayerChars = TextLength(playerLine);
    bool advancePressed = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);

    if (currentMapIndex != 0 || !openingMapDialogActive)
    {
        return;
    }

    if (!openingMapPlayerReplyVisible)
    {
        if (openingMapDialogCharCount < (float)totalKieranChars)
        {
            openingMapDialogCharCount += FIRST_ARENA_DIALOG_SPEED * GetFrameTime();

            if (openingMapDialogCharCount > (float)totalKieranChars)
            {
                openingMapDialogCharCount = (float)totalKieranChars;
            }
        }

        if (!advancePressed)
        {
            return;
        }

        if (openingMapDialogCharCount < (float)totalKieranChars)
        {
            openingMapDialogCharCount = (float)totalKieranChars;
            return;
        }

        openingMapPlayerReplyVisible = true;
        openingMapPlayerDialogCharCount = 0.0f;
        return;
    }

    if (openingMapPlayerDialogCharCount < (float)totalPlayerChars)
    {
        openingMapPlayerDialogCharCount += FIRST_ARENA_DIALOG_SPEED * GetFrameTime();

        if (openingMapPlayerDialogCharCount > (float)totalPlayerChars)
        {
            openingMapPlayerDialogCharCount = (float)totalPlayerChars;
        }
    }

    if (advancePressed && openingMapPlayerDialogCharCount < (float)totalPlayerChars)
    {
        openingMapPlayerDialogCharCount = (float)totalPlayerChars;
        return;
    }

    if (advancePressed && openingMapPlayerDialogCharCount >= (float)totalPlayerChars)
    {
        if (openingMapDialogPage == 0)
        {
            openingMapDialogPage = 1;
            openingMapPlayerReplyVisible = false;
            openingMapDialogCharCount = 0.0f;
            openingMapPlayerDialogCharCount = 0.0f;
        }
        else
        {
            openingMapDialogActive = false;
            openingMapEscapeActive = true;
            openingMapEscapeJumped = false;
            chaseStarted = true;
        }
    }
}

void UpdateFirstArenaDialog(void)
{
    int totalKieranChars = TextLength(FIRST_ARENA_DIALOG_KIERAN_LINE_1) +
                           TextLength(FIRST_ARENA_DIALOG_KIERAN_LINE_2);
    int totalPlayerChars = TextLength(FIRST_ARENA_DIALOG_PLAYER_LINE);
    bool advancePressed = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);

    if (!IsFirstArenaMap() || !firstArenaRunInFinished)
    {
        return;
    }

    if (!firstArenaPlayerReplyVisible)
    {
        if (firstArenaDialogCharCount < (float)totalKieranChars)
        {
            firstArenaDialogCharCount += FIRST_ARENA_DIALOG_SPEED * GetFrameTime();

            if (firstArenaDialogCharCount > (float)totalKieranChars)
            {
                firstArenaDialogCharCount = (float)totalKieranChars;
            }
        }

        if (!advancePressed)
        {
            return;
        }

        if (firstArenaDialogCharCount < (float)totalKieranChars)
        {
            firstArenaDialogCharCount = (float)totalKieranChars;
            return;
        }

        firstArenaPlayerReplyVisible = true;
        firstArenaPlayerDialogCharCount = 0.0f;
        return;
    }

    if (firstArenaPlayerDialogCharCount < (float)totalPlayerChars)
    {
        firstArenaPlayerDialogCharCount += FIRST_ARENA_DIALOG_SPEED * GetFrameTime();

        if (firstArenaPlayerDialogCharCount > (float)totalPlayerChars)
        {
            firstArenaPlayerDialogCharCount = (float)totalPlayerChars;
        }
    }

    if (advancePressed && firstArenaPlayerDialogCharCount < (float)totalPlayerChars)
    {
        firstArenaPlayerDialogCharCount = (float)totalPlayerChars;
        return;
    }

    if (advancePressed && firstArenaPlayerDialogCharCount >= (float)totalPlayerChars)
    {
        firstArenaOutroWalkStarted = true;
    }
}

void UpdateFinalFightDialog(void)
{
    int totalKieranChars = TextLength(FINAL_FIGHT_DIALOG_KIERAN_LINE_1) +
                           TextLength(FINAL_FIGHT_DIALOG_KIERAN_LINE_2);
    int totalPlayerChars = TextLength(FINAL_FIGHT_DIALOG_PLAYER_LINE);
    int totalFuryChars = TextLength(FINAL_FIGHT_DIALOG_FURY_LINE);
    int totalSafetyChars = TextLength(FINAL_FIGHT_DIALOG_SAFETY_LINE);
    int totalTrapChars = TextLength(FINAL_FIGHT_DIALOG_TRAP_LINE);
    bool advancePressed = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);

    if (!finalFightDialogActive)
    {
        return;
    }

    if (finalFightDialogPhase == FINAL_FIGHT_DIALOG_PHASE_FURY)
    {
        if (finalFightFuryDialogCharCount < (float)totalFuryChars)
        {
            finalFightFuryDialogCharCount += FINAL_FIGHT_DIALOG_SPEED * GetFrameTime();

            if (finalFightFuryDialogCharCount > (float)totalFuryChars)
            {
                finalFightFuryDialogCharCount = (float)totalFuryChars;
            }
        }

        if (!advancePressed)
        {
            return;
        }

        if (finalFightFuryDialogCharCount < (float)totalFuryChars)
        {
            finalFightFuryDialogCharCount = (float)totalFuryChars;
            return;
        }

        if (IsFinalFightStage1())
        {
            StartFinalFightLift();
        }
        else
        {
            finalFightDialogActive = false;
        }

        return;
    }

    if (finalFightDialogPhase == FINAL_FIGHT_DIALOG_PHASE_SAFETY)
    {
        if (finalFightSafetyDialogCharCount < (float)totalSafetyChars)
        {
            finalFightSafetyDialogCharCount += FINAL_FIGHT_DIALOG_SPEED * GetFrameTime();

            if (finalFightSafetyDialogCharCount > (float)totalSafetyChars)
            {
                finalFightSafetyDialogCharCount = (float)totalSafetyChars;
            }
        }

        if (!advancePressed)
        {
            return;
        }

        if (finalFightSafetyDialogCharCount < (float)totalSafetyChars)
        {
            finalFightSafetyDialogCharCount = (float)totalSafetyChars;
            return;
        }

        finalFightDialogActive = false;
        finalFightSafetyPlatformPulling = true;
        return;
    }

    if (finalFightDialogPhase == FINAL_FIGHT_DIALOG_PHASE_TRAP)
    {
        if (finalFightTrapDialogCharCount < (float)totalTrapChars)
        {
            finalFightTrapDialogCharCount += FINAL_FIGHT_DIALOG_SPEED * GetFrameTime();

            if (finalFightTrapDialogCharCount > (float)totalTrapChars)
            {
                finalFightTrapDialogCharCount = (float)totalTrapChars;
            }
        }

        if (!advancePressed)
        {
            return;
        }

        if (finalFightTrapDialogCharCount < (float)totalTrapChars)
        {
            finalFightTrapDialogCharCount = (float)totalTrapChars;
            return;
        }

        StartFinalFightStage2();
        return;
    }

    if (!finalFightPlayerReplyVisible)
    {
        if (finalFightDialogCharCount < (float)totalKieranChars)
        {
            finalFightDialogCharCount += FINAL_FIGHT_DIALOG_SPEED * GetFrameTime();

            if (finalFightDialogCharCount > (float)totalKieranChars)
            {
                finalFightDialogCharCount = (float)totalKieranChars;
            }
        }

        if (!advancePressed)
        {
            return;
        }

        if (finalFightDialogCharCount < (float)totalKieranChars)
        {
            finalFightDialogCharCount = (float)totalKieranChars;
            return;
        }

        finalFightPlayerReplyVisible = true;
        finalFightPlayerDialogCharCount = 0.0f;
        return;
    }

    if (finalFightPlayerDialogCharCount < (float)totalPlayerChars)
    {
        finalFightPlayerDialogCharCount += FINAL_FIGHT_DIALOG_SPEED * GetFrameTime();

        if (finalFightPlayerDialogCharCount > (float)totalPlayerChars)
        {
            finalFightPlayerDialogCharCount = (float)totalPlayerChars;
        }
    }

    if (advancePressed && finalFightPlayerDialogCharCount < (float)totalPlayerChars)
    {
        finalFightPlayerDialogCharCount = (float)totalPlayerChars;
        return;
    }

    if (advancePressed && finalFightPlayerDialogCharCount >= (float)totalPlayerChars)
    {
        finalFightDialogPhase = FINAL_FIGHT_DIALOG_PHASE_FURY;
        finalFightFuryDialogCharCount = 0.0f;
        finalFightFuryMode = true;
    }
}

/* Enemy chase and boss behaviour */
void UpdateKieran(void)
{
    float dt = GetFrameTime();
    float holeLeft = 0.0f;
    float holeRight = 0.0f;
    float playerCenter = player.position.x + player.width * 0.5f;
    float kieranCenter = kieran.position.x + kieran.width * 0.5f;
    float dx = playerCenter - kieranCenter;
    float moveDirection = 0.0f;
    Rectangle platformRect = GetFloatingPlatformRect();
    float platformTop = GetFloatingPlatformSurfaceY();
    float chaseSpeed = kieran.speed;

    if (kieranFreezeTimer > 0.0f)
    {
        kieranFreezeTimer -= dt;

        if (kieranFreezeTimer < 0.0f)
        {
            kieranFreezeTimer = 0.0f;
        }

        if (IsFinalFightStage2())
        {
            Rectangle brickPlatformRect = GetFinalFightBrickPlatformRect();

            kieran.velocity = (Vector2){0.0f, 0.0f};
            kieran.position.x = brickPlatformRect.x + brickPlatformRect.width * 0.5f - kieran.width * 0.5f;
            kieran.position.y = brickPlatformRect.y - kieran.height;
            kieran.onGround = true;
            kieran.facingLeft = false;
            return;
        }

        kieran.velocity.x = 0.0f;
        UpdateActorPhysics(&kieran);

        if (IsFinalFightStage2() && kieran.position.y > screenHeight + FINAL_FIGHT_KIERAN_RESPAWN_MARGIN)
        {
            Rectangle respawnRect = GetFinalFightFallingPlatformRect(0);

            kieran.position.x = respawnRect.x + respawnRect.width * 0.5f - kieran.width * 0.5f;
            kieran.position.y = respawnRect.y - kieran.height;
            kieran.velocity = (Vector2){0.0f, 0.0f};
            kieran.onGround = true;
            kieranStage2PlatformIndex = 0;
        }

        return;
    }

    if (IsFinalFightStage1())
    {
        float groundTop = GetGroundTopY();

        if (!finalFightStage1RunInFinished)
        {
            kieran.facingLeft = false;
            kieran.velocity.x = kieran.speed;
            kieran.position.x += kieran.velocity.x * dt;
            kieran.position.y = groundTop - kieran.height;
            kieran.onGround = true;

            if (kieran.position.x >= FINAL_FIGHT_STAGE1_KIERAN_STOP_X)
            {
                kieran.position.x = FINAL_FIGHT_STAGE1_KIERAN_STOP_X;
                kieran.velocity.x = 0.0f;
                finalFightStage1RunInFinished = true;
                finalFightDialogActive = true;
                finalFightPlayerReplyVisible = false;
                finalFightDialogCharCount = 0.0f;
                finalFightPlayerDialogCharCount = 0.0f;
                finalFightFuryDialogCharCount = 0.0f;
                finalFightDialogPhase = FINAL_FIGHT_DIALOG_PHASE_INTRO;
                player.velocity.x = 0.0f;
                player.facingLeft = true;
            }
        }
        else
        {
            kieran.velocity.x = 0.0f;
            UpdateActorPhysics(&kieran);
        }

        return;
    }

    if (IsFinalFightStage2())
    {
        Rectangle brickPlatformRect = GetFinalFightBrickPlatformRect();

        kieran.velocity = (Vector2){0.0f, 0.0f};
        kieran.position.x = brickPlatformRect.x + brickPlatformRect.width * 0.5f - kieran.width * 0.5f;
        kieran.position.y = brickPlatformRect.y - kieran.height;
        kieran.onGround = true;
        kieran.facingLeft = false;
        kieranStage2PlatformIndex = -1;

        return;
    }

    if (finalFightDialogActive)
    {
        kieran.velocity = (Vector2){0.0f, 0.0f};
        kieran.onGround = true;
        kieran.facingLeft = false;
        return;
    }

    if (IsFirstArenaMap())
    {
        if (!firstArenaRunInStarted &&
            playerCenter >= screenWidth * FIRST_ARENA_TRIGGER_X_RATIO)
        {
            firstArenaRunInStarted = true;
        }

        if (firstArenaRunInStarted && !firstArenaRunInFinished)
        {
            kieran.facingLeft = false;
            kieran.velocity.x = kieran.speed;
            kieran.position.x += kieran.velocity.x * dt;
            kieran.position.y = kieran.spawn.y;
            kieran.onGround = true;

            if (kieran.position.x >= FIRST_ARENA_KIERAN_STOP_X)
            {
                kieran.position.x = FIRST_ARENA_KIERAN_STOP_X;
                kieran.velocity.x = 0.0f;
                firstArenaRunInFinished = true;
                firstArenaPlayerReplyVisible = false;
                firstArenaDialogCharCount = 0.0f;
                firstArenaPlayerDialogCharCount = 0.0f;
                player.velocity.x = 0.0f;
                player.facingLeft = true;
            }
        }
        else
        {
            kieran.velocity.x = 0.0f;
            kieran.position.y = kieran.spawn.y;
            kieran.onGround = true;
        }

        return;
    }

    if (currentMapIndex == 0 && !chaseStarted)
    {
        float groundTop = GetGroundTopY();

        if (!openingMapRunInStarted)
        {
            openingMapRunInStarted = true;
        }

        if (!openingMapRunInFinished)
        {
            kieran.facingLeft = false;
            kieran.velocity.x = OPENING_KIERAN_WALK_SPEED;
            kieran.position.x += kieran.velocity.x * dt;
            kieran.position.y = groundTop - kieran.height;
            kieran.onGround = true;

            if (kieran.position.x >= OPENING_KIERAN_STOP_X)
            {
                kieran.position.x = OPENING_KIERAN_STOP_X;
                kieran.velocity.x = 0.0f;
                openingMapRunInFinished = true;
                openingMapDialogActive = true;
                openingMapPlayerReplyVisible = false;
                openingMapDialogCharCount = 0.0f;
                openingMapPlayerDialogCharCount = 0.0f;
                openingMapDialogPage = 0;
                player.velocity.x = 0.0f;
                player.facingLeft = true;
            }
        }
        else
        {
            kieran.velocity.x = 0.0f;
            kieran.position.y = groundTop - kieran.height;
            kieran.onGround = true;
        }

        return;
    }

    if (currentMapIndex == 0 && openingMapEscapeActive)
    {
        chaseSpeed = kieran.speed * OPENING_ESCAPE_KIERAN_SPEED_SCALE;
    }

    if (!chaseStarted)
    {
        kieran.velocity.x = 0.0f;
        return;
    }

    if (AbsFloat(dx) > 8.0f)
    {
        moveDirection = (dx < 0.0f) ? -1.0f : 1.0f;
    }

    if (currentMapIndex == 0 && openingMapEscapeActive &&
        dx > 0.0f && dx < OPENING_ESCAPE_MIN_GAP)
    {
        moveDirection = 0.0f;
    }

    kieran.velocity.x = moveDirection * chaseSpeed;

    if (moveDirection < 0.0f)
    {
        kieran.facingLeft = true;
    }
    else if (moveDirection > 0.0f)
    {
        kieran.facingLeft = false;
    }

    GetHoleBounds(&holeLeft, &holeRight);

    if (kieran.onGround)
    {
        bool playerOnPlatform = playerCenter >= platformRect.x &&
                                playerCenter <= platformRect.x + platformRect.width &&
                                AbsFloat((player.position.y + player.height) - platformTop) < 8.0f;
        bool kieranLeftOfHole = kieranCenter < holeLeft;
        bool kieranRightOfHole = kieranCenter > holeRight;
        bool kieranOnPlatform = !kieranLeftOfHole &&
                                !kieranRightOfHole &&
                                AbsFloat((kieran.position.y + kieran.height) - platformTop) < 8.0f;
        bool playerLeftOfHole = !playerOnPlatform && playerCenter < holeLeft;
        bool playerRightOfHole = !playerOnPlatform && playerCenter > holeRight;
        bool headingRight = moveDirection > 0.0f;
        bool headingLeft = moveDirection < 0.0f;
        float nextCenter = kieranCenter + (kieran.velocity.x * dt);

        bool jumpFromLeftGround = headingRight &&
                                  kieranLeftOfHole &&
                                  (playerOnPlatform || playerRightOfHole) &&
                                  nextCenter >= holeLeft - 18.0f;
        bool jumpFromRightGround = headingLeft &&
                                   kieranRightOfHole &&
                                   (playerOnPlatform || playerLeftOfHole) &&
                                   nextCenter <= holeRight + 18.0f;
        bool jumpFromLeftPlatformEdge = headingLeft &&
                                        kieranOnPlatform &&
                                        playerLeftOfHole &&
                                        nextCenter <= platformRect.x + 22.0f;
        bool jumpFromRightPlatformEdge = headingRight &&
                                         kieranOnPlatform &&
                                         playerRightOfHole &&
                                         nextCenter >= platformRect.x + platformRect.width - 22.0f;

        if (jumpFromLeftGround || jumpFromRightGround ||
            jumpFromLeftPlatformEdge || jumpFromRightPlatformEdge)
        {
            kieran.velocity.y = -kieran.jumpVelocity;
            kieran.onGround = false;
        }
    }

    UpdateActorPhysics(&kieran);

    if (kieran.position.y > screenHeight + 200.0f)
    {
        ResetActor(&kieran);
    }
}

/* Rendering */
/* Full-screen background draw */
void DrawBackgroundFull(void)
{
    Texture2D backgroundTex = textures[TEXTURE_BACKGROUND];

    if (IsFinalFightMap() && textures[TEXTURE_ARENA_BACKGROUND].id != 0)
    {
        backgroundTex = textures[TEXTURE_ARENA_BACKGROUND];
    }

    if (backgroundTex.id == 0) return;

    DrawTexturePro(
        backgroundTex,
        (Rectangle){0, 0, (float)backgroundTex.width, (float)backgroundTex.height},
        (Rectangle){0, 0, (float)screenWidth, (float)screenHeight},
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
}

void DrawGround(void)
{
    int grassY = (int)GetGroundTopY();
    int totalRowsToDraw = ((screenHeight - grassY) / DRAW_TILE_SIZE) + 2;
    int holeStart = GetHoleStartColumn();
    int holeEnd = holeStart + HOLE_WIDTH - 1;
    bool solidFloor = IsFirstArenaMap();

    for (int x = 0; x < VISIBLE_COLUMNS; x++)
    {
        if (!solidFloor && x >= holeStart && x <= holeEnd)
            continue;

        int drawX = x * DRAW_TILE_SIZE;
        Vector2 introOffset = GetOpeningIntroGroundOffset(x);

        Texture2D topTex = GetGrassTexture(groundGrassType[x]);
        Texture2D underTex = GetUndergroundTexture(undergroundType[x]);

        if (!solidFloor && x == holeStart - 1 && textures[TEXTURE_CORNER_RIGHT].id != 0)
        {
            topTex = textures[TEXTURE_CORNER_RIGHT];
        }
        else if (!solidFloor && x == holeEnd + 1 && textures[TEXTURE_CORNER_LEFT].id != 0)
        {
            topTex = textures[TEXTURE_CORNER_LEFT];
        }
        else if (!solidFloor && (x == holeStart - 2 || x == holeEnd + 2) &&
                 textures[TEXTURE_CLOSE_CORNER].id != 0)
        {
            topTex = textures[TEXTURE_CLOSE_CORNER];
        }

        if (topTex.id != 0)
        {
            DrawTextureEx(
                topTex,
                (Vector2){(float)drawX + introOffset.x, (float)grassY + introOffset.y},
                0.0f,
                TILE_SCALE,
                WHITE
            );
        }
        else
        {
            DrawRectangle((int)(drawX + introOffset.x), (int)(grassY + introOffset.y),
                          DRAW_TILE_SIZE, DRAW_TILE_SIZE, GREEN);
        }

        for (int row = 1; row < totalRowsToDraw; row++)
        {
            int dirtY = grassY + row * DRAW_TILE_SIZE;
            Texture2D rowTex = underTex;

            if (!solidFloor && row == 1)
            {
                if (x == holeStart - 1 && textures[TEXTURE_UNDER_CORNER_RIGHT].id != 0)
                {
                    rowTex = textures[TEXTURE_UNDER_CORNER_RIGHT];
                }
                else if (x == holeEnd + 1 && textures[TEXTURE_UNDER_CORNER_LEFT].id != 0)
                {
                    rowTex = textures[TEXTURE_UNDER_CORNER_LEFT];
                }
                else if ((x == holeStart - 2 || x == holeEnd + 2) &&
                         textures[TEXTURE_UNDER_CLOSE_CORNER].id != 0)
                {
                    rowTex = textures[TEXTURE_UNDER_CLOSE_CORNER];
                }
            }

            if (rowTex.id != 0)
            {
                DrawTextureEx(
                    rowTex,
                    (Vector2){(float)drawX + introOffset.x, (float)dirtY + introOffset.y},
                    0.0f,
                    TILE_SCALE,
                    WHITE
                );
            }
            else
            {
                DrawRectangle((int)(drawX + introOffset.x), (int)(dirtY + introOffset.y),
                              DRAW_TILE_SIZE, DRAW_TILE_SIZE, BROWN);
            }
        }
    }
}

void DrawFloatingPlatform(void)
{
    Texture2D platformTex = GetCurrentPlatformTexture();
    Vector2 introOffset = {0.0f, 0.0f};

    if (platformTex.id == 0) return;

    Rectangle platformRect = GetFloatingPlatformRect();

    if (UseOpeningIntro())
    {
        float progress = GetOpeningIntroProgress(0.18f);
        introOffset.y = -(screenHeight + 220.0f) * (1.0f - progress);
    }

    DrawTexturePro(
        platformTex,
        (Rectangle){0, 0,
                    (float)platformTex.width,
                    (float)platformTex.height},
        (Rectangle){platformRect.x + introOffset.x, platformRect.y + introOffset.y,
                    platformRect.width, platformRect.height},
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
}

void DrawPlayer(void)
{
    Texture2D playerTex = GetPlayerFrame();

    if (playerTex.id == 0)
    {
        Rectangle body = GetActorBounds(&player);
        DrawRectangleRec(body, player.onGround ? SKYBLUE : BLUE);
        DrawRectangleLinesEx(body, 3.0f, (Color){15, 30, 70, 255});
        DrawText("YOU", (int)player.position.x - 2, (int)player.position.y - 24, 22, WHITE);
        return;
    }

    DrawActorSprite(playerTex, &player);
}

void DrawPlayerBook(void)
{
    Texture2D bookTex;
    Rectangle drawRect;
    Rectangle source;

    if (!playerBook.active)
    {
        return;
    }

    bookTex = GetPlayerBookTexture(GetPlayerBookFrameIndex());
    drawRect = GetPlayerBookBounds();

    if (bookTex.id == 0)
    {
        DrawRectangleRec(drawRect, GOLD);
        return;
    }

    source = (Rectangle){0.0f, 0.0f, (float)bookTex.width, (float)bookTex.height};

    if (playerBook.facingLeft)
    {
        source.x = (float)bookTex.width;
        source.width = -(float)bookTex.width;
    }

    DrawTexturePro(
        bookTex,
        source,
        drawRect,
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
}

void DrawKieran(void)
{
    Texture2D kieranTex = GetKieranFrame();

    if (kieranTex.id == 0)
    {
        DrawRectangleRec(GetActorBounds(&kieran), RED);
        return;
    }

    DrawActorSprite(kieranTex, &kieran);

    if (!UseOpeningIntro() && !(currentMapIndex == 0 && !chaseStarted))
    {
        DrawText("EVIL LEARY", (int)kieran.position.x - 10, (int)kieran.position.y - 24, 22,
                 (Color){255, 120, 120, 255});
    }
}

void DrawKieranHealthBar(void)
{
    if (!IsFinalFightStage2())
    {
        return;
    }

    const int barWidth = 180;
    const int barHeight = 18;
    int fillWidth = (int)((barWidth - 8) * ((float)kieranHealth / (float)KIERAN_MAX_HEALTH));
    int barX = (int)(kieran.position.x + (kieran.width - barWidth) * 0.5f);
    int barY = (int)kieran.position.y - 42;

    if (fillWidth < 0)
    {
        fillWidth = 0;
    }

    if (barX < 12)
    {
        barX = 12;
    }
    else if (barX + barWidth > screenWidth - 12)
    {
        barX = screenWidth - 12 - barWidth;
    }

    if (barY < 20)
    {
        barY = 20;
    }

    DrawRectangle(barX, barY, barWidth, barHeight, (Color){45, 20, 20, 255});
    DrawRectangle(barX + 4, barY + 4, fillWidth, barHeight - 8, (Color){210, 50, 50, 255});
    DrawRectangleLines(barX, barY, barWidth, barHeight, WHITE);
}

void DrawFinalFightTimer(void)
{
    if (!IsFinalFightStage2() || !finalFightStage2Started)
    {
        return;
    }

    int remainingSeconds = (int)ceilf(finalFightTimeRemaining);
    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;
    int boxWidth = 168;
    int boxHeight = 44;
    int boxX = screenWidth - boxWidth - 30;
    int boxY = 26;

    if (remainingSeconds < 0)
    {
        remainingSeconds = 0;
        minutes = 0;
        seconds = 0;
    }

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, (Color){0, 0, 0, 170});
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, WHITE);
    DrawText(TextFormat("TIME %d:%02d", minutes, seconds),
             boxX + 16, boxY + 10, 26, WHITE);
}

void DrawTree(void)
{
    DrawPropCentered(
        textures[TEXTURE_TREE],
        GetTreeCenterX(),
        GetDecorBaseY(),
        DECOR_SCALE
    );
}

void DrawBush(void)
{
    if (textures[TEXTURE_BUSH_2].id == 0 || textures[TEXTURE_TREE].id == 0) return;

    Rectangle treeRect = GetTreeBounds();
    float bushX = treeRect.x - (textures[TEXTURE_BUSH_2].width * DECOR_SCALE) + 120.0f;

    DrawProp(textures[TEXTURE_BUSH_2], bushX, treeRect.y + treeRect.height, DECOR_SCALE);
}

void DrawLeftBushes(void)
{
    if (textures[TEXTURE_BUSH_1].id == 0 && textures[TEXTURE_BUSH_2].id == 0) return;

    if (textures[TEXTURE_BUSH_1].id != 0)
    {
        DrawProp(textures[TEXTURE_BUSH_1], GetLeftBush1X(), GetDecorBaseY(), DECOR_SCALE);
    }

    if (textures[TEXTURE_BUSH_2].id != 0)
    {
        DrawProp(textures[TEXTURE_BUSH_2], GetLeftBush2X(), GetDecorBaseY(), DECOR_SCALE);
    }
}

void DrawFlower(void)
{
    if ((textures[TEXTURE_FLOWER_1].id == 0 && textures[TEXTURE_FLOWER_2].id == 0) ||
        textures[TEXTURE_BUSH_2].id == 0 || textures[TEXTURE_TREE].id == 0) return;

    Rectangle treeRect = GetTreeBounds();
    float bushX = treeRect.x - (textures[TEXTURE_BUSH_2].width * DECOR_SCALE) + 120.0f;
    Texture2D flowerTex = textures[TEXTURE_FLOWER_1 + (((int)(GetTime() * 1.5f)) % 2)];
    Texture2D baseFlowerTex = textures[TEXTURE_FLOWER_1];

    if (baseFlowerTex.id == 0)
    {
        baseFlowerTex = textures[TEXTURE_FLOWER_2];
    }

    if (flowerTex.id == 0)
    {
        flowerTex = baseFlowerTex;
    }

    if (flowerTex.id == 0)
    {
        return;
    }

    float flowerTargetHeight = ((baseFlowerTex.id != 0) ? baseFlowerTex.height : flowerTex.height) *
                               DECOR_SCALE;
    float flowerScale = flowerTargetHeight / (float)flowerTex.height;
    float flowerX = bushX + 80.0f - (flowerTex.width * flowerScale) * 0.25f;

    DrawProp(flowerTex, flowerX, treeRect.y + treeRect.height, flowerScale);
}

void DrawFence(void)
{
    if (textures[TEXTURE_FENCE].id == 0 || textures[TEXTURE_TREE].id == 0) return;

    Rectangle treeRect = GetTreeBounds();
    Rectangle platformRect = GetFloatingPlatformRect();
    float platformCenterX = platformRect.x + platformRect.width * 0.5f;
    float treeCenterX = treeRect.x + treeRect.width * 0.5f;
    float fenceX = ((platformCenterX + treeCenterX) * 0.5f) -
                   (textures[TEXTURE_FENCE].width * DECOR_SCALE) + 460.0f;

    DrawProp(textures[TEXTURE_FENCE], fenceX, treeRect.y + treeRect.height, DECOR_SCALE);
}

void DrawStoneBrick(void)
{
    DrawProp(textures[TEXTURE_STONE_BRICK_1], GetStoneBrickX(), GetDecorBaseY(), DECOR_SCALE);
}

void DrawSignRight(void)
{
    DrawProp(textures[TEXTURE_SIGN_RIGHT], GetSignRightX(), GetDecorBaseY(), DECOR_SCALE);
}

void DrawFinalFightLiftPlatform(int platformIndex)
{
    const float brickScale = TILE_SCALE;
    const float brickSeamOverlap = 4.0f;
    Texture2D leftTex = textures[TEXTURE_BRICK_LEFT_CORNER];
    Texture2D middleTex = textures[TEXTURE_BRICK_TILE];
    Texture2D rightTex = textures[TEXTURE_BRICK_RIGHT_CORNER];
    Rectangle platformRect = GetFinalFightLiftPlatformRect(platformIndex);
    float platformX = platformRect.x;
    float platformY = platformRect.y;
    float flippedPlatformY = platformY + (leftTex.height * brickScale) - 2.0f;
    float currentX = platformX;

    if (leftTex.id == 0 || middleTex.id == 0 || rightTex.id == 0 || platformRect.width <= 0.0f)
    {
        return;
    }

    DrawTextureEx(leftTex, (Vector2){currentX, platformY}, 0.0f, brickScale, WHITE);
    currentX += (leftTex.width * brickScale) - brickSeamOverlap;
    DrawTextureEx(middleTex, (Vector2){currentX, platformY}, 0.0f, brickScale, WHITE);
    currentX += (middleTex.width * brickScale) - brickSeamOverlap;
    DrawTextureEx(rightTex, (Vector2){currentX, platformY}, 0.0f, brickScale, WHITE);

    currentX = platformX;

    DrawTexturePro(
        leftTex,
        (Rectangle){0.0f, (float)leftTex.height, (float)leftTex.width, -(float)leftTex.height},
        (Rectangle){currentX, flippedPlatformY, leftTex.width * brickScale, leftTex.height * brickScale},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
    currentX += (leftTex.width * brickScale) - brickSeamOverlap;
    DrawTexturePro(
        middleTex,
        (Rectangle){0.0f, (float)middleTex.height, (float)middleTex.width, -(float)middleTex.height},
        (Rectangle){currentX, flippedPlatformY, middleTex.width * brickScale, middleTex.height * brickScale},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
    currentX += (middleTex.width * brickScale) - brickSeamOverlap;
    DrawTexturePro(
        rightTex,
        (Rectangle){0.0f, (float)rightTex.height, (float)rightTex.width, -(float)rightTex.height},
        (Rectangle){currentX, flippedPlatformY, rightTex.width * brickScale, rightTex.height * brickScale},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
}

void DrawFinalFightStage1FloorColumn(int column, float yOffset)
{
    int grassY = (int)GetGroundTopY();
    int totalRowsToDraw = ((screenHeight - grassY) / DRAW_TILE_SIZE) + 2;
    int drawX = column * DRAW_TILE_SIZE;
    Texture2D topTex = textures[TEXTURE_BRICK_TILE];
    Texture2D underTex = textures[TEXTURE_BRICK_TILE_UNDER];

    if (textures[TEXTURE_BRICK_LEFT_CORNER].id == 0 ||
        textures[TEXTURE_BRICK_TILE].id == 0 ||
        textures[TEXTURE_BRICK_RIGHT_CORNER].id == 0 ||
        textures[TEXTURE_BRICK_LEFT_CORNER_UNDER].id == 0 ||
        textures[TEXTURE_BRICK_TILE_UNDER].id == 0 ||
        textures[TEXTURE_BRICK_RIGHT_CORNER_UNDER].id == 0)
    {
        return;
    }

    if (column == 0)
    {
        topTex = textures[TEXTURE_BRICK_LEFT_CORNER];
        underTex = textures[TEXTURE_BRICK_LEFT_CORNER_UNDER];
    }
    else if (column == VISIBLE_COLUMNS - 1)
    {
        topTex = textures[TEXTURE_BRICK_RIGHT_CORNER];
        underTex = textures[TEXTURE_BRICK_RIGHT_CORNER_UNDER];
    }

    DrawTextureEx(topTex, (Vector2){(float)drawX, (float)grassY - yOffset}, 0.0f, TILE_SCALE, WHITE);

    for (int row = 1; row < totalRowsToDraw; row++)
    {
        int dirtY = grassY + row * DRAW_TILE_SIZE;
        DrawTextureEx(underTex, (Vector2){(float)drawX, (float)dirtY - yOffset}, 0.0f, TILE_SCALE, WHITE);
    }
}

void DrawFinalFightStage1Floor(void)
{
    float floorDropOffset = finalFightLiftActive ?
                            -(finalFightLiftOffset * FINAL_FIGHT_FLOOR_DROP_RATIO) :
                            0.0f;

    for (int x = 0; x < VISIBLE_COLUMNS; x++)
    {
        if (finalFightLiftActive && IsFinalFightLiftColumn(x))
        {
            continue;
        }

        DrawFinalFightStage1FloorColumn(x, floorDropOffset);
    }

    if (!finalFightLiftActive)
    {
        return;
    }

    for (int i = 0; i < finalFightLiftPlatformCount; i++)
    {
        DrawFinalFightLiftPlatform(i);
    }
}

void DrawFinalFightSafetyPlatform(void)
{
    Texture2D platformTex = textures[TEXTURE_PLATFORM_MIDDLE];

    if (!finalFightSafetyPlatformActive || platformTex.id == 0)
    {
        return;
    }

    DrawTextureEx(
        platformTex,
        finalFightSafetyPlatformPosition,
        0.0f,
        FINAL_FIGHT_SAFETY_PLATFORM_SCALE,
        WHITE
    );
}

void DrawFinalFightStage2Platforms(void)
{
    if (!IsFinalFightStage2())
    {
        return;
    }

    for (int i = 0; i < FINAL_FIGHT_FALLING_PLATFORM_COUNT; i++)
    {
        Texture2D platformTex = GetFinalFightFallingPlatformTexture(i);

        if (platformTex.id == 0)
        {
            continue;
        }

        DrawTextureEx(
            platformTex,
            finalFightFallingPlatforms[i].position,
            0.0f,
            FINAL_FIGHT_FALLING_PLATFORM_SCALE,
            WHITE
        );
    }
}

void DrawFinalFightBrickPlatform(void)
{
    const float brickScale = TILE_SCALE;
    const float brickSeamOverlap = 4.0f;
    Texture2D leftTex = textures[TEXTURE_BRICK_LEFT_CORNER];
    Texture2D middleTex = textures[TEXTURE_BRICK_TILE];
    Texture2D rightTex = textures[TEXTURE_BRICK_RIGHT_CORNER];
    Rectangle platformRect = GetFinalFightBrickPlatformRect();
    float platformX = platformRect.x;
    float platformY = platformRect.y;
    float flippedPlatformY = platformY + (leftTex.height * brickScale) - 2.0f;
    float currentX = platformX;

    if (!IsFinalFightStage2() || leftTex.id == 0 || middleTex.id == 0 || rightTex.id == 0)
    {
        return;
    }

    DrawTextureEx(leftTex, (Vector2){currentX, platformY}, 0.0f, brickScale, WHITE);
    currentX += (leftTex.width * brickScale) - brickSeamOverlap;
    DrawTextureEx(middleTex, (Vector2){currentX, platformY}, 0.0f, brickScale, WHITE);
    currentX += (middleTex.width * brickScale) - brickSeamOverlap;
    DrawTextureEx(rightTex, (Vector2){currentX, platformY}, 0.0f, brickScale, WHITE);

    currentX = platformX;

    DrawTexturePro(
        leftTex,
        (Rectangle){0.0f, (float)leftTex.height, (float)leftTex.width, -(float)leftTex.height},
        (Rectangle){currentX, flippedPlatformY, leftTex.width * brickScale, leftTex.height * brickScale},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
    currentX += (leftTex.width * brickScale) - brickSeamOverlap;
    DrawTexturePro(
        middleTex,
        (Rectangle){0.0f, (float)middleTex.height, (float)middleTex.width, -(float)middleTex.height},
        (Rectangle){currentX, flippedPlatformY, middleTex.width * brickScale, middleTex.height * brickScale},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
    currentX += (middleTex.width * brickScale) - brickSeamOverlap;
    DrawTexturePro(
        rightTex,
        (Rectangle){0.0f, (float)rightTex.height, (float)rightTex.width, -(float)rightTex.height},
        (Rectangle){currentX, flippedPlatformY, rightTex.width * brickScale, rightTex.height * brickScale},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
}

void DrawFirstArenaPlatforms(void)
{
    if (!IsFirstArenaMap())
    {
        return;
    }

    if (textures[TEXTURE_PLATFORM_TOP].id != 0)
    {
        DrawTextureEx(
            textures[TEXTURE_PLATFORM_TOP],
            (Vector2){110.0f, 145.0f},
            0.0f,
            3.5f,
            WHITE
        );
    }

    if (textures[TEXTURE_PLATFORM_ARENA].id != 0)
    {
        DrawTextureEx(
            textures[TEXTURE_PLATFORM_ARENA],
            (Vector2){1540.0f, 215.0f},
            0.0f,
            3.5f,
            WHITE
        );
    }

    if (textures[TEXTURE_PLATFORM_MIDDLE].id != 0)
    {
        DrawTextureEx(
            textures[TEXTURE_PLATFORM_MIDDLE],
            (Vector2){820.0f, 270.0f},
            0.0f,
            3.0f,
            WHITE
        );
    }
}

void DrawFirstArenaTree(void)
{
    if (!IsFirstArenaMap() || textures[TEXTURE_TREE].id == 0)
    {
        return;
    }

    DrawProp(textures[TEXTURE_TREE], 12.0f, GetDecorBaseY(), DECOR_SCALE);
    DrawProp(
        textures[TEXTURE_TREE],
        (float)screenWidth - (textures[TEXTURE_TREE].width * DECOR_SCALE) - 12.0f,
        GetDecorBaseY(),
        DECOR_SCALE
    );
}

void DrawFirstArenaBricks(void)
{
    const float brickScale = TILE_SCALE;
    const int underRowCount = 5;
    const float arrangementOffsetX = -220.0f;
    const float stoneGap = 80.0f;
    const float bushScale = 4.0f;
    Texture2D topRow[3] = {
        textures[TEXTURE_BRICK_LEFT_CORNER],
        textures[TEXTURE_BRICK_TILE],
        textures[TEXTURE_BRICK_RIGHT_CORNER]
    };
    Texture2D bottomRow[3] = {
        textures[TEXTURE_BRICK_LEFT_CORNER_UNDER],
        textures[TEXTURE_BRICK_TILE_UNDER],
        textures[TEXTURE_BRICK_RIGHT_CORNER_UNDER]
    };
    float totalWidth = 0.0f;
    float maxTopHeight = 0.0f;
    float maxBottomHeight = 0.0f;
    Texture2D stoneTex = textures[TEXTURE_STONE_BRICK_1];
    Texture2D bushTex = textures[TEXTURE_BUSH_1];

    if (!IsFirstArenaMap())
    {
        return;
    }

    for (int i = 0; i < 3; i++)
    {
        if (topRow[i].id == 0 || bottomRow[i].id == 0)
        {
            return;
        }

        totalWidth += topRow[i].width * brickScale;

        if ((topRow[i].height * brickScale) > maxTopHeight)
        {
            maxTopHeight = topRow[i].height * brickScale;
        }

        if ((bottomRow[i].height * brickScale) > maxBottomHeight)
        {
            maxBottomHeight = bottomRow[i].height * brickScale;
        }
    }

    float startX = (((float)screenWidth - totalWidth) * 0.5f) + arrangementOffsetX;
    float topRowY = GetGroundTopY() - maxTopHeight - (maxBottomHeight * underRowCount);
    float currentX = startX;

    for (int i = 0; i < 3; i++)
    {
        DrawTextureEx(topRow[i], (Vector2){currentX, topRowY}, 0.0f, brickScale, WHITE);

        for (int underRow = 0; underRow < underRowCount; underRow++)
        {
            float bottomRowY = topRowY + maxTopHeight + (maxBottomHeight * underRow);
            DrawTextureEx(bottomRow[i], (Vector2){currentX, bottomRowY}, 0.0f, brickScale, WHITE);
        }

        currentX += topRow[i].width * brickScale;
    }

    if (stoneTex.id != 0)
    {
        float stoneScale = TILE_SCALE;
        float towerRightX = startX + totalWidth;
        float stoneX = towerRightX + stoneGap;
        float stoneY = GetGroundTopY() - (stoneTex.height * stoneScale);

        DrawTextureEx(stoneTex, (Vector2){stoneX, stoneY}, 0.0f, stoneScale, WHITE);

        if (bushTex.id != 0)
        {
            float bushWidth = bushTex.width * bushScale;
            float bushX = towerRightX + ((stoneX - towerRightX) - bushWidth) * 0.5f;
            float bushY = GetGroundTopY() - (bushTex.height * bushScale);

            DrawTextureEx(bushTex, (Vector2){bushX, bushY}, 0.0f, bushScale, WHITE);
        }
    }
}

/* Dialogue box for the first arena cutscene */
void DrawDialogBox(void)
{
    if (!IsFirstArenaMap() || !firstArenaRunInFinished || firstArenaOutroWalkStarted ||
        textures[TEXTURE_DIALOG_1].id == 0)
    {
        return;
    }

    Texture2D dialogTex = textures[TEXTURE_DIALOG_1];
    float dialogScale = 4.0f;
    float dialogWidth = dialogTex.width * dialogScale;
    float dialogHeight = dialogTex.height * dialogScale;
    float dialogX = ((float)screenWidth - dialogWidth) * 0.5f;
    float dialogY = 46.0f;

    DrawTexturePro(
        dialogTex,
        (Rectangle){0.0f, 0.0f, (float)dialogTex.width, (float)dialogTex.height},
        (Rectangle){dialogX, dialogY, dialogWidth, dialogHeight},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );

    Font dialogFont = GetFontDefault();
    float fontSize = 22.0f;
    float spacing = 1.0f;
    Vector2 textStart = {dialogX + 56.0f, dialogY + 76.0f};
    int revealedChars = (int)firstArenaDialogCharCount;
    int line1Length = TextLength(FIRST_ARENA_DIALOG_KIERAN_LINE_1);
    int line2Length = TextLength(FIRST_ARENA_DIALOG_KIERAN_LINE_2);
    int line1Chars = revealedChars;
    int line2Chars = revealedChars - line1Length;
    char line1Buffer[128] = {0};
    char line2Buffer[128] = {0};
    char playerBuffer[128] = {0};

    if (line1Chars > line1Length)
    {
        line1Chars = line1Length;
    }

    if (line2Chars > line2Length)
    {
        line2Chars = line2Length;
    }

    if (line1Chars > 0)
    {
        CopyVisibleText(line1Buffer, sizeof(line1Buffer), FIRST_ARENA_DIALOG_KIERAN_LINE_1,
                        line1Chars);
        DrawDialogText(dialogFont, line1Buffer, textStart, fontSize, spacing, BLACK);
    }

    if (line2Chars > 0)
    {
        CopyVisibleText(line2Buffer, sizeof(line2Buffer), FIRST_ARENA_DIALOG_KIERAN_LINE_2,
                        line2Chars);
        DrawDialogText(dialogFont, line2Buffer, (Vector2){textStart.x, textStart.y + 30.0f},
                       fontSize, spacing, BLACK);
    }

    if (!firstArenaPlayerReplyVisible)
    {
        if (revealedChars >= line1Length + line2Length)
        {
            DrawDialogText(dialogFont, "Press Enter",
                           (Vector2){dialogX + dialogWidth - 170.0f, dialogY + dialogHeight - 44.0f},
                           18.0f, spacing, DARKGRAY);
        }
        return;
    }

    CopyVisibleText(playerBuffer, sizeof(playerBuffer), FIRST_ARENA_DIALOG_PLAYER_LINE,
                    (int)firstArenaPlayerDialogCharCount);
    DrawDialogText(dialogFont, playerBuffer, (Vector2){textStart.x, textStart.y + 86.0f},
                   fontSize, spacing, BLACK);

    if (firstArenaPlayerDialogCharCount >= (float)TextLength(FIRST_ARENA_DIALOG_PLAYER_LINE))
    {
        DrawDialogText(dialogFont, "Press Enter",
                       (Vector2){dialogX + dialogWidth - 170.0f, dialogY + dialogHeight - 44.0f},
                       18.0f, spacing, DARKGRAY);
    }
}

/* Dialogue box for the opening cutscene */
void DrawOpeningDialogBox(void)
{
    const char *kieranLine1 = (openingMapDialogPage == 0) ?
                              OPENING_DIALOG_KIERAN_LINE_1 :
                              OPENING_DIALOG_2_KIERAN_LINE_1;
    const char *kieranLine2 = (openingMapDialogPage == 0) ?
                              OPENING_DIALOG_KIERAN_LINE_2 :
                              OPENING_DIALOG_2_KIERAN_LINE_2;
    const char *playerLine = (openingMapDialogPage == 0) ?
                             OPENING_DIALOG_PLAYER_LINE :
                             OPENING_DIALOG_2_PLAYER_LINE;

    if (currentMapIndex != 0 || !openingMapDialogActive || textures[TEXTURE_DIALOG_1].id == 0)
    {
        return;
    }

    Texture2D dialogTex = textures[TEXTURE_DIALOG_1];
    float dialogScale = 4.0f;
    float dialogWidth = dialogTex.width * dialogScale;
    float dialogHeight = dialogTex.height * dialogScale;
    float dialogX = ((float)screenWidth - dialogWidth) * 0.5f;
    float dialogY = 46.0f;

    DrawTexturePro(
        dialogTex,
        (Rectangle){0.0f, 0.0f, (float)dialogTex.width, (float)dialogTex.height},
        (Rectangle){dialogX, dialogY, dialogWidth, dialogHeight},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );

    Font dialogFont = GetFontDefault();
    float fontSize = 22.0f;
    float spacing = 1.0f;
    Vector2 textStart = {dialogX + 56.0f, dialogY + 76.0f};
    int revealedChars = (int)openingMapDialogCharCount;
    int line1Length = TextLength(kieranLine1);
    int line2Length = TextLength(kieranLine2);
    int line1Chars = revealedChars;
    int line2Chars = revealedChars - line1Length;
    char line1Buffer[128] = {0};
    char line2Buffer[128] = {0};
    char playerBuffer[128] = {0};

    if (line1Chars > line1Length)
    {
        line1Chars = line1Length;
    }

    if (line2Chars > line2Length)
    {
        line2Chars = line2Length;
    }

    if (line1Chars > 0)
    {
        CopyVisibleText(line1Buffer, sizeof(line1Buffer), kieranLine1, line1Chars);
        DrawDialogText(dialogFont, line1Buffer, textStart, fontSize, spacing, BLACK);
    }

    if (line2Chars > 0)
    {
        CopyVisibleText(line2Buffer, sizeof(line2Buffer), kieranLine2, line2Chars);
        DrawDialogText(dialogFont, line2Buffer, (Vector2){textStart.x, textStart.y + 30.0f},
                       fontSize, spacing, BLACK);
    }

    if (!openingMapPlayerReplyVisible)
    {
        if (revealedChars >= line1Length + line2Length)
        {
            DrawDialogText(dialogFont, "Press Enter",
                           (Vector2){dialogX + dialogWidth - 170.0f, dialogY + dialogHeight - 44.0f},
                           18.0f, spacing, DARKGRAY);
        }
        return;
    }

    CopyVisibleText(playerBuffer, sizeof(playerBuffer), playerLine,
                    (int)openingMapPlayerDialogCharCount);
    DrawDialogText(dialogFont, playerBuffer, (Vector2){textStart.x, textStart.y + 86.0f},
                   fontSize, spacing, BLACK);

    if (openingMapPlayerDialogCharCount >= (float)TextLength(playerLine))
    {
        DrawDialogText(dialogFont, "Press Enter",
                       (Vector2){dialogX + dialogWidth - 170.0f, dialogY + dialogHeight - 44.0f},
                       18.0f, spacing, DARKGRAY);
    }
}

/* Dialogue box for the final fight sequence */
void DrawFinalFightDialogBox(void)
{
    if (!finalFightDialogActive || textures[TEXTURE_DIALOG_1].id == 0)
    {
        return;
    }

    Texture2D dialogTex = textures[TEXTURE_DIALOG_1];
    float dialogScale = 4.0f;
    float dialogWidth = dialogTex.width * dialogScale;
    float dialogHeight = dialogTex.height * dialogScale;
    float dialogX = ((float)screenWidth - dialogWidth) * 0.5f;
    float dialogY = 46.0f;
    Font dialogFont = GetFontDefault();
    float fontSize = 22.0f;
    float spacing = 1.0f;
    Vector2 textStart = {dialogX + 56.0f, dialogY + 76.0f};
    int revealedChars = (int)finalFightDialogCharCount;
    int line1Length = TextLength(FINAL_FIGHT_DIALOG_KIERAN_LINE_1);
    int line2Length = TextLength(FINAL_FIGHT_DIALOG_KIERAN_LINE_2);
    int line1Chars = revealedChars;
    int line2Chars = revealedChars - line1Length;
    char line1Buffer[128] = {0};
    char line2Buffer[128] = {0};
    char playerLine1[128] = {0};
    char playerLine2[128] = {0};
    char furyLine1[128] = {0};
    char furyLine2[128] = {0};
    char safetyLine1[128] = {0};
    char safetyLine2[128] = {0};
    char trapLine1[128] = {0};
    char trapLine2[128] = {0};
    int furyChars = (int)finalFightFuryDialogCharCount;
    int safetyChars = (int)finalFightSafetyDialogCharCount;
    int trapChars = (int)finalFightTrapDialogCharCount;
    float playerTextMaxWidth = dialogWidth - 112.0f;

    if (line1Chars > line1Length)
    {
        line1Chars = line1Length;
    }

    if (line2Chars > line2Length)
    {
        line2Chars = line2Length;
    }

    DrawTexturePro(
        dialogTex,
        (Rectangle){0.0f, 0.0f, (float)dialogTex.width, (float)dialogTex.height},
        (Rectangle){dialogX, dialogY, dialogWidth, dialogHeight},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );

    if (finalFightDialogPhase == FINAL_FIGHT_DIALOG_PHASE_FURY)
    {
        WrapDialogText(
            furyLine1,
            sizeof(furyLine1),
            furyLine2,
            sizeof(furyLine2),
            FINAL_FIGHT_DIALOG_FURY_LINE,
            furyChars,
            dialogFont,
            fontSize,
            spacing,
            playerTextMaxWidth
        );
        DrawDialogText(dialogFont, furyLine1, (Vector2){textStart.x, textStart.y + 48.0f},
                       fontSize, spacing, BLACK);

        if (furyLine2[0] != '\0')
        {
            DrawDialogText(dialogFont, furyLine2, (Vector2){textStart.x, textStart.y + 78.0f},
                           fontSize, spacing, BLACK);
        }

        if (finalFightFuryDialogCharCount >= (float)TextLength(FINAL_FIGHT_DIALOG_FURY_LINE))
        {
            DrawDialogText(dialogFont, "Press Enter",
                           (Vector2){dialogX + dialogWidth - 170.0f, dialogY + dialogHeight - 44.0f},
                           18.0f, spacing, DARKGRAY);
        }
        return;
    }

    if (finalFightDialogPhase == FINAL_FIGHT_DIALOG_PHASE_SAFETY)
    {
        WrapDialogText(
            safetyLine1,
            sizeof(safetyLine1),
            safetyLine2,
            sizeof(safetyLine2),
            FINAL_FIGHT_DIALOG_SAFETY_LINE,
            safetyChars,
            dialogFont,
            fontSize,
            spacing,
            playerTextMaxWidth
        );
        DrawDialogText(dialogFont, safetyLine1, (Vector2){textStart.x, textStart.y + 48.0f},
                       fontSize, spacing, BLACK);

        if (safetyLine2[0] != '\0')
        {
            DrawDialogText(dialogFont, safetyLine2, (Vector2){textStart.x, textStart.y + 78.0f},
                           fontSize, spacing, BLACK);
        }

        if (finalFightSafetyDialogCharCount >= (float)TextLength(FINAL_FIGHT_DIALOG_SAFETY_LINE))
        {
            DrawDialogText(dialogFont, "Press Enter",
                           (Vector2){dialogX + dialogWidth - 170.0f, dialogY + dialogHeight - 44.0f},
                           18.0f, spacing, DARKGRAY);
        }
        return;
    }

    if (finalFightDialogPhase == FINAL_FIGHT_DIALOG_PHASE_TRAP)
    {
        WrapDialogText(
            trapLine1,
            sizeof(trapLine1),
            trapLine2,
            sizeof(trapLine2),
            FINAL_FIGHT_DIALOG_TRAP_LINE,
            trapChars,
            dialogFont,
            fontSize,
            spacing,
            playerTextMaxWidth
        );
        DrawDialogText(dialogFont, trapLine1, (Vector2){textStart.x, textStart.y + 48.0f},
                       fontSize, spacing, BLACK);

        if (trapLine2[0] != '\0')
        {
            DrawDialogText(dialogFont, trapLine2, (Vector2){textStart.x, textStart.y + 78.0f},
                           fontSize, spacing, BLACK);
        }

        if (finalFightTrapDialogCharCount >= (float)TextLength(FINAL_FIGHT_DIALOG_TRAP_LINE))
        {
            DrawDialogText(dialogFont, "Press Enter",
                           (Vector2){dialogX + dialogWidth - 170.0f, dialogY + dialogHeight - 44.0f},
                           18.0f, spacing, DARKGRAY);
        }
        return;
    }

    if (line1Chars > 0)
    {
        CopyVisibleText(line1Buffer, sizeof(line1Buffer), FINAL_FIGHT_DIALOG_KIERAN_LINE_1,
                        line1Chars);
        DrawDialogText(dialogFont, line1Buffer, textStart, fontSize, spacing, BLACK);
    }

    if (line2Chars > 0)
    {
        CopyVisibleText(line2Buffer, sizeof(line2Buffer), FINAL_FIGHT_DIALOG_KIERAN_LINE_2,
                        line2Chars);
        DrawDialogText(dialogFont, line2Buffer, (Vector2){textStart.x, textStart.y + 30.0f},
                       fontSize, spacing, BLACK);
    }

    if (!finalFightPlayerReplyVisible)
    {
        if (revealedChars >= line1Length + line2Length)
        {
            DrawDialogText(dialogFont, "Press Enter",
                           (Vector2){dialogX + dialogWidth - 170.0f, dialogY + dialogHeight - 44.0f},
                           18.0f, spacing, DARKGRAY);
        }
        return;
    }

    WrapDialogText(
        playerLine1,
        sizeof(playerLine1),
        playerLine2,
        sizeof(playerLine2),
        FINAL_FIGHT_DIALOG_PLAYER_LINE,
        (int)finalFightPlayerDialogCharCount,
        dialogFont,
        fontSize,
        spacing,
        playerTextMaxWidth
    );
    DrawDialogText(dialogFont, playerLine1, (Vector2){textStart.x, textStart.y + 68.0f},
                   fontSize, spacing, BLACK);

    if (playerLine2[0] != '\0')
    {
        DrawDialogText(dialogFont, playerLine2, (Vector2){textStart.x, textStart.y + 98.0f},
                       fontSize, spacing, BLACK);
    }

    if (finalFightPlayerDialogCharCount >= (float)TextLength(FINAL_FIGHT_DIALOG_PLAYER_LINE))
    {
        DrawDialogText(dialogFont, "Press Enter",
                       (Vector2){dialogX + dialogWidth - 170.0f, dialogY + dialogHeight - 44.0f},
                       18.0f, spacing, DARKGRAY);
    }
}

void DrawMapTransition(void)
{
    if (transitionState == TRANSITION_NONE)
    {
        return;
    }

    if (transitionState == TRANSITION_HOLD)
    {
        const char *intertitle = "8 hours later...";
        const char *continueText = "Press Enter";
        int fontSize = 96;
        int textWidth = MeasureText(intertitle, fontSize);
        int continueFontSize = 28;
        int continueWidth = MeasureText(continueText, continueFontSize);

        if (transitionShowGameOver)
        {
            intertitle = "GAME OVER";
        }
        else if (transitionShowVictory)
        {
            intertitle = "Do your assignments.";
        }

        textWidth = MeasureText(intertitle, fontSize);

        DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);
        DrawText(
            intertitle,
            (screenWidth - textWidth) / 2,
            (screenHeight - fontSize) / 2,
            fontSize,
            WHITE
        );
        DrawText(
            continueText,
            (screenWidth - continueWidth) / 2,
            (screenHeight - fontSize) / 2 + 120,
            continueFontSize,
            (Color){210, 210, 210, 255}
        );
        return;
    }

    if (transitionRadius <= 0.0f)
    {
        DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);
        return;
    }

    if (transitionRadius >= GetMaxTransitionRadius())
    {
        return;
    }

    int topY = (int)(transitionCenter.y - transitionRadius);
    int bottomY = (int)(transitionCenter.y + transitionRadius);

    if (topY > 0)
    {
        DrawRectangle(0, 0, screenWidth, topY, BLACK);
    }

    if (bottomY < screenHeight)
    {
        DrawRectangle(0, bottomY, screenWidth, screenHeight - bottomY, BLACK);
    }

    for (int y = topY; y < bottomY; y += MAP_TRANSITION_SLICE_HEIGHT)
    {
        int drawY = y;
        int sliceHeight = MAP_TRANSITION_SLICE_HEIGHT;
        float sampleY = (float)y + sliceHeight * 0.5f;
        float dy = sampleY - transitionCenter.y;
        float inside = transitionRadius * transitionRadius - (dy * dy);
        float xSpan = (inside > 0.0f) ? sqrtf(inside) : 0.0f;
        int circleLeft = (int)(transitionCenter.x - xSpan);
        int circleRight = (int)(transitionCenter.x + xSpan);

        if (drawY < 0)
        {
            sliceHeight += drawY;
            drawY = 0;
        }

        if (drawY >= screenHeight || sliceHeight <= 0)
        {
            continue;
        }

        if (drawY + sliceHeight > screenHeight)
        {
            sliceHeight = screenHeight - drawY;
        }

        if (circleLeft > 0)
        {
            DrawRectangle(0, drawY, circleLeft, sliceHeight, BLACK);
        }

        if (circleRight < screenWidth)
        {
            DrawRectangle(circleRight, drawY, screenWidth - circleRight, sliceHeight, BLACK);
        }
    }
}

/* Cleanup */
/* Texture cleanup */
void UnloadGameTextures(void)
{
    UnloadFrameStrip(girlPlayerIdleFrames, PLAYER_IDLE_FRAMES);
    UnloadFrameStrip(girlPlayerWalkFrames, PLAYER_WALK_FRAMES);
    UnloadFrameStrip(girlPlayerAttackFrames, PLAYER_ATTACK_FRAMES);
    UnloadFrameStrip(girlPlayerBookFrames, PLAYER_BOOK_FRAMES);

    for (int i = 0; i < TEXTURE_COUNT; i++)
    {
        if (textures[i].id != 0)
            UnloadTexture(textures[i]);
    }
}
