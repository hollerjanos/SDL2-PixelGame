// Using SDL, SDL_iamge, standard IO and strings
#include "SDL.h"
#include "SDL_image.h"
#include <stdio.h>
#include <string>

// Screen dimension constants
constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

// Starts up SDL and creates window
bool init();

// Loads media
bool loadMedia();

// Frees media and shuts down SDL
void close();

// Loads individual image
SDL_Surface *loadSurface(std::string path);

// Loads individual image as texture
SDL_Texture *loadTexture(std::string path);

// The window we'll be rendering to
SDL_Window *gWindow = NULL;

// The window renderer
SDL_Renderer *gRenderer = NULL;

// The surface contained by the window
SDL_Surface *gScreenSurface = NULL;

// Texture wrapper class
class LTexture
{
public:
    //Initializes variables
    LTexture()
    {
        this->mTexture = NULL;
        this->mWidth = 0;
        this->mHeight = 0;
    }

    // Deallocates memory
    ~LTexture()
    {
        // Deallocate
        free();
    }

    // Loads image at specified path
    bool loadFromFile(std::string path)
    {
        // Get rid of pre-existing texture
        free();

        // The final texture
        SDL_Texture *newTexture = NULL;

        // Load image at specified path
        SDL_Surface *loadedSurface = IMG_Load(path.c_str());
        if (loadedSurface == NULL)
        {
            printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        }
        else
        {
            // Colour key image
            SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

            // Create texture from surface pixels
            newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
            if (newTexture == NULL)
            {
                printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
            }
            else
            {
                // Get image dimensions
                this->mWidth = loadedSurface->w;
                this->mHeight = loadedSurface->h;
            }

            // Get rid of old loaded surface
            SDL_FreeSurface(loadedSurface);
        }

        // Return success
        this->mTexture = newTexture;
        return this->mTexture != NULL;
    }

    // Deallocates texture
    void free()
    {
        // Free texture if it exists
        if (this->mTexture != NULL)
        {
            SDL_DestroyTexture(this->mTexture);
            this->mTexture = NULL;
            this->mWidth = 0;
            this->mHeight = 0;
        }
    }

    // Set colour modulation
    void setColour(Uint8 red, Uint8 green, Uint8 blue)
    {
        SDL_SetTextureColorMod(this->mTexture, red, green, blue);
    }

    // Renders texture at given point
    void render(int x, int y, SDL_Rect *clip = NULL)
    {
        // Set rendering space and render to screen
        SDL_Rect renderQuad = { x, y, this->mWidth, this->mHeight };

        // Set clip rendering dimensions
        if (clip != NULL)
        {
            renderQuad.w = clip->w;
            renderQuad.h = clip->h;
        }

        // Render to screen
        SDL_RenderCopy(gRenderer, this->mTexture, clip, &renderQuad);
    }

    // Gets image dimensions
    int getWidth()
    {
        return this->mWidth;
    }
    int getHeight()
    {
        return this->mHeight;
    }

private:
    // The actual hardware texture
    SDL_Texture *mTexture;

    // Image dimensions
    int mWidth;
    int mHeight;
};

LTexture gModulatedTexture;

bool init()
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_ERROR: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        // Create window
        gWindow = SDL_CreateWindow(
            "SDL Tutorial",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
        );
        if (gWindow == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            // Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL)
            {
                printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                // Initialize renderer colour
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia()
{
    // Loading success flag
    bool success = true;

    // Load sprite sheet texture
    if (!gModulatedTexture.loadFromFile("modulation.png"))
    {
        printf("Failed to load sprite sheet texture image!\n");
        success = false;
    }

    return success;
}

void close()
{
    // Free loaded image
    gModulatedTexture.free();

    // Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

SDL_Surface *loadSurface(std::string path)
{
    // The final optimized image
    SDL_Surface *optimizedSurface = NULL;

    // Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    }
    else
    {
        // Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
        if (optimizedSurface == NULL)
        {
            printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    return optimizedSurface;
}

SDL_Texture *loadTexture(std::string path)
{
    // The final texture
    SDL_Texture *newTexture = NULL;

    // Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    }
    else
    {
        // Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

int main(int argc, char *argv[])
{
    // Start up SDL and create window
    if (!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        // Load media
        if (!loadMedia())
        {
            printf("Failed to load media!\n");
        }
        else
        {
            // Main loop flag
            bool quit = false;

            // Event handler
            SDL_Event event;

            // Modulation components
            Uint8 r = 255;
            Uint8 g = 255;
            Uint8 b = 255;

            // While application is running
            while (!quit)
            {
                // Handle events on queue
                while (SDL_PollEvent(&event) != 0)
                {
                    // User requests quit
                    if (event.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    // On keypress change RGB values
                    else if (event.type == SDL_KEYDOWN)
                    {
                        switch (event.key.keysym.sym)
                        {
                            // Increase red
                            case SDLK_q:
                                r += 32;
                                break;

                            // Increase green
                            case SDLK_w:
                                g += 32;
                                break;

                            // Increase blue
                            case SDLK_e:
                                b += 32;
                                break;

                            // Decrease red
                            case SDLK_a:
                                r -= 32;
                                break;

                            // Decrease green
                            case SDLK_s:
                                g -= 32;
                                break;

                            // Decrease blue
                            case SDLK_d:
                                b -= 32;
                                break;
                        }
                    }
                }

                // Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                // Modulate and render texture
                gModulatedTexture.setColour(r, g, b);
                gModulatedTexture.render(0, 0);

                // Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }

    // Free resources and close SDL
    close();

    return 0;
}