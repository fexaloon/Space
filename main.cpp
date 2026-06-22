#include "raylib.h"
#include "raymath.h"
#include <string>
#include <vector>
#include <cmath>

// ============================================================================
// GAME STATE ENUMERATIONS
// ============================================================================

enum GameState {
    STATE_OPENING_CREDITS,
    STATE_MENU,
    STATE_INTRO_EGG,
    STATE_GAMEPLAY,
    STATE_COMBAT
};

enum MenuView {
    MENU_VIEW_TITLE = 0,
    MENU_VIEW_SAVE_SLOTS = 1,
    MENU_VIEW_OPTIONS = 2
};

enum GameplayArea {
    AREA_INSIDE_CAVE,
    AREA_OUTSIDE_CAVE
};

enum CombatPhase {
    COMBAT_PHASE_STRIKE = 0,
    COMBAT_PHASE_BLOCK = 1,
    COMBAT_PHASE_SAP = 2,
    COMBAT_PHASE_ITEMS = 3,
    COMBAT_PHASE_VICTORY = 4
};

// ============================================================================
// GLOBAL CONSTANTS & CONFIGURATION
// ============================================================================

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const int TARGET_FPS = 60;
const char* WINDOW_TITLE = "HIM: Hell is Minatory";

const int NUM_PALETTE_COLORS = 20;
const int NUM_JEISO_FRAMES = 3;
const int NUM_BROTHER_FRAMES = 3;
const int NUM_SAVE_SLOTS = 10;
const int MAX_NAME_LENGTH = 32;

// ============================================================================
// GLOBAL PALETTE SYSTEM
// ============================================================================

Color customPalette[NUM_PALETTE_COLORS];

inline Color MakeColor(int r, int g, int b, int a) {
    return { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
}

inline Vector2 MakeVector2(float x, float y) {
    return { x, y };
}

void InitializePalette() {
    customPalette[0]  = { 255,   0,   0, 255 };  // RED
    customPalette[1]  = { 255, 215,   0, 255 };  // GOLD
    customPalette[2]  = {   0,   0, 255, 255 };  // BLUE
    customPalette[3]  = { 128,   0, 128, 255 };  // PURPLE
    customPalette[4]  = { 128,   0,   0, 255 };  // MAROON
    customPalette[5]  = {   0, 255, 255, 255 };  // CYAN
    customPalette[6]  = { 255, 192, 203, 255 };  // PINK
    customPalette[7]  = { 255, 165,   0, 255 };  // ORANGE
    customPalette[8]  = { 144, 238, 144, 255 };  // LIGHT GREEN
    customPalette[9]  = { 230, 230, 250, 255 };  // LAVENDER
    customPalette[10] = { 255, 255,   0, 255 };  // YELLOW
    customPalette[11] = { 165,  42,  42, 255 };  // BROWN
    customPalette[12] = {   0, 128, 128, 255 };  // TEAL
    customPalette[13] = { 255, 105, 180, 255 };  // HOT PINK
    customPalette[14] = {  64, 224, 208, 255 };  // TURQUOISE
    customPalette[15] = { 176,  96, 255, 255 };  // BLUE VIOLET
    customPalette[16] = { 255, 127,   0, 255 };  // WEB ORANGE
    customPalette[17] = {  32, 178, 170, 255 };  // LIGHT SEA GREEN
    customPalette[18] = { 255,  20, 147, 255 };  // DEEP PINK
    customPalette[19] = { 100, 149, 237, 255 };  // CORNFLOWER BLUE
}

// ============================================================================
// TEXTURE MANAGEMENT SYSTEM
// ============================================================================

struct TextureAssets {
    Texture2D jeisoFront[NUM_JEISO_FRAMES];
    Texture2D jeisoBack[NUM_JEISO_FRAMES];
    Texture2D jeisoSide[NUM_JEISO_FRAMES];
    Texture2D brotherFront[NUM_BROTHER_FRAMES];
    Texture2D brotherBack[NUM_BROTHER_FRAMES];
    Texture2D brotherSide[NUM_BROTHER_FRAMES];
    Texture2D momSprite;
    Texture2D eggSprites[7];
    Texture2D eggOnMap;
    Texture2D caveBackground;
    Texture2D caveFloor;
    Texture2D caveExit1;
    Texture2D caveExit2;
    bool isLoaded;
};

TextureAssets textures = {};

void LoadGameTextures() {
    for (int i = 0; i < NUM_JEISO_FRAMES; i++) {
        textures.jeisoFront[i] = LoadTexture("assets/jeiso_front.png");
        textures.jeisoBack[i] = LoadTexture("assets/jeiso_back.png");
        textures.jeisoSide[i] = LoadTexture("assets/jeiso_side.png");
    }

    for (int i = 0; i < NUM_BROTHER_FRAMES; i++) {
        textures.brotherFront[i] = LoadTexture("assets/brother_front.png");
        textures.brotherBack[i] = LoadTexture("assets/brother_back.png");
        textures.brotherSide[i] = LoadTexture("assets/brother_side.png");
    }

    textures.momSprite = LoadTexture("assets/mom.png");
    for (int i = 0; i < 7; i++) {
        std::string eggPath = "assets/egg" + std::to_string(i) + ".png";
        textures.eggSprites[i] = LoadTexture(eggPath.c_str());
    }

    textures.eggOnMap = LoadTexture("assets/eggonmap.png");
    textures.caveBackground = LoadTexture("assets/cavebackground.png");
    textures.caveFloor = LoadTexture("assets/cavefloor.png");
    textures.caveExit1 = LoadTexture("assets/caveexit1.png");
    textures.caveExit2 = LoadTexture("assets/caveexit2.png");

    textures.isLoaded = true;
}

void UnloadGameTextures() {
    if (!textures.isLoaded) return;

    for (int i = 0; i < NUM_JEISO_FRAMES; i++) {
        UnloadTexture(textures.jeisoFront[i]);
        UnloadTexture(textures.jeisoBack[i]);
        UnloadTexture(textures.jeisoSide[i]);
    }

    for (int i = 0; i < NUM_BROTHER_FRAMES; i++) {
        UnloadTexture(textures.brotherFront[i]);
        UnloadTexture(textures.brotherBack[i]);
        UnloadTexture(textures.brotherSide[i]);
    }

    UnloadTexture(textures.momSprite);
    for (int i = 0; i < 7; i++) {
        UnloadTexture(textures.eggSprites[i]);
    }

    UnloadTexture(textures.eggOnMap);
    UnloadTexture(textures.caveBackground);
    UnloadTexture(textures.caveFloor);
    UnloadTexture(textures.caveExit1);
    UnloadTexture(textures.caveExit2);

    textures.isLoaded = false;
}

// ============================================================================
// AUDIO MANAGEMENT SYSTEM
// ============================================================================

struct AudioAssets {
    Sound hatchCrack;
    Sound textType;
    Sound strikeHit;
    Sound swirlTransition;
    Music menuBGM;
    Music caveBGM;
    Music jungleBGM;
    Music battleBGM;
    bool isLoaded;
};

AudioAssets audio = {};

void InitializeAudio() {
    InitAudioDevice();

    audio.hatchCrack = LoadSound("assets/hatch_crack.wav");
    audio.textType = LoadSound("assets/text_type.wav");
    audio.strikeHit = LoadSound("assets/strike_hit.wav");
    audio.swirlTransition = LoadSound("assets/swirl_transition.wav");

    audio.menuBGM = LoadMusicStream("assets/menu_bgm.wav");
    audio.caveBGM = LoadMusicStream("assets/cave_bgm.wav");
    audio.jungleBGM = LoadMusicStream("assets/jungle_bgm.wav");
    audio.battleBGM = LoadMusicStream("assets/battle_bgm.wav");

    audio.isLoaded = true;
}

void UnloadGameAudio() {
    if (!audio.isLoaded) return;

    UnloadSound(audio.hatchCrack);
    UnloadSound(audio.textType);
    UnloadSound(audio.strikeHit);
    UnloadSound(audio.swirlTransition);

    UnloadMusicStream(audio.menuBGM);
    UnloadMusicStream(audio.caveBGM);
    UnloadMusicStream(audio.jungleBGM);
    UnloadMusicStream(audio.battleBGM);

    audio.isLoaded = false;
}

// ============================================================================
// GAME ENTITY SYSTEM
// ============================================================================

struct Player {
    Vector2 position;
    float scale;
    int colorIndex;
    std::string name;
    int currentFrame;
    int level;
    int experience;
    int health;
    int maxHealth;
};

struct NPC {
    Vector2 position;
    float scale;
    std::string name;
    std::string dialogue;
    int currentFrame;
    bool hasBeenSpokenTo;
};

struct InteractableObject {
    Vector2 position;
    float scale;
    std::string interactionText;
    bool isActive;
};

// ============================================================================
// GAME ENGINE CORE CLASS
// ============================================================================

class GameEngine {
private:
    GameState currentState;
    MenuView currentMenuView;
    GameplayArea currentArea;
    CombatPhase currentCombatPhase;

    Player player;
    NPC brother;
    std::vector<InteractableObject> interactables;

    float openingCreditsTimer;
    int eggCrackCounter;
    float screenShakeIntensity;
    Vector2 screenShakeOffset;
    float fadeTimer;
    float typewriterTimer;
    float blinkTimer;

    int menuSelection;
    int optionSelection;
    std::string characterNameInput;
    int nameInputIndex;
    int jeisoColorIndex;
    int boxColorIndex;

    int brotherHealth;
    int brotherMaxHealth;
    bool combatPhaseComplete[5];
    std::string combatDialogue;
    float combatTypewriterProgress;

    const char* ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    int keyboardGridX;
    int keyboardGridY;

public:
    GameEngine()
        : currentState(STATE_OPENING_CREDITS),
          currentMenuView(MENU_VIEW_TITLE),
          currentArea(AREA_INSIDE_CAVE),
          currentCombatPhase(COMBAT_PHASE_STRIKE),
          openingCreditsTimer(0.0f),
          eggCrackCounter(0),
          screenShakeIntensity(0.0f),
          screenShakeOffset({0.0f, 0.0f}),
          fadeTimer(0.0f),
          typewriterTimer(0.0f),
          blinkTimer(0.0f),
          menuSelection(0),
          optionSelection(0),
          nameInputIndex(0),
          jeisoColorIndex(0),
          boxColorIndex(4),
          brotherHealth(50),
          brotherMaxHealth(50),
          combatTypewriterProgress(0.0f),
          keyboardGridX(0),
          keyboardGridY(0) {
        InitializeGameEntities();
        for (int i = 0; i < 5; i++) combatPhaseComplete[i] = false;
    }

    ~GameEngine() {
        UnloadGameAudio();
        UnloadGameTextures();
        CloseAudioDevice();
    }

    void InitializeGameEntities() {
        player.position = { 960.0f, 800.0f };
        player.scale = 250.0f;
        player.colorIndex = jeisoColorIndex;
        player.name = "Player";
        player.currentFrame = 0;
        player.level = 1;
        player.experience = 0;
        player.health = 20;
        player.maxHealth = 20;

        brother.position = { 400.0f, 600.0f };
        brother.scale = 250.0f;
        brother.name = "Brother";
        brother.currentFrame = 0;
        brother.hasBeenSpokenTo = false;
        brother.dialogue = "";

        InteractableObject momObj;
        momObj.position = { 200.0f, 150.0f };
        momObj.scale = 850.0f;
        momObj.interactionText = "She is dead";
        momObj.isActive = true;
        interactables.push_back(momObj);

        InteractableObject eggObj;
        eggObj.position = { 1700.0f, 900.0f };
        eggObj.scale = 200.0f;
        eggObj.interactionText = "This is your egg that you came from";
        eggObj.isActive = true;
        interactables.push_back(eggObj);

        InteractableObject signpostObj;
        signpostObj.position = { 1700.0f, 950.0f };
        signpostObj.scale = 100.0f;
        signpostObj.interactionText = player.name + " is too inexperienced to read this.";
        signpostObj.isActive = true;
        interactables.push_back(signpostObj);
    }

    void Update() {
        switch (currentState) {
            case STATE_OPENING_CREDITS:
                UpdateOpeningCredits();
                break;
            case STATE_MENU:
                UpdateMenu();
                break;
            case STATE_INTRO_EGG:
                UpdateEggHatching();
                break;
            case STATE_GAMEPLAY:
                UpdateGameplay();
                break;
            case STATE_COMBAT:
                UpdateCombat();
                break;
        }
    }

    void Draw() {
        BeginDrawing();
        ClearBackground(BLACK);

        switch (currentState) {
            case STATE_OPENING_CREDITS:
                DrawOpeningCredits();
                break;
            case STATE_MENU:
                DrawMenu();
                break;
            case STATE_INTRO_EGG:
                DrawEggHatching();
                break;
            case STATE_GAMEPLAY:
                DrawGameplay();
                break;
            case STATE_COMBAT:
                DrawCombat();
                break;
        }

        EndDrawing();
    }

    void UpdateOpeningCredits() {
        openingCreditsTimer += GetFrameTime();
        if (openingCreditsTimer >= 3.0f || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            currentState = STATE_MENU;
            openingCreditsTimer = 0.0f;
        }
    }

    void DrawOpeningCredits() {
        ClearBackground(BLACK);
        const char* creditText = "THIS GAME WAS MADE BY AN INDIE GAME PRESENTS";
        int textWidth = MeasureText(creditText, 40);
        DrawText(creditText, WINDOW_WIDTH / 2 - textWidth / 2, WINDOW_HEIGHT / 2 - 20, 40, WHITE);
    }

    void UpdateMenu() {
        if (currentMenuView == MENU_VIEW_TITLE) {
            UpdateMenuTitle();
        } else if (currentMenuView == MENU_VIEW_SAVE_SLOTS) {
            UpdateMenuSaveSlots();
        } else if (currentMenuView == MENU_VIEW_OPTIONS) {
            UpdateMenuOptions();
        }
    }

    void UpdateMenuTitle() {
        if (IsKeyPressed(KEY_DOWN)) {
            menuSelection = (menuSelection + 1) % 4;
        }
        if (IsKeyPressed(KEY_UP)) {
            menuSelection = (menuSelection - 1 + 4) % 4;
        }

        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
            switch (menuSelection) {
                case 0:
                    currentMenuView = MENU_VIEW_SAVE_SLOTS;
                    menuSelection = 0;
                    break;
                case 1:
                    currentMenuView = MENU_VIEW_OPTIONS;
                    menuSelection = 0;
                    break;
                case 2:
                    break;
                case 3:
                    CloseWindow();
                    break;
            }
        }
    }

    void UpdateMenuSaveSlots() {
        if (IsKeyPressed(KEY_DOWN)) {
            menuSelection = (menuSelection + 1) % NUM_SAVE_SLOTS;
        }
        if (IsKeyPressed(KEY_UP)) {
            menuSelection = (menuSelection - 1 + NUM_SAVE_SLOTS) % NUM_SAVE_SLOTS;
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            currentMenuView = MENU_VIEW_TITLE;
            menuSelection = 0;
        }

        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
            characterNameInput = "";
            nameInputIndex = 0;
            keyboardGridX = 0;
            keyboardGridY = 0;
            currentState = STATE_INTRO_EGG;
        }
    }

    void UpdateMenuOptions() {
        if (IsKeyPressed(KEY_DOWN)) {
            optionSelection = (optionSelection + 1) % 3;
        }
        if (IsKeyPressed(KEY_UP)) {
            optionSelection = (optionSelection - 1 + 3) % 3;
        }

        if (optionSelection == 0) {
            if (IsKeyPressed(KEY_RIGHT)) {
                jeisoColorIndex = (jeisoColorIndex + 1) % NUM_PALETTE_COLORS;
                player.colorIndex = jeisoColorIndex;
            }
            if (IsKeyPressed(KEY_LEFT)) {
                jeisoColorIndex = (jeisoColorIndex - 1 + NUM_PALETTE_COLORS) % NUM_PALETTE_COLORS;
                player.colorIndex = jeisoColorIndex;
            }
        } else if (optionSelection == 1) {
            if (IsKeyPressed(KEY_RIGHT)) {
                boxColorIndex = (boxColorIndex + 1) % NUM_PALETTE_COLORS;
            }
            if (IsKeyPressed(KEY_LEFT)) {
                boxColorIndex = (boxColorIndex - 1 + NUM_PALETTE_COLORS) % NUM_PALETTE_COLORS;
            }
        } else if (optionSelection == 2) {
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT)) {
                PlaySound(audio.textType);
            }
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            currentMenuView = MENU_VIEW_TITLE;
            menuSelection = 0;
        }
    }

    void DrawMenu() {
        if (currentMenuView == MENU_VIEW_TITLE) {
            DrawMenuTitle();
        } else if (currentMenuView == MENU_VIEW_SAVE_SLOTS) {
            DrawMenuSaveSlots();
        } else if (currentMenuView == MENU_VIEW_OPTIONS) {
            DrawMenuOptions();
        }
    }

    void DrawMenuTitle() {
        ClearBackground(BLACK);
        DrawText("HIM", WINDOW_WIDTH / 2 - 150, 200, 120, BLUE);
        DrawText("HELL IS MINATORY", WINDOW_WIDTH / 2 - 250, 350, 60, RED);
        DrawText("CREATED BY JIFFY 2026", WINDOW_WIDTH / 2 - 200, 800, 30, WHITE);
        DrawText("TikTok: jiffyjinxx", WINDOW_WIDTH / 2 - 150, 850, 30, WHITE);

        const char* menuItems[] = { "START GAME", "OPTIONS", "CREDITS", "EXIT GAME" };
        for (int i = 0; i < 4; i++) {
            Color textColor = (i == menuSelection) ? YELLOW : WHITE;
            DrawText(menuItems[i], WINDOW_WIDTH / 2 - 100, 450 + i * 80, 40, textColor);
        }
    }

    void DrawMenuSaveSlots() {
        ClearBackground(BLACK);
        DrawText("SELECT SAVE SLOT", WINDOW_WIDTH / 2 - 150, 50, 40, WHITE);
        for (int i = 0; i < NUM_SAVE_SLOTS; i++) {
            std::string slotText = "EMPTY SLOT " + std::to_string(i + 1);
            Color textColor = (i == menuSelection) ? YELLOW : WHITE;
            DrawText(slotText.c_str(), 100, 150 + i * 80, 30, textColor);
        }
    }

    void DrawMenuOptions() {
        ClearBackground(BLACK);
        DrawText("OPTIONS", WINDOW_WIDTH / 2 - 100, 50, 40, WHITE);
        const char* optionsText[] = { "MAIN CHARACTER COLOR", "TEXT BOX COLOR", "SOUNDTEST" };
        for (int i = 0; i < 3; i++) {
            Color textColor = (i == optionSelection) ? YELLOW : WHITE;
            DrawText(optionsText[i], 100, 150 + i * 100, 30, textColor);
            if (i == 0) {
                DrawRectangle(500, 150 + i * 100, 50, 50, customPalette[jeisoColorIndex]);
            } else if (i == 1) {
                DrawRectangle(500, 150 + i * 100, 50, 50, customPalette[boxColorIndex]);
            }
        }
    }

    void UpdateEggHatching() {
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
            eggCrackCounter++;
            screenShakeIntensity = 25.0f;
            PlaySound(audio.hatchCrack);
        }

        if (screenShakeIntensity > 0.0f) {
            screenShakeIntensity -= 50.0f * GetFrameTime();
        }

        screenShakeOffset.x = (float)(rand() % 50 - 25) * (screenShakeIntensity / 25.0f);
        screenShakeOffset.y = (float)(rand() % 50 - 25) * (screenShakeIntensity / 25.0f);

        if (eggCrackCounter >= 4) {
            fadeTimer += GetFrameTime();
            if (fadeTimer >= 1.2f) {
                currentState = STATE_GAMEPLAY;
                player.position = { 960.0f, 800.0f };
                player.scale = 250.0f;
                player.name = characterNameInput.length() > 0 ? characterNameInput : "Jeiso";
                eggCrackCounter = 0;
                fadeTimer = 0.0f;
            }
        }
    }

    void DrawEggHatching() {
        ClearBackground(BLACK);
        if (eggCrackCounter < 7 && textures.isLoaded) {
            Texture2D eggTex = textures.eggSprites[eggCrackCounter];
            if (eggTex.id > 0) {
                Vector2 eggPosition = MakeVector2(
                    (float)(WINDOW_WIDTH / 2 - eggTex.width / 2),
                    (float)(WINDOW_HEIGHT / 2 - eggTex.height / 2));
                eggPosition.x += screenShakeOffset.x;
                eggPosition.y += screenShakeOffset.y;
                DrawTextureEx(eggTex, eggPosition, 0.0f, 1.0f, WHITE);
            }
        }

        if (eggCrackCounter >= 4) {
            float fadeAlpha = fadeTimer / 1.2f;
            DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(0, 0, 0, (int)(fadeAlpha * 255)));
        }
    }

    void UpdateGameplay() {
        if (currentArea == AREA_INSIDE_CAVE) {
            UpdateGameplayCave();
        } else if (currentArea == AREA_OUTSIDE_CAVE) {
            UpdateGameplayOutside();
        }
    }

    void UpdateGameplayCave() {
        if (IsKeyPressed(KEY_UP) && player.position.y > 100) {
            player.position.y -= 50;
            if (player.position.y < 150) {
                fadeTimer = 0.0f;
                currentArea = AREA_OUTSIDE_CAVE;
            }
        }
        if (IsKeyPressed(KEY_DOWN) && player.position.y < WINDOW_HEIGHT - 100) {
            player.position.y += 50;
        }
        if (IsKeyPressed(KEY_LEFT) && player.position.x > 100) {
            player.position.x -= 50;
        }
        if (IsKeyPressed(KEY_RIGHT) && player.position.x < WINDOW_WIDTH - 100) {
            player.position.x += 50;
        }

        if (IsKeyPressed(KEY_Z)) {
            for (auto& obj : interactables) {
                float distance = Vector2Distance(player.position, obj.position);
                if (distance < 200.0f) {
                }
            }
        }
    }

    void UpdateGameplayOutside() {
        if (!brother.hasBeenSpokenTo) {
            if (IsKeyPressed(KEY_Z)) {
                float distanceToBrother = Vector2Distance(player.position, brother.position);
                if (distanceToBrother < 300.0f) {
                    brother.hasBeenSpokenTo = true;
                    currentState = STATE_COMBAT;
                }
            } else {
                if (player.position.x > brother.position.x + 150) {
                    player.position.x = brother.position.x + 150;
                }
            }
        }

        if (IsKeyPressed(KEY_UP) && player.position.y > 100) {
            player.position.y -= 50;
        }
        if (IsKeyPressed(KEY_DOWN) && player.position.y < WINDOW_HEIGHT - 100) {
            player.position.y += 50;
        }
        if (IsKeyPressed(KEY_LEFT) && player.position.x > 100) {
            player.position.x -= 50;
        }
        if (IsKeyPressed(KEY_RIGHT) && player.position.x < WINDOW_WIDTH - 100) {
            player.position.x += 50;
        }
    }

    bool IsValidTexture(const Texture2D& texture) const {
        return texture.id > 0;
    }

    void DrawRetroScene() {
        if (currentArea == AREA_INSIDE_CAVE) {
            DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                MakeColor(20, 18, 35, 255),
                MakeColor(58, 76, 120, 255));

            DrawRectangle(0, 620, WINDOW_WIDTH, 460, MakeColor(32, 28, 33, 255));
            DrawRectangle(0, 560, WINDOW_WIDTH, 80, MakeColor(66, 58, 70, 255));

            for (int i = 0; i < 8; i++) {
                int x = 180 + i * 220;
                DrawTriangle(
                    { (float)x, 650.0f },
                    { (float)x - 80.0f, 860.0f },
                    { (float)x + 80.0f, 860.0f },
                    MakeColor(70, 62, 72, 255));
                DrawCircle(x, 610, 32, MakeColor(96, 90, 112, 255));
            }

            DrawRectangle(WINDOW_WIDTH / 2 - 140, 420, 280, 240, MakeColor(80, 80, 105, 255));
            DrawRectangle(WINDOW_WIDTH / 2 - 120, 360, 240, 80, MakeColor(96, 96, 126, 255));
            DrawCircle(WINDOW_WIDTH / 2, 330, 70, MakeColor(130, 130, 154, 255));
            DrawRectangle(WINDOW_WIDTH / 2 - 40, 520, 80, 100, MakeColor(92, 88, 96, 255));

            DrawRectangle(120, 730, 140, 120, MakeColor(110, 90, 75, 255));
            DrawCircle(190, 720, 38, MakeColor(255, 232, 130, 255));

            DrawRectangle(1580, 730, 180, 140, MakeColor(115, 95, 85, 255));
            DrawCircle(1670, 720, 48, MakeColor(255, 238, 150, 255));

            DrawRectangle(0, 780, WINDOW_WIDTH, 300, MakeColor(45, 45, 55, 255));
        } else {
            DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                MakeColor(120, 200, 255, 255),
                MakeColor(24, 92, 180, 255));

            DrawRectangle(0, 640, WINDOW_WIDTH, 440, MakeColor(54, 152, 64, 255));
            DrawRectangle(0, 600, WINDOW_WIDTH, 60, MakeColor(88, 188, 94, 255));

            for (int i = 0; i < 6; i++) {
                int x = 150 + i * 280;
                DrawTriangle(
                    { (float)x, 650.0f },
                    { (float)x - 120.0f, 900.0f },
                    { (float)x + 120.0f, 900.0f },
                    MakeColor(46, 96, 44, 255));
            }

            DrawRectangle(90, 560, 80, 120, MakeColor(104, 63, 24, 255));
            DrawCircle(130, 525, 55, MakeColor(39, 129, 42, 255));
            DrawRectangle(280, 520, 90, 160, MakeColor(88, 60, 24, 255));
            DrawCircle(325, 485, 60, MakeColor(43, 132, 44, 255));

            DrawRectangle(1600, 560, 80, 140, MakeColor(104, 63, 24, 255));
            DrawCircle(1640, 520, 60, MakeColor(41, 120, 41, 255));

            DrawRectangle(0, 760, WINDOW_WIDTH, 300, MakeColor(30, 100, 40, 255));
        }
    }

    void DrawPlayerSprite(const Vector2& position, Color tint) {
        DrawCircle((int)position.x, (int)position.y + 36, 18, MakeColor(0, 0, 0, 80));
        DrawCircle((int)position.x, (int)position.y - 18, 24, tint);
        DrawRectangle((int)position.x - 18, (int)position.y + 6, 36, 42, tint);
        DrawRectangle((int)position.x - 18, (int)position.y + 48, 12, 44, MakeColor(90, 55, 25, 255));
        DrawRectangle((int)position.x + 6, (int)position.y + 48, 12, 44, MakeColor(90, 55, 25, 255));
        DrawRectangle((int)position.x - 18, (int)position.y - 8, 36, 8, MakeColor(40, 40, 40, 255));
        DrawCircle((int)position.x - 8, (int)position.y - 20, 3, WHITE);
        DrawCircle((int)position.x + 8, (int)position.y - 20, 3, WHITE);
        DrawCircle((int)position.x - 8, (int)position.y - 20, 2, BLACK);
        DrawCircle((int)position.x + 8, (int)position.y - 20, 2, BLACK);
        DrawRectangle((int)position.x - 10, (int)position.y + 12, 20, 6, WHITE);
    }

    void DrawNPCSprite(const Vector2& position, Color tint) {
        DrawCircle((int)position.x, (int)position.y + 36, 18, MakeColor(0, 0, 0, 80));
        DrawCircle((int)position.x, (int)position.y - 18, 24, tint);
        DrawRectangle((int)position.x - 18, (int)position.y + 6, 36, 42, tint);
        DrawRectangle((int)position.x - 18, (int)position.y + 48, 16, 44, MakeColor(56, 38, 21, 255));
        DrawRectangle((int)position.x + 2, (int)position.y + 48, 16, 44, MakeColor(56, 38, 21, 255));
        DrawRectangle((int)position.x - 18, (int)position.y - 8, 36, 8, MakeColor(30, 30, 30, 255));
        DrawCircle((int)position.x - 8, (int)position.y - 20, 3, WHITE);
        DrawCircle((int)position.x + 8, (int)position.y - 20, 3, WHITE);
        DrawCircle((int)position.x - 8, (int)position.y - 20, 2, BLACK);
        DrawCircle((int)position.x + 8, (int)position.y - 20, 2, BLACK);
    }

    void DrawGameplay() {
        DrawRetroScene();

        if (currentArea == AREA_INSIDE_CAVE) {
            DrawCircle((int)interactables[0].position.x, (int)interactables[0].position.y, 44, MakeColor(185, 72, 72, 150));
            DrawCircle((int)interactables[0].position.x, (int)interactables[0].position.y, 24, MakeColor(255, 216, 96, 180));

            DrawCircle((int)interactables[1].position.x, (int)interactables[1].position.y, 44, MakeColor(92, 196, 115, 140));
            DrawCircle((int)interactables[1].position.x, (int)interactables[1].position.y, 24, MakeColor(255, 255, 255, 180));

            DrawRectangle((int)interactables[2].position.x - 24, (int)interactables[2].position.y - 28, 48, 56, MakeColor(179, 140, 92, 255));
            DrawRectangle((int)interactables[2].position.x - 14, (int)interactables[2].position.y - 38, 28, 16, MakeColor(255, 211, 128, 255));
        }

        if (IsValidTexture(textures.jeisoFront[0])) {
            DrawTextureEx(textures.jeisoFront[0],
                MakeVector2(player.position.x - 50, player.position.y - 50),
                0.0f,
                0.3f,
                customPalette[player.colorIndex]);
        } else {
            DrawPlayerSprite(player.position, customPalette[player.colorIndex]);
        }

        if (currentArea == AREA_OUTSIDE_CAVE) {
            if (IsValidTexture(textures.brotherFront[0])) {
                DrawTextureEx(textures.brotherFront[0],
                    MakeVector2(brother.position.x - 50, brother.position.y - 50),
                    0.0f,
                    0.3f,
                    WHITE);
            } else {
                DrawNPCSprite(brother.position, MakeColor(180, 90, 255, 255));
            }
        }

        blinkTimer += GetFrameTime();
        if (std::fmod(blinkTimer, 0.6f) < 0.3f) {
            DrawText("PRESS [Z] TO TALK", 50, WINDOW_HEIGHT - 80, 26, MakeColor(255, 230, 120, 255));
        }
    }

    void UpdateCombat() {
        if (IsKeyPressed(KEY_DOWN)) {
        }

        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
            HandleCombatAction();
        }
    }

    void HandleCombatAction() {
        switch (currentCombatPhase) {
            case COMBAT_PHASE_STRIKE:
                brotherHealth -= 1;
                combatPhaseComplete[0] = true;
                currentCombatPhase = COMBAT_PHASE_BLOCK;
                PlaySound(audio.strikeHit);
                break;
            case COMBAT_PHASE_BLOCK:
                combatPhaseComplete[1] = true;
                currentCombatPhase = COMBAT_PHASE_SAP;
                break;
            case COMBAT_PHASE_SAP:
                brotherHealth -= 4;
                combatPhaseComplete[2] = true;
                currentCombatPhase = COMBAT_PHASE_ITEMS;
                break;
            case COMBAT_PHASE_ITEMS:
                brotherHealth -= 10;
                combatPhaseComplete[3] = true;
                currentCombatPhase = COMBAT_PHASE_VICTORY;
                break;
            case COMBAT_PHASE_VICTORY:
                player.level = 1;
                player.experience += 10;
                currentState = STATE_GAMEPLAY;
                currentArea = AREA_OUTSIDE_CAVE;
                brother.hasBeenSpokenTo = true;
                break;
        }
    }

    void DrawCombat() {
        ClearBackground(MakeColor(40, 20, 60, 255));

        if (textures.isLoaded) {
            DrawTextureEx(textures.brotherFront[0], MakeVector2(300, 300), 0.0f, 0.5f, WHITE);
            DrawTextureEx(textures.jeisoFront[0], MakeVector2(1300, 300), 0.0f, 0.5f, customPalette[player.colorIndex]);
        }

        DrawRectangle(200, 800, 1520, 200, customPalette[boxColorIndex]);
        DrawRectangleLines(200, 800, 1520, 200, WHITE);

        const char* actions[] = { "STRIKE", "BLOCK", "SAP", "ITEMS", "ESCAPE" };
        for (int i = 0; i < 5; i++) {
            Color actionColor = WHITE;
            if (currentCombatPhase != i) {
                actionColor = MakeColor(100, 100, 100, 255);
            }
            DrawText(actions[i], 250 + i * 280, 820, 30, actionColor);
        }

        DrawText("Brother says: Press Z to activate fight commands", 100, 100, 25, WHITE);
    }
};

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    ToggleFullscreen();
    SetTargetFPS(TARGET_FPS);

    InitializePalette();

    GameEngine game;
    LoadGameTextures();
    InitializeAudio();

    while (!WindowShouldClose()) {
        game.Update();
        game.Draw();
    }

    CloseWindow();
    return 0;
}
