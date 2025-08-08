#pragma once
#include "structs.h"
#include <iostream>

#pragma region Instructions

// To add new color just add it in the enum list
// Then add it in the same order in Colors[]
// If you add a color between red and green, it has to be between red and green in the Array too

#pragma endregion

namespace PrettyColors
{
	enum ColorsList
	{
	// Basic
		white,
		black,
		red,
		green,
		blue,
	// Additional
		lime,
		yellow,
		orange,
		rose,
		pink,
		purple,
		cobalt,
		cyan,
		brown,
		gray,
	// Dark
		dRed,
		dGreen,
		dBlue,
	// Light
		lRed,
		lGreen,
		lBlue,
		AllColors
	};

	static const Color4f Colors[int(ColorsList::AllColors)]{
		// Basic
	Color4f{1.0f,1.0f,1.0f,1.0f},	// white
	Color4f{0.0f,0.0f,0.0f,1.0f},	// black
	Color4f{0.80f,0.1f,0.1f,1.0f},	// red
	Color4f{0.1f,0.85f,0.1f,1.0f},	// green	
	Color4f{0.0f,0.2f,0.9f,1.0f},	// blue
		// Additional
	Color4f{0.55f,0.85f,0.25f,1.0f},// lime
	Color4f{0.95f,0.85f,0.0f,1.0f},	// yellow
	Color4f{1.0f,0.5f,0.0f,1.0f},	// orange
	Color4f{1.0f,0.7f,0.75f,1.0f},	// rose
	Color4f{1.0f,0.0f,1.0f,1.0f},	// pink
	Color4f{0.55f,0.0f,0.9f,1.0f},	// purple
	Color4f{0.0f,0.3f,0.85f,1.0f},	// cobalt
	Color4f{0.0f,0.8f,0.9f,1.0f},	// cyan
	Color4f{0.55f,0.3f,0.1f,1.0f},	// brown
	Color4f{0.3f,0.3f,0.3f,1.0f},	// gray
		// Dark
	Color4f{0.40f,0.1f,0.1f,1.0f},	// dRed
	Color4f{0.1f,0.5f,0.1f,1.0f},	// dGreen
	Color4f{0.1f,0.1f,0.5f,1.0f},	// dBlue							 
		// Light					 
	Color4f{0.9f,0.1f,0.1f,1.0f},	// lRed
	Color4f{0.0f,0.85f,0.1f,1.0f},	// lGreen
	Color4f{0.0f,0.5f,0.9f,1.0f},	// lBlue						 
	};
		
	// Returns white if name is out of bounds
	static const Color4f GetColor(ColorsList name)
	{
		if (name < 0 || name > int(ColorsList::AllColors))
		{
			return Colors[white];
		}
		return Colors[name];
	}

	static const int GetColorsAmount()
	{
		return int(ColorsList::AllColors);
	}

};