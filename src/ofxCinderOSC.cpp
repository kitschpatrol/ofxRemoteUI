//
//  ofxCinderOSC.cpp
//  WorkbenchTablet
//
//  Created by Eric Mika on 1/19/16.
//
//
#include "ofxCinderOSC.h"

#include "cinder/Log.h"
#include "cinder/app/App.h"

using namespace asio;
using namespace asio::ip;

#pragma mark - ofxOscMessage

ofxOscMessage::ofxOscMessage() {
}

ofxOscMessage::~ofxOscMessage() {
}

std::string ofxOscMessage::getAddress() const {
	return mBackingMessage.getAddress();
}

std::string ofxOscMessage::getRemoteIp() const {
	return mBackingMessage.mSenderIpAddress.to_string();
}

int ofxOscMessage::getNumArgs() const {
	return static_cast<int>(mBackingMessage.getNumArgs());
}

int32_t ofxOscMessage::getArgAsInt32(int index) const {
	return mBackingMessage[index].int32();
}

float ofxOscMessage::getArgAsFloat(int index) const {
	return mBackingMessage[index].flt();
}

std::string ofxOscMessage::getArgAsString(int index) const {
	return mBackingMessage[index].string();
}

void ofxOscMessage::setAddress(std::string _address) {
	mBackingMessage.setAddress(_address);
}

void ofxOscMessage::addIntArg(int32_t argument) {
	mBackingMessage.append(argument);
}
void ofxOscMessage::addFloatArg(float argument) {
	mBackingMessage.append(argument);
}
void ofxOscMessage::addStringArg(std::string argument) {
	mBackingMessage.append(argument);
}

ofxOscMessage &ofxOscMessage::copy(const ofxOscMessage &other) {
	// clear();
	// copy address

	this->mBackingMessage = other.mBackingMessage;

	//	address = other.address;
	//
	//	remote_host = other.remote_host;
	//	remote_port = other.remote_port;
	//
	//	// copy arguments
	//	for ( int i=0; i<(int)other.args.size(); ++i )
	//	{
	//		ofxOscArgType argType = other.getArgType( i );
	//		if ( argType == OFXOSC_TYPE_INT32 )
	//			args.push_back( new ofxOscArgInt32( other.getArgAsInt32( i ) ) );
	//		else if ( argType == OFXOSC_TYPE_INT64 )
	//			args.push_back( new ofxOscArgInt64( other.getArgAsInt64( i ) ) );
	//		else if ( argType == OFXOSC_TYPE_FLOAT )
	//			args.push_back( new ofxOscArgFloat( other.getArgAsFloat( i ) ) );
	//		else if ( argType == OFXOSC_TYPE_STRING )
	//			args.push_back( new ofxOscArgString( other.getArgAsString( i ) ) );
	//		else
	//		{
	//			assert( false && "bad argument type" );
	//		}
	//	}

	return *this;
}

#pragma mark - ofxOscSender

void ofxOscSender::setup(std::string hostname, int port, bool broadcast) {
	if (mSenderRef != nullptr) {
		// mSocket->close(); // Spares a asio.system:48 runtime crash on Mac Release builds?
		mSenderRef->close();
	}

	// TODO deal with creating

	//	static int localPort = 12345;

	//	mSocket = std::make_shared<udp::socket>(udp::socket(ci::app::App::get()->io_service(), udp::endpoint(udp::v4(), 1234)));
	//	mSenderRef = std::shared_ptr<ci::osc::SenderUdp>(new ci::osc::SenderUdp(mSocket, udp::endpoint(address::from_string(hostname), port)));

	//	mSocket->set_option(asio::socket_base::broadcast(true));
	// why no work!?
	//	std::cout << "address_v4::broadcast() " << address_v4::broadcast() << std::endl;
	//	mSocket = std::shared_ptr<udp::socket>(new udp::socket(ci::app::App::get()->io_service(), udp::endpoint(udp::v4(), 1234)));
	//	udp::endpoint remoteEndpoint = udp::endpoint(address::from_string(hostname), port);
	//	mSenderRef = std::shared_ptr<ci::osc::SenderUdp>(new ci::osc::SenderUdp(mSocket, remoteEndpoint));

	// SIgnature
	//	SenderUdp( uint16_t localPort,
	//			   const std::string &destinationHost,
	//			   uint16_t destinationPort,
	//			   const protocol &protocol = protocol::v4(),
	//			   asio::io_service &service = ci::app::App::get()->io_service() );

	mSenderRef = std::shared_ptr<ci::osc::SenderUdp>(new ci::osc::SenderUdp(12345, hostname, port));

	mSenderRef->setSocketTransportErrorFn(																					 //
			[](const asio::error_code &error, const std::string &oscAddress) {					 //
				CI_LOG_E("Set Socket Error: " << error << " OSC Address: " << oscAddress); //																																				 //
			});

	mSenderRef->bind();
}

void ofxOscSender::sendMessage(ofxOscMessage &message) {
	mSenderRef->send(message.mBackingMessage);
}

#pragma mark - ofxOscReceiver

void ofxOscReceiver::setup(int listen_port) {
	if (mReceiverRef != nullptr) {
		while (mMessages.size() > 0) {
			mMessages.pop();
		}
		mReceiverRef->close();
	}

	mReceiverRef = std::unique_ptr<ci::osc::ReceiverUdp>(new ci::osc::ReceiverUdp(listen_port));

	mReceiverRef->setSocketErrorFn(																									 //
			[](const asio::error_code &error, const udp::endpoint &oscAddress) {				 //
				CI_LOG_E("Set Socket Error: " << error << " OSC Address: " << oscAddress); //

			});

	// TODO mRemoteIP?

	mReceiverRef->setListener("*",																	 //
														[&](const ci::osc::Message &message) { //

															mMessages.push(message); //
														});

	mReceiverRef->bind();
	mReceiverRef->listen();
}

bool ofxOscReceiver::hasWaitingMessages() {
	return (mMessages.size() > 0);
}

bool ofxOscReceiver::getNextMessage(ofxOscMessage *message) {
	if (mMessages.size() == 0) {
		return false;
	} else {
		// TODO better way to do this?
		ofxOscMessage translatedMessage;

		translatedMessage.mBackingMessage = mMessages.front();
		message->copy(translatedMessage);
		mMessages.pop();
		return true;
	}
}
