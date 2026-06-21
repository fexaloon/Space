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
    Texture2D eggSprites[7];  // egg.png through egg6.png
    Texture2D eggOnMap;
    Texture2D caveBackground;
    Texture2D caveFloor;
    Texture2D caveExit1;
    Texture2D caveExit2;
    
    bool isLoaded;
};

TextureAssets textures = {};

void LoadGameTextures() {
    // Load Jeiso sprites (placeholder - use actual asset names)
    for (int i = 0; i < NUM_JEISO_FRAMES; i++) {
        textures.jeisoFront[i] = LoadTexture("assets/jeiso_front.png");
        textures.jeisoBack[i] = LoadTexture("assets/jeiso_back.png");
        textures.jeisoSide[i] = LoadTexture("assets/jeiso_side.png");
    }
    
    // Load Brother sprites
    for (int i = 0; i < NUM_BROTHER_FRAMES; i++) {
        textures.brotherFront[i] = LoadTexture("assets/brother_front.png");
        textures.brotherBack[i] = LoadTexture("assets/brother_back.png");
        textures.brotherSide[i] = LoadTexture("assets/brother_side.png");
    }
    
    // Load environment assets
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
    
    // Timing & State Tracking
    float openingCreditsTimer;
    float eggCrackCounter;
    float screenShakeIntensity;
    Vector2 screenShakeOffset;
    float fadeTimer;
    float typewriterTimer;
    float blinkTimer;
    
    // Menu & Input
    int menuSelection;
    int optionSelection;
    std::string characterNameInput;
    int nameInputIndex;
    int jeisoColorIndex;
    int boxColorIndex;
    
    // Combat Tracking
    int brotherHealth;
    int brotherMaxHealth;
    bool combatPhaseComplete[5];
    std::string combatDialogue;
    float combatTypewriterProgress;
    
    // Color Grid Keyboard
    const char* ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    int keyboardGridX;
    int keyboardGridY;
    
public:
    GameEngine() : 
        currentState(STATE_OPENING_CREDITS),
        currentMenuView(MENU_VIEW_TITLE),
        currentArea(AREA_INSIDE_CAVE),
        currentCombatPhase(COMBAT_PHASE_STRIKE),
        openingCreditsTimer(0.0f),
        eggCrackCounter(0),
        screenShakeIntensity(0.0f),
        screenShakeOffset({0, 0}),
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
        keyboardGridY(0)
    {
        InitializeGameEntities();
        for (int i = 0; i < 5; i++) combatPhaseComplete[i] = false;
    }
    
    ~GameEngine() {
        UnloadGameAudio();
        UnloadGameTextures();
        CloseAudioDevice();
    }
    
    void InitializeGameEntities() {
        // Initialize Player
        player.position = { 960.0f, 800.0f };
        player.scale = 250.0f;
        player.colorIndex = jeisoColorIndex;
        player.name = "Player";
        player.currentFrame = 0;
        player.level = 1;
        player.experience = 0;
        player.health = 20;
        player.maxHealth = 20;
        
        // Initialize Brother NPC
        brother.position = { 400.0f, 600.0f };
        brother.scale = 250.0f;
        brother.name = "Brother";
        brother.currentFrame = 0;
        brother.hasBeenSpokenTo = false;
        brother.dialogue = "";
        
        // Initialize Interactables
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
    
    // ========================================================================
    // STATE MACHINE & MAIN UPDATE LOOP
    // ========================================================================
    
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
    
    // ========================================================================
    // STATE: OPENING CREDITS
    // ========================================================================
    
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
        
        DrawText(creditText, 
                WINDOW_WIDTH / 2 - textWidth / 2, 
                WINDOW_HEIGHT / 2 - 20, 
                40, 
                WHITE);
    }
    
    // ========================================================================
    // STATE: MENU
    // ========================================================================
    
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
                case 0:  // START GAME
                    currentMenuView = MENU_VIEW_SAVE_SLOTS;
                    menuSelection = 0;
                    break;
                case 1:  // OPTIONS
                    currentMenuView = MENU_VIEW_OPTIONS;
                    menuSelection = 0;
                    break;
                case 2:  // CREDITS
                    // TODO: Implement credits view
                    break;
                case 3:  // EXIT GAME
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
            // Start name input
            characterNameInput = "";
            nameInputIndex = 0;
            keyboardGridX = 0;
            keyboardGridY = 0;
            UpdateNameInput();
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
        
        if (optionSelection == 0) {  // Main Character Color
            if (IsKeyPressed(KEY_RIGHT)) {
                jeisoColorIndex = (jeisoColorIndex + 1) % NUM_PALETTE_COLORS;
                player.colorIndex = jeisoColorIndex;
            }
            if (IsKeyPressed(KEY_LEFT)) {
                jeisoColorIndex = (jeisoColorIndex - 1 + NUM_PALETTE_COLORS) % NUM_PALETTE_COLORS;
                player.colorIndex = jeisoColorIndex;
            }
        } else if (optionSelection == 1) {  // Text Box Color
            if (IsKeyPressed(KEY_RIGHT)) {
                boxColorIndex = (boxColorIndex + 1) % NUM_PALETTE_COLORS;
            }
            if (IsKeyPressed(KEY_LEFT)) {
                boxColorIndex = (boxColorIndex - 1 + NUM_PALETTE_COLORS) % NUM_PALETTE_COLORS;
            }
        } else if (optionSelection == 2) {  // Sound Test
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
        
        // Title "HIM"
        DrawText("HIM", 
                WINDOW_WIDTH / 2 - 150, 
                200, 
                120, 
                BLUE);
        
        // Subtitle "HELL IS MINATORY"
        DrawText("HELL IS MINATORY", 
                WINDOW_WIDTH / 2 - 250, 
                350, 
                60, 
                RED);
        
        // Credits
        DrawText("CREATED BY JIFFY 2026", 
                WINDOW_WIDTH / 2 - 200, 
                800, 
                30, 
                WHITE);
        DrawText("TikTok: jiffyjinxx", 
                WINDOW_WIDTH / 2 - 150, 
                850, 
                30, 
                WHITE);
        
        // Menu Options
        const char* menuItems[] = { "START GAME", "OPTIONS", "CREDITS", "EXIT GAME" };
        for (int i = 0; i < 4; i++) {
            Color textColor = (i == menuSelection) ? YELLOW : WHITE;
            DrawText(menuItems[i], 
                    WINDOW_WIDTH / 2 - 100, 
                    450 + i * 80, 
                    40, 
                    textColor);
        }
    }
    
    void DrawMenuSaveSlots() {
        ClearBackground(BLACK);
        DrawText("SELECT SAVE SLOT", WINDOW_WIDTH / 2 - 150, 50, 40, WHITE);
        
        for (int i = 0; i < NUM_SAVE_SLOTS; i++) {
            std::string slotText = "EMPTY SLOT " + std::to_string(i + 1);
            Color textColor = (i == menuSelection) ? YELLOW : WHITE;
            DrawText(slotText.c_str(), 
                    100, 
                    150 + i * 80, 
                    30, 
                    textColor);
        }
    }
    
    void DrawMenuOptions() {
        ClearBackground(BLACK);
        DrawText("OPTIONS", WINDOW_WIDTH / 2 - 100, 50, 40, WHITE);
        
        const char* optionsText[] = { 
            "MAIN CHARACTER COLOR", 
            "TEXT BOX COLOR", 
            "SOUNDTEST" 
        };
        
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
    
    // ========================================================================
    // NAME INPUT: GRAPHICAL MATRIX KEYBOARD
    // ========================================================================
    
    void UpdateNameInput() {
        // Arrow key navigation
        if (IsKeyPressed(KEY_DOWN)) {
            keyboardGridY = (keyboardGridY + 1) % 3;
        }
        if (IsKeyPressed(KEY_UP)) {
            keyboardGridY = (keyboardGridY - 1 + 3) % 3;
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            keyboardGridX = (keyboardGridX + 1) % 9;
        }
        if (IsKeyPressed(KEY_LEFT)) {
            keyboardGridX = (keyboardGridX - 1 + 9) % 9;
        }
        
        // Confirm selection
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
            int letterIndex = keyboardGridY * 9 + keyboardGridX;
            if (letterIndex < 27 && characterNameInput.length() < MAX_NAME_LENGTH) {
                characterNameInput += ALPHABET[letterIndex];
                PlaySound(audio.textType);
            }
        }
        
        // Backspace
        if (IsKeyPressed(KEY_BACKSPACE) && characterNameInput.length() > 0) {
            characterNameInput.pop_back();
        }
    }
    
    // ========================================================================
    // STATE: EGG HATCHING
    // ========================================================================
    
    void UpdateEggHatching() {
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
            eggCrackCounter++;
            screenShakeIntensity = 25.0f;
            PlaySound(audio.hatchCrack);
        }
        
        // Decay screen shake
        if (screenShakeIntensity > 0.0f) {
            screenShakeIntensity -= 50.0f * GetFrameTime();
        }
        
        // Generate screen shake offset
        screenShakeOffset.x = (float)(rand() % 50 - 25) * (screenShakeIntensity / 25.0f);
        screenShakeOffset.y = (float)(rand() % 50 - 25) * (screenShakeIntensity / 25.0f);
        
        // Transition to gameplay when egg is fully cracked (4-5 hits)
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
        
        // Draw egg sprite based on crack counter
        if (eggCrackCounter < 7 && textures.isLoaded) {
            Texture2D eggTex = textures.eggSprites[eggCrackCounter];
            DrawTextureEx(eggTex, 
                         Vector2(WINDOW_WIDTH / 2 - eggTex.width / 2, 
                                WINDOW_HEIGHT / 2 - eggTex.height / 2) + screenShakeOffset, 
                         0.0f, 
                         1.0f, 
                         WHITE);
        }
        
        // Draw fading black overlay after reaching final egg state
        if (eggCrackCounter >= 4) {
            float fadeAlpha = fadeTimer / 1.2f;
            DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                         Color(0, 0, 0, (int)(fadeAlpha * 255)));
        }
    }
    
    // ========================================================================
    // STATE: GAMEPLAY
    // ========================================================================
    
    void UpdateGameplay() {
        if (currentArea == AREA_INSIDE_CAVE) {
            UpdateGameplayCave();
        } else if (currentArea == AREA_OUTSIDE_CAVE) {
            UpdateGameplayOutside();
        }
    }
    
    void UpdateGameplayCave() {
        // Handle player movement (simplified)
        if (IsKeyPressed(KEY_UP) && player.position.y > 100) {
            player.position.y -= 50;
            // Check for exit
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
        
        // Interaction
        if (IsKeyPressed(KEY_Z)) {
            // Check distance to interactables and trigger interaction
            for (auto& obj : interactables) {
                float distance = Vector2Distance(player.position, obj.position);
                if (distance < 200.0f) {
                    // TODO: Show interaction dialogue box
                }
            }
        }
    }
    
    void UpdateGameplayOutside() {
        // Brother dialogue sequence
        if (!brother.hasBeenSpokenTo) {
            if (IsKeyPressed(KEY_Z)) {
                float distanceToBrother = Vector2Distance(player.position, brother.position);
                if (distanceToBrother < 300.0f) {
                    brother.hasBeenSpokenTo = true;
                    // Trigger name override sequence
                    currentState = STATE_COMBAT;
                }
            } else {
                // Block movement past brother
                if (player.position.x > brother.position.x + 150) {
                    player.position.x = brother.position.x + 150;
                }
            }
        }
        
        // Standard movement
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
    
    void DrawGameplay() {
        // Draw background
        if (currentArea == AREA_INSIDE_CAVE) {
            DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Color(50, 50, 50, 255));
            DrawText("CAVE", 100, 50, 40, WHITE);
        } else {
            DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Color(34, 100, 34, 255));
            DrawText("JUNGLE", 100, 50, 40, WHITE);
        }
        
        // Draw player
        if (textures.isLoaded) {
            DrawTextureEx(textures.jeisoFront[0], 
                         Vector2(player.position.x - 50, player.position.y - 50), 
                         0.0f, 
                         0.3f, 
                         customPalette[player.colorIndex]);
        }
        
        // Draw NPCs and interactables
        if (currentArea == AREA_OUTSIDE_CAVE) {
            DrawTextureEx(textures.brotherFront[0], 
                         Vector2(brother.position.x - 50, brother.position.y - 50), 
                         0.0f, 
                         0.3f, 
                         WHITE);
        }
        
        if (currentArea == AREA_INSIDE_CAVE) {
            // Draw interactables
            DrawCircle((int)interactables[0].position.x, (int)interactables[0].position.y, 
                      50, Color(128, 0, 0, 100));  // Mom marker
        }
        
        // Helper text
        blinkTimer += GetFrameTime();
        if (std::fmod(blinkTimer, 0.5f) < 0.25f) {
            DrawText("PRESS [Z] TO TALK TO PEOPLE AND INTERACT WITH THINGS!!!", 
                    50, WINDOW_HEIGHT - 100, 25, YELLOW);
        }
    }
    
    // ========================================================================
    // STATE: COMBAT
    // ========================================================================
    
    void UpdateCombat() {
        if (IsKeyPressed(KEY_DOWN)) {
            // Navigate menu (implementation depends on phase)
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
        ClearBackground(Color(40, 20, 60, 255));  // Purple-tinted background
        
        // Draw battle participants
        if (textures.isLoaded) {
            // Draw Brother
            DrawTextureEx(textures.brotherFront[0], 
                         Vector2(300, 300), 
                         0.0f, 
                         0.5f, 
                         WHITE);
            
            // Draw Jeiso
            DrawTextureEx(textures.jeisoFront[0], 
                         Vector2(1300, 300), 
                         0.0f, 
                         0.5f, 
                         customPalette[player.colorIndex]);
        }
        
        // Draw combat UI
        DrawRectangle(200, 800, 1520, 200, customPalette[boxColorIndex]);
        DrawRectangleLines(200, 800, 1520, 200, WHITE);
        
        // Combat actions
        const char* actions[] = { "STRIKE", "BLOCK", "SAP", "ITEMS", "ESCAPE" };
        for (int i = 0; i < 5; i++) {
            Color actionColor = WHITE;
            if (currentCombatPhase != i) {
                actionColor = Color(100, 100, 100, 255);  // Grayed out
            }
            DrawText(actions[i], 250 + i * 280, 820, 30, actionColor);
        }
        
        // Dialogue
        DrawText("Brother says: Press Z to activate fight commands", 100, 100, 25, WHITE);
    }
};

// ============================================================================
// MAIN PROGRAM ENTRY POINT
// ============================================================================

int main() {
    // Initialize window
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    ToggleFullscreen();
    SetTargetFPS(TARGET_FPS);
    
    // Initialize palette
    InitializePalette();
    
    // Create game engine
    GameEngine game;
    
    // Load all assets
    LoadGameTextures();
    game.InitializeAudio();
    
    // Main game loop
    while (!WindowShouldClose()) {
        game.Update();
        game.Draw();
    }
    
    // Cleanup and shutdown
    CloseWindow();
    
    return 0;
}
