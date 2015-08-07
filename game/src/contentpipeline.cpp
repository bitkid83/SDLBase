#include "contentpipeline.h"

#include <iostream>


Texture::Texture(SDL_Renderer *ren)
{ 
	mRenderer = ren;
}

Texture::~Texture() { }


bool Texture::LoadTexture(const char *filename)
{
	FreeTexture();

	SDL_Texture *tmptex;
	SDL_Surface *loadedImage = IMG_Load(filename);

	if (loadedImage == nullptr) {
		std::cout << "Unable to load image: " << filename << ".\nSDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	else {
		tmptex = SDL_CreateTextureFromSurface(mRenderer, loadedImage);
		if (tmptex == nullptr)	{
			std::cout << "Unable to create texture from loaded image: " << filename << ".\nSDL_Error: " << SDL_GetError() << std::endl;
			return false;
		}
		else {
			mWidth = loadedImage->w;
			mHeight = loadedImage->h;
		}

		SDL_FreeSurface(loadedImage);
	}
	mTexture = tmptex;
	return mTexture != nullptr;
}

void Texture::FreeTexture()
{
	if (mTexture != nullptr) {
		SDL_DestroyTexture(mTexture);
		mTexture = nullptr;
		mWidth = 0;
		mHeight = 0;
	}
}

void Texture::RenderTexture(int x, int y)
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	SDL_RenderCopy(mRenderer, mTexture, nullptr, &renderQuad);
}

int Texture::getWidth()
{
	return mWidth;
}

int Texture::getHeight()
{
	return mHeight;
}

void SpriteSheet::RenderTexture(int x, int y)
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	if (mClips != nullptr) {
		renderQuad.w = mClips->w;
		renderQuad.h = mClips->h;
		SDL_RenderCopy(mRenderer, mTexture, &mClips[mCurrentFrame], &renderQuad);
	}
	else {
		std::cout << "mClips is NULL!\n";
		return;
	}

	mCurrentFrame++;
	if (mCurrentFrame == mFrameCount) {
		mCurrentFrame = 0;
	}
}

SDL_Rect *SpriteSheet::SetClips(const int framecount)
{
	//Store the number of animation frames for this object
	mFrameCount = framecount;

	//mClips is an array of SDL_Rects, to which we store each frame from the loaded sprite sheet
	mClips = new SDL_Rect[framecount];

	//The size of each individual frame from the sprite sheet
	int clipWidth = mWidth / framecount;

	int tmp = 0;

	//Iterate through a horizontal sprite sheet and store each frame into the array of SDL_Rects
	for (int i = 0; i < framecount; i++) {
		mClips[i].x = tmp;
		mClips[i].y = 0;
		mClips[i].w = clipWidth;
		mClips[i].h = mHeight;
		tmp += clipWidth;
	}

	//need a way to produce the clipping rectangles algorithmically
	/*mClips[0].x = 0;
	mClips[0].y = 0;
	mClips[0].w = 32;
	mClips[0].h = 30;

	mClips[1].x = 32;
	mClips[1].y = 0;
	mClips[1].w = 32;
	mClips[1].h = 30;

	mClips[2].x = 64;
	mClips[2].y = 0;
	mClips[2].w = 32;
	mClips[2].h = 30;

	mClips[3].x = 96;
	mClips[3].y = 0;
	mClips[3].w = 32;
	mClips[3].h = 30;

	mClips[4].x = 128;
	mClips[4].y = 0;
	mClips[4].w = 32;
	mClips[4].h = 30;*/

	return mClips;
}