/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include <cstdint>

#ifdef _WIN32
    #define ONAIRVR_EXPORT __declspec(dllexport)
#else
    #define ONAIRVR_EXPORT
#endif

#ifdef _WIN32
    #define ONAIRVR_CALL __stdcall
#else
    #define ONAIRVR_CALL
#endif


#ifdef ONAIRVRUNREALSERVERPLUGIN_EXPORTS
    #define ONAIRVR_API ONAIRVR_EXPORT ONAIRVR_CALL
#else
    #define ONAIRVR_API ONAIRVR_CALL 
#endif

#define ONAIRVR_RESULT_OK                       0
#define ONAIRVR_RESULT_NOT_SUPPORT_GPU          0x00010001
#define ONAIRVR_RESULT_LICENSE_NOT_VERIFIED     0x00020001
#define ONAIRVR_RESULT_LICENSE_NOT_FOUND        0x00020002
#define ONAIRVR_RESULT_LICENSE_INVALID          0x00020003
#define ONAIRVR_RESULT_LICENSE_EXPIRED          0x00020004

typedef enum
{
    ONAIRVR_CLIENT_NONE,
    ONAIRVR_CLIENT_MONOSCOPIC,
    ONAIRVR_CLIENT_STEREOSCOPIC
} ONAIRVR_CLIENT_TYPE;

typedef enum
{
    ONAIRVR_INPUT_SENDING_POLICY_NEVER = 0,
    ONAIRVR_INPUT_SENDING_POLICY_ALWAYS,
    ONAIRVR_INPUT_SENDING_POLICY_NONZERO_ALWAYS_ZERO_ONCE
} ONAIRVR_INPUT_SENDING_POLICY;

typedef struct _ONAIRVR_VECTOR3D
{
    float x;
    float y;
    float z;

    _ONAIRVR_VECTOR3D()
        : x(0), y(0), z(0) {}

    _ONAIRVR_VECTOR3D(float inX, float inY, float inZ)
        : x(inX), y(inY), z(inZ) {}
} ONAIRVR_VECTOR3D;

typedef struct _ONAIRVR_VECTOR2D
{
    float x;
    float y;

    _ONAIRVR_VECTOR2D()
        : x(0), y(0) {}

    _ONAIRVR_VECTOR2D(float inX, float inY)
        : x(inX), y(inY) {}
} ONAIRVR_VECTOR2D;

typedef struct _ONAIRVR_QUATERNION
{
    float x;
    float y;
    float z;
    float w;

    _ONAIRVR_QUATERNION() : x(0), y(0), z(0), w(1) {}
    
    _ONAIRVR_QUATERNION(float inX, float inY, float inZ, float inW) 
        : x(inX), y(inY), z(inZ), w(inW) {}
} ONAIRVR_QUATERNION;

typedef struct _ONAIRVR_VIEWPORT
{
    int x;
    int y;
    int width;
    int height;

    _ONAIRVR_VIEWPORT(int inX, int inY, int inWidth, int inHeight)
        : x(inX), y(inY), width(inWidth), height(inHeight) {}
} ONAIRVR_VIEWPORT;

typedef struct
{
    static const int MaxUserIDLength = 511;

    char userID[MaxUserIDLength + 1];
    ONAIRVR_CLIENT_TYPE clientType;
    int videoWidth;
    int videoHeight;
    float leftEyeCameraNearPlane[4];
    float frameRate;
    float ipd;
    ONAIRVR_VECTOR3D eyeCenterPosition;
} ONAIRVR_CLIENT_CONFIG;

typedef struct
{
    const void* pointerCookieTexture;
    int pointerCookieTextureSize;
    float pointerCookieDepthScaleMultiplier;
} ONAIRVR_INPUT_SENDER_ARGS;

#ifdef __cplusplus
extern "C"
{
#endif/
    void ONAIRVR_API onairvr_InitModule(void* gfxDevice, int TextureFormat);

    int ONAIRVR_API onairvr_SetLicenseFile(const char* path);
    void ONAIRVR_API onairvr_SetVideoEncoderParameters(float maxFrameRate, float defaultFrameRate, int maxBitRate, int defaultBitRate, int gopCount);
    int ONAIRVR_API onairvr_StartUp(int maxClientCount, int portSTAP, int portAMP, bool loopbackOnlyForSTAP, int audioSampleRate);
    void ONAIRVR_API onairvr_StartUp_RenderThread(void* gfxDevice);
    void ONAIRVR_API onairvr_Shutdown();
    void ONAIRVR_API onairvr_Shutdown_RenderThread();

    bool ONAIRVR_API onairvr_GetConfig(int playerID, ONAIRVR_CLIENT_CONFIG* result);
    void ONAIRVR_API onairvr_SetConfig(int playerID, const ONAIRVR_CLIENT_CONFIG& config);
    void ONAIRVR_API onairvr_RecenterPose(int playerID);
    void ONAIRVR_API onairvr_EnableNetworkTimeWarp(int playerID, bool enable);
    void ONAIRVR_API onairvr_Disconnect(int playerID);

    bool ONAIRVR_API onairvr_CheckMessageQueue(int& srcPlayerID, const char** firstMessage);
    void ONAIRVR_API onairvr_RemoveFirstMessage();

    bool ONAIRVR_API onairvr_IsStreaming(int playerID);
    void ONAIRVR_API onairvr_InitStreams_RenderThread(int playerID);
    void ONAIRVR_API onairvr_ResetStreams_RenderThread(int playerID);
    void ONAIRVR_API onairvr_CleanupStreams_RenderThread(int playerID);
    void ONAIRVR_API onairvr_EncodeVideoFrame(int playerID, const ONAIRVR_VIEWPORT& viewport, double timeStamp, const ONAIRVR_QUATERNION& orientation, void* RenderTargetTexture);
    void ONAIRVR_API onairvr_EncodeAudioFrame(int playerID, const float* data, int sampleCount, int channels, double timeStamp);
    void ONAIRVR_API onairvr_EncodeAudioFrameForAllPlayers(const float* data, int sampleCount, int channels, double timeStamp);
    void ONAIRVR_API onairvr_AdjustBitRate_RenderThread(int playerID, uint32_t bitRate);
 
    void ONAIRVR_API onairvr_AcceptPlayer(int playerID);
    void ONAIRVR_API onairvr_Update(float deltaTime);

    uint8_t ONAIRVR_API onairvr_RegisterInputSender(int playerID, const char* name, const ONAIRVR_INPUT_SENDER_ARGS* args = nullptr);
    void ONAIRVR_API onairvr_UnregisterInputSender(int playerID, uint8_t deviceID);
    void ONAIRVR_API onairvr_PendInput(int playerID, uint8_t deviceID, uint8_t controlID, const float* values, int length, ONAIRVR_INPUT_SENDING_POLICY policy);
    bool ONAIRVR_API onairvr_GetInput(int playerID, uint8_t deviceID, uint8_t controlID, float* values, int length, double* timeStamp = nullptr);
    void ONAIRVR_API onairvr_SendPendingInputs(int playerID);
    void ONAIRVR_API onairvr_ResetInput(int playerID);

#ifdef __cplusplus
}
#endif
