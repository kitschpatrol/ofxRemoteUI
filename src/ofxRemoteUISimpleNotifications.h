//
//  ofxRemoteUISimpleNotifications.h
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 18/11/13.
//
//

#ifndef emptyExample_ofxRemoteUISimpleNotifications_h
#define emptyExample_ofxRemoteUISimpleNotifications_h

#define NOTIFICATION_ALPHA_OVERFLOW 3.0
#define NOTIFICATION_COLOR ofColor(200, 16, 16, 255 * a)

#define PARAM_UPDATE_COLOR ofColor(0, 200, 0, 255 * a)
#define FRESH_COLOR ofColor(0)
#define PARAM_WATCH_COLOR ofColor(0, 128, 255)
#define NOTIFICATION_LINEHEIGHT 20
#include "RemoteParam.h"

#if defined CINDER_AVAILABLE
#include "cinder/Cinder.h"
#include "cinder/Utilities.h"
#include "cinder/gl/gl.h"
#include "ofStolenUtils.h"

#endif

#if defined OF_VERSION_MINOR /*if OF exists*/
#include "ofMain.h"
#endif

#ifdef USE_OFX_FONTSTASH
#include "ofxFontStash.h"
#endif

class ofxRemoteUISimpleNotifications {

public:
	struct SimpleNotification {
		string msg;
		float time;
	};

	struct ParamNotification {
		string value;
		float time;
		ofColor color;
		ofColor bgColor;
		bool range;
		float rangeMin;
		float rangeMax;
		float pct;
	};

	ofxRemoteUISimpleNotifications() {
#ifdef USE_OFX_FONTSTASH
		font = NULL;
#endif

#if defined CINDER_AVAILABLE

#if defined CINDER_MSW
		mFont = ci::gl::TextureFont::create(ci::Font("Lucida Console", 50));
#else
		mFont = ci::gl::TextureFont::create(ci::Font("Menlo", 50));
#endif
		mFontHeight = mFont->measureString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890").y;
#endif
	};

	void update(float dt) {
		vector<int> toDeleteIndexes;
		// walk all notif, write down expired ones's indexes
		for (int i = 0; i < notifications.size(); i++) {
			notifications[i].time -= dt;
			if (notifications[i].time < 0.0f) {
				toDeleteIndexes.push_back(i);
			}
		}
		// delete expired notifications
		for (int i = toDeleteIndexes.size() - 1; i >= 0; i--) {
			notifications.erase(notifications.begin() + toDeleteIndexes[i]);
		}

		vector<string> toDeleteParams;
		typedef std::map<string, ParamNotification>::iterator it_type;
		for (it_type it = paramNotifications.begin(); it != paramNotifications.end(); it++) {
			// it->first;
			it->second.time -= dt;
			if (it->second.time < 0.0f) {
				toDeleteParams.push_back(it->first);
			}
		}

		// delete expired paramUpdates
		for (int i = 0; i < toDeleteParams.size(); i++) {
			paramNotifications.erase(toDeleteParams[i]);
		}
	};

	void draw(float x, float y) {
		const float spacing = NOTIFICATION_LINEHEIGHT;
		float yy = y;

#if defined CINDER_AVAILABLE
		ci::gl::ScopedBlendAlpha textAlpha;
#endif

		// Notifications
		for (const SimpleNotification &notification : notifications) {
			const float a = ofClamp(NOTIFICATION_ALPHA_OVERFLOW * notification.time, 0.0f, 1.0f);
			const float hh = drawStringWithBox("ofxRemoteUIServer: " + notification.msg, x, yy, ofColor(0, 0, 0, 255 * a), NOTIFICATION_COLOR);
			yy -= hh;
		}

		// Param Notifications
		for (auto &paramPair : paramNotifications) {
			// Assign pair elements for convenience
			const std::string &paramName = paramPair.first;
			const ParamNotification &paramNotification = paramPair.second;

			// Fade out as needed
			const float a = ofClamp(NOTIFICATION_ALPHA_OVERFLOW * paramNotification.time, 0.0f, 1.0f);
			const float fresh = 1.0f - ofClamp((screenTime + 1) - paramNotification.time, 0.0f, 1.0f);

			// Draw the label and background
			ofColor bgColor = (fresh > 0.1) ? paramNotification.bgColor : ofColor(paramNotification.bgColor, 255.0 * a);

			string total = paramName + ": " + paramNotification.value;
			float hh = drawStringWithBox(total, x, yy, bgColor, ofColor(0, 0, 0));

#if defined CINDER_AVAILABLE
			int xx = x + mFont->measureString(total).x;
			int yyy = yy;
#else

			int xx = x + total.length() * 8 + 4;				// todo really measure this
			int yyy = yy + 6 - NOTIFICATION_LINEHEIGHT; // todo really measure this

#endif

			// Color Swatches to right of label
			if (paramNotification.color.a != 0) {

#if defined OF_VERSION_MINOR
				ofPushStyle();
				ofSetColor(it->second.color, a * 255);

#ifdef USE_OFX_FONTSTASH
				if (font != NULL) { // let's find the X where to draw the color swatch - this is time wasted TODO!
					ofRectangle r = font->getBBox(total, 15, 0, 0);
					float diff = floor(NOTIFICATION_LINEHEIGHT - r.height);
					ofDrawRectangle(x + r.width + r.x + 4, yy + r.y - diff / 2, 40, NOTIFICATION_LINEHEIGHT);
				} else {
					ofDrawRectangle(xx, yyy, 40, NOTIFICATION_LINEHEIGHT);
				}
#else
				ofDrawRectangle(xx, yyy, 40, NOTIFICATION_LINEHEIGHT);
#endif
				ofPopStyle();
#endif

#if defined CINDER_AVAILABLE
				ci::gl::ScopedColor someColor(ci::ColorA8u(paramNotification.color, a * 255));
				ci::gl::drawSolidRect(makeCiRectWidthHeight(xx, yyy, mFontHeight, mFontHeight)); // TODO real height?
#endif
			}

			// Sliders to right of label
			if (paramNotification.range) {
				int sliderW = 80;
				int pad = 9;
				int knobW = 6;
				int markH = 2;
				int voff = (NOTIFICATION_LINEHEIGHT - knobW) / 2;

#if defined OF_VERSION_MINOR /*if OF exists*/

				ofSetColor(0);
				ofDrawRectangle(xx, yyy, sliderW, NOTIFICATION_LINEHEIGHT);
				ofSetColor(45);
				ofDrawRectangle(xx + pad, yyy + pad, sliderW - 2 * pad, NOTIFICATION_LINEHEIGHT - 2 * pad);
				ofSetColor(bgColor);
				ofLine(xx + sliderW / 2, yyy + NOTIFICATION_LINEHEIGHT / 2 + markH, xx + sliderW / 2, yyy + NOTIFICATION_LINEHEIGHT / 2 - markH);
				ofDrawRectangle(xx + pad - knobW / 2 + (sliderW - 2 * pad) * ofClamp(it->second.pct, 0, 1), yyy + voff, knobW, knobW);
#endif

#if defined CINDER_AVAILABLE

				sliderW = mFontHeight * 5.0;
				const float knobSize = mFontHeight * 0.8;
				const float trackHeight = mFontHeight * 0.1;

				// background block
				ci::gl::ScopedColor someColor(ci::Color("black"));
				ci::gl::drawSolidRect(makeCiRectWidthHeight(xx, yy, sliderW, mFontHeight));

				// Track
				ci::gl::ScopedColor someOtherColor(ci::Color(45.0 / 255.0, 45.0 / 255.0, 45.0 / 255.0));
				ci::gl::drawSolidRect(makeCiRectWidthHeight(xx + knobSize * 0.5, yy + (mFontHeight - trackHeight) * 0.5, sliderW - knobSize, trackHeight));

				// Knob
				ci::gl::ScopedColor backgroundColor(ci::ColorA8u(bgColor));
				const float offset = ofClamp(paramNotification.pct, 0, 1) * (sliderW - knobSize);
				ci::gl::drawSolidRect(makeCiRectWidthHeight(xx + offset, yy + ((mFontHeight - knobSize) * 0.5), knobSize, knobSize));
#endif
			}
			yy -= hh;
		}

		// Watched Parameters (respecting order map)
		for (auto &paramWatchOrderPair : paramWatchOrder) {
			// int order = it2->first;
			const std::string name = paramWatchOrderPair.second;
			float hh = drawStringWithBox("[" + name + "] " + paramWatch[name].value, x, yy, ofColor(0, 0, 0), paramWatch[name].color);
			yy -= hh;
		}
	};

	void addNotification(const string &msg) {
		SimpleNotification n;
		n.msg = msg;
		n.time = screenTime;
		notifications.push_back(n);
	};

	void addParamUpdate(const string &paramName, const RemoteUIParam &p, const ofColor &bgColor, const ofColor &paramC = ofColor(0, 0)) {
		ParamNotification n;
		n.color = paramC;
		n.bgColor = bgColor;
		n.value = p.getValueAsString();
		n.time = screenTime;
		switch (p.type) {
			case REMOTEUI_PARAM_FLOAT:
				n.rangeMin = p.minFloat;
				n.rangeMax = p.maxFloat;
				n.pct = ofMap(p.floatVal, p.minFloat, p.maxFloat, 0, 1);
				n.range = true;
				if (n.value.size() < 10) {
					for (int i = 0; i < n.value.size() - 10; i++) {
						n.value += " ";
					}
				}
				break;
			case REMOTEUI_PARAM_INT:
				n.rangeMin = p.minInt;
				n.rangeMax = p.maxInt;
				n.pct = ofMap(p.intVal, p.minInt, p.maxInt, 0, 1);
				n.range = true;
				break;
			default:
				n.range = false;
				break;
		}
		paramNotifications[paramName] = n;
	};

	void addParamWatch(const string &paramName, const string &paramValue, const ofColor &paramC) {
		ParamNotification n;
		n.color = paramC;
		n.bgColor = ofColor(0);
		n.value = paramValue;
		n.time = screenTime;
		n.range = false;
		if (paramWatch.find(paramName) == paramWatch.end()) {
			paramWatchOrder[paramWatchOrder.size()] = paramName;
		}
		paramWatch[paramName] = n;
	};

#ifdef USE_OFX_FONTSTASH
	void drawUiWithFontStash(ofxFontStash *font_) {
		font = font_;
	}
	void drawUiWithBitmapFont() {
		font = NULL;
	}
#endif

	void setNotificationScreenTime(float t) {
		screenTime = t;
	}

private:
	// return height of box
	float drawStringWithBox(const string &text, int x, int y, const ofColor &background, const ofColor &foreground) {
#ifdef USE_OFX_FONTSTASH
		if (font == NULL) {
			ofDrawBitmapStringHighlight(text, x, y, background, foreground);
			return NOTIFICATION_LINEHEIGHT;
		} else {
			ofRectangle r = font->getBBox(text, 15, x, y);
			float diff = floor(NOTIFICATION_LINEHEIGHT - r.height);
			r.x = x - 4;
			r.y -= diff * 0.5f;
			r.width += diff + 2;
			r.height += diff;
			r.height = ceil(r.height);
			ofPushStyle();
			ofSetColor(background);
			ofDrawRectangle(r);
			ofSetColor(foreground);
			font->draw(text, 15, x, y);
			ofPopStyle();
			return floor(r.height);
		}
#endif

#ifdef OF_VERSION_MINOR
		ofDrawBitmapStringHighlight(text, x, y, background, foreground);
		return NOTIFICATION_LINEHEIGHT;
#endif

#ifdef CINDER_AVAILABLE
		ci::vec2 size = mFont->measureString(text);
		ci::gl::ScopedColor backgroundColor(background);
		ci::gl::drawSolidRect(makeCiRectWidthHeight(x, y, size.x, mFontHeight));
		ci::gl::ScopedColor textColor(foreground);
		mFont->drawString(text, ci::vec2(x, y + (mFontHeight * .625))); // ugh
		return mFontHeight;
#endif
	}

#ifdef USE_OFX_FONTSTASH
	ofxFontStash *font;
#endif
#ifdef CINDER_AVAILABLE
	ci::gl::TextureFontRef mFont;

	// convenience since OF likes to define rects in terms of x,y,w,h instead of x1,y1,x2,y2
	ci::Rectf makeCiRectWidthHeight(float x, float y, float width, float height) {
		return ci::Rectf(x, y, x + width, y + height);
	}

	float mFontHeight;

#endif
	vector<SimpleNotification> notifications;
	map<string, ParamNotification> paramNotifications;
	map<string, ParamNotification> paramWatch;
	map<int, string> paramWatchOrder;
	float screenTime = 5.0;
};

#endif
