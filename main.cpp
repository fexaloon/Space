#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int TARGET_FPS = 60;
const char* WINDOW_TITLE = "HIM: Hell is Minatory";

const int NUM_PALETTE_COLORS = 20;
const int NUM_JEISO_FRAMES = 3;
const int NUM_BROTHER_FRAMES = 3;
const int NUM_SAVE_SLOTS = 10;
const int MAX_NAME_LENGTH = 24;

inline Color MakeColor(int r, int g, int b, int a) {
    return { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
}

inline Vector2 MakeVector2(float x, float y) {
    return { x, y };
}

inline float Clampf(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

inline int Clampi(int v, int lo, int hi) {
    return std::max(lo, std::min(hi, v));
}

struct TextureAssets {
    Texture2D jeisoFront[NUM_JEISO_FRAMES];
    Texture2D jeisoBack[NUM_JEISO_FRAMES];
    Texture2D jeisoSide[NUM_JEISO_FRAMES];
    Texture2D brotherFront[NUM_BROTHER_FRAMES];
    Texture2D brotherBack[NUM_BROTHER_FRAMES];
    Texture2D brotherSide[NUM_BROTHER_FRAMES];
    Texture2D momSprite;
    Texture2D eggSprites[7];
    Texture2D caveBackground;
    Texture2D caveFloor;
    Texture2D caveExit1;
    Texture2D caveExit2;
    bool isLoaded;
};

TextureAssets textures = {};

struct AudioAssets {
    Sound hatchCrack;
    Sound textType;
    Sound strikeHit;
    Sound swirlTransition;
    Music menuBGM;
    Music caveBGM;
    Music battleBGM;
    bool isLoaded;
};

AudioAssets audio = {};

Color customPalette[NUM_PALETTE_COLORS];

struct SaveData {
    std::string playerName;
    int colorIndex;
    int boxColorIndex;
    bool completedIntro;
    bool hasSeenBrother;
};

struct InteractableObject {
    Vector2 position;
    std::string interactionText;
    bool active;
};

struct Player {
    Vector2 position;
    float scale;
    int colorIndex;
    std::string name;
    int level;
    int experience;
    int health;
    int maxHealth;
    int frameIndex;
    bool facingRight;
};

struct NPC {
    Vector2 position;
    std::string name;
    std::string dialogue;
    bool hasBeenSpokenTo;
    bool isHostile;
    int frameIndex;
    bool facingRight;
};

struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float life;
    float maxLife;
    float size;
};

struct DialogueBox {
    bool active;
    std::string text;
    int lineCount;
    float timer;
    float duration;
};

struct CombatAction {
    std::string name;
    int damage;
    bool available;
};

enum GameState {
    STATE_OPENING_CREDITS,
    STATE_MENU,
    STATE_INTRO_EGG,
    STATE_GAMEPLAY,
    STATE_COMBAT,
    STATE_GAME_OVER
};

enum MenuView {
    MENU_VIEW_TITLE = 0,
    MENU_VIEW_SAVE_SLOTS = 1,
    MENU_VIEW_OPTIONS = 2,
    MENU_VIEW_CREDITS = 3
};

enum GameplayArea {
    AREA_INSIDE_CAVE = 0,
    AREA_OUTSIDE_CAVE = 1,
    AREA_FINAL_CHAMBER = 2
};

enum CombatPhase {
    COMBAT_PHASE_STRIKE = 0,
    COMBAT_PHASE_BLOCK = 1,
    COMBAT_PHASE_SAP = 2,
    COMBAT_PHASE_ITEMS = 3,
    COMBAT_PHASE_VICTORY = 4
};

class GameEngine {
private:
    GameState currentState;
    MenuView currentMenuView;
    GameplayArea currentArea;
    CombatPhase currentCombatPhase;

    Player player;
    NPC brother;
    std::vector<InteractableObject> interactables;
    std::vector<Particle> particles;
    DialogueBox dialogueBox;
    SaveData saveData;

    float openingCreditsTimer;
    float eggCrackCounter;
    float screenShakeIntensity;
    Vector2 screenShakeOffset;
    float fadeTimer;
    float blinkTimer;
    float titleFlashTimer;
    float combatTextTimer;
    float cameraShakeTimer;
    float transitionTimer;

    int menuSelection;
    int optionSelection;
    int saveSlotSelection;
    int keyboardGridX;
    int keyboardGridY;
    std::string characterNameInput;
    int jeisoColorIndex;
    int boxColorIndex;
    int brotherHealth;
    int brotherMaxHealth;
    int combatTurn;
    bool combatPhaseComplete[5];
    bool nameInputActive;
    bool showTitleFlash;
    bool showPrompt;
    bool shouldFadeOut;
    bool hasSeenIntroCutscene;
    bool tutorialShown;

public:
    GameEngine()
        : currentState(STATE_OPENING_CREDITS),
          currentMenuView(MENU_VIEW_TITLE),
          currentArea(AREA_INSIDE_CAVE),
          currentCombatPhase(COMBAT_PHASE_STRIKE),
          player({ MakeVector2(640.0f, 540.0f), 250.0f, 0, "Jeiso", 1, 0, 20, 20, 0, true }),
          brother({ MakeVector2(960.0f, 520.0f), "Brother", "", false, false, 0, false }),
          openingCreditsTimer(0.0f),
          eggCrackCounter(0.0f),
          screenShakeIntensity(0.0f),
          screenShakeOffset({0.0f, 0.0f}),
          fadeTimer(0.0f),
          blinkTimer(0.0f),
          titleFlashTimer(0.0f),
          combatTextTimer(0.0f),
          cameraShakeTimer(0.0f),
          transitionTimer(0.0f),
          menuSelection(0),
          optionSelection(0),
          saveSlotSelection(0),
          keyboardGridX(0),
          keyboardGridY(0),
          characterNameInput(""),
          jeisoColorIndex(0),
          boxColorIndex(4),
          brotherHealth(50),
          brotherMaxHealth(50),
          combatTurn(0),
          nameInputActive(false),
          showTitleFlash(true),
          showPrompt(true),
          shouldFadeOut(false),
          hasSeenIntroCutscene(false),
          tutorialShown(false) {
        InitializePalette();
        InitializeGameEntities();
        for (int i = 0; i < 5; ++i) combatPhaseComplete[i] = false;
        saveData.playerName = "Jeiso";
        saveData.colorIndex = 0;
        saveData.boxColorIndex = 4;
        saveData.completedIntro = false;
        saveData.hasSeenBrother = false;
    }

    void Update() {
        switch (currentState) {
            case STATE_OPENING_CREDITS: UpdateOpeningCredits(); break;
            case STATE_MENU: UpdateMenu(); break;
            case STATE_INTRO_EGG: UpdateEggHatching(); break;
            case STATE_GAMEPLAY: UpdateGameplay(); break;
            case STATE_COMBAT: UpdateCombat(); break;
            case STATE_GAME_OVER: UpdateGameOver(); break;
        }

        titleFlashTimer += GetFrameTime();
        if (titleFlashTimer > 0.25f) {
            titleFlashTimer = 0.0f;
            showTitleFlash = !showTitleFlash;
        }

        if (dialogueBox.active) {
            dialogueBox.timer += GetFrameTime();
            if (dialogueBox.timer >= dialogueBox.duration) {
                dialogueBox.active = false;
                dialogueBox.timer = 0.0f;
            }
        }

        if (cameraShakeTimer > 0.0f) {
            cameraShakeTimer -= GetFrameTime();
            if (cameraShakeTimer <= 0.0f) {
                screenShakeOffset = {0.0f, 0.0f};
            }
        }

        if (shouldFadeOut) {
            transitionTimer += GetFrameTime();
            if (transitionTimer >= 1.0f) {
                shouldFadeOut = false;
                transitionTimer = 0.0f;
            }
        }
    }

    void Draw() {
        BeginDrawing();
        ClearBackground(MakeColor(8, 12, 24, 255));

        switch (currentState) {
            case STATE_OPENING_CREDITS: DrawOpeningCredits(); break;
            case STATE_MENU: DrawMenu(); break;
            case STATE_INTRO_EGG: DrawEggHatching(); break;
            case STATE_GAMEPLAY: DrawGameplay(); break;
            case STATE_COMBAT: DrawCombat(); break;
            case STATE_GAME_OVER: DrawGameOver(); break;
        }

        if (shouldFadeOut) {
            int alpha = (int)(Clampf(transitionTimer / 1.0f, 0.0f, 1.0f) * 255.0f);
            DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(0, 0, 0, alpha));
        }

        EndDrawing();
    }

    void InitializeAudio() {
        InitAudioDevice();
        audio.hatchCrack = SafeLoadSound("assets/hatch_crack.wav");
        audio.textType = SafeLoadSound("assets/text_type.wav");
        audio.strikeHit = SafeLoadSound("assets/strike_hit.wav");
        audio.swirlTransition = SafeLoadSound("assets/swirl_transition.wav");
        audio.menuBGM = LoadMusicStream("assets/menu_bgm.wav");
        audio.caveBGM = LoadMusicStream("assets/cave_bgm.wav");
        audio.battleBGM = LoadMusicStream("assets/battle_bgm.wav");
        audio.isLoaded = true;
    }

    void UnloadAudio() {
        if (audio.hatchCrack.frameCount > 0) UnloadSound(audio.hatchCrack);
        if (audio.textType.frameCount > 0) UnloadSound(audio.textType);
        if (audio.strikeHit.frameCount > 0) UnloadSound(audio.strikeHit);
        if (audio.swirlTransition.frameCount > 0) UnloadSound(audio.swirlTransition);
        if (audio.menuBGM.data != nullptr) UnloadMusicStream(audio.menuBGM);
        if (audio.caveBGM.data != nullptr) UnloadMusicStream(audio.caveBGM);
        if (audio.battleBGM.data != nullptr) UnloadMusicStream(audio.battleBGM);
    }

    void LoadTextures() {
        for (int i = 0; i < NUM_JEISO_FRAMES; ++i) {
            textures.jeisoFront[i] = SafeLoadTexture("assets/jeiso_front.png");
            textures.jeisoBack[i] = SafeLoadTexture("assets/jeiso_back.png");
            textures.jeisoSide[i] = SafeLoadTexture("assets/jeiso_side.png");
        }
        for (int i = 0; i < NUM_BROTHER_FRAMES; ++i) {
            textures.brotherFront[i] = SafeLoadTexture("assets/brother_front.png");
            textures.brotherBack[i] = SafeLoadTexture("assets/brother_back.png");
            textures.brotherSide[i] = SafeLoadTexture("assets/brother_side.png");
        }
        textures.momSprite = SafeLoadTexture("assets/mom.png");
        for (int i = 0; i < 7; ++i) {
            std::string path = "assets/egg" + std::to_string(i) + ".png";
            textures.eggSprites[i] = SafeLoadTexture(path);
        }
        textures.caveBackground = SafeLoadTexture("assets/cavebackground.png");
        textures.caveFloor = SafeLoadTexture("assets/cavefloor.png");
        textures.caveExit1 = SafeLoadTexture("assets/caveexit1.png");
        textures.caveExit2 = SafeLoadTexture("assets/caveexit2.png");
        textures.isLoaded = true;
    }

    void UnloadTextures() {
        if (!textures.isLoaded) return;
        for (int i = 0; i < NUM_JEISO_FRAMES; ++i) {
            if (textures.jeisoFront[i].id > 0) UnloadTexture(textures.jeisoFront[i]);
            if (textures.jeisoBack[i].id > 0) UnloadTexture(textures.jeisoBack[i]);
            if (textures.jeisoSide[i].id > 0) UnloadTexture(textures.jeisoSide[i]);
        }
        for (int i = 0; i < NUM_BROTHER_FRAMES; ++i) {
            if (textures.brotherFront[i].id > 0) UnloadTexture(textures.brotherFront[i]);
            if (textures.brotherBack[i].id > 0) UnloadTexture(textures.brotherBack[i]);
            if (textures.brotherSide[i].id > 0) UnloadTexture(textures.brotherSide[i]);
        }
        if (textures.momSprite.id > 0) UnloadTexture(textures.momSprite);
        for (int i = 0; i < 7; ++i) {
            if (textures.eggSprites[i].id > 0) UnloadTexture(textures.eggSprites[i]);
        }
        if (textures.caveBackground.id > 0) UnloadTexture(textures.caveBackground);
        if (textures.caveFloor.id > 0) UnloadTexture(textures.caveFloor);
        if (textures.caveExit1.id > 0) UnloadTexture(textures.caveExit1);
        if (textures.caveExit2.id > 0) UnloadTexture(textures.caveExit2);
        textures.isLoaded = false;
    }

private:
    void InitializePalette() {
        customPalette[0] = MakeColor(255, 0, 0, 255);
        customPalette[1] = MakeColor(255, 215, 0, 255);
        customPalette[2] = MakeColor(0, 0, 255, 255);
        customPalette[3] = MakeColor(128, 0, 128, 255);
        customPalette[4] = MakeColor(128, 0, 0, 255);
        customPalette[5] = MakeColor(0, 255, 255, 255);
        customPalette[6] = MakeColor(255, 192, 203, 255);
        customPalette[7] = MakeColor(255, 165, 0, 255);
        customPalette[8] = MakeColor(144, 238, 144, 255);
        customPalette[9] = MakeColor(230, 230, 250, 255);
        customPalette[10] = MakeColor(255, 255, 0, 255);
        customPalette[11] = MakeColor(165, 42, 42, 255);
        customPalette[12] = MakeColor(0, 128, 128, 255);
        customPalette[13] = MakeColor(255, 105, 180, 255);
        customPalette[14] = MakeColor(64, 224, 208, 255);
        customPalette[15] = MakeColor(176, 96, 255, 255);
        customPalette[16] = MakeColor(255, 127, 0, 255);
        customPalette[17] = MakeColor(32, 178, 170, 255);
        customPalette[18] = MakeColor(255, 20, 147, 255);
        customPalette[19] = MakeColor(100, 149, 237, 255);
        jeisoColorIndex = 0;
        boxColorIndex = 4;
    }

    void InitializeGameEntities() {
        player.position = MakeVector2(640.0f, 540.0f);
        player.scale = 250.0f;
        player.colorIndex = jeisoColorIndex;
        player.name = saveData.playerName.empty() ? "Jeiso" : saveData.playerName;
        player.level = 1;
        player.experience = 0;
        player.health = 20;
        player.maxHealth = 20;
        player.frameIndex = 0;
        player.facingRight = true;

        brother.position = MakeVector2(960.0f, 520.0f);
        brother.name = "Brother";
        brother.dialogue = "The cave remembers your name...";
        brother.hasBeenSpokenTo = false;
        brother.isHostile = false;
        brother.frameIndex = 0;
        brother.facingRight = false;

        interactables.clear();
        InteractableObject momObj;
        momObj.position = MakeVector2(250.0f, 420.0f);
        momObj.interactionText = "A shrine of memory and sorrow.";
        momObj.active = true;
        interactables.push_back(momObj);

        InteractableObject eggObj;
        eggObj.position = MakeVector2(1120.0f, 620.0f);
        eggObj.interactionText = "Your egg waits in the dark, humming softly.";
        eggObj.active = true;
        interactables.push_back(eggObj);

        InteractableObject signpostObj;
        signpostObj.position = MakeVector2(930.0f, 610.0f);
        signpostObj.interactionText = "This sign points to the hollow beyond.";
        signpostObj.active = true;
        interactables.push_back(signpostObj);
    }

    Texture2D SafeLoadTexture(const std::string& path) {
        if (FileExists(path.c_str())) {
            return LoadTexture(path.c_str());
        }
        return Texture2D{};
    }

    Sound SafeLoadSound(const std::string& path) {
        if (FileExists(path.c_str())) {
            return LoadSound(path.c_str());
        }
        return Sound{};
    }

    void PlaySoundIfAvailable(Sound sound) {
        if (sound.frameCount > 0) {
            PlaySound(sound);
        }
    }

    void TriggerDialogue(const std::string& text, float duration = 2.0f) {
        dialogueBox.active = true;
        dialogueBox.text = text;
        dialogueBox.lineCount = 1;
        dialogueBox.timer = 0.0f;
        dialogueBox.duration = duration;
    }

    void SpawnBurst(const Vector2& pos, int count, Color c) {
        for (int i = 0; i < count; ++i) {
            Particle p;
            p.position = pos;
            p.velocity = MakeVector2((float)(rand() % 200 - 100), (float)(rand() % 200 - 100));
            p.color = c;
            p.life = 0.6f + (float)(rand() % 20) / 100.0f;
            p.maxLife = p.life;
            p.size = 2.0f + (float)(rand() % 6);
            particles.push_back(p);
        }
    }

    void UpdateParticles() {
        for (size_t i = 0; i < particles.size(); ++i) {
            particles[i].position.x += particles[i].velocity.x * GetFrameTime();
            particles[i].position.y += particles[i].velocity.y * GetFrameTime();
            particles[i].life -= GetFrameTime();
            particles[i].velocity.x *= 0.95f;
            particles[i].velocity.y *= 0.95f;
        }
        particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& p) {
            return p.life <= 0.0f;
        }), particles.end());
    }

    void DrawParticles() {
        for (const auto& p : particles) {
            float alpha = Clampf(p.life / p.maxLife, 0.0f, 1.0f);
            Color c = p.color;
            c.a = (unsigned char)(alpha * 255.0f);
            DrawCircle((int)p.position.x, (int)p.position.y, p.size, c);
        }
    }

    void UpdateOpeningCredits() {
        openingCreditsTimer += GetFrameTime();
        if (openingCreditsTimer >= 3.0f || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z)) {
            currentState = STATE_MENU;
            openingCreditsTimer = 0.0f;
        }
    }

    void DrawOpeningCredits() {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(6, 8, 16, 255));
        DrawText("HIM", WINDOW_WIDTH / 2 - 90, 140, 110, MakeColor(255, 255, 255, 255));
        DrawText("HELL IS MINATORY", WINDOW_WIDTH / 2 - 220, 280, 44, MakeColor(255, 90, 90, 255));
        DrawText("A cavernous odyssey of memory, hatchlings, and dread", WINDOW_WIDTH / 2 - 280, 360, 24, MakeColor(200, 220, 240, 255));
        DrawText("Created with a handcrafted retro engine", WINDOW_WIDTH / 2 - 240, 420, 24, MakeColor(180, 190, 220, 255));
        if (showTitleFlash) {
            DrawText("PRESS ENTER TO START", WINDOW_WIDTH / 2 - 200, 560, 32, MakeColor(255, 220, 90, 255));
        }
    }

    void UpdateMenu() {
        if (currentMenuView == MENU_VIEW_TITLE) {
            UpdateMenuTitle();
        } else if (currentMenuView == MENU_VIEW_SAVE_SLOTS) {
            UpdateMenuSaveSlots();
        } else if (currentMenuView == MENU_VIEW_OPTIONS) {
            UpdateMenuOptions();
        } else if (currentMenuView == MENU_VIEW_CREDITS) {
            if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
                currentMenuView = MENU_VIEW_TITLE;
            }
        }
    }

    void UpdateMenuTitle() {
        if (IsKeyPressed(KEY_DOWN)) {
            menuSelection = (menuSelection + 1) % 4;
        }
        if (IsKeyPressed(KEY_UP)) {
            menuSelection = (menuSelection - 1 + 4) % 4;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_Z)) {
            switch (menuSelection) {
                case 0:
                    currentMenuView = MENU_VIEW_SAVE_SLOTS;
                    saveSlotSelection = 0;
                    break;
                case 1:
                    currentMenuView = MENU_VIEW_OPTIONS;
                    optionSelection = 0;
                    break;
                case 2:
                    currentMenuView = MENU_VIEW_CREDITS;
                    break;
                case 3:
                    CloseWindow();
                    break;
            }
        }
    }

    void UpdateMenuSaveSlots() {
        if (IsKeyPressed(KEY_DOWN)) {
            saveSlotSelection = (saveSlotSelection + 1) % NUM_SAVE_SLOTS;
        }
        if (IsKeyPressed(KEY_UP)) {
            saveSlotSelection = (saveSlotSelection - 1 + NUM_SAVE_SLOTS) % NUM_SAVE_SLOTS;
        }
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
            currentMenuView = MENU_VIEW_TITLE;
            menuSelection = 0;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_Z)) {
            characterNameInput = "";
            keyboardGridX = 0;
            keyboardGridY = 0;
            nameInputActive = true;
            currentState = STATE_INTRO_EGG;
            hasSeenIntroCutscene = true;
            saveData.completedIntro = true;
        }
    }

    void UpdateMenuOptions() {
        if (IsKeyPressed(KEY_DOWN)) {
            optionSelection = (optionSelection + 1) % 3;
        }
        if (IsKeyPressed(KEY_UP)) {
            optionSelection = (optionSelection - 1 + 3) % 3;
        }
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
            currentMenuView = MENU_VIEW_TITLE;
            menuSelection = 0;
        }

        if (optionSelection == 0) {
            if (IsKeyPressed(KEY_RIGHT)) {
                jeisoColorIndex = (jeisoColorIndex + 1) % NUM_PALETTE_COLORS;
                player.colorIndex = jeisoColorIndex;
                saveData.colorIndex = jeisoColorIndex;
            }
            if (IsKeyPressed(KEY_LEFT)) {
                jeisoColorIndex = (jeisoColorIndex - 1 + NUM_PALETTE_COLORS) % NUM_PALETTE_COLORS;
                player.colorIndex = jeisoColorIndex;
                saveData.colorIndex = jeisoColorIndex;
            }
        } else if (optionSelection == 1) {
            if (IsKeyPressed(KEY_RIGHT)) {
                boxColorIndex = (boxColorIndex + 1) % NUM_PALETTE_COLORS;
                saveData.boxColorIndex = boxColorIndex;
            }
            if (IsKeyPressed(KEY_LEFT)) {
                boxColorIndex = (boxColorIndex - 1 + NUM_PALETTE_COLORS) % NUM_PALETTE_COLORS;
                saveData.boxColorIndex = boxColorIndex;
            }
        } else if (optionSelection == 2) {
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT)) {
                PlaySoundIfAvailable(audio.textType);
            }
        }
    }

    void DrawMenu() {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(8, 10, 20, 255));

        if (currentMenuView == MENU_VIEW_TITLE) {
            DrawMenuTitle();
        } else if (currentMenuView == MENU_VIEW_SAVE_SLOTS) {
            DrawMenuSaveSlots();
        } else if (currentMenuView == MENU_VIEW_OPTIONS) {
            DrawMenuOptions();
        } else if (currentMenuView == MENU_VIEW_CREDITS) {
            DrawCredits();
        }
    }

    void DrawMenuTitle() {
        DrawText("HIM", WINDOW_WIDTH / 2 - 80, 120, 90, WHITE);
        DrawText("HELL IS MINATORY", WINDOW_WIDTH / 2 - 220, 220, 40, MakeColor(255, 92, 92, 255));
        DrawText("A handcrafted adventure", WINDOW_WIDTH / 2 - 160, 300, 26, MakeColor(210, 220, 240, 255));

        const char* items[] = { "START GAME", "OPTIONS", "CREDITS", "EXIT" };
        for (int i = 0; i < 4; ++i) {
            Color c = (i == menuSelection) ? MakeColor(255, 230, 110, 255) : WHITE;
            DrawText(items[i], WINDOW_WIDTH / 2 - 110, 380 + i * 70, 34, c);
        }
    }

    void DrawMenuSaveSlots() {
        DrawText("SELECT SAVE SLOT", WINDOW_WIDTH / 2 - 180, 100, 40, WHITE);
        for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
            std::string text = "SAVE " + std::to_string(i + 1) + " : " + (i == 0 ? "ACTIVE" : "EMPTY");
            Color c = (i == saveSlotSelection) ? MakeColor(255, 230, 110, 255) : MakeColor(220, 220, 220, 255);
            DrawText(text.c_str(), 180, 180 + i * 44, 24, c);
        }
    }

    void DrawMenuOptions() {
        DrawText("OPTIONS", WINDOW_WIDTH / 2 - 100, 100, 40, WHITE);
        const char* labels[] = { "PLAYER COLOR", "TEXT BOX COLOR", "SOUND TEST" };
        for (int i = 0; i < 3; ++i) {
            Color c = (i == optionSelection) ? MakeColor(255, 230, 110, 255) : WHITE;
            DrawText(labels[i], 180, 220 + i * 90, 28, c);
            if (i == 0) {
                DrawRectangle(520, 220 + i * 90, 50, 50, customPalette[jeisoColorIndex]);
            } else if (i == 1) {
                DrawRectangle(520, 220 + i * 90, 50, 50, customPalette[boxColorIndex]);
            }
        }
    }

    void DrawCredits() {
        DrawText("CREDITS", WINDOW_WIDTH / 2 - 120, 100, 40, WHITE);
        DrawText("Lead design: Jiffy", 220, 220, 28, MakeColor(255, 255, 255, 255));
        DrawText("Art direction: Retro cavern fantasy", 220, 270, 28, MakeColor(255, 255, 255, 255));
        DrawText("Audio: Ambient cave ambience", 220, 320, 28, MakeColor(255, 255, 255, 255));
        DrawText("Press backspace to return", 220, 600, 24, MakeColor(255, 230, 90, 255));
    }

    void UpdateNameInput() {
        if (IsKeyPressed(KEY_DOWN)) keyboardGridY = (keyboardGridY + 1) % 3;
        if (IsKeyPressed(KEY_UP)) keyboardGridY = (keyboardGridY - 1 + 3) % 3;
        if (IsKeyPressed(KEY_RIGHT)) keyboardGridX = (keyboardGridX + 1) % 9;
        if (IsKeyPressed(KEY_LEFT)) keyboardGridX = (keyboardGridX - 1 + 9) % 9;

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_Z)) {
            int idx = keyboardGridY * 9 + keyboardGridX;
            if (idx < 27 && characterNameInput.length() < MAX_NAME_LENGTH) {
                characterNameInput += "ABCDEFGHIJKLMNOPQRSTUVWXYZ_"[idx];
                PlaySoundIfAvailable(audio.textType);
                player.name = characterNameInput;
                saveData.playerName = characterNameInput;
            }
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !characterNameInput.empty()) {
            characterNameInput.pop_back();
            player.name = characterNameInput.empty() ? "Jeiso" : characterNameInput;
            saveData.playerName = player.name;
        }
    }

    void UpdateEggHatching() {
        if (nameInputActive) {
            UpdateNameInput();
        }

        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
            eggCrackCounter += 1.0f;
            screenShakeIntensity = 25.0f;
            cameraShakeTimer = 0.15f;
            screenShakeOffset = MakeVector2((float)(rand() % 50 - 25), (float)(rand() % 50 - 25));
            PlaySoundIfAvailable(audio.hatchCrack);
        }

        if (screenShakeIntensity > 0.0f) {
            screenShakeIntensity -= 50.0f * GetFrameTime();
        }
        if (cameraShakeTimer > 0.0f) {
            screenShakeOffset.x = (float)(rand() % 50 - 25) * (screenShakeIntensity / 25.0f);
            screenShakeOffset.y = (float)(rand() % 50 - 25) * (screenShakeIntensity / 25.0f);
        }

        if (eggCrackCounter >= 4.0f) {
            fadeTimer += GetFrameTime();
            if (fadeTimer >= 1.2f) {
                currentState = STATE_GAMEPLAY;
                currentArea = AREA_INSIDE_CAVE;
                player.position = MakeVector2(220.0f, 500.0f);
                player.name = characterNameInput.empty() ? "Jeiso" : characterNameInput;
                saveData.playerName = player.name;
                eggCrackCounter = 0.0f;
                fadeTimer = 0.0f;
                nameInputActive = false;
                TriggerDialogue("The egg cracks open. A new path unfolds.");
            }
        }
    }

    void DrawEggHatching() {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(0, 0, 0, 255));
        if (textures.isLoaded) {
            if (eggCrackCounter < 7.0f && textures.eggSprites[(int)eggCrackCounter].id > 0) {
                Texture2D eggTex = textures.eggSprites[(int)eggCrackCounter];
                Vector2 pos = MakeVector2(WINDOW_WIDTH / 2 - eggTex.width / 2 + screenShakeOffset.x,
                                          WINDOW_HEIGHT / 2 - eggTex.height / 2 + screenShakeOffset.y);
                DrawTextureEx(eggTex, pos, 0.0f, 1.0f, WHITE);
            } else {
                DrawCircle(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 120 + (int)eggCrackCounter * 10, MakeColor(255, 230, 140, 255));
                DrawCircle(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 70 + (int)eggCrackCounter * 8, MakeColor(190, 120, 80, 255));
            }
        }
        DrawText("BREAK THE EGG", WINDOW_WIDTH / 2 - 120, 80, 36, WHITE);
        DrawText("Press Z to crack it", WINDOW_WIDTH / 2 - 140, 640, 26, MakeColor(255, 220, 90, 255));
        if (eggCrackCounter >= 4.0f) {
            float alpha = Clampf(fadeTimer / 1.2f, 0.0f, 1.0f);
            DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(0, 0, 0, (int)(alpha * 255.0f)));
        }
    }

    void UpdateGameplay() {
        UpdateParticles();
        if (currentArea == AREA_INSIDE_CAVE) {
            UpdateGameplayCave();
        } else if (currentArea == AREA_OUTSIDE_CAVE) {
            UpdateGameplayOutside();
        } else if (currentArea == AREA_FINAL_CHAMBER) {
            UpdateGameplayFinalChamber();
        }

        if (IsKeyPressed(KEY_F)) {
            shouldFadeOut = true;
            transitionTimer = 0.0f;
        }

        if (player.health <= 0) {
            currentState = STATE_GAME_OVER;
        }
    }

    void UpdateGameplayCave() {
        if (IsKeyPressed(KEY_UP) && player.position.y > 140.0f) player.position.y -= 60.0f;
        if (IsKeyPressed(KEY_DOWN) && player.position.y < WINDOW_HEIGHT - 140.0f) player.position.y += 60.0f;
        if (IsKeyPressed(KEY_LEFT) && player.position.x > 120.0f) {
            player.position.x -= 60.0f;
            player.facingRight = false;
        }
        if (IsKeyPressed(KEY_RIGHT) && player.position.x < WINDOW_WIDTH - 120.0f) {
            player.position.x += 60.0f;
            player.facingRight = true;
        }

        if (IsKeyPressed(KEY_Z)) {
            for (auto& obj : interactables) {
                float dist = Vector2Distance(player.position, obj.position);
                if (dist < 220.0f && obj.active) {
                    TriggerDialogue(obj.interactionText, 2.5f);
                    if (obj.interactionText.find("egg") != std::string::npos) {
                        SpawnBurst(obj.position, 20, MakeColor(255, 240, 120, 255));
                    }
                }
            }
            float distToBrother = Vector2Distance(player.position, brother.position);
            if (distToBrother < 220.0f) {
                currentState = STATE_COMBAT;
                combatTurn = 0;
                triggerCombatStart();
            }
        }

        if (player.position.y < 180.0f) {
            currentArea = AREA_OUTSIDE_CAVE;
            player.position = MakeVector2(220.0f, 500.0f);
            TriggerDialogue("The cave opens to a tragic sunrise.");
        }
    }

    void UpdateGameplayOutside() {
        if (IsKeyPressed(KEY_UP) && player.position.y > 140.0f) player.position.y -= 60.0f;
        if (IsKeyPressed(KEY_DOWN) && player.position.y < WINDOW_HEIGHT - 140.0f) player.position.y += 60.0f;
        if (IsKeyPressed(KEY_LEFT) && player.position.x > 120.0f) {
            player.position.x -= 60.0f;
            player.facingRight = false;
        }
        if (IsKeyPressed(KEY_RIGHT) && player.position.x < WINDOW_WIDTH - 120.0f) {
            player.position.x += 60.0f;
            player.facingRight = true;
        }

        if (IsKeyPressed(KEY_Z)) {
            float dist = Vector2Distance(player.position, brother.position);
            if (dist < 220.0f) {
                brother.hasBeenSpokenTo = true;
                saveData.hasSeenBrother = true;
                currentState = STATE_COMBAT;
                combatTurn = 0;
                triggerCombatStart();
            }
        }

        if (player.position.x > WINDOW_WIDTH - 160.0f) {
            currentArea = AREA_FINAL_CHAMBER;
            player.position = MakeVector2(220.0f, 480.0f);
            TriggerDialogue("The final chamber breathes like a giant heart.");
        }
    }

    void UpdateGameplayFinalChamber() {
        if (IsKeyPressed(KEY_UP) && player.position.y > 140.0f) player.position.y -= 60.0f;
        if (IsKeyPressed(KEY_DOWN) && player.position.y < WINDOW_HEIGHT - 140.0f) player.position.y += 60.0f;
        if (IsKeyPressed(KEY_LEFT) && player.position.x > 120.0f) {
            player.position.x -= 60.0f;
            player.facingRight = false;
        }
        if (IsKeyPressed(KEY_RIGHT) && player.position.x < WINDOW_WIDTH - 120.0f) {
            player.position.x += 60.0f;
            player.facingRight = true;
        }
    }

    void DrawGameplay() {
        DrawGameplayBackground();
        DrawParticles();

        if (currentArea == AREA_INSIDE_CAVE) {
            DrawCaveLayer();
        } else if (currentArea == AREA_OUTSIDE_CAVE) {
            DrawOutsideLayer();
        } else if (currentArea == AREA_FINAL_CHAMBER) {
            DrawFinalChamberLayer();
        }

        DrawInteractables();
        DrawPlayerSprite(player.position, customPalette[player.colorIndex]);
        DrawNPCSprite(brother.position, MakeColor(180, 90, 255, 255));

        DrawHUD();
        DrawDialogue();

        if (!tutorialShown) {
            DrawText("Use arrow keys to move and Z to interact", 40, 660, 22, MakeColor(255, 230, 90, 255));
            if (IsKeyPressed(KEY_Z)) tutorialShown = true;
        }
    }

    void DrawGameplayBackground() {
        if (currentArea == AREA_INSIDE_CAVE) {
            DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(20, 16, 30, 255), MakeColor(58, 76, 120, 255));
        } else if (currentArea == AREA_OUTSIDE_CAVE) {
            DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(120, 200, 255, 255), MakeColor(24, 92, 180, 255));
        } else {
            DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(30, 16, 24, 255), MakeColor(92, 40, 70, 255));
        }
    }

    void DrawCaveLayer() {
        DrawRectangle(0, 520, WINDOW_WIDTH, 200, MakeColor(40, 40, 48, 255));
        DrawRectangle(0, 470, WINDOW_WIDTH, 70, MakeColor(78, 76, 86, 255));
        for (int i = 0; i < 8; ++i) {
            int x = 120 + i * 140;
            DrawTriangle({(float)x, 500.0f}, {(float)x - 60.0f, 690.0f}, {(float)x + 60.0f, 690.0f}, MakeColor(70, 64, 76, 255));
            DrawCircle(x, 470, 30, MakeColor(100, 96, 112, 255));
        }
        DrawRectangle(120, 350, 220, 180, MakeColor(90, 74, 56, 255));
        DrawRectangle(150, 290, 160, 80, MakeColor(110, 90, 70, 255));
        DrawCircle(230, 270, 50, MakeColor(140, 140, 170, 255));
        DrawRectangle(900, 340, 240, 190, MakeColor(90, 72, 56, 255));
        DrawCircle(1020, 320, 50, MakeColor(132, 132, 162, 255));
    }

    void DrawOutsideLayer() {
        DrawRectangle(0, 520, WINDOW_WIDTH, 200, MakeColor(56, 132, 64, 255));
        DrawRectangle(0, 470, WINDOW_WIDTH, 70, MakeColor(84, 164, 80, 255));
        for (int i = 0; i < 10; ++i) {
            int x = 70 + i * 120;
            DrawTriangle({(float)x, 500.0f}, {(float)x - 60.0f, 690.0f}, {(float)x + 60.0f, 690.0f}, MakeColor(46, 96, 44, 255));
        }
        DrawRectangle(100, 360, 140, 160, MakeColor(100, 62, 24, 255));
        DrawCircle(170, 330, 50, MakeColor(40, 120, 40, 255));
        DrawRectangle(1040, 360, 140, 170, MakeColor(106, 68, 28, 255));
        DrawCircle(1110, 330, 52, MakeColor(44, 126, 44, 255));
    }

    void DrawFinalChamberLayer() {
        DrawRectangle(0, 520, WINDOW_WIDTH, 200, MakeColor(70, 24, 46, 255));
        DrawRectangle(0, 470, WINDOW_WIDTH, 70, MakeColor(112, 54, 82, 255));
        for (int i = 0; i < 6; ++i) {
            int x = 140 + i * 180;
            DrawCircle(x, 440, 60, MakeColor(180, 70, 120, 255));
        }
        DrawCircle(WINDOW_WIDTH / 2, 240, 120, MakeColor(255, 220, 140, 255));
        DrawCircle(WINDOW_WIDTH / 2, 240, 80, MakeColor(255, 120, 140, 255));
    }

    void DrawInteractables() {
        for (const auto& obj : interactables) {
            if (!obj.active) continue;
            if (obj.interactionText.find("shrine") != std::string::npos) {
                DrawCircle((int)obj.position.x, (int)obj.position.y, 40, MakeColor(185, 72, 72, 180));
                DrawCircle((int)obj.position.x, (int)obj.position.y, 20, MakeColor(255, 216, 96, 220));
            } else if (obj.interactionText.find("egg") != std::string::npos) {
                DrawCircle((int)obj.position.x, (int)obj.position.y, 36, MakeColor(92, 196, 115, 160));
                DrawCircle((int)obj.position.x, (int)obj.position.y, 16, MakeColor(255, 255, 255, 220));
            } else {
                DrawRectangle((int)obj.position.x - 24, (int)obj.position.y - 28, 48, 56, MakeColor(179, 140, 92, 255));
                DrawRectangle((int)obj.position.x - 14, (int)obj.position.y - 38, 28, 16, MakeColor(255, 211, 128, 255));
            }
        }
    }

    void DrawPlayerSprite(const Vector2& pos, Color tint) {
        if (textures.isLoaded && textures.jeisoFront[0].id > 0) {
            float scale = 0.28f;
            if (!player.facingRight) scale = -0.28f;
            Vector2 drawPos = MakeVector2(pos.x - 60.0f, pos.y - 60.0f);
            DrawTextureEx(textures.jeisoFront[0], drawPos, 0.0f, scale < 0 ? -scale : scale, tint);
            return;
        }

        DrawCircle((int)pos.x, (int)pos.y + 20, 18, MakeColor(0, 0, 0, 80));
        DrawCircle((int)pos.x, (int)pos.y - 10, 22, tint);
        DrawRectangle((int)pos.x - 18, (int)pos.y + 8, 36, 38, tint);
        DrawRectangle((int)pos.x - 14, (int)pos.y + 24, 10, 36, MakeColor(90, 55, 25, 255));
        DrawRectangle((int)pos.x + 4, (int)pos.y + 24, 10, 36, MakeColor(90, 55, 25, 255));
    }

    void DrawNPCSprite(const Vector2& pos, Color tint) {
        if (textures.isLoaded && textures.brotherFront[0].id > 0) {
            float scale = 0.28f;
            if (!brother.facingRight) scale = -0.28f;
            Vector2 drawPos = MakeVector2(pos.x - 60.0f, pos.y - 60.0f);
            DrawTextureEx(textures.brotherFront[0], drawPos, 0.0f, scale < 0 ? -scale : scale, tint);
            return;
        }

        DrawCircle((int)pos.x, (int)pos.y + 20, 18, MakeColor(0, 0, 0, 80));
        DrawCircle((int)pos.x + 2, (int)pos.y - 8, 22, tint);
        DrawRectangle((int)pos.x - 18, (int)pos.y + 8, 36, 38, tint);
        DrawRectangle((int)pos.x - 14, (int)pos.y + 24, 10, 36, MakeColor(56, 38, 21, 255));
        DrawRectangle((int)pos.x + 4, (int)pos.y + 24, 10, 36, MakeColor(56, 38, 21, 255));
    }

    void DrawHUD() {
        DrawRectangle(20, 20, 280, 120, MakeColor(12, 24, 40, 220));
        DrawText("HP", 36, 34, 24, WHITE);
        for (int i = 0; i < player.maxHealth; ++i) {
            Color heartColor = i < player.health ? MakeColor(255, 80, 80, 255) : MakeColor(80, 80, 80, 255);
            DrawText("♥", 36 + i * 18, 58, 24, heartColor);
        }
        DrawText(("NAME: " + player.name).c_str(), 36, 92, 20, MakeColor(255, 230, 90, 255));
        DrawText(("LVL: " + std::to_string(player.level)).c_str(), 36, 116, 20, MakeColor(255, 255, 255, 255));
    }

    void DrawDialogue() {
        if (!dialogueBox.active) return;
        DrawRectangle(80, 560, WINDOW_WIDTH - 160, 110, MakeColor(16, 24, 36, 220));
        DrawRectangleLines(80, 560, WINDOW_WIDTH - 160, 110, MakeColor(255, 220, 90, 255));
        DrawText(dialogueBox.text.c_str(), 110, 590, 24, WHITE);
    }

    void triggerCombatStart() {
        brotherHealth = brotherMaxHealth;
        combatTurn = 0;
        currentCombatPhase = COMBAT_PHASE_STRIKE;
        for (int i = 0; i < 5; ++i) combatPhaseComplete[i] = false;
        TriggerDialogue("Brother rises in the dim light. Choose with care.");
    }

    void UpdateCombat() {
        if (IsKeyPressed(KEY_DOWN)) {
            currentCombatPhase = (CombatPhase)((int)currentCombatPhase + 1);
            if (currentCombatPhase > COMBAT_PHASE_VICTORY) currentCombatPhase = COMBAT_PHASE_STRIKE;
        }
        if (IsKeyPressed(KEY_UP)) {
            currentCombatPhase = (CombatPhase)((int)currentCombatPhase - 1);
            if (currentCombatPhase < COMBAT_PHASE_STRIKE) currentCombatPhase = COMBAT_PHASE_VICTORY;
        }
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
            HandleCombatAction();
        }
        combatTextTimer += GetFrameTime();
    }

    void HandleCombatAction() {
        switch (currentCombatPhase) {
            case COMBAT_PHASE_STRIKE:
                brotherHealth -= 3;
                combatPhaseComplete[0] = true;
                currentCombatPhase = COMBAT_PHASE_BLOCK;
                PlaySoundIfAvailable(audio.strikeHit);
                break;
            case COMBAT_PHASE_BLOCK:
                player.health = Clampi(player.health + 1, 0, player.maxHealth);
                combatPhaseComplete[1] = true;
                currentCombatPhase = COMBAT_PHASE_SAP;
                break;
            case COMBAT_PHASE_SAP:
                brotherHealth -= 6;
                combatPhaseComplete[2] = true;
                currentCombatPhase = COMBAT_PHASE_ITEMS;
                break;
            case COMBAT_PHASE_ITEMS:
                brotherHealth -= 10;
                combatPhaseComplete[3] = true;
                currentCombatPhase = COMBAT_PHASE_VICTORY;
                break;
            case COMBAT_PHASE_VICTORY:
                combatPhaseComplete[4] = true;
                brother.hasBeenSpokenTo = true;
                currentState = STATE_GAMEPLAY;
                currentArea = AREA_OUTSIDE_CAVE;
                player.experience += 10;
                if (player.experience >= 10) {
                    player.level += 1;
                    player.experience = 0;
                }
                TriggerDialogue("The battle ends. The path beyond opens.");
                break;
        }
    }

    void DrawCombat() {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(28, 10, 40, 255));
        DrawText("BATTLE", 100, 60, 46, WHITE);
        DrawText("BROTHER", 220, 160, 36, WHITE);
        DrawText("YOU", 900, 160, 36, WHITE);
        DrawNPCSprite(MakeVector2(260.0f, 420.0f), MakeColor(180, 90, 255, 255));
        DrawPlayerSprite(MakeVector2(960.0f, 420.0f), customPalette[player.colorIndex]);

        DrawRectangle(120, 520, 1040, 150, MakeColor(70, 60, 90, 255));
        DrawRectangleLines(120, 520, 1040, 150, MakeColor(255, 230, 110, 255));

        const char* actions[] = { "STRIKE", "BLOCK", "SAP", "ITEMS", "ESCAPE" };
        for (int i = 0; i < 5; ++i) {
            Color c = (i == (int)currentCombatPhase) ? MakeColor(255, 230, 110, 255) : MakeColor(180, 180, 180, 255);
            DrawText(actions[i], 180 + i * 160, 560, 28, c);
        }

        DrawText("Press Up/Down to select, Z to confirm", 180, 620, 22, MakeColor(255, 240, 180, 255));
    }

    void UpdateGameOver() {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_Z)) {
            currentState = STATE_MENU;
            currentMenuView = MENU_VIEW_TITLE;
            menuSelection = 0;
        }
    }

    void DrawGameOver() {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(8, 8, 8, 255));
        DrawText("GAME OVER", WINDOW_WIDTH / 2 - 180, 180, 72, MakeColor(255, 80, 80, 255));
        DrawText("The cave swallows your light.", WINDOW_WIDTH / 2 - 220, 320, 32, WHITE);
        DrawText("Press Enter to return to menu", WINDOW_WIDTH / 2 - 240, 560, 28, MakeColor(255, 220, 90, 255));
    }
};

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    GameEngine game;
    game.LoadTextures();
    game.InitializeAudio();

    while (!WindowShouldClose()) {
        game.Update();
        game.Draw();
    }

    game.UnloadAudio();
    game.UnloadTextures();
    CloseWindow();
    return 0;
}
