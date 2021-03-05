#include "PickingShader.hpp"
#include "../Utils.hpp"

static constexpr const char* pVS = R"(
#version 330                                                                        
                                                                                    
layout (location = 0) in vec3 Position;                                             
                                                                                    
uniform mat4 gWVP;                                                                  
                                                                                    
void main()                                                                         
{                                                                                   
    gl_Position = gWVP * vec4(Position, 1.0);                                       
}
)";

static constexpr const char* pFS = R"(
#version 330                                                                        
                                                                                                                                                                                                                                                        
uniform uint gDrawIndex;                                                            
uniform uint gObjectIndex;                                                          
                                                                                    
out vec3 FragColor;

void main()                                                                         
{                                                                                   
   FragColor = vec3(float(gObjectIndex), float(gDrawIndex),float(gl_PrimitiveID + 1));                 
}
)";

bool PickingShader::init() {
	if (!Technique::init())
		return false;

	if (!addShader(GL_VERTEX_SHADER, pVS))
		return false;

	if (!addShader(GL_FRAGMENT_SHADER, pFS))
		return false;

	if (!finalize())
		return false;

	wvpLocation_ = getUniformLocation("gWVP");
	objectIndexLocation_ = getUniformLocation("gObjectIndex");
	drawIndexLocation_ = getUniformLocation("gDrawIndex");

	if (INVALID_UNIFORM_LOCATION == wvpLocation_ || INVALID_UNIFORM_LOCATION == objectIndexLocation_ || INVALID_UNIFORM_LOCATION == drawIndexLocation_)
		return false;

	return true;
}

void PickingShader::setWVP(const Matrix4f& wvp) {
	glUniformMatrix4fv(wvpLocation_, 1, GL_TRUE, (const GLfloat*)wvp.m);
}

void PickingShader::setObjectIndex(unsigned objIdx) {
	glUniform1ui(objectIndexLocation_, objIdx);
}

void PickingShader::drawStartedCB(unsigned drawIndex) {
	glUniform1ui(drawIndexLocation_, drawIndex);
}