#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <memory>
#include <vector>
#include "idk.hpp"

// Function to check for collision between two objects
bool checkCollision(const GameObject& obj1, const GameObject& obj2) {
    return (obj1.getX() < obj2.getX() + obj2.getWidth() &&
        obj1.getX() + obj1.getWidth() > obj2.getX() &&
        obj1.getY() < obj2.getY() + obj2.getHeight() &&
        obj1.getY() + obj1.getHeight() > obj2.getY());
}

void showStartingPage(SDL_Renderer* renderer, int& numBalls) {
    bool selecting = true;
    SDL_Event event;

    SDL_Surface* imageSurface = IMG_Load("start.bmp");
    if (!imageSurface) {

        SDL_Log("Failed to load image: %s", IMG_GetError());
        return;
    }

    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    if (!imageTexture) {

        SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
        SDL_FreeSurface(imageSurface);
        return;
    }

    while (selecting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                exit(0);
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_1) {
                    numBalls = 1;
                    selecting = false;
                }
                else if (event.key.keysym.sym == SDLK_2) {
                    numBalls = 2;
                    selecting = false;
                }
                else if (event.key.keysym.sym == SDLK_3) {
                    numBalls = 3;
                    selecting = false;
                }
                else if (event.key.keysym.sym == SDLK_4) {
                    numBalls = 4;
                    selecting = false;
                }
                else if (event.key.keysym.sym == SDLK_5) {
                    numBalls = 5;
                    selecting = false;
                }
            }
            SDL_Delay(10);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render the image on the starting screen
        SDL_RenderCopy(renderer, imageTexture, nullptr, nullptr);

        // Present the renderer
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(imageTexture);
}


int main(int argc, char* argv[]) {
    int player1 = 0, player2 = 0;
    int numBalls = 1;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    showStartingPage(renderer, numBalls);

    std::unique_ptr<Paddle> leftPaddle = std::make_unique<Paddle>(0, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2);
    std::unique_ptr<Paddle> rightPaddle = std::make_unique<Paddle>(SCREEN_WIDTH - PADDLE_WIDTH, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2);

    std::vector<std::unique_ptr<Ball>> balls;
    balls.push_back(std::make_unique<Ball>(SCREEN_WIDTH / 2 - BALL_WIDTH / 2, SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2));

    for (int i = 1; i < numBalls; ++i) {
        int ballType = rand() % 3;

        if (ballType == 0) {
            balls.push_back(std::make_unique<Ball>(SCREEN_WIDTH / 2 - BALL_WIDTH / 2, SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2));
        }
        else if(ballType == 1) {
            balls.push_back(std::make_unique<FastBall>(SCREEN_WIDTH / 2 - BALL_WIDTH / 2, SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2));
        }
        else {
            balls.push_back(std::make_unique<Horizontal_Ball>(SCREEN_WIDTH / 2 - BALL_WIDTH / 2, SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2));
        }
    }

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_Q]) {
            leftPaddle->setYVelocity(-5);
        }
        else if (keystates[SDL_SCANCODE_A]) {
            leftPaddle->setYVelocity(5);
        }
        else {
            leftPaddle->setYVelocity(0);
        }

        if (keystates[SDL_SCANCODE_UP]) {
            rightPaddle->setYVelocity(-5);
        }
        else if (keystates[SDL_SCANCODE_DOWN]) {
            rightPaddle->setYVelocity(5);
        }
        else {
            rightPaddle->setYVelocity(0);
        }

        leftPaddle->update();
        rightPaddle->update();

        // Check for collision between ball and paddles
        for (const auto& ball : balls) {
            ball->update();

            // Check for collision between ball and paddles
            if (checkCollision(*ball, *leftPaddle) || checkCollision(*ball, *rightPaddle)) {
                ball->reverseVelocity();
            }

            // Check for scoring
            if (ball->getX() < 0) {
                ball->reset();
                player1++;
                std::cout << player1 << "-" << player2<<"\n";
            }
            if (ball->getX() > SCREEN_WIDTH) {
                ball->reset();
                player2++;
                std::cout << player1 << "-" << player2<<"\n";
            }
        }

        if (leftPaddle->getY() < 0) {
            leftPaddle->stop();
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
        for (const auto& ball : balls) {
            ball->draw(renderer);
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}