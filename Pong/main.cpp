#include <iostream>
#include <raylib.h>

// TODO: Collision problem where it gets sticky if ball it's top of the CPU paddle instead of the face
// TODO: Create timer for ball when it resets to the center after scoring

struct Paddle {
    Vector2 position;
    Vector2 size;
    float speed;
};

struct Ball {
    Vector2 position;
    Vector2 velocity;
    float radius;
};

struct GameState {
    Paddle player;
    Paddle cpu;
    Ball ball;
    int playerScore, cpuScore;
    float screenWidth, screenHeight;
};

void InitGame(GameState& state)
{
    state.screenWidth = 1280;
    state.screenHeight = 800;

    InitWindow(state.screenWidth, state.screenHeight, "PONG");
    SetTargetFPS(60);

    state.ball.position = { state.screenWidth / 2, state.screenHeight / 2 };
    state.ball.radius = 15;
    state.ball.velocity = { 7, 7 };

    state.player.size = { 20, 100 };
    state.player.position = { state.screenWidth - state.player.size.x - 10, state.screenHeight / 2 - state.player.size.y / 2};    
    state.player.speed = 6;    

    state.cpu.size = { 20, 100 };
    state.cpu.position = { 10,  state.screenHeight / 2 - state.cpu.size.y / 2 };
    state.cpu.speed = 6;    

    state.playerScore = 0, state.cpuScore = 0;
}

void PlayerInput(GameState& state)
{
    if (IsKeyDown(KEY_UP))
    {
        state.player.position.y -= state.player.speed;
    }
    if (IsKeyDown(KEY_DOWN))
    {
        state.player.position.y += state.player.speed;
    }    
}

void BallReset(GameState& state)
{

    state.ball.position.x = GetScreenWidth() / 2;
    state.ball.position.y = GetScreenHeight() / 2;
    int velocityRNG[2] = { -1, 1 };
    state.ball.velocity.x *= velocityRNG[GetRandomValue(0, 1)];
    state.ball.velocity.y *= velocityRNG[GetRandomValue(0, 1)];
}

void Update(GameState& state)
{
    // Move ball
    state.ball.position.x += state.ball.velocity.x;
    state.ball.position.y += state.ball.velocity.y;

    // Checking ball collision with screen height
    if (state.ball.position.y + state.ball.radius >= GetScreenHeight() || state.ball.position.y - state.ball.radius <= 0)
    {
        state.ball.velocity.y *= -1;
    }

    // Checking ball collision with screen width
    if (state.ball.position.x + state.ball.radius >= GetScreenWidth())
    {
        state.cpuScore++;
        BallReset(state);
    }
    if (state.ball.position.x - state.ball.radius <= 0)
    {
        state.playerScore++;
        BallReset(state);
    }        

    // Checking ball collision with player
    if (CheckCollisionCircleRec({ state.ball.position.x, state.ball.position.y }, state.ball.radius, { state.player.position.x, state.player.position.y, state.player.size.x, state.player.size.y })) {
        state.ball.position.x = state.player.position.x - state.player.size.x;
        state.ball.velocity.x *= -1;
    }    

    // Checking ball collision with CPU
    if (CheckCollisionCircleRec({ state.ball.position.x, state.ball.position.y }, state.ball.radius, { state.cpu.position.x, state.cpu.position.y, state.cpu.size.x, state.cpu.size.y })) {        
        state.ball.position.x += 1;
        state.ball.velocity.x *= -1;
    }    

    // Restrict player movement to be within the screen height boundary
    if (state.player.position.y <= 0) {
        state.player.position.y = 0;
    }
    if (state.player.position.y + state.player.size.y >= GetScreenHeight()) {
        state.player.position.y = GetScreenHeight() - state.player.size.y;
    }

    // CPU AI
    if (state.cpu.position.y + state.cpu.size.y / 2 > state.ball.position.y) {
        state.cpu.position.y -= state.cpu.speed;
    }
    if (state.cpu.position.y + state.cpu.size.y / 2 <= state.ball.position.y) {
        state.cpu.position.y += state.cpu.speed;
    }        
}

void RenderGame(const GameState& state) {    
    ClearBackground(BLACK);

    for (int i = 0; i < state.screenHeight; i += 40)
    {
        DrawLine(state.screenWidth / 2, i, state.screenWidth / 2, state.screenHeight / 30 + i, WHITE);
    }    

    DrawCircle(state.ball.position.x, state.ball.position.y, state.ball.radius, WHITE);
    DrawRectangle(state.player.position.x, state.player.position.y, state.player.size.x, state.player.size.y, WHITE);
    DrawRectangle(state.cpu.position.x, state.cpu.position.y, state.cpu.size.x, state.cpu.size.y, WHITE);

    DrawText(TextFormat("%i", state.cpuScore), state.screenWidth/ 4 - 20, 20, 80, WHITE);
    DrawText(TextFormat("%i", state.playerScore), 3 * state.screenWidth / 4 - 20, 20, 80, WHITE);
}


int main()
{
    GameState gameState;
    InitGame(gameState);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        PlayerInput(gameState);
        Update(gameState);
        RenderGame(gameState);

        EndDrawing();
    }
    return 0;
}