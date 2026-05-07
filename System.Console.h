#pragma once
#include <stdio.h>
#include "System.String.h"

static void Console_Write(String* Input)
{
	if(!Input || !Input->data)return;
	printf("%s", Input->data);
}

static void Console_WriteLine(String* Input)
{
	if(!Input || !Input->data)return;
	printf("%s\n", Input->data);
}

enum FontColor
{
	Blue,
	Yellow,
	Red,
	Green,
	Cayan,
	White,
	Black
};

struct console_statc_actions
{
	void        (*Write)(String* Input);
	void		(*WriteLine)(String* Input);
	void		(*Clear)();
	void		(*SetFontColor)(enum FontColor color);
};

static struct console_statc_actions Console = {
    .Write = Console_Write,
	.WriteLine = Console_WriteLine
};










