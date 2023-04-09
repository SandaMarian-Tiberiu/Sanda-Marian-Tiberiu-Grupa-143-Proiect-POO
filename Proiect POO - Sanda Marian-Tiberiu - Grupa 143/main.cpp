#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


class LTexture													//encapsulates an SDL texture object, and provides a number of methods for loading, rendering, and managing textures.
{
public:
	
	LTexture();													//Constructor

	~LTexture();												//Destructor

	bool loadFromFile(std::string path);						//Loads image at specified path

	void free();												//Deallocates texture

	void setColor(Uint8 red, Uint8 green, Uint8 blue);			//Sets color modulation

	void setBlendMode(SDL_BlendMode blending);					//Sets blending ??

	void setAlpha(Uint8 alpha);									//Sets alpha modulation

	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);						//Renders texture at given point

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};


class Dot														//The dot that will move around on the screen
{
public:
	
	static const int DOT_WIDTH = 20;							//Dimensions
	static const int DOT_HEIGHT = 20;

	static const int DOT_VEL = 10;								//Maximum axis velocity

	
	Dot();														//Constructor

	
	void handleEvent(SDL_Event& e);								//Takes key presses and adjusts the dot's velocity

	void move(SDL_Rect& wall);									//Moves the dot and checks collision

	void render();												//Shows the dot on the screen

private:
	
	int mPosX, mPosY;											//The X and Y offsets of the dot
	
	int mVelX, mVelY;											//The velocity of the dot

	SDL_Rect mCollider;											//Collision
};


bool checkCollision(SDL_Rect a, SDL_Rect b);					//Box collision detector

SDL_Renderer* gRenderer = NULL;									//The window renderer


LTexture gDotTexture;											//Scene textures

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	free();														//Get rid of preexisting texture

	SDL_Texture* newTexture = NULL;								//The final texture

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());			//Load image at specified path
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));						//Color key image

		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);					//Create texture from surface pixels
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		
		SDL_FreeSurface(loadedSurface);									//Get rid of old loaded surface
	}

	//Return success ??
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free()													//Free texture if it exists
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_SetTextureColorMod(mTexture, red, green, blue);						//Modulate texture rgb
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(mTexture, blending);							//Set blending function ??
}

void LTexture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(mTexture, alpha);								//Modulate texture alpha
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };						//Set rendering space and render to screen

	if (clip != NULL)														//Set clip rendering dimensions ??
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);								//Render to screen
}

int LTexture::getWidth()											//Getter
{
	return mWidth;
}

int LTexture::getHeight()											//Getter
{
	return mHeight;
}

Dot::Dot()
{
	//Initialize the offsets
	mPosX = 0;
	mPosY = 0;

	//Set collision box dimension
	mCollider.w = DOT_WIDTH;
	mCollider.h = DOT_HEIGHT;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;
}

void Dot::handleEvent(SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)						//If a key was pressed
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY -= DOT_VEL; break;
		case SDLK_DOWN: mVelY += DOT_VEL; break;
		case SDLK_LEFT: mVelX -= DOT_VEL; break;
		case SDLK_RIGHT: mVelX += DOT_VEL; break;
		}
	}
	
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)					//If a key was released
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY += DOT_VEL; break;
		case SDLK_DOWN: mVelY -= DOT_VEL; break;
		case SDLK_LEFT: mVelX += DOT_VEL; break;
		case SDLK_RIGHT: mVelX -= DOT_VEL; break;
		}
	}
}

void Dot::move(SDL_Rect& wall)
{
	//Move the dot left or right
	mPosX += mVelX;
	mCollider.x = mPosX;

	if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH) || checkCollision(mCollider, wall))								//If the dot collided or went too far to the left or right
	{
		//Move back
		mPosX -= mVelX;
		mCollider.x = mPosX;
	}

	//Move the dot up or down
	mPosY += mVelY;
	mCollider.y = mPosY;

	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT) || checkCollision(mCollider, wall))								//If the dot collided or went too far up or down
	{
		//Move back
		mPosY -= mVelY;
		mCollider.y = mPosY;
	}
}
void Dot::render()
{
	gDotTexture.render(mPosX, mPosY);							//Show the dot
}


class alphabeta {
public:

	alphabeta()						//Constructor
	{
		success = true;				//Initialization flag

		if (SDL_Init(SDL_INIT_VIDEO) < 0)						//Initialize SDL
		{
			printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))						//Set texture filtering to linear
			{
				printf("Warning: Linear texture filtering not enabled!");
			}

			gWindow = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);							//Create window
			if (gWindow == NULL)
			{
				printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);												//Create vsynced renderer for window
				if (gRenderer == NULL)
				{
					printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
					success = false;
				}
				else
				{
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);												//Initialize renderer color

					//Initialize PNG loading
					int imgFlags = IMG_INIT_PNG;
					if (!(IMG_Init(imgFlags) & imgFlags))
					{
						printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
						success = false;
					}
				}
			}
		}

	}

	~alphabeta()					//Destructor
	{
		gDotTexture.free();							//Free loaded images

		//Destroy window	
		SDL_DestroyRenderer(gRenderer);
		SDL_DestroyWindow(gWindow);
		gWindow = NULL;
		gRenderer = NULL;

		//Quit SDL subsystems
		IMG_Quit();
		SDL_Quit();
	}

	bool getsuccess() {
		return success;
	}

private:
	SDL_Window* gWindow = NULL;										//The window we'll be rendering to
	bool success;

};



class Media_loader {
public:
	Media_loader() {
		success = true;

		if (!gDotTexture.loadFromFile("Project files/dot.bmp"))
		{
			printf("Failed to load dot texture!\n");
			success = false;
		}
	}

	bool getsuccess() {
		return success;
	}

private:
	bool success;
};




bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	return true;					//If none of the sides from A are outside B
}

int main(int argc, char* args[])
{
	alphabeta game;
	Media_loader media;
	//Start up SDL and create window
	if (!game.getsuccess())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!media.getsuccess())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			bool quit = false;							//Main loop flag
			
			SDL_Event e;								//Event handler

			Dot dot;									//The dot that will be moving around on the screen

			//Set the wall
			SDL_Rect wall;
			wall.x = 300;
			wall.y = 40;
			wall.w = 40;
			wall.h = 400;

			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					//Handle input for the dot
					dot.handleEvent(e);
				}

				//Move the dot and check collision
				dot.move(wall);

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render wall
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
				SDL_RenderDrawRect(gRenderer, &wall);

				dot.render();				//Render dot

				SDL_RenderPresent(gRenderer);						//Update screen

				SDL_Delay(16);
			}
		}
	}
	

	return 0;
}