#pragma once
#include "unit.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

//typedef struct {
//	Vec2i position;
//	Vec2i attachPosition;
//	Vec2i size;
//	SDL_Texture *texture;
//	Color bgColor;
//} UIBox;
//
//#define UI_TEXT_INPUT_MAX 32
//typedef struct {
//	char text[UI_TEXT_INPUT_MAX];
//	u8 textLen;
//
//	Vec2i position;
//	Vec2i attachPosition;
//	Vec2i size;
//	SDL_Texture *texture;
//	Color color;
//	Color bgColor;
//	u8 fontSize;
//} UITextInput;

typedef enum {
	UI_BUTTON,
	UI_BOX,
	UI_LABEL,
	UI_TEXT_INPUT
} UIType;

typedef struct {
	Color bgColor;
	Color borderColor;

	// function pointer
	void (*onClick)(void *state);
} UIButton;

typedef struct {
	Color bgColor;
	Color borderColor;
} UIBox;

typedef struct {
	Color color;
	Color bgColor;
	Color borderColor;
} UILabel;

typedef struct {
	Color color;
	Color bgColor;
	Color borderColor;

	void (*onChange)(void *state);
} UITextInput;

typedef struct {
	u32 parentID;
	UIType type;

	Vec2i position;
	Vec2i attachPosition;
	Vec2i size;

	SDL_Texture *texture;

	union {
		UIButton button;
		UIBox box;
		UILabel label;
		UITextInput textInput;
	} data;
} UIElement;

typedef struct {
	UIElement *array;
	u32 len;
	u32 size;
} UI;

#define NUM_TEXT_INPUTS 2
typedef enum {
	textInputPositionX,
	textInputPositionY,
	textInputNone
} TextInputID;

void initUI(UI *ui, u16 size); // UI, prealloc size
u16 newUIElement(UI *ui);

SDL_Texture *newTextTexture(SDL_Renderer *renderer, char *text, TTF_Font *font, Color color);
UITextInput newUITextInput();

UIBox newBox(Vec2i pos, Vec2i size, SDL_Texture *texture, Color bgColor);
void setUITextInputText(SDL_Renderer *renderer, TTF_Font *font, UITextInput *textInput, char *str);
