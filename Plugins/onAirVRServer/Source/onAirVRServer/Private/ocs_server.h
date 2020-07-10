/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include <cstdint>

#ifdef _WIN32
    #define OCS_EXPORT __declspec(dllexport)
#else
    #define OCS_EXPORT
#endif

#ifdef _WIN32
    #define OCS_CALL __stdcall
#else
    #define OCS_CALL
#endif


#ifdef OCSUNREALSERVERPLUGIN_EXPORTS
    #define OCS_API OCS_EXPORT OCS_CALL
#else
    #define OCS_API OCS_CALL 
#endif

#define OCS_RESULT_OK                       0
#define OCS_RESULT_NOT_SUPPORT_GPU          0x00010001
#define OCS_RESULT_LICENSE_NOT_VERIFIED     0x00020001
#define OCS_RESULT_LICENSE_NOT_FOUND        0x00020002
#define OCS_RESULT_LICENSE_INVALID          0x00020003
#define OCS_RESULT_LICENSE_EXPIRED          0x00020004

typedef enum
{
    OCS_CLIENT_NONE,
    OCS_CLIENT_MONOSCOPIC,
    OCS_CLIENT_STEREOSCOPIC
} OCS_CLIENT_TYPE;

typedef enum 
{
    OCS_INPUT_DIRECTION_UP = 0,
    OCS_INPUT_DIRECTION_DOWN,
    OCS_INPUT_DIRECTION_LEFT,
    OCS_INPUT_DIRECTION_RIGHT
} OCS_INPUT_DIRECTION;

//typedef enum
//{
//    OCS_INPUT_SENDING_POLICY_NEVER = 0,
//    OCS_INPUT_SENDING_POLICY_ALWAYS,
//    OCS_INPUT_SENDING_POLICY_NONZERO_ALWAYS_ZERO_ONCE,
//    OCS_INPUT_SENDING_POLICY_ON_CHANGE
//} OCS_INPUT_SENDING_POLICY;

typedef struct _OCS_VECTOR3D
{
    float x;
    float y;
    float z;

    _OCS_VECTOR3D()
        : x(0), y(0), z(0) {}

    _OCS_VECTOR3D(float inX, float inY, float inZ)
        : x(inX), y(inY), z(inZ) {}
} OCS_VECTOR3D;

typedef struct _OCS_VECTOR2D
{
    float x;
    float y;

    _OCS_VECTOR2D()
        : x(0), y(0) {}

    _OCS_VECTOR2D(float inX, float inY)
        : x(inX), y(inY) {}
} OCS_VECTOR2D;

typedef struct _OCS_QUATERNION
{
    float x;
    float y;
    float z;
    float w;

    _OCS_QUATERNION() : x(0), y(0), z(0), w(1) {}
    
    _OCS_QUATERNION(float inX, float inY, float inZ, float inW) 
        : x(inX), y(inY), z(inZ), w(inW) {}
} OCS_QUATERNION;

typedef struct _OCS_VIEWPORT
{
    int x;
    int y;
    int width;
    int height;

    _OCS_VIEWPORT(int inX, int inY, int inWidth, int inHeight)
        : x(inX), y(inY), width(inWidth), height(inHeight) {}
} OCS_VIEWPORT;

typedef struct
{
    static const int MaxUserIDLength = 511;

    char userID[MaxUserIDLength + 1];
    OCS_CLIENT_TYPE clientType;
    int videoWidth;
    int videoHeight;
    float cameraProjection[4];
    float frameRate;
    float ipd;
    OCS_VECTOR3D eyeCenterPosition;
} OCS_CLIENT_CONFIG;

typedef struct
{
    const void* pointerCookieTexture;
    int pointerCookieTextureSize;
    float pointerCookieDepthScaleMultiplier;
} OCS_INPUT_SENDER_ARGS;

#ifdef __cplusplus
extern "C"
{
#endif/
    void OCS_API ocs_InitModule(void* gfxDevice, int TextureFormat);

    int OCS_API ocs_SetLicenseFile(const char* path);
    void OCS_API ocs_SetVideoEncoderParameters(float maxFrameRate, int gopCount);
    int OCS_API ocs_StartUp(int maxClientCount, int portSTAP, int portAMP, bool loopbackOnlyForSTAP, int audioSampleRate);
    void OCS_API ocs_StartUp_RenderThread(void* gfxDevice);
    void OCS_API ocs_Shutdown();
    void OCS_API ocs_Shutdown_RenderThread();
    void OCS_API ocs_EnableProfilers(int profilers);

    bool OCS_API ocs_GetConfig(int playerID, OCS_CLIENT_CONFIG* result);
    void OCS_API ocs_SetConfig(int playerID, const OCS_CLIENT_CONFIG& config);
    void OCS_API ocs_RecenterPose(int playerID);
    void OCS_API ocs_EnableNetworkTimeWarp(int playerID, bool enable);
    void OCS_API ocs_Disconnect(int playerID);

    bool OCS_API ocs_CheckMessageQueue(int& srcPlayerID, const char** firstMessage);
    void OCS_API ocs_RemoveFirstMessage();

    bool OCS_API ocs_IsStreaming(int playerID);
    void OCS_API ocs_InitStreams_RenderThread(int playerID);
    void OCS_API ocs_ResetStreams_RenderThread(int playerID);
    void OCS_API ocs_CleanupStreams_RenderThread(int playerID);
    void OCS_API ocs_EncodeVideoFrame(int playerID, const OCS_VIEWPORT& viewport, int64_t trackingTimestamp, const OCS_QUATERNION& orientation, void* RenderTargetTexture);
    void OCS_API ocs_EncodeAudioFrame(int playerID, const float* data, int sampleCount, int channels, double timeStamp);
    void OCS_API ocs_EncodeAudioFrameForAllPlayers(const float* data, int sampleCount, int channels, double timeStamp);
 
    void OCS_API ocs_AcceptPlayer(int playerID);
    void OCS_API ocs_Update(float deltaTime);

    //uint8_t OCS_API ocs_RegisterInputSender(int playerID, const char* name, const OCS_INPUT_SENDER_ARGS* args = nullptr);
    //void OCS_API ocs_UnregisterInputSender(int playerID, uint8_t deviceID);
    int64_t OCS_API ocs_GetInputRecvTimestamp(int playerID);
    bool OCS_API ocs_GetInputState(int playerID, uint8_t device, uint8_t control, uint8_t* state);
    bool OCS_API ocs_GetInputByteAxis(int playerID, uint8_t device, uint8_t control, uint8_t* axis);
    bool OCS_API ocs_GetInputAxis(int playerID, uint8_t device, uint8_t control, float* axis);
    bool OCS_API ocs_GetInputAxis2D(int playerID, uint8_t device, uint8_t control, OCS_VECTOR2D* axis2D);
    bool OCS_API ocs_GetInputPose(int playerID, uint8_t device, uint8_t control, OCS_VECTOR3D* position, OCS_QUATERNION* rotation);
    bool OCS_API ocs_GetInputTouch2D(int playerID, uint8_t device, uint8_t control, OCS_VECTOR2D* position, uint8_t* state);
    bool OCS_API ocs_IsInputActive(int playerID, uint8_t device, uint8_t control);
    bool OCS_API ocs_IsInputDirectionActive(int playerID, uint8_t device, uint8_t control, OCS_INPUT_DIRECTION direction);
    bool OCS_API ocs_GetInputActivated(int playerID, uint8_t device, uint8_t control);
    bool OCS_API ocs_GetInputDirectionActivated(int playerID, uint8_t device, uint8_t control, OCS_INPUT_DIRECTION direction);
    bool OCS_API ocs_GetInputDeactivated(int playerID, uint8_t device, uint8_t control);
    bool OCS_API ocs_GetInputDirectionDeactivated(int playerID, uint8_t device, uint8_t control, OCS_INPUT_DIRECTION direction);
    void OCS_API ocs_BeginPendInput(int playerID, int64_t& timestamp);
    void OCS_API ocs_PendInputState(int playerID, uint8_t device, uint8_t control, uint8_t state);
    void OCS_API ocs_PendInputRaycastHit(int playerID, uint8_t device, uint8_t control, const OCS_VECTOR3D& origin, const OCS_VECTOR3D& hitPosition, const OCS_VECTOR3D& hitNormal);
    void OCS_API ocs_PendInputVibration(int playerID, uint8_t device, uint8_t control, float frequency, float amplitude);
    void OCS_API ocs_SendPendingInputs(int playerID, int64_t timestamp);
    void OCS_API ocs_ClearInput(int playerID);
    void OCS_API ocs_UpdateInputFrame(int playerID);

#ifdef __cplusplus
}
#endif
