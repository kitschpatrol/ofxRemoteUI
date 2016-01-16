//
//  RemoteParam.h
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 24/06/13.
//
//

#ifndef emptyExample_RemoteParam_h
#define emptyExample_RemoteParam_h

#ifdef CINDER_AVAILABLE
#include "cinder/Cinder.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"
#endif

// Logging Wrappers
#ifdef OF_AVAILABLE
#define RLOG_NOTICE (ofLogNotice("ofxRemoteUI"))
#define RLOG_ERROR (ofLogError("ofxRemoteUI"))
#define RLOG_WARNING (ofLogWarning("ofxRemoteUI"))
#define RLOG_VERBOSE (ofLogVerbose("ofxRemoteUI"))
#elif defined(CINDER_AVAILABLE)
// First, re-wrap cinder logging macros to take stream input
#define CI_STREAM_LOG_V (::cinder::log::Entry(ci::log::LEVEL_VERBOSE, ::cinder::log::Location(CINDER_CURRENT_FUNCTION, __FILE__, __LINE__)))
#define CI_STREAM_LOG_D (::cinder::log::Entry(ci::log::LEVEL_DEBUG, ::cinder::log::Location(CINDER_CURRENT_FUNCTION, __FILE__, __LINE__)))
#define CI_STREAM_LOG_I (::cinder::log::Entry(ci::log::LEVEL_INFO, ::cinder::log::Location(CINDER_CURRENT_FUNCTION, __FILE__, __LINE__)))
#define CI_STREAM_LOG_W (::cinder::log::Entry(ci::log::LEVEL_WARNING, ::cinder::log::Location(CINDER_CURRENT_FUNCTION, __FILE__, __LINE__)))
#define CI_STREAM_LOG_E (::cinder::log::Entry(ci::log::LEVEL_ERROR, ::cinder::log::Location(CINDER_CURRENT_FUNCTION, __FILE__, __LINE__)))
#define CI_STREAM_LOG_F (::cinder::log::Entry(ci::log::LEVEL_FATAL, ::cinder::log::Location(CINDER_CURRENT_FUNCTION, __FILE__, __LINE__)))

#define RLOG_NOTICE (CI_STREAM_LOG_I)
#define RLOG_ERROR (CI_STREAM_LOG_E)
#define RLOG_WARNING (CI_STREAM_LOG_W)
#define RLOG_VERBOSE (CI_STREAM_LOG_V)
#else
// TODO!
#define RLOG_NOTICE (cout << endl)
#define RLOG_ERROR (cout << endl)
#define RLOG_WARNING (cout << endl)
#define RLOG_VERBOSE (cout << endl)
#endif

#endif // emptyExample_RemoteParam_h

using namespace std;

#include <sstream>
#include <stdio.h>

enum RemoteUICallClientAction {
	SERVER_CONNECTED,
	SERVER_DISCONNECTED,
	SERVER_SENT_FULL_PARAMS_UPDATE,
	SERVER_PRESETS_LIST_UPDATED,
	SERVER_CONFIRMED_SAVE,
	SERVER_DID_RESET_TO_DEFAULTS,
	SERVER_DELETED_PRESET,
	SERVER_SAVED_PRESET,
	SERVER_DID_RESET_TO_XML,
	SERVER_DID_SET_PRESET,
	SERVER_REPORTS_MISSING_PARAMS_IN_PRESET,
	NEIGHBORS_UPDATED,
	NEIGHBOR_JUST_LAUNCHED_SERVER,
	// group presets
	SERVER_SAVED_GROUP_PRESET,
	SERVER_DID_SET_GROUP_PRESET,
	SERVER_DELETED_GROUP_PRESET,
	SERVER_SENT_LOG_LINE
};

enum RemoteUICallServerAction {
	CLIENT_CONNECTED,
	CLIENT_DISCONNECTED,
	CLIENT_UPDATED_PARAM,
	CLIENT_DID_SET_PRESET,
	CLIENT_SAVED_PRESET,
	CLIENT_DELETED_PRESET,
	CLIENT_SAVED_STATE,
	CLIENT_DID_RESET_TO_XML,
	CLIENT_DID_RESET_TO_DEFAULTS,
	// group presets
	CLIENT_DID_SET_GROUP_PRESET,
	CLIENT_SAVED_GROUP_PRESET,
	CLIENT_DELETED_GROUP_PRESET
};

enum RemoteUIParamType {
	REMOTEUI_PARAM_UNKNOWN = 0,
	REMOTEUI_PARAM_FLOAT = 100,
	REMOTEUI_PARAM_INT,
	REMOTEUI_PARAM_BOOL,
	REMOTEUI_PARAM_STRING,
	REMOTEUI_PARAM_ENUM,
	REMOTEUI_PARAM_COLOR,
	REMOTEUI_PARAM_SPACER,
};

enum ActionType {
	HELO_ACTION,
	REQUEST_ACTION,
	SEND_PARAM_ACTION,
	CIAO_ACTION,
	TEST_ACTION,
	PRESET_LIST_ACTION,
	SET_PRESET_ACTION,
	SAVE_PRESET_ACTION,
	DELETE_PRESET_ACTION,
	RESET_TO_XML_ACTION,
	RESET_TO_DEFAULTS_ACTION,
	SAVE_CURRENT_STATE_ACTION,
	GET_MISSING_PARAMS_IN_PRESET,
	SET_GROUP_PRESET_ACTION,
	SAVE_GROUP_PRESET_ACTION,
	DELETE_GROUP_PRESET_ACTION,
	SEND_LOG_LINE_ACTION
};

enum ArgType { FLT_ARG, INT_ARG, BOL_ARG, STR_ARG, ENUM_ARG, COLOR_ARG, SPACER_ARG, NULL_ARG };

struct DecodedMessage {
	ActionType action;
	ArgType argument;
	string paramName;
};

#ifndef OF_AVAILABLE // if OF is not available, redefine ofColor to myColor
//#warning "Openframeworks is not available!"
#define ofColor myColor

struct myColor {
	myColor() {
	}
	myColor(int rr, int gg, int bb, int aa) {
		r = rr;
		g = gg;
		b = bb;
		a = aa;
	}
	myColor(int bright) {
		r = g = b = bright;
		a = 255;
	}
	bool operator==(const myColor &c) {
		return r == c.r && g == c.g && b == c.b && a == c.a;
	}
	bool operator!=(const myColor &c) {
		return r != c.r || g != c.g || b != c.b || a != c.a;
	}
	union {
		struct {
			unsigned char r, g, b, a;
		};
		unsigned char v[4];
	};
#ifdef CINDER_AVAILABLE // if cinder available, define an easy port to cinderColor
	//#warning "Compiling for Cinder!"

	// Construct color from cinder color
	myColor(ci::ColorA8u color) {
		r = color.r;
		g = color.g;
		b = color.b;
		a = color.a;
	}

	// TODO more of these?
	ci::ColorA8u toCinder() {
		return ci::ColorA8u(r, g, b, a);
	}
#endif
};
#endif

class RemoteUIParam { // I am lazy and I know it

public:
	RemoteUIParam() {
		type = REMOTEUI_PARAM_UNKNOWN;
		floatValAddr = NULL;
		intValAddr = NULL;
		boolValAddr = NULL;
		stringValAddr = NULL;
		redValAddr = NULL;
		floatVal = minFloat = maxFloat = 0;
		intVal = minInt = maxInt = 0;
		redVal = greenVal = blueVal = alphaVal = 0;
		boolVal = false;
		stringVal = "empty";
		floatGetter = nullptr;
		floatSetter = nullptr;

		r = g = b = a = 0; // bg color
		group = OFXREMOTEUI_DEFAULT_PARAM_GROUP;
	};

	bool isUsingGetterSetter() const {
		switch (type) {
			case REMOTEUI_PARAM_FLOAT: // GSFDONE
				return (floatGetter != nullptr) && (floatSetter != nullptr);
				break;
			case REMOTEUI_PARAM_BOOL: // GSBDONE
				return (boolGetter != nullptr) && (boolSetter != nullptr);
				break;
			case REMOTEUI_PARAM_ENUM:
			case REMOTEUI_PARAM_INT:
			case REMOTEUI_PARAM_STRING:
			case REMOTEUI_PARAM_COLOR:
			case REMOTEUI_PARAM_SPACER:
				return false;
				break;
			default:
				RLOG_ERROR << "weird RemoteUIParam " << type << " at isEqualTo() !";
				return false;
				break;
		}
	}

	bool isEqualTo(RemoteUIParam &p) {
		bool equal = true;
		switch (type) {
			case REMOTEUI_PARAM_FLOAT: // GSDONE
				if (p.floatVal != floatVal)
					equal = false;
				if (p.minFloat != minFloat)
					equal = false;
				if (p.maxFloat != maxFloat)
					equal = false;
				break;
			case REMOTEUI_PARAM_ENUM:
			case REMOTEUI_PARAM_INT:
				if (p.intVal != intVal)
					equal = false;
				if (p.minInt != minInt)
					equal = false;
				if (p.maxInt != maxInt)
					equal = false;
				break;
			case REMOTEUI_PARAM_BOOL: // GSDONE
				if (p.boolVal != boolVal)
					equal = false;
				break;
			case REMOTEUI_PARAM_STRING:
				if (p.stringVal != stringVal)
					equal = false;
				break;
			case REMOTEUI_PARAM_COLOR:
				if (p.redVal != redVal || p.greenVal != greenVal || p.blueVal != blueVal || p.alphaVal != alphaVal)
					equal = false;
				break;
			case REMOTEUI_PARAM_SPACER:
				equal = false;
				break;
			default:
				RLOG_ERROR << "weird RemoteUIParam " << type << " at isEqualTo() !";
				equal = false;
				break;
		}
		return equal;
	}

	string getValueAsString() const {
		std::ostringstream ss;
		char aux[50];
		switch (type) {
			case REMOTEUI_PARAM_FLOAT: // GSDONE
				ss << floatVal;
				return ss.str();
			case REMOTEUI_PARAM_ENUM:
				if (intVal >= minInt && intVal <= maxInt && (intVal - minInt) < enumList.size())
					ss << enumList[intVal - minInt];
				else
					ss << "Invalid Enum!";
				return ss.str();
			case REMOTEUI_PARAM_INT:
				ss << intVal;
				return ss.str();
			case REMOTEUI_PARAM_BOOL: // GSDONE
				return boolVal ? "TRUE" : "FALSE";
			case REMOTEUI_PARAM_STRING:
				return stringVal;
			case REMOTEUI_PARAM_COLOR: {
				sprintf(aux, "RGBA: [%d, %d, %d, %d]", redVal, greenVal, blueVal, alphaVal);
				return string(aux);
			}
			case REMOTEUI_PARAM_SPACER:
				return "";
			default:
				return "unknown value (BUG!)";
		}
	}

	string getValueAsStringFromPointer() {
		std::ostringstream ss;
		char aux[50];
		switch (type) {
			case REMOTEUI_PARAM_FLOAT: // GSDONE
				if (isUsingGetterSetter()) {
					ss << floatGetter();
				} else {
					ss << *floatValAddr;
				}
				return ss.str();
			case REMOTEUI_PARAM_ENUM: {
				int v = *intValAddr;
				if (v >= minInt && v <= maxInt && (v - minInt) < enumList.size())
					ss << enumList[v - minInt];
				else
					ss << "Invalid Enum!";
				return ss.str();
			}
			case REMOTEUI_PARAM_INT:
				ss << *intValAddr;
				return ss.str();
			case REMOTEUI_PARAM_BOOL: { // GSDONE
				bool v;
				if (isUsingGetterSetter()) {
					v = floatGetter();
				} else {
					v = *floatValAddr;
				}
				return v ? "TRUE" : "FALSE";
			}
			case REMOTEUI_PARAM_STRING:
				return *stringValAddr;
			case REMOTEUI_PARAM_COLOR: {
				sprintf(aux, "RGBA: [%d, %d, %d, %d]", redValAddr[0], redValAddr[1], redValAddr[2], redValAddr[3]);
				return string(aux);
			}
			case REMOTEUI_PARAM_SPACER:
				return "";
			default:
				return "unknown value (BUG!)";
		}
	}

	void print() {
		switch (type) {
			case REMOTEUI_PARAM_FLOAT: // GSDONE
				printf("float: %.2f [%.2f, %.2f]\n", floatVal, minFloat, maxFloat);
				break;
			case REMOTEUI_PARAM_INT:
				printf("int: %d [%d, %d]\n", intVal, minInt, maxInt);
				break;
			case REMOTEUI_PARAM_COLOR:
				printf("color: RGBA(%d %d %d %d)\n", redVal, greenVal, blueVal, alphaVal);
				break;
			case REMOTEUI_PARAM_ENUM:
				printf("enum: %d [%d, %d]\n", intVal, minInt, maxInt);
				break;
			case REMOTEUI_PARAM_BOOL: // GSBDONE
				printf("bool: %s\n", boolVal ? "TRUE" : "FALSE");
				break;
			case REMOTEUI_PARAM_STRING:
				printf("string: %s\n", stringVal.c_str());
				break;
			case REMOTEUI_PARAM_SPACER:
				printf("Spacer\n");
				break;
			default:
				printf("weird RemoteUIParam at print()!\n");
				break;
		}
	};

	void setBgColor(const ofColor &c) {
		r = c.r;
		g = c.g;
		b = c.b;
		a = c.a;
	}

	RemoteUIParamType type;

	float *floatValAddr; // used in server
	float floatVal;			 // used in client
	float minFloat;
	float maxFloat;
	std::function<float()> floatGetter;
	std::function<void(float)> floatSetter;
	std::function<bool()> boolGetter;
	std::function<void(bool)> boolSetter;

	int *intValAddr;
	int intVal;
	int minInt;
	int maxInt;

	bool *boolValAddr;
	bool boolVal;

	string *stringValAddr;
	string stringVal;

	unsigned char redVal, greenVal, blueVal, alphaVal;
	unsigned char *redValAddr;

	string group;
	vector<string> enumList; // for enum type

	unsigned char r, g, b, a; // param bg color [0,255]
};

struct RemoteUIClientCallBackArg {
	RemoteUICallClientAction action;
	string msg; // sort of a wildcard; usually its the preset name
	string host;
	string group;
	int port;
	vector<string> paramList; // wildacard, used for missing param list
};

struct RemoteUIServerCallBackArg {
	RemoteUICallServerAction action;
	string paramName;
	RemoteUIParam param; // get a copy o the new value of the param (only makes sense when action==SERVER_SENT_FULL_PARAMS_UPDATE)
	string msg;					 // sort of a wildcard; usually its the preset name
	string host;
	string group;
};
