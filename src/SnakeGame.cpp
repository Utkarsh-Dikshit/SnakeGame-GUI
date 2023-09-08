#include <raylib.h>
#include <deque>
#include <raymath.h>
using namespace std;

Color bgColour = {255, 255, 153, 255}; 
Color snakeColorHead = {42, 48, 212, 255}; 
Color snakeColorTail = {42, 124, 212, 255};

int cellsize = 20;
int cellcount_x = 40, cellcount_y = 25;
int width_window = 50;
int screenWidth = 800, screenHeight = 500;
double lastUpdateTime = 0.0;
bool gamePause = false;

bool UpdateTime(double interval){
    double currentTime = GetTime(); // Get elapsed time in seconds since InitWindow()
    
    if (currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }

    return false;
}

bool elementPresent (Vector2 element, deque <Vector2> deque){
    for (unsigned int i = 0; i < deque.size(); i++){
        if (Vector2Equals (deque[i], element)){
            return true;
        }
    }
    return false;
}

class Food {
    public:
        Vector2 position = {5, 6};
        Texture2D texture;

        // CONSTRUCTOR
        Food(deque <Vector2> snakeBody){
            Image image = LoadImage ("src/Fruit/Decreased_size.png");
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
            position = randomPosition(snakeBody);
        }

        // DECONSTRUCTOR
        ~Food(){
            UnloadTexture(texture);
        }

        void draw(){
            // DrawTexture(texture, position.x *cellsize, position.y*cellsize, WHITE);
            
            // Below draw texture function is used just to decrease the size of our fruit
            // It is better to decrease the size by using any website, rather than using scale option 
            DrawTextureEx (texture, Vector2{width_window + position.x*cellsize, width_window + position.y*cellsize}, 0, 0.46 /*Scale*/, WHITE);
            
        }

        Vector2 randomCell(){
            float x = GetRandomValue(0, cellcount_x - 1);
            float y = GetRandomValue(0, cellcount_y - 1);
            
            /* 
            You can also use:-
                position.x = rand()%(cellcount_x - 1);
                position.y = rand()%(cellcount_y - 1);
                return Vector2 {position.x, position.y};
            */
           
            return Vector2 {x, y};
        }
        
        Vector2 randomPosition(deque <Vector2> snakeBody){
            
            position = randomCell();

            while (elementPresent(position, snakeBody)){
                position = randomCell();
            }
            return position;
        }
};

// Using Dequeue (data structure) (including deque) for snake (as we can add and remove element from both ends);
class Snake{
    public:
        deque <Vector2> body = {Vector2{5,5}, Vector2{4,5}, Vector2{3,5}};
        bool add = false;

        void draw(){
            for(unsigned int i = 0; i < body.size(); i++){
                float x = body [i].x;
                float y = body [i].y;

                // Below function will draw rectangle as a snake 
                // DrawRectangle(x*cellsize, y*cellsize, cellsize, cellsize, snakeColorTail);
                if (i == 0){
                    Rectangle rec = Rectangle {width_window + x*cellsize, width_window + y*cellsize, float (cellsize), float (cellsize)};
                    DrawRectangleRounded(rec, 0.8, 7, snakeColorHead);
                }
                if (i != 0){
                    // Below things are for rounded rectangle as a snake
                    Rectangle rec = Rectangle {width_window + x*cellsize, width_window + y*cellsize, float (cellsize), float (cellsize)};
                    DrawRectangleRounded(rec, 0.8, 7, snakeColorTail);
                }
            }
        }

        // To move the snake, we take the last cell and move it to the ahead of first cell on the basis of movement from user.
        Vector2 dir = {1,0};
        void update(){
            body.push_front(Vector2Add(body[0] /*SNAKE HEAD*/, dir));

            if (add == true){
                add = false;
            }
            else{
                body.pop_back(); // removes the last element from the deque
            }
        }

        void reset(){
            body = {Vector2{5,5}, Vector2{4,5}, Vector2{3,5}};
            dir = {1, 0};
        }
};

class MainGame{
    public:
        Snake snake = Snake();
        Food food = Food(snake.body);
        bool run = true;
        int score = 0;
        int highScore = 0;
        Sound eat, background, gameOverSound;

        // Constructor
        MainGame(){
            InitAudioDevice();
            eat = LoadSound("src/sounds/eat.wav");
            background = LoadSound("src/sounds/Background.mp3");
            gameOverSound = LoadSound("src/sounds/gameOver.wav");
        }
        
        // Deconstructure
        ~MainGame(){
            UnloadSound(eat);
            UnloadSound(background);
            UnloadSound(gameOverSound);
        }


        void draw(){
            food.draw();
            snake.draw();
        }

        void update(){
            if (run == true){
                snake.update();
                CheckCollisionWithFood();
                CheckCollisionWithWalls();
                CheckCollisionWithItself();
            }
        }
        void CheckCollisionWithFood(){
            if (Vector2Equals (snake.body[0], food.position)){
                food.position = food.randomPosition(snake.body);
                snake.add = true;
                score += 100;
                if (score >= highScore){
                    highScore = score;
                }
                PlaySound(eat);
            }
        }

        void CheckCollisionWithWalls(){
            if (snake.body[0].x == cellcount_x || snake.body[0].x == -1){
                GameOver();
            }
            if (snake.body[0].y == cellcount_y || snake.body[0].y == -1){
                GameOver();
            }
        }

        void CheckCollisionWithItself(){
            deque <Vector2> temp_Body = snake.body;
            temp_Body.pop_front();

            if (elementPresent(snake.body[0], temp_Body)){
                GameOver();
            }
        }

        void keyPressed(){
            run = true;
            gamePause = false;
            ResumeSound(background); // only resume if a sound is paused
        }

        void GameOver(){
            StopSound(background);
            PlaySound(gameOverSound);
            WaitTime(1.75);
            snake.reset();
            food.position = food.randomPosition(snake.body);
            run = false;
            score = 0;
        }
   
};

int main (){

    // cout << "Starting the game..." << endl;

    InitWindow(2*width_window + screenWidth, 2*width_window + screenHeight, "HungrySnake Harvest"); // {width, height, name(any)}

    Image image = LoadImage ("src/icon/snake.png");
    SetWindowIcon(image);

    SetTargetFPS(60);

    MainGame game = MainGame();
    
    while (WindowShouldClose() == false){    
        
        BeginDrawing();
        
        ClearBackground(bgColour);

        DrawText("HungrySnake Harvest", width_window - 5, 8 /*Height (Y)*/, 30, BLACK);
        
        DrawText("Pause: Enter_Key", screenWidth/2 + 50, 15, 22, BLACK);
        DrawText("Exit: Esc_Key", screenWidth/2 + 300, 15, 22, BLACK);

        DrawText(TextFormat("Score : %i", game.score), width_window-5, width_window + screenHeight + 10, 30, BLACK);
        DrawText(TextFormat("High Score : %i", game.highScore), width_window + 300, width_window + screenHeight + 15, 25, BLACK);

        DrawText("By Utkarsh Dikshit", 2*width_window + screenWidth - 150, 2*width_window + screenHeight - 20, 12, RED);

        DrawRectangleLinesEx(Rectangle{(float)width_window - 5, (float)width_window - 5, (float)screenWidth + 10, (float)screenHeight + 10}, 5, BLACK);

        game.draw();

        if (IsSoundPlaying(game.background) == false && game.run == true && !gamePause){
            PlaySound(game.background);
        }

        if (UpdateTime(0.1) && !gamePause){
            game.update();
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)){
            gamePause = !gamePause;
            if (gamePause){
                PauseSound(game.background);
            }
            if (!gamePause){
                ResumeSound(game.background);
            }
        }
        if (gamePause){
            DrawText("P A U S E D", screenWidth/2 - 50, screenHeight/2, 30, GREEN);
        }

        if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && game.snake.dir.y != 1){
            game.snake.dir = {0, -1};
            game.keyPressed();
        }
        if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && game.snake.dir.y != -1){
            game.snake.dir = {0, 1};
            game.keyPressed();
        }
        if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && game.snake.dir.x != 1){
            game.snake.dir = {-1, 0};
            game.keyPressed();
        }
        if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && game.snake.dir.x != -1){
            game.snake.dir = {1, 0};
            game.keyPressed();
        }
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}