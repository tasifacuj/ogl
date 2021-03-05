#pragma once

class RenderCallbackInterface {
public:
	virtual ~RenderCallbackInterface(){}
	virtual void drawStartedCB(unsigned drawIndex) = 0;
};