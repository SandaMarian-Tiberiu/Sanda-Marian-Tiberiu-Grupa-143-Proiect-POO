#include <SDL.h>
#include <iostream>
#include <memory>

// Constants for screen dimensions and paddle/ball properties
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int PADDLE_WIDTH = 10;
const int PADDLE_HEIGHT = 80;
const int BALL_WIDTH = 10;
const int BALL_HEIGHT = 10;
const int BALL_SPEED = 1;

// Base class for game objects (paddles and ball)
class GameObject {
public:
    GameObject(int xPos, int yPos, int width, int height, int velocity) :
        x(xPos), y(yPos), width(width), height(height), velocity(velocity) {}
    virtual ~GameObject() {}

    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    void incY() { y = y + 5; }
    void decY() { y = y - 5; }
    

    virtual void update() = 0;
    virtual void draw(SDL_Renderer* renderer) const = 0;

protected:
    int x, y;
    int width, height;
    int velocity;
};

// Class for player paddles
class Paddle : public GameObject {
public:
    Paddle(int xPos, int yPos) :
        GameObject(xPos, yPos, PADDLE_WIDTH, PADDLE_HEIGHT, 0) {}

    void update() override {
        
    }

    void draw(SDL_Renderer* renderer) const override {
        SDL_Rect rect = { x, y, width, height };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    void stop() { y = 0; }
    void replace() { y = SCREEN_HEIGHT - height; }
};

// Class for ball
class Ball : public GameObject {
public:
    Ball(int xPos, int yPos) :
        GameObject(xPos, yPos, BALL_WIDTH, BALL_HEIGHT, BALL_SPEED) {}

    void update() override {
        x += velocity;

        if (direction == 1) {
            y += velocity;
        }
        else {
            y -= velocity;
        }

        if (y < 0 || y + height > SCREEN_HEIGHT) {
            direction = direction * -1;
        }
    }

    void draw(SDL_Renderer* renderer) const override {
        SDL_Rect rect = { x, y, width, height };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    void reset() {
        x = SCREEN_WIDTH / 2 - BALL_WIDTH / 2;
        y = SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2;
        velocity = BALL_SPEED;
        direction = rand() % 2 == 0 ? -1 : 1;
    }

    void reverseVelocity() {
        velocity = -velocity;
        velocity += rand() % 5 - 1; // Add -1, 0, or 3 to the ball's velocity
        direction = rand() % 2 == 0 ? -1 : 1;
    }

private:
    int direction = rand() % 2 == 0 ? -1 : 1;
};

// Function to check for collision between two objects
bool checkCollision(const GameObject& obj1, const GameObject& obj2) {
    return (obj1.getX() < obj2.getX() + obj2.getWidth() && obj1.getX() + obj1.getWidth() > obj2.getX() && obj1.getY() < obj2.getY() + obj2.getHeight() && obj1.getY() + obj1.getHeight() > obj2.getY());
}


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    std::unique_ptr<Paddle> leftPaddle = std::make_unique<Paddle>(0, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2);
    std::unique_ptr<Paddle> rightPaddle = std::make_unique<Paddle>(SCREEN_WIDTH - PADDLE_WIDTH, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2);
    std::unique_ptr<Ball> ball = std::make_unique<Ball>(SCREEN_WIDTH / 2 - BALL_WIDTH / 2, SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_W]) {
            leftPaddle->decY();
        }
        if (keystates[SDL_SCANCODE_S]) {
            leftPaddle->incY();
        }
        if (keystates[SDL_SCANCODE_UP]) {
            rightPaddle->decY();
        }
        if (keystates[SDL_SCANCODE_DOWN]) {
            rightPaddle->incY();
        }

        ball->update();

        // Check for collision between ball and paddles
        if (checkCollision(*ball, *leftPaddle) || checkCollision(*ball, *rightPaddle)) {
            ball->reverseVelocity();
        }

        // Check for scoring
        if (ball->getX() < 0) {
            ball->reset();
        }
        if (ball->getX() > SCREEN_WIDTH) {
            ball->reset();
        }

        if (leftPaddle->getY() < 0) {
            leftPaddle -> stop();
        }
        else if (leftPaddle->getY() > SCREEN_HEIGHT - leftPaddle->getHeight()) {
            leftPaddle->replace();
        }

        if (rightPaddle->getY() < 0) {
            rightPaddle->stop();
        }
        else if (rightPaddle->getY() > SCREEN_HEIGHT - rightPaddle->getHeight()) {
            rightPaddle->replace();
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        leftPaddle->draw(renderer);
        rightPaddle->draw(renderer);
        ball->draw(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
