//
//  ofxCinderOSC.hpp
//  WorkbenchTablet
//
//  Created by Eric Mika on 1/19/16.
//
//

#pragma once

#include "Osc.h"
#include <queue>
#include <stdio.h>

class ofxOscMessage {
	friend class ofxOscReceiver;
	friend class ofxOscSender;

public:
	ofxOscMessage();
	~ofxOscMessage();

	ofxOscMessage(const ofxOscMessage &other) {
		copy(other);
	}
	ofxOscMessage &operator=(const ofxOscMessage &other) {
		return copy(other);
	}
	ofxOscMessage &copy(const ofxOscMessage &other);

	std::string getAddress() const;
	std::string getRemoteIp() const;
	int getNumArgs() const;
	int32_t getArgAsInt32(int index) const;
	float getArgAsFloat(int index) const;
	std::string getArgAsString(int index) const;
	void setAddress(std::string _address);

	void addIntArg(int32_t argument);
	void addFloatArg(float argument);
	void addStringArg(std::string argument);

private:
	ci::osc::Message mBackingMessage;
	std::string mRemoteIP;
};

class ofxOscSender {
	friend class ofxOscReceiver;

public:
	void setup(std::string hostname, int port, bool broadcast = false);
	void sendMessage(ofxOscMessage &message);
	// void sendBundle( ofxOscBundle& bundle ); // TODO
	ci::osc::UdpSocketRef mSocket; // sharedptr
private:
	std::shared_ptr<ci::osc::SenderUdp> mSenderRef = nullptr;
};

class ofxOscReceiver {
public:
	void setup(int listen_port);
	bool hasWaitingMessages();
	bool getNextMessage(ofxOscMessage *message);

private:
	std::unique_ptr<ci::osc::ReceiverUdp> mReceiverRef = nullptr;
	std::queue<ci::osc::Message> mMessages;
};
