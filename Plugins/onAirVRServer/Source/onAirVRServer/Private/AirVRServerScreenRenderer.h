/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "RendererInterface.h"

class FAirVRServerScreenRenderer 
{
public:
    struct PanelRenderInfo
    {
        FIntRect RenderViewport;
        FIntRect ScreenViewport;

        PanelRenderInfo() {}
        PanelRenderInfo(const FIntRect& InRenderViewport, const FIntRect& InScreenViewport)
            : RenderViewport(InRenderViewport), ScreenViewport(InScreenViewport) {}
    };

public:
    FAirVRServerScreenRenderer();

public:
    void DrawPanels(class FRHICommandListImmediate& RHICmdList, const TArray<PanelRenderInfo>& Panels, class FRHITexture2D* ScreenTexture, class FRHITexture2D* SrcTexture) const;

private:
    FIntRect AdjustScreenViewportToPreserveAspectRatio(const FIntRect& Viewport, float AspectRatio) const;
    void DrawQuad(class FRHICommandListImmediate& RHICmdList, const FLinearColor& Color) const;

private:
    IRendererModule* RendererModule;
};