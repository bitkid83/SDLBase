#ifndef _CONTENT_PIPELINE_H
#define _CONTENT_PIPELINE_H

#include "SDL.h"
#include "SDL_image.h"

class Texture
{
public:
	Texture();
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
	SpriteSheet(SDL_Renderer *ren, int fps_scale);
	SDL_Rect *SetClips(const int framecount);
	SDL_Rect *mClips;
	void RenderTexture(int x, int y, double angle, SDL_RendererFlip flip, bool frame_step);
	//void RenderTextureEx(int x, int y, double angle, int flip);

private:
	int mCurrentFrame = 0;
	int mRenderFrame;
	int mFrameRateScale;
	int mFrameCount;
};

#endif