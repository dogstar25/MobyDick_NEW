#pragma once
#include "../game.h"

namespace ImGui
{
	namespace settings {


	}


	void MobyDickInit(Game*);
	void MobyDickNewFrame();
	void MobyDickRenderFrame();
	void MobyDickFPSFrame();
	void setFont64();
	void textCentered(std::string text);
	void setCursorToCentered(float itemToCenterSize);

}
