#include "ofStolenUtils.h"
#include <string.h>

int ofToInt(const std::string &intString) {
	int x = 0;
	std::istringstream cur(intString);
	cur >> x;
	return x;
}

//----------------------------------------
float ofToFloat(const std::string &floatString) {
	float x = 0;
	std::istringstream cur(floatString);
	cur >> x;
	return x;
}

float ofClamp(float value, float min, float max) {
	// Or use glm::clamp...
	return value < min ? min : value > max ? max : value;
}

//check for division by zero???
//--------------------------------------------------
float ofMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp) {
	if (fabs(inputMin - inputMax) < FLT_EPSILON) {
		return outputMin;
	}
	else {
		float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);

		if (clamp) {
			if (outputMax < outputMin) {
				if (outVal < outputMax)outVal = outputMax;
				else if (outVal > outputMin)outVal = outputMin;
			}
			else {
				if (outVal > outputMax)outVal = outputMax;
				else if (outVal < outputMin)outVal = outputMin;
			}
		}
		return outVal;
	}
}
