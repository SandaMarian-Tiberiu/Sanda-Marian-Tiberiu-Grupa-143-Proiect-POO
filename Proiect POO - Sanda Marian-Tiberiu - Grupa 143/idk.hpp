
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
    GameObject(int xPos, int yPos, int width, int height, int xVelocity, int yVelocity) :
        x(xPos), y(yPos), width(width), height(height), x_velocity(xVelocity), y_velocity(yVelocity) {}

    virtual ~GameObject() {}

    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getXVelocity() const { return x_velocity; }
    int getYVelocity() const { return y_velocity; }

    void setXVelocity(int velocity) { x_velocity = velocity; }
    void setYVelocity(int velocity) { y_velocity = velocity; }

    virtual void update() = 0;
    virtual void draw(SDL_Renderer* renderer) const = 0;

protected:
    int x, y;
    int width, height;
    int x_velocity, y_velocity;

};

// Class for player paddles
class Paddle : public GameObject {
public:
    Paddle(int xPos, int yPos) :
        GameObject(xPos, yPos, PADDLE_WIDTH, PADDLE_HEIGHT, 0, 0) {}

    void update() override {
        y += y_velocity;
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
        GameObject(xPos, yPos, BALL_WIDTH, BALL_HEIGHT, BALL_SPEED, BALL_SPEED) {
        setXVelocity(x_velocity * ((rand() % 2 == 0) ? 1 : -1));
        setYVelocity(y_velocity * ((rand() % 2 == 0) ? 1 : -1));
    }

    void update() override {
        
        x += x_velocity;
        y += y_velocity;

        if (y < 0 || y + height > SCREEN_HEIGHT) {
            y_velocity = -y_velocity;
        }
    }

    void draw(SDL_Renderer* renderer) const override {
        SDL_Rect rect = { x, y, width, height };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    virtual void reset() {
        x = SCREEN_WIDTH / 2 - BALL_WIDTH / 2;
        y = SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2;
        x_velocity = BALL_SPEED*((rand() % 2 == 0) ? 1 : -1);
        y_velocity = BALL_SPEED*((rand() % 2 == 0) ? 1 : -1);
    }

    virtual void reverseVelocity() {

        if (x_velocity < 0) {
            x_velocity = -(x_velocity - rand() % 3);
        }
        else { 
        x_velocity = -(x_velocity + rand() % 3); 
        }

        y_velocity = rand()%6-3;
        if (y_velocity == 0) { y_velocity = 1; }
    }
};

class FastBall : public Ball {
public:
    FastBall(int xPos, int yPos) :
        Ball(xPos, yPos) {
        setXVelocity(x_velocity * ((rand() % 2 == 0) ? 1 : -1));
        setYVelocity(y_velocity * ((rand() % 2 == 0) ? 1 : -1));
    }

    void update() override {
        x += x_velocity*2;
        y += y_velocity;

        if (y < 0 || y + height > SCREEN_HEIGHT) {
            y_velocity = -y_velocity;
        }
    }

    void draw(SDL_Renderer* renderer) const override {
        
        SDL_Rect rect = { x, y, width, height };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    void reset() override {
        x = SCREEN_WIDTH / 2 - BALL_WIDTH / 2;
        y = SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2;
        x_velocity = BALL_SPEED * ((rand() % 2 == 0) ? 1 : -1);
        y_velocity = BALL_SPEED * ((rand() % 2 == 0) ? 1 : -1);
    }

    void reverseVelocity() override {
        if (x_velocity < 0) {
            x_velocity = -(x_velocity - rand() % 3);
        }
        else {
            x_velocity = -(x_velocity + rand() % 3);
        }

        y_velocity = rand() % 6 - 3;
        if (y_velocity == 0) { y_velocity = 1; }
    }
};

class Horizontal_Ball : public Ball {
public:
    Horizontal_Ball(int xPos, int yPos) :
        Ball(xPos, yPos) {
        setXVelocity(x_velocity * 2 * ((rand() % 2 == 0) ? 1 : -1));
    }

    void update() override {
        x += x_velocity;
        

        if (y < 0 || y + height > SCREEN_HEIGHT) {
            y_velocity = -y_velocity;
        }
    }

    void draw(SDL_Renderer* renderer) const override {

        SDL_Rect rect = { x, y, width, height };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    void reset() override {
        x = SCREEN_WIDTH / 2 - BALL_WIDTH / 2;
        y = SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2;
        x_velocity = BALL_SPEED*2 * ((rand() % 2 == 0) ? 1 : -1);

    }

    void reverseVelocity() override {
        if (x_velocity < 0) {
            x_velocity = -(x_velocity - rand() % 3);
        }
        else {
            x_velocity = -(x_velocity + rand() % 3);
        }

    }
};