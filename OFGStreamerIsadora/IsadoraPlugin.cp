// ===========================================================================
//	Isadora Demo Plugin			   ©2003 Mark F. Coniglio. All rights reserved.
// ===========================================================================
//
//	IMPORTANT: This source code ("the software") is supplied to you in
//	consideration of your agreement to the following terms. If you do not
//	agree to the terms, do not install, use, modify or redistribute the
//	software.
//
//	Mark Coniglio (dba TroikaTronix) grants you a personal, non exclusive
//	license to use, reproduce, modify this software with and to redistribute it,
//	with or without modifications, in source and/or binary form. Except as
//	expressly stated in this license, no other rights are granted, express
//	or implied, to you by TroikaTronix.
//
//	This software is provided on an "AS IS" basis. TroikaTronix makes no
//	warranties, express or implied, including without limitation the implied
//	warranties of non-infringement, merchantability, and fitness for a 
//	particular purpurse, regarding this software or its use and operation
//	alone or in combination with your products.
//
//	In no event shall TroikaTronix be liable for any special, indirect, incidental,
//	or consequential damages arising in any way out of the use, reproduction,
//	modification and/or distribution of this software.
//
// ===========================================================================
//
// CUSTOMIZING THIS SOURCE CODE
// To customize this file, search for the text ###. All of the places where
// you will need to customize the file are marked with this pattern of 
// characters.
//
// ABOUT IMAGE BUFFER MAPS:
//
// The ImageBufferMap structure, and its accompanying functions,
// exists as a convenience to those writing video processing plugins.
//
// Basically, an image buffer contains an arbitrary number of input and
// output buffers (in the form of ImageBuffers). The ImageBufferMap code
// will automatically create intermediary buffers if needed, so that the
// size and depth of the source image buffers sent to your callback are
// the same for all buffers.
// 
// Typically, the ImageBufferMap is created in your CreateActor function,
// and dispose in the DiposeActor function.

// ---------------------------------------------------------------------------------
// INCLUDES
// ---------------------------------------------------------------------------------
#include "ofVideoPlayer.h"
#include "ofPixels.h"

#include "IsadoraTypes.h"
#include "IsadoraCallbacks.h"
#include "ImageBufferUtil.h"
#include "PluginDrawUtil.h"

// STANDARD INCLUDES
#include <string.h>
#include <stdio.h>
#include <iostream>



// ---------------------------------------------------------------------------------
// MacOS Specific
// ---------------------------------------------------------------------------------
#if TARGET_OS_MAC
#define EXPORT_
#endif

// ---------------------------------------------------------------------------------
// Win32  Specific
// ---------------------------------------------------------------------------------
#if TARGET_OS_WIN32

	#include <windows.h>
	
	#define EXPORT_ __declspec(dllexport)
	
	#ifdef __cplusplus
	extern "C" {
	#endif

	BOOL WINAPI DllMain ( HINSTANCE hInst, DWORD wDataSeg, LPVOID lpvReserved );

	#ifdef __cplusplus
	}
	#endif

	BOOL WINAPI DllMain (
		HINSTANCE	/* hInst */,
		DWORD		wDataSeg,
		LPVOID		/* lpvReserved */)
	{
	switch(wDataSeg) {
	
	case DLL_PROCESS_ATTACH:
		return 1;
		break;
	case DLL_PROCESS_DETACH:
		break;
		
	default:
		return 1;
		break;
	}
	return 0;
	}

#endif

// ---------------------------------------------------------------------------------
//	Exported Function Definitions
// ---------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

EXPORT_ void GetActorInfo(void* inParam, ActorInfo* outActorParams);

#ifdef __cplusplus
}
#endif


// ---------------------------------------------------------------------------------
//	FORWARD DECLARTIONS
// ---------------------------------------------------------------------------------
static void
ReceiveMessage(
	IsadoraParameters*	ip,
	MessageMask			inMessageMask,
	PortIndex			inPortIndex1,
	const MsgData*		inData,
	UInt32				inLen,
	long				inRefCon);

// ---------------------------------------------------------------------------------
// GLOBAL VARIABLES
// ---------------------------------------------------------------------------------
// ### Declare global variables, common to all instantiations of this plugin here

// Example: static int gMyGlobalVariable = 5;

// ---------------------------------------------------------------------------------
// PluginInfo struct
// ---------------------------------------------------------------------------------
// ### This structure neeeds to contain all variables used by your plugin. Memory for
// this struct is allocated during the CreateActor function, and disposed during
// the DisposeActor function, and is private to each copy of the plugin.
//
// If your plugin needs global data, declare them as static variables within this
// file. Any static variable will be global to all instantiations of the plugin.

typedef struct {

	ActorInfo*				mActorInfoPtr;		// our ActorInfo Pointer - set during create actor function
	MessageReceiverRef		mMessageReceiver;	// pointer to our message receiver reference
	Boolean					mNeedsDraw;			// set to true when the video output needs to be drawn
	
	
	ImageBufferMap			mImageBufferMap;	// used by most video plugins -- see about ImageBufferMaps above
	
	float					mRedAmount;			// ### Demo Plugin - value = -1.0 to +1.0
	float					mGreenAmount;		// ### Demo Plugin - value = -1.0 to +1.0
	float					mBlueAmount;		// ### Demo Plugin - value = -1.0 to +1.0
    String*                 mRtsp;                // RTSP url to get the stream from
	Boolean					mBypass;
    ofVideoPlayer*          fingerMovie;
	
} PluginInfo;

// A handy macro for casting the mActorDataPtr to PluginInfo*
#if __cplusplus
#define	GetPluginInfo_(actorDataPtr)		static_cast<PluginInfo*>((actorDataPtr)->mActorDataPtr);
#else
#define	GetPluginInfo_(actorDataPtr)		(PluginInfo*)((actorDataPtr)->mActorDataPtr);
#endif

// ---------------------------------------------------------------------------------
//	Constants
// ---------------------------------------------------------------------------------
//	Defines various constants used throughout the plugin.

// ### GROUP ID
// Define the group under which this plugin will be displayed in the Isadora interface.
// These are defined under "Actor Types" in IsadoraTypes.h

static const OSType	kActorClass 	= kGroupVideo;

// ### PLUGIN IN
// Define the plugin's unique four character identifier. Contact TroikaTronix to
// obtain a unique four character code if you want to ensure that someone else
// has not developed a plugin with the same code. Note that TroikaTronix reserves
// all plugin codes that begin with an underline, an at-sign, and a pound sign
// (e.g., '_', '@', and '#'.)

static const OSType	kActorID		= FOUR_CHAR_CODE('7m7B');

// ### ACTOR NAME
// The name of the actor. This is the name that will be shown in the User Interface.

static const char* kActorName		= "OFGStreamer";

// ### PROPERTY DEFINITION STRING
// The property string. This string determines the inputs and outputs for your plugin.
// See the IsadoraCallbacks.h under the heading "PROPERTY DEFINITION STRING" for the
// meaning ofthese codes. (The IsadoraCallbacks.h header can be seen by opening up
// the IzzySDK Framework while in the Files view.)
//
// IMPORTANT: You cannot use spaces in the property name. Instead, use underscores (_)
// where you want to have a space.
//
// Note that each line ends with a carriage return (\r), and that only the last line of
// the bunch ends with a semicolon. This means that what you see below is one long
// null-terminated c-string, with the individual lines separated by carriage returns.

static const char* sPropertyDefinitionString =

// INPUT PROPERTY DEFINITIONS
//	TYPE 	PROPERTY NAME	ID		DATATYPE	DISPLAY FMT			MIN		MAX		INIT VALUE
	"INPROP red				red		float		number				-100	100		0\r"
	"INPROP green			grn		float		number				-100	100		0\r"
	"INPROP blue			blu		float		number				-100	100		0\r"
	"INPROP	bypass			byps	bool		onoff				0		1		0\r"
	"INPROP	rtsp			rtsp	string		text				*		*		rtsp://\r"
// OUTPUT PROPERTY DEFINITIONS
//	TYPE 	 PROPERTY NAME	ID		DATATYPE	DISPLAY FMT			MIN		MAX		INIT VALUE
	"OUTPROP video_out		vout	data		video				*		*		0\r";

// ### Property Index Constants
// Properties are referenced by a one-based index. The first input property will
// be 1, the second 2, etc. Similarly, the first output property starts at 1.
// You whould have one constant for each input and output property defined in the 
// property definition string.

enum
{
	kInputRed = 1,
	kInputGreen,
	kInputBlue,
	kInputBypass,
    kInputRtsp,
	
	kOutputVideo = 1
};


// ---------------------
//	Help String
// ---------------------
// ### Help Strings
//
// The first help string is for the actor in general. This followed by help strings
// for all of the inputs, and then by the help strings for all of the outputs. These
// should be given in the order that they are defined in the Property Definition
// String above.
//
// In all, the total number of help strings should be (num inputs + num outputs + 1)
//
// Note that each string is followed by a comma -- it is a common mistake to forget the
// comma which results in the two strings being concatenated into one.

const char* sHelpStrings[] =
{
	"Streams video from network using gstreamer.",

	"The video source that will be colorized.",
	
	"The amount by which the red component of the image will be changed. Values greater than zero "
		"increase the amount of red, values less than zero descrease it.",

	"The amount by which the green component of the image will be changed. Values greater than zero "
		"increase the amount of green, values less than zero descrease it.",

	"The amount by which the blue component of the image will be changed. Values greater than zero "
		"increase the amount of blue, values less than zero descrease it.",
		
	"Bypasses this effect when set to 'on'. When 'off' the effect functions normally.",
    
    "RTSP url of the stream server",
	
	"The colorized video output."
};


// ---------------------------------------------------------------------------------
//		¥ CreateActor
// ---------------------------------------------------------------------------------
// Called once, prior to the first activation of an actor in its Scene. The
// corresponding DisposeActor actor function will not be called until the file
// owning this actor is closed, or the actor is destroyed as a result of being
// cut or deleted.

static void
CreateActor(
	IsadoraParameters*	ip,	
	ActorInfo*			ioActorInfo)		// pointer to this actor's ActorInfo struct - unique to each instance of an actor
{
	// creat the PluginInfo struct - initializing it to all zeroes
	PluginInfo* info = (PluginInfo*) IzzyMallocClear_(ip, sizeof(PluginInfo));
	PluginAssert_(ip, info != nil);
	
	ioActorInfo->mActorDataPtr = info;
	info->mActorInfoPtr = ioActorInfo;

	// ### allocation and initialization of private member variables
	
	// set number of input and output buffers in our buffer map
	// and then initialize it
	info->mImageBufferMap.mInputBufferCount = 1;
	info->mImageBufferMap.mOutputBufferCount = 1;
	CreateImageBufferMap(ip, &info->mImageBufferMap);
    
    info->fingerMovie = new ofVideoPlayer();
    info->fingerMovie->setUseTexture(false);
}

// ---------------------------------------------------------------------------------
//		¥ DisposeActor
// ---------------------------------------------------------------------------------
// Called when the file owning this actor is closed, or when the actor is destroyed
// as a result of its being cut or deleted.
//
static void
DisposeActor(
	IsadoraParameters*	ip,
	ActorInfo*			ioActorInfo)		// pointer to this actor's ActorInfo struct - unique to each instance of an actor
{
	PluginInfo* info = GetPluginInfo_(ioActorInfo);
	PluginAssert_(ip, info != nil);
	
	// ### destruction of private member variables
	
	// destroy our image buffer map
	DisposeImageBufferMap(ip, &info->mImageBufferMap);

	// destroy the PluginInfo struct allocated with IzzyMallocClear_ the CreateActor function
	PluginAssert_(ip, ioActorInfo->mActorDataPtr != nil);
	IzzyFree_(ip, ioActorInfo->mActorDataPtr);

}

// ---------------------------------------------------------------------------------
//		¥ ActivateActor
// ---------------------------------------------------------------------------------
//	Called when the scene that owns this actor is activated or deactivated. The
//	inActivate flag will be true when the scene is activated, false when deactivated.
//
static void
ActivateActor(
	IsadoraParameters*	ip,
	ActorInfo*			inActorInfo,		// pointer to this actor's ActorInfo struct - unique to each instance of an actor
	Boolean				inActivate)			// true when actor is becoming active, false otherwise.
{
	PluginInfo* info = GetPluginInfo_(inActorInfo);
	
	// ------------------------
	// ACTIVATE
	// ------------------------
	
	if (inActivate) {
	
		// Isadora passes various messages to plugins that request them.
		// These include Mouse Moved messages, Key Down/Key Up messages,
		// Video Frame Clock messages, etc. The complete list can be found
		// in the enumeration in MessageReceiverCommon.h
		
		// You ask Isadora¨ for these messages by calling CreateMessageReceiver_
		// with a pointer to your function, and the message types you would
		// like to receive. (These are bitmapped flags, so you can combine as
		// many as you like: kWantKeyDown | kWantKeyDown for instance.)
		
		// Here we request that our ReceiveMessage function is called
		// whenever the Isadora New Video Frame message is sent,
		// which happens periodically, 30 times per second. We set the ref
		// con to our ActorInfo ptr so that we can access that information
		// from ReceiveMessage callback.
		
		MessageReceiveFunction* msgRcvFunc = ReceiveMessage;
		
		// if the "bypass" flag is off, then we want to receive messages
		if (info->mBypass == false) {
			
			// we should not already have a message receiver
			PluginAssert_(ip, info->mMessageReceiver == nil);
			
			// create a message receiver that will be notified of
			// video frame ticks
			info->mMessageReceiver = CreateMessageReceiver_(
				ip,
				msgRcvFunc,
				0,
				kWantVideoFrameTick,
				(long) inActorInfo);
            
            info->fingerMovie->load("rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov");
            info->fingerMovie->play();

		}
		
		// set the needs draw flag so that we will be drawn as soon
		// as possible
		info->mNeedsDraw = true;
	
	// ------------------------
	// DEACTIVATE
	// ------------------------
	
	} else {
	
		// dispose our message receiver when we are deactivated.
		if (info->mMessageReceiver != nil) {
			DisposeMessageReceiver_(ip, info->mMessageReceiver);
			info->mMessageReceiver = nil;
			info->mNeedsDraw |= true;
		}
		
		// ### dispose any data that you don't need when 
		// you are not active.
		DisposeOwnedImageBuffers(ip, &info->mImageBufferMap);
		ClearSourceBuffers(ip, &info->mImageBufferMap);
        info->fingerMovie->stop();
	}
}

// ---------------------------------------------------------------------------------
//		¥ GetParameterString
// ---------------------------------------------------------------------------------
//	Returns the property definition string. Called when an instance of the actor
//	needs to be instantiated.

static const char*
GetParameterString(
	IsadoraParameters*	/* ip */,
	ActorInfo*			/* inActorInfo */)
{
	return sPropertyDefinitionString;
}

// ---------------------------------------------------------------------------------
//		¥ GetHelpString
// ---------------------------------------------------------------------------------
//	Returns the help string for a particular property. If you have a fixed number of
//	input and output properties, it is best to use the PropertyTypeAndIndexToHelpIndex_
//	function to determine the correct help string to return.

static void
GetHelpString(
	IsadoraParameters*	ip,
	ActorInfo*			inActorInfo,
	PropertyType		inPropertyType,			// kPropertyTypeInvalid when requesting help for the actor
												// or kInputProperty or kOutputProperty when requesting help for a specific property
	PropertyIndex		inPropertyIndex1,		// the one-based index of the property (when inPropertyType is not kPropertyTypeInvalid)
	char*				outParamaterString,		// receives the help string
	UInt32				inMaxCharacters)		// size of the outParamaterString buffer
{
	const char* helpstr = nil;
	
	// The PropertyTypeAndIndexToHelpIndex_ converts the inPropertyType and
	// inPropertyIndex1 parameters to determine the zero-based index into
	// your list of help strings.
	UInt32 index1 = PropertyTypeAndIndexToHelpIndex_(ip, inActorInfo, inPropertyType, inPropertyIndex1);
	
	// get the help string
	helpstr = sHelpStrings[index1];
	
	// copy it to the output string
	strncpy(outParamaterString, helpstr, inMaxCharacters);
}
	
// ---------------------------------------------------------------------------------
//		¥ HandlePropertyChangeValue	[INTERRUPT SAFE]
// ---------------------------------------------------------------------------------
//	### This function is called whenever one of the input values of an actor changes.
//	The one-based property index of the input is given by inPropertyIndex1.
//	The new value is given by inNewValue, the previous value by inOldValue.
//
static void
HandlePropertyChangeValue(
	IsadoraParameters*	ip,
	ActorInfo*			inActorInfo,
	PropertyIndex		inPropertyIndex1,			// the one-based index of the property than changed values
	ValuePtr			/* inOldValue */,			// the property's old value
	ValuePtr			inNewValue,					// the property's new value
	Boolean				/* inInitializing */)		// true if the value is being set when an actor is first initalized
{
	PluginInfo* info = GetPluginInfo_(inActorInfo);

	// ### When you add/change/remove properties, you will need to add cases
	// to this switch statement, to process the messages for your
	// input properties
	
	// The value comes to you encapsulated in a Value structure. See 
	// ValueCommon.h for details about the contents of this structure.
	
	switch (inPropertyIndex1) {
	// ### The red, green and blue inputs are all processed in the same way:
	// the floating point value is in the u.fvalue member of the Value struct
	// pointed to by inNewValue. Because the min and max values specfieid in
	// the Property Definition string was -100 to +100, the resulting value
	// here is -1.0 to +1.0.
	
	case kInputRed:
		{
			// set new red amount, as value between 0.0 and 1.0
			info->mRedAmount = inNewValue->u.fvalue / 100.0;
			// set mNeedsDraw flag to ensure that new video image is drawn
			info->mNeedsDraw = true;
		}
		break;
	
	case kInputGreen:
		{
			// set new green amount, as value between 0.0 and 1.0
			info->mGreenAmount = inNewValue->u.fvalue / 100.0;
			// set mNeedsDraw flag to ensure that new video image is drawn
			info->mNeedsDraw = true;
		}
		break;
	
	case kInputBlue:
		{
			// set new blue amount, as value between 0.0 and 1.0
			info->mBlueAmount = inNewValue->u.fvalue / 100.0;
			// set mNeedsDraw flag to ensure that new video image is drawn
			info->mNeedsDraw = true;
		}
		break;
    case kInputRtsp:
        {
            // set new blue amount, as value between 0.0 and 1.0
            info->mRtsp = inNewValue->u.str;
            // set mNeedsDraw flag to ensure that new video image is drawn
            info->mNeedsDraw = true;
        }
        break;
            
	
	// When the bypass input property is set to 0 (off = default) this
	// actor processes the video normally. When it is set to 1 (on) all
	// processing is bypassed and the input video is passed directly
	// to the output.
	case kInputBypass:
		// store member variable for on/off
		info->mBypass = (inNewValue->u.ivalue != 0);
		
		// if "bypass" is going from on to off, we need to
		// reallocate our message receiver so that we will
		// start receiving video frame tick messages again
		if (info->mBypass == false) {
			if (info->mMessageReceiver == nil) {
				info->mMessageReceiver = CreateActorMessageReceiver_(
					ip,
					inActorInfo,
					ReceiveMessage,
					0,
					kWantVideoFrameTick,
					(long) inActorInfo);
			}
			
		// if "bypass" is going from off to on, then we want to
		// stop processing video. We dispose our message receiver
		// here to save processing power -- when bypass is "on" the
		// incoming video is sent directly to the output -- see the
		// case kInputVideoIn above.
		} else {
			if (info->mMessageReceiver != nil) {
				DisposeMessageReceiver_(ip, info->mMessageReceiver);
				info->mMessageReceiver = nil;
				info->mNeedsDraw |= true;
			}
		}
		break;
	}
}


// ---------------------------------------------------------------------------------
//		¥ GetActorDefinedArea
// ---------------------------------------------------------------------------------
//	If the mGetActorDefinedAreaProc in the ActorInfo struct points to this function,
//	it indicates to Isadora that the object would like to draw either an icon or else
//	an graphic representation of its function.
//
//	### This function uses the 'PICT' 0 resource stored with the plugin to draw an icon.
//  You should replace this picture (located in the Plugin Resources.rsrc file) with
//  the icon for your actor.
// 
static ActorPictInfo	gPictInfo = { false, nil, nil, 0, 0 };

static Boolean
GetActorDefinedArea(
	IsadoraParameters*			ip,			
	ActorInfo*					inActorInfo,
	SInt16*						outTopAreaWidth,			// returns the width to reserve for the top Actor Defined Area
	SInt16*						outTopAreaMinHeight,		// returns the minimum height of the top area
	SInt16*						outBotAreaHeight,			// returns the width to reserve for the bottom Actor Defined Area
	SInt16*						outBotAreaMinWidth)			// returns the minimum width of the bottom area
{
	if (gPictInfo.mInitialized == false) {
		PrepareActorDefinedAreaPict_(ip, inActorInfo, 0, &gPictInfo);
	}
	
	// place picture in top area
	*outTopAreaWidth = gPictInfo.mWidth;
	*outTopAreaMinHeight = gPictInfo.mHeight;
	
	// don't draw anything in bottom area
	*outBotAreaHeight = 0;
	*outBotAreaMinWidth = 0;
	
	return true;
}

// ---------------------------------------------------------------------------------
//		¥ DrawActorDefinedArea
// ---------------------------------------------------------------------------------
//	If GetActorDefinedArea is defined, then this function will be called whenever
//	your ActorDefinedArea needs to be drawn.
//
//	Beacuse we are using the PICT 0 resource stored with this plugin, we can use
//	the DrawActorDefinedAreaPict_ supplied by the Isadora callbacks.
//
//  DrawActorDefinedAreaPict_ is Alpha Channel aware, so you can have nice
//	shading if you like.

static void
DrawActorDefinedArea(
	IsadoraParameters*			ip,
	ActorInfo*					inActorInfo,
	void*						/* inDrawingContext */,		// unused at present
	ActorDefinedAreaPart		inActorDefinedAreaPart,		// the part of the actor that needs to be drawn
	ActorAreaDrawFlagsT			/* inAreaDrawFlags */,		// actor draw flags
	Rect*						inADAArea,					// rect enclosing the entire Actor Defined Area
	Rect*						/* inUpdateArea */,			// subset of inADAArea that needs updating
	Boolean						inSelected)					// TRUE if actor is currently selected
{
	if (inActorDefinedAreaPart == kActorDefinedAreaTop
	&& gPictInfo.mInitialized == true) {
		DrawActorDefinedAreaPict_(ip, inActorInfo, inSelected, inADAArea, &gPictInfo);
	}
}
	
// ---------------------------------------------------------------------------------
//		¥ GetActorInfo
// ---------------------------------------------------------------------------------
//	This is function is called by to get the actor's class and ID, and to get
//	pointers to the all of the plugin functions declared locally.
//
//	All members of the ActorInfo struct pointed to by outActorParams have been
//	set to 0 on entry. You only need set functions defined by your plugin
//	
EXPORT_ void
GetActorInfo(
	void*				/* inParam */,
	ActorInfo*			outActorParams)
{
	// REQUIRED information
	outActorParams->mActorName							= kActorName;
	outActorParams->mClass								= kActorClass;
	outActorParams->mID									= kActorID;
	outActorParams->mCompatibleWithVersion				= kCurrentIsadoraCallbackVersion;
	
	// REQUIRED functions
	outActorParams->mGetActorParameterStringProc		= GetParameterString;
	outActorParams->mGetActorHelpStringProc				= GetHelpString;
	outActorParams->mCreateActorProc					= CreateActor;
	outActorParams->mDisposeActorProc					= DisposeActor;
	outActorParams->mActivateActorProc					= ActivateActor;
	outActorParams->mHandlePropertyChangeValueProc		= HandlePropertyChangeValue;
	
	// OPTIONAL FUNCTIONS
	outActorParams->mHandlePropertyChangeTypeProc		= NULL;
	outActorParams->mHandlePropertyConnectProc			= NULL;
	outActorParams->mPropertyValueToStringProc			= NULL;
	outActorParams->mPropertyStringToValueProc			= NULL;
	outActorParams->mGetActorDefinedAreaProc			= GetActorDefinedArea;
	outActorParams->mDrawActorDefinedAreaProc			= DrawActorDefinedArea;
	outActorParams->mMouseTrackInActorDefinedAreaProc	= NULL;
}


// ---------------------------------------------------------------------------------
//		¥ ProcessVideoFrame
// ---------------------------------------------------------------------------------
//	### This is the code that does the actual processing of a video frame. Modify
// this code to create your own filter.
//

static void
ProcessVideoFrame(
	IsadoraParameters*	/* ip */,	// not used in this function, but needed to call PluginAssert_
	PluginInfo*			info,
	ImageBufferPtr		outBuf)
{
	UInt32* srcData;// = static_cast<UInt32*>(srcBuf->mBaseAddress);
	UInt32 srcStride;// = srcBuf->mRowBytes - srcBuf->mWidth * sizeof(UInt32);

	UInt32* outData = static_cast<UInt32*>(outBuf->mBaseAddress);
	UInt32 outStride = outBuf->mRowBytes - outBuf->mWidth * sizeof(UInt32);
	
	// ### this is where your video processing code would go
	// here, we are increasing or decreasing the red, green,
	// and blue components of the video image.
	SInt16 redFactor = static_cast<SInt16>(256.0 * info->mRedAmount);
	SInt16 greenFactor = static_cast<SInt16>(256.0 * info->mGreenAmount);
	SInt16 blueFactor = static_cast<SInt16>(256.0 * info->mBlueAmount);
	
	// for each row
	SInt16 row = 0;
    info->fingerMovie->update();
    
    ofPixels pxls = info->fingerMovie->getPixels();
    
    int channelCount = info->fingerMovie->getPixelsRef().getNumChannels();
    int w = info->fingerMovie->getWidth();
    int h = info->fingerMovie->getHeight();
    // remove getData()
//    for(int y = 0; y < h; y++) {
//        unsigned char * cursor = info->fingerMovie->getPixels() + ((w * channelCount) * y);
//        for(int x = 0; x < w - 1; x++) {
//    
//            //unsigned char* pxData = pxls.getConstPixelsIter()Data();
//        }
//    }

    
	while (row < outBuf->mHeight) {
		
		// and for each column in that row
		SInt16 col = 0;
		while (col < outBuf->mWidth) {
		
			// IMRORTANT: For Mac/Windows cross-platform compatbility,
			// make sure to use the RED_, GREEN_ and BLUE_ macros to
			// extract pixels from a the raw data, and use RGB_ to 
			// combine them back.
			//
			// On the Mac, the pixels are arranged 00RRGGBB with
			// the blue in the low 8 bits.
			// Under Quicktime for Windows, the data is arranged
			// BBGGRR00 - in reverse order. Using the macros is
			// a easy way to ensure your code will operate on 
			// both platforms.
			//
			
			SInt16 red = RED_(*srcData);
			SInt16 green = GREEN_(*srcData);
			SInt16 blue = BLUE_(*srcData);
			
			// adjust the colors
			red += (red * redFactor) / 256;
			if (red < 0)
				red = 0;
			else if (red > 255)
				red = 255;
			
			green += (green * greenFactor) / 256;
			if (green < 0)
				green = 0;
			else if (green > 255)
				green = 255;
			
			blue += (blue * blueFactor) / 256;
			if (blue < 0)
				blue = 0;
			else if (blue > 255)
				blue = 255;

			*(outData) = ARGB_(255, red, green, blue);
			
			// increment src and out pixel
			srcData++;
			outData++;
			
			// increment column count
			col++;
		}

		// skip to the next line of video, using the
		// stride values computed above
		srcData = (UInt32*)((char*) srcData + srcStride);
		outData = (UInt32*)((char*) outData + outStride);
		
		// increment row count
		row++;
	}
}

// ---------------------------------------------------------------------------------
//		¥ ReceiveMessage
// ---------------------------------------------------------------------------------
//	Isadora broadcasts messages to its Message Receives depending on what message
//	they are listening to. In this case, we are listening for kWantVideoFrameTick,
//	which is broadcast periodically (30 times per second.) When we receive the
//	message, we check to see if our video frame needs to be updated. If so, we
//	process the incoming video and pass the newly generated frame to the output.

static void
ReceiveMessage(
	IsadoraParameters*	ip,
	MessageMask			/* inMessageMask */,		// the message that caused this ReceiveMessage to be called -- one of the kWant... constants
	PortIndex			/* inIndex1 */,				// for MIDI messages, the port from which the message arrived. n/a otherwise
	const MsgData*		/* inData */,				// the data associated with this message
	UInt32				/* inLen */,				// the length of the data associated with this message
	long				inRefCon)					// in our use, actually the pointer to our ActorInfo
{
	// Convert the refCon into the ActorInfo* that it
	// really is, so that we can get at our data
	ActorInfo* actorInfo = reinterpret_cast<ActorInfo*>(inRefCon);

	// get pointer to plugin info
	PluginInfo* info = GetPluginInfo_(actorInfo);

	// We use this Value struct in a few places below...
	Value v = { kData, nil };

	// set a flag to remember if we had an output buffer before we
	// called UpdateImageBufferMap
	Boolean wasOutputBuffer = info->mImageBufferMap.mOutputBuffersValid;
	
	// ensure that the ImageBufferMap is up valid for the
	// current input Image Buffer
	UpdateImageBufferMap(ip, &info->mImageBufferMap);
	
	// use GetImageBufferPtr to get the input and output buffers
	ImageBufferPtr out = GetOutputImageBufferPtr(&info->mImageBufferMap, 0);

    info->fingerMovie->update();
    
    if (info->fingerMovie->isFrameNew() && info->mNeedsDraw && out != nil){
        bool drawFrame = false;
        if (out->mBitDepth == 32) {
			ProcessVideoFrame(ip, info, out);
			drawFrame = true;
		}
        
        // if the drawFrame flag got set, then we need to output the
		// new video data to our output port here.
		
		if (drawFrame) {
			
			// IMPORTANT: We have changed the data in the output buffer
			// so we need to increment the data change count so that
			// those looking at our data will know that there is new
			// data in the buffer
			
			out->mInfo.mDataChangeCount++;
			v.u.data = out;
			
			// send the new video frame to the video output property
			SetOutputPropertyValue_(ip, info->mActorInfoPtr, kOutputVideo, &v);
		}
    }
    
	// if we don't have a valid output buffer

	if (info->mImageBufferMap.mOutputBuffersValid == false) {
		
		// if there was an output buffer preivously, we need to 
		// send a 'nil' buffer to let other modules know that 
		// our ouptut is now invalid
		if (wasOutputBuffer) {
			v.u.data = nil;
			SetOutputPropertyValue_(ip, actorInfo, kOutputVideo, &v);
		}

	// otherwise, if our mNeedsDraw flag is true, and if we have both
	// and input buffer and an output buffer, then we can proceed to 
	// process the video image
	// 
	// we only draw the image if the following are true:
	// 1) the mNeedsDraw variable is set to true (this is set in the
	//    InputPropertyChangeValue callback above.)
	// 2) the input image buffer (img1) is not nil
	// 3) the output image buffer (out) is not nil
	
	} else if (info->mNeedsDraw && out != nil) {

		// call EnterVideoProcessing_ so that Isadora will accumulate the
		// amount of time spent processing the video data - this is not
		// requried by highly recommended so that the VPO value in the
		// Status Window stays accurate.
		UInt64 vpStart = EnterVideoProcessing_(ip);
		
		// clear the mNeedsDraw flag
		info->mNeedsDraw = false;
	
		// assume for the moment that we won't draw the frame
		// set this value to true if we change the output
		Boolean drawFrame = false;
		
		// we only process 32 bit data in this plugin
		if (out->mBitDepth == 32) {
			ProcessVideoFrame(ip, info, out);
			drawFrame = true;
		}
		
		// if the drawFrame flag got set, then we need to output the
		// new video data to our output port here.
		
		if (drawFrame) {
			
			// IMPORTANT: We have changed the data in the output buffer
			// so we need to increment the data change count so that
			// those looking at our data will know that there is new
			// data in the buffer
			
			out->mInfo.mDataChangeCount++;
			v.u.data = out;
			
			// send the new video frame to the video output property
			SetOutputPropertyValue_(ip, info->mActorInfoPtr, kOutputVideo, &v);
		}
		
		// make sure to compliment EnterVideoProcessing_ with 
		// and ExitVideoProcessing_ call
		ExitVideoProcessing_(ip, vpStart);
	}
}
