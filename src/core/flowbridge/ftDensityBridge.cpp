/*  ************************************************************************************
 *
 *  ftDensityBridge
 *
 *  Created by Matthias Oostrik on 03/16.14.
 *  Copyright 2014 http://www.MatthiasOostrik.com All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  ************************************************************************************ */

#include "ftDensityBridge.h"

namespace flowTools {
	
	void ftDensityBridge::setup(int _width, int _height){
		width = _width;
		height = _height;
		
		colorMaskSwapBuffer.allocate(width, height, GL_RGBA32F);
		colorMaskSwapBuffer.black();
		
		luminanceMaskFbo.allocate(width, height, GL_R32F);
		luminanceMaskFbo.black();
		
		bVelocityTextureSet = false;
		bDensityTextureSet = false;
		
		parameters.setName("density input");
//		parameters.add(power.set("mag. power", .5, 0, 1));
//		parameters.add(cutOff.set("mag. cutOff", 1, 0, 1));
		//		parameters.add(hue.set("hue", 0, -1, 1)); // does not work properly (does in the minus range?)
		parameters.add(strength.set("speed", 1, 0, 100));
		parameters.add(saturation.set("color saturation", 1.5, 0, 3));
//		parameters.add(blurRadius.set("blur radius", 5, 0, 10));
		
	};
	
	void ftDensityBridge::update(float _deltaTime) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		colorMaskSwapBuffer.black();
		
		if (!bVelocityTextureSet || !bDensityTextureSet) {
			ofLogVerbose("ftDensityBridge: velocity or density texture not set, can't update");
		}
		else {
			densityBridgeShader.update(*colorMaskSwapBuffer.getBuffer(),
									   *densityTexture,
									   *velocityTexture,
									   power.get(),
									   strength.get() * _deltaTime,
									   cutOff.get());
			colorMaskSwapBuffer.swap();
			HSLShader.update(*colorMaskSwapBuffer.getBuffer(),
							 colorMaskSwapBuffer.getBackTexture(),
							 hue.get(),
							 saturation.get(),
							 1);
			
//			if (blurRadius.get() > 0) {
//				gaussianBlurShader.update(*colorMaskSwapBuffer.getBuffer(), 3, blurRadius.get());
//			}
			
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			luminanceShader.update(luminanceMaskFbo, colorMaskSwapBuffer.getTexture());
		}
		
		ofPopStyle();
	}
	
	void ftDensityBridge::draw(int _x, int _y, int _width, int _height, ofBlendMode _blendmode) {
		ofPushStyle();
		ofEnableBlendMode(_blendmode);
		colorMaskSwapBuffer.getTexture().draw(_x, _y, _width, _height);
		ofPopStyle();
	}
	
	void ftDensityBridge::setDensity(ofTexture &tex) {
		densityTexture = &tex;
		bDensityTextureSet = true;
	}
	
	void ftDensityBridge::setVelocity(ofTexture &tex) {
		velocityTexture = &tex;
		bVelocityTextureSet = true;
	}
}
