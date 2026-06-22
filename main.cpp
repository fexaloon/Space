#include "raylib.h"
#include <cmath>
#include <string>
#include <vector>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int TARGET_FPS = 60;
const char* WINDOW_TITLE = "HIM: Hell is Minatory";

inline Color MakeColor(int r, int g, int b, int a) {
    return { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
}

inline Vector2 MakeVector2(float x, float y) {
    return { x, y };
}

struct TextureAssets {
    Texture2D jeisoFront;
    Texture2D brotherFront;
    bool isLoaded;
};

TextureAssets textures = {};

struct AudioAssets {
    Sound textType;
    Sound hatchCrack;
    bool isLoaded;
};

AudioAssets audio = {};

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

void LoadGameTextures() {
    textures.jeisoFront = SafeLoadTexture("assets/jeiso_front.png");
    textures.brotherFront = SafeLoadTexture("assets/brother_front.png");
    textures.isLoaded = true;
}

void UnloadGameTextures() {
    if (textures.jeisoFront.id > 0) UnloadTexture(textures.jeisoFront);
    if (textures.brotherFront.id > 0) UnloadTexture(textures.brotherFront);
    textures.isLoaded = false;
}

void InitializeAudio() {
    InitAudioDevice();
    audio.textType = SafeLoadSound("assets/text_type.wav");
    audio.hatchCrack = SafeLoadSound("assets/hatch_crack.wav");
    audio.isLoaded = true;
}

void UnloadAudio() {
    if (audio.textType.frameCount > 0) UnloadSound(audio.textType);
    if (audio.hatchCrack.frameCount > 0) UnloadSound(audio.hatchCrack);
    audio.isLoaded = false;
}

enum GameState {
    STATE_OPENING,
    STATE_MENU,
    STATE_GAMEPLAY,
    STATE_COMBAT
};

struct Player {
    Vector2 position;
    std::string name;
    int colorIndex;
    int health;
};

struct NPC {
    Vector2 position;
    bool talkedTo;
};

class GameEngine {
private:
    GameState state;
    Player player;
    NPC brother;
    int menuSelection;
    int titleBlinkTimer;
    bool showPrompt;
    int combatPhase;

public:
    GameEngine()
        : state(STATE_OPENING),
          player({ MakeVector2(640.0f, 540.0f), "Jeiso", 2, 20 }),
          brother({ MakeVector2(980.0f, 520.0f), false }),
          menuSelection(0),
          titleBlinkTimer(0),
          showPrompt(true),
          combatPhase(0) {
    }

    void Update() {
        switch (state) {
            case STATE_OPENING:
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z)) {
                    state = STATE_MENU;
                }
                break;
            case STATE_MENU:
                if (IsKeyPressed(KEY_DOWN)) menuSelection = (menuSelection + 1) % 2;
                if (IsKeyPressed(KEY_UP)) menuSelection = (menuSelection + 1) % 2;
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_Z)) {
                    if (menuSelection == 0) {
                        state = STATE_GAMEPLAY;
                    } else {
                        CloseWindow();
                    }
                }
                break;
            case STATE_GAMEPLAY:
                HandleMovement();
                if (IsKeyPressed(KEY_Z)) {
                    float dist = Vector2Distance(player.position, brother.position);
                    if (dist < 220.0f) {
                        state = STATE_COMBAT;
                    }
                }
                break;
            case STATE_COMBAT:
                if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
                    combatPhase = (combatPhase + 1) % 3;
                    if (combatPhase == 0) {
                        brother.talkedTo = true;
                        state = STATE_GAMEPLAY;
                    }
                }
                break;
        }

        titleBlinkTimer++;
        if (titleBlinkTimer > 40) {
            titleBlinkTimer = 0;
            showPrompt = !showPrompt;
        }
    }

    void Draw() {
        BeginDrawing();
        ClearBackground(MakeColor(8, 12, 24, 255));

        switch (state) {
            case STATE_OPENING:
                DrawOpening();
                break;
            case STATE_MENU:
                DrawMenu();
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

private:
    void HandleMovement() {
        if (IsKeyPressed(KEY_UP) && player.position.y > 140) player.position.y -= 60;
        if (IsKeyPressed(KEY_DOWN) && player.position.y < WINDOW_HEIGHT - 140) player.position.y += 60;
        if (IsKeyPressed(KEY_LEFT) && player.position.x > 120) player.position.x -= 60;
        if (IsKeyPressed(KEY_RIGHT) && player.position.x < WINDOW_WIDTH - 120) player.position.x += 60;
    }

    void DrawOpening() {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(10, 10, 18, 255));
        DrawText("HIM", WINDOW_WIDTH / 2 - 90, 170, 100, MakeColor(255, 255, 255, 255));
        DrawText("HELL IS MINATORY", WINDOW_WIDTH / 2 - 220, 300, 44, MakeColor(255, 88, 88, 255));
        DrawText("A retro adventure in a cave world", WINDOW_WIDTH / 2 - 220, 380, 28, MakeColor(200, 220, 255, 255));
        if (showPrompt) {
            DrawText("PRESS ENTER TO START", WINDOW_WIDTH / 2 - 180, 560, 32, MakeColor(255, 220, 90, 255));
        }
    }

    void DrawMenu() {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(8, 10, 20, 255));
        DrawText("MAIN MENU", WINDOW_WIDTH / 2 - 140, 140, 48, WHITE);

        const char* items[] = { "START GAME", "EXIT" };
        for (int i = 0; i < 2; ++i) {
            Color c = (i == menuSelection) ? MakeColor(255, 220, 90, 255) : WHITE;
            DrawText(items[i], WINDOW_WIDTH / 2 - 120, 300 + i * 90, 36, c);
        }
    }

    void DrawGameplay() {
        DrawBackground();
        DrawWorldObjects();
        DrawPlayer(player.position, player.colorIndex);
        DrawNPC(brother.position, MakeColor(200, 120, 255, 255));

        DrawText("ARROWS = MOVE", 40, 40, 24, MakeColor(255, 255, 255, 220));
        DrawText("Z = TALK", 40, 74, 24, MakeColor(255, 255, 0, 220));
    }

    void DrawCombat() {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(32, 18, 44, 255));
        DrawText("BROTHER", 180, 130, 46, WHITE);
        DrawText("YOU", 900, 130, 46, WHITE);
        DrawPlayer(MakeVector2(980.0f, 400.0f), player.colorIndex);
        DrawNPC(MakeVector2(260.0f, 400.0f), MakeColor(200, 120, 255, 255));

        DrawRectangle(120, 560, 1040, 120, MakeColor(70, 60, 90, 255));
        const char* lines[] = { "Press Z to continue", "The cave hums around you", "The path is yours" };
        DrawText(lines[combatPhase], 160, 590, 28, MakeColor(255, 240, 180, 255));
    }

    void DrawBackground() {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MakeColor(20, 26, 44, 255));
        DrawRectangle(0, 520, WINDOW_WIDTH, 200, MakeColor(56, 86, 58, 255));
        DrawRectangle(0, 470, WINDOW_WIDTH, 60, MakeColor(84, 120, 84, 255));

        for (int i = 0; i < 10; ++i) {
            int x = 80 + i * 120;
            DrawTriangle(
                { (float)x, 500.0f },
                { (float)x - 45.0f, 690.0f },
                { (float)x + 45.0f, 690.0f },
                MakeColor(64, 72, 90, 255));
        }

        DrawRectangle(120, 360, 220, 160, MakeColor(90, 74, 56, 255));
        DrawRectangle(160, 300, 140, 80, MakeColor(110, 90, 70, 255));
        DrawCircle(230, 280, 45, MakeColor(128, 128, 160, 255));
        DrawRectangle(920, 340, 240, 180, MakeColor(95, 80, 60, 255));
        DrawCircle(1040, 320, 50, MakeColor(140, 140, 170, 255));
    }

    void DrawWorldObjects() {
        DrawCircle(260, 470, 38, MakeColor(180, 80, 80, 180));
        DrawCircle(260, 470, 18, MakeColor(255, 230, 120, 220));
        DrawRectangle(950, 470, 48, 48, MakeColor(120, 90, 70, 255));
    }

    void DrawPlayer(const Vector2& pos, int colorIndex) {
        Color tint = MakeColor(120 + colorIndex * 5, 70 + colorIndex * 3, 180 + colorIndex * 2, 255);

        if (textures.isLoaded && textures.jeisoFront.id > 0) {
            DrawTextureEx(textures.jeisoFront, MakeVector2(pos.x - 60, pos.y - 60), 0.0f, 0.28f, tint);
        } else {
            DrawCircle((int)pos.x, (int)pos.y + 20, 18, MakeColor(0, 0, 0, 80));
            DrawCircle((int)pos.x, (int)pos.y - 10, 22, tint);
            DrawRectangle((int)pos.x - 18, (int)pos.y + 8, 36, 38, tint);
            DrawRectangle((int)pos.x - 14, (int)pos.y + 24, 10, 36, MakeColor(90, 55, 25, 255));
            DrawRectangle((int)pos.x + 4, (int)pos.y + 24, 10, 36, MakeColor(90, 55, 25, 255));
        }
    }

    void DrawNPC(const Vector2& pos, Color tint) {
        if (textures.isLoaded && textures.brotherFront.id > 0) {
            DrawTextureEx(textures.brotherFront, MakeVector2(pos.x - 60, pos.y - 60), 0.0f, 0.28f, tint);
        } else {
            DrawCircle((int)pos.x, (int)pos.y + 20, 18, MakeColor(0, 0, 0, 80));
            DrawCircle((int)pos.x + 2, (int)pos.y - 8, 22, tint);
            DrawRectangle((int)pos.x - 18, (int)pos.y + 8, 36, 38, tint);
            DrawRectangle((int)pos.x - 14, (int)pos.y + 24, 10, 36, MakeColor(56, 38, 21, 255));
            DrawRectangle((int)pos.x + 4, (int)pos.y + 24, 10, 36, MakeColor(56, 38, 21, 255));
        }
    }
};

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    LoadGameTextures();
    InitializeAudio();

    GameEngine game;
    while (!WindowShouldClose()) {
        game.Update();
        game.Draw();
    }

    UnloadAudio();
    UnloadGameTextures();
    CloseWindow();
    return 0;
}
