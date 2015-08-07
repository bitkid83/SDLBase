#ifndef _CONTENT_PIPELINE_H
#define _CONTENT_PIPELINE_H

#include "SDL.h"
#include "SDL_image.h"

class Texture
{
public:
	Texture(SDL_Renderer *ren);
	~Texture();

	bool LoadTexture(const char *filename);
	void FreeTexture();
	virtual void RenderTexture(int x, int y);

	int getWidth();
	int getHeight();	

	int posx, posy;

protected:
	SDL_Renderer *mRenderer;
	SDL_Texture *mTexture;
	int mWidth, mHeight;
};

class SpriteSheet : public Texture
{
public:
	SDL_Rect *SetClips(const int framecount);
	SDL_Rect *mClips;
	void RenderTexture(int x, int y);

private:
	int mCurrentFrame = 0;
	int mFrameCount;
};

#endif