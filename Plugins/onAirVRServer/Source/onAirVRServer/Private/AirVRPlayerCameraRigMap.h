/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRCameraRig.h"

class FAirVRPlayerCameraRigMap
{
public:
    struct Item 
    {
        class FAirVRCameraRig* CameraRig;
        FIntRect RenderViewport;
        FIntRect ScreenViewport;
        bool bEncode;
        bool bIsStereoscopic;

    public:
        Item() : CameraRig(nullptr), bEncode(false), bIsStereoscopic(false) {}

        Item(class FAirVRCameraRig* InCameraRig, FIntRect InRenderViewport = FIntRect(), FIntRect InScreenViewport = FIntRect(), bool bInEncode = false, bool bInIsStereoscopic = false)
            : CameraRig(InCameraRig), RenderViewport(InRenderViewport), ScreenViewport(InScreenViewport), bEncode(bInEncode), bIsStereoscopic(bInIsStereoscopic)
        {}

        bool IsValid() const  { return CameraRig != nullptr; }

        FIntRect GetRenderViewport(EStereoscopicPass StereoPass) const
        {
            if (bIsStereoscopic) {
                switch (StereoPass) {
                    case eSSP_LEFT_EYE:
                        return FIntRect(RenderViewport.Min, FIntPoint(RenderViewport.Max.X - RenderViewport.Width() / 2, RenderViewport.Max.Y));
                    case eSSP_RIGHT_EYE:
                        return FIntRect(FIntPoint(RenderViewport.Min.X + RenderViewport.Width() / 2, RenderViewport.Min.Y), RenderViewport.Max);
                    default:
                        break;
                }
            }
            return RenderViewport;
        }

        bool ShouldRender() const
        {
            check(CameraRig);
            return RenderViewport.Area() > 0;
        }

        void Reset()
        {
            CameraRig = nullptr;
            bEncode = false;
            bIsStereoscopic = false;
        }
    };

public:
    FAirVRPlayerCameraRigMap(class FAirVREventDispatcher* InEventDispatcher) : EventDispatcher(InEventDispatcher) {}

    void GetBoundPlayers(TArray<int32>& Result) const
    {
        BoundPlayers.GetKeys(Result);
    }

    void GetUnboundPlayers(TArray<int32>& Result) const
    {
        UnboundPlayers.GetKeys(Result);
    }

    int32 GetAnyUnboundPlayer() const
    {
        if (UnboundPlayers.Num() == 0) {
            return -1;
        }

        TArray<int32> Players;
        UnboundPlayers.GetKeys(Players);

        return Players.Num() > 0 ? Players[0] : -1;
    }

    bool GetItem(int32 PlayerControllerID, Item& Result) const
    {
        if (BoundPlayers.Contains(PlayerControllerID)) {
            Result = BoundPlayers.FindRef(PlayerControllerID);
            return true;
        }
        else if (UnboundPlayers.Contains(PlayerControllerID)) {
            Result = UnboundPlayers.FindRef(PlayerControllerID);
            return true;
        }
        return false;
    }

    bool GetItemOfBoundPlayer(int PlayerID, Item& Result) const
    {
        for (auto& Pair : BoundPlayers) {
            if (Pair.Value.CameraRig->GetPlayerID() == PlayerID) {
                Result = Pair.Value;
                return true;
            }
        }
        return false;
    }

    int32 GetPlayer(class FAirVRCameraRig* CameraRig) const;

    void GetItemsOfBoundPlayers(TArray<Item>& Result) const;
    void GetItemsOfUnboundPlayers(TArray<Item>& Result) const;

    void MarkCameraRigAsBound(const class FAirVRCameraRig* CameraRig);
    void MarkCameraRigAsUnbound(const class FAirVRCameraRig* CameraRig);
    void Reset();

    void OnStartGameFrame(FWorldContext& WorldContext);
    void UpdateCameraRigs();
    void OnEndGameFrame(FWorldContext& WorldContext, FVector2D& MaxRenderTargetSize);

private:
    void AddUniqueUnboundPlayer(int32 ControllerID);
    void RemovePlayerIfExists(int32 ControllerID);
    void UpdatePlayerLists(FWorldContext& WorldContext);
    void UpdateViewports(FVector2D ScreenSize, ESplitScreenType::Type SplitScreenType, FVector2D& MaxRenderTargetSize);
    bool CalculateNextScreenViewport(ESplitScreenType::Type SplitScreenType, FVector2D ScreenSize, int& InOutPanel, FIntRect& Result);

private:
    class FAirVREventDispatcher* EventDispatcher;
    TMap<int32, Item> UnboundPlayers;
    TMap<int32, Item> BoundPlayers;
};
