/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

enum class EFramebufferIndex : uint8 {
    Mono,
    LeftEye,
    RightEye
};

class IAirVRCameraRigRenderer
{
public:
    enum class Type {
        CameraRigComponent
    };

public:
    IAirVRCameraRigRenderer(Type InType) : RendererType(InType) {}

    Type GetType() const    { return RendererType; }

public:
    virtual void OnCameraRigRendererBound(class FAirVRCameraRigStreaming* CameraRigStreaming, const class UAirVRClientConfig* Config) = 0;
    virtual void OnCameraRigRendererUnbound(class FAirVRCameraRigStreaming* CameraRigStreaming) = 0;
    virtual void OnCameraRigRendererStartRender(class FAirVRCameraRigStreaming* CameraRigStreaming) = 0;
    virtual void OnCameraRigRendererStopRender(class FAirVRCameraRigStreaming* CameraRigStreaming) = 0; 
    virtual void OnCameraRigRendererPreRender(class FAirVRCameraRigStreaming* CameraRigStreaming, EFramebufferIndex FramebufferIndex, class UTextureRenderTarget2D* Framebuffer) = 0;
    virtual void OnCameraRigRendererUpdateHeadPose(class FAirVRCameraRigStreaming* CameraRigStreaming, const FQuat& Orientation) = 0;

private:
    Type RendererType;
};
