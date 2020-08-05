/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRServerScreenRenderer.h"
#include "AirVRServerPrivate.h"

#include "RendererPrivate.h"
#include "GlobalShader.h"
#include "ScreenRendering.h"
#include "PipelineStateCache.h"
#include "ClearQuad.h"

FAirVRServerScreenRenderer::FAirVRServerScreenRenderer()
{
    static const FName RendererModuleName("Renderer");
    RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
    check(RendererModule);
}

void FAirVRServerScreenRenderer::DrawPanels(class FRHICommandListImmediate& RHICmdList, const TArray<PanelRenderInfo>& Panels, class FRHITexture2D* ScreenTexture, class FRHITexture2D* SrcTexture) const
{
    const uint32 ScreenWidth = ScreenTexture->GetSizeX();
    const uint32 ScreenHeight = ScreenTexture->GetSizeY();
    const uint32 TextureWidth = SrcTexture->GetSizeX();
    const uint32 TextureHeight = SrcTexture->GetSizeY();

    FRHIRenderPassInfo RPInfo(ScreenTexture, ERenderTargetActions::DontLoad_Store);
    RHICmdList.BeginRenderPass(RPInfo, TEXT("DrawPanels"));
    RHICmdList.SetViewport(0, 0, 0, ScreenWidth, ScreenHeight, 1.0f);
    DrawQuad(RHICmdList, FLinearColor::Black);

    FGraphicsPipelineStateInitializer GraphicsPSOInit;
    RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
    GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
    GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
    GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
    GraphicsPSOInit.PrimitiveType = PT_TriangleList;

    const auto FeatureLevel = GMaxRHIFeatureLevel;
    auto ShaderMap = GetGlobalShaderMap(FeatureLevel);

    TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
    TShaderMapRef<FScreenPS> PixelShader(ShaderMap);

    GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
    GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
    GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

    SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

    PixelShader->SetParameters(RHICmdList, TStaticSamplerState<SF_Bilinear>::GetRHI(), SrcTexture);

    for (auto Panel : Panels) {
        FIntRect ScreenViewport = AdjustScreenViewportToPreserveAspectRatio(Panel.ScreenViewport, (float)Panel.RenderViewport.Width() / Panel.RenderViewport.Height());
        RendererModule->DrawRectangle(RHICmdList,
                                      ScreenViewport.Min.X, ScreenViewport.Min.Y, ScreenViewport.Width(), ScreenViewport.Height(),
                                      (float)Panel.RenderViewport.Min.X / TextureWidth, (float)Panel.RenderViewport.Min.Y / TextureHeight, 
                                      (float)Panel.RenderViewport.Width() / TextureWidth, (float)Panel.RenderViewport.Height() / TextureHeight,
                                      FIntPoint(ScreenWidth, ScreenHeight),
                                      FIntPoint(1, 1),
                                      VertexShader);
    }
    RHICmdList.EndRenderPass();
}

FIntRect FAirVRServerScreenRenderer::AdjustScreenViewportToPreserveAspectRatio(const FIntRect& Viewport, float AspectRatio) const
{
    float ViewportAspectRatio = (float)Viewport.Width() / Viewport.Height();
    if (FMath::IsNearlyZero(ViewportAspectRatio - AspectRatio)) {
        return Viewport;
    }
    
    if (ViewportAspectRatio > AspectRatio) {
        int32 NewWidth = Viewport.Height() * AspectRatio;
        int32 Left = Viewport.Min.X + (Viewport.Width() - NewWidth) / 2;
        return FIntRect(Left, Viewport.Min.Y, Left + NewWidth, Viewport.Max.Y);
    }
    else {
        int32 NewHeight = Viewport.Width() / AspectRatio;
        int32 Top = Viewport.Min.Y + (Viewport.Height() - NewHeight) / 2;
        return FIntRect(Viewport.Min.X, Top, Viewport.Max.X, Top + NewHeight);
    }
}

void FAirVRServerScreenRenderer::DrawQuad(class FRHICommandListImmediate& RHICmdList, const FLinearColor& Color) const
{
#if (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 16)
    DrawClearQuad(RHICmdList, GMaxRHIFeatureLevel, Color);
#else
    DrawClearQuad(RHICmdList, Color);
#endif
}
