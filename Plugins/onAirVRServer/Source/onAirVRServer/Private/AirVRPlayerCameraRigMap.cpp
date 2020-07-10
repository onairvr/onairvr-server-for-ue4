/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRPlayerCameraRigMap.h"
#include "AirVRServerPrivate.h"

#include "AirVRCameraRig.h"

int32 FAirVRPlayerCameraRigMap::GetPlayer(class FAirVRCameraRig* CameraRig) const
{
    for (const auto& It : BoundPlayers) {
        if (It.Value.CameraRig == CameraRig) {
            return It.Key;
        }
    }

    for (const auto& It : UnboundPlayers) {
        if (It.Value.CameraRig == CameraRig) {
            return It.Key;
        }
    }
    return -1;
}

void FAirVRPlayerCameraRigMap::GetItemsOfBoundPlayers(TArray<Item>& Result) const
{
    for (const auto& It : BoundPlayers) {
        Result.Add(It.Value);
    }
}

void FAirVRPlayerCameraRigMap::GetItemsOfUnboundPlayers(TArray<Item>& Result) const
{
    for (const auto& It : UnboundPlayers) {
        Result.Add(It.Value);
    }
}

void FAirVRPlayerCameraRigMap::MarkCameraRigAsBound(const class FAirVRCameraRig* CameraRig)
{
    for (const auto& It : UnboundPlayers) {
        if (It.Value.CameraRig == CameraRig) {
            check(BoundPlayers.Contains(It.Key) == false);

            BoundPlayers.Add(It.Key, It.Value);
            UnboundPlayers.Remove(It.Key);
            break;
        }
    }
}

void FAirVRPlayerCameraRigMap::MarkCameraRigAsUnbound(const class FAirVRCameraRig* CameraRig)
{
    for (const auto& It : BoundPlayers) {
        if (It.Value.CameraRig == CameraRig) {
            check(UnboundPlayers.Contains(It.Key) == false);

            UnboundPlayers.Add(It.Key, It.Value);
            BoundPlayers.Remove(It.Key);
            break;
        }
    }
}

void FAirVRPlayerCameraRigMap::Reset()
{
    for (auto& It : BoundPlayers) {
        check(UnboundPlayers.Contains(It.Key) == false);

        It.Value.CameraRig->Reset();
        UnboundPlayers.Add(It.Key, It.Value);
    }
    BoundPlayers.Empty();
}

void FAirVRPlayerCameraRigMap::OnStartGameFrame(FWorldContext& WorldContext)
{
    UpdatePlayerLists(WorldContext);
}

void FAirVRPlayerCameraRigMap::UpdateCameraRigs(FWorldContext& WorldContext)
{
    for (auto& It : BoundPlayers) {
        It.Value.CameraRig->Update(WorldContext);
    }
}

void FAirVRPlayerCameraRigMap::OnEndGameFrame(FWorldContext& WorldContext, FVector2D& MaxRenderTargetSize)
{
    FVector2D ScreenSize;
    WorldContext.GameViewport->GetViewportSize(ScreenSize);
    UpdateViewports(ScreenSize, WorldContext.GameViewport->GetCurrentSplitscreenConfiguration(), MaxRenderTargetSize);
}

void FAirVRPlayerCameraRigMap::AddUniqueUnboundPlayer(int32 ControllerID)
{
    if (UnboundPlayers.Contains(ControllerID) == false && BoundPlayers.Contains(ControllerID) == false) {
        FAirVRPlayerCameraRigMap::Item Item(new FAirVRCameraRig(EventDispatcher));
        UnboundPlayers.Add(ControllerID, Item);
    }
}

void FAirVRPlayerCameraRigMap::RemovePlayerIfExists(int32 ControllerID)
{
    check(UnboundPlayers.Contains(ControllerID) == false || BoundPlayers.Contains(ControllerID) == false);

    Item Item;
    if (UnboundPlayers.Contains(ControllerID)) {
        UnboundPlayers.RemoveAndCopyValue(ControllerID, Item);
    }
    else if (BoundPlayers.Contains(ControllerID)) {
        BoundPlayers.RemoveAndCopyValue(ControllerID, Item);
    }

    check(Item.CameraRig);
    delete Item.CameraRig;
}

void FAirVRPlayerCameraRigMap::UpdatePlayerLists(FWorldContext& WorldContext)
{
    if (WorldContext.OwningGameInstance == nullptr) {
        return;
    }

    TArray<int32> AllControllerIDs;
    BoundPlayers.GetKeys(AllControllerIDs);
    UnboundPlayers.GetKeys(AllControllerIDs);

    for (auto PlayerIt = WorldContext.OwningGameInstance->GetLocalPlayerIterator(); PlayerIt; ++PlayerIt) {
        if (AllControllerIDs.Contains((*PlayerIt)->GetControllerId())) {
            AllControllerIDs.Remove((*PlayerIt)->GetControllerId());
        }
        else {
            AddUniqueUnboundPlayer((*PlayerIt)->GetControllerId());
        }
    }

    for (auto ID : AllControllerIDs) {
        RemovePlayerIfExists(ID);
    }
}

void FAirVRPlayerCameraRigMap::UpdateViewports(FVector2D ScreenSize, ESplitScreenType::Type SplitScreenType, FVector2D& MaxRenderTargetSize)
{
    const uint32 Padding = 2;

    TMap<int32, Item> Pairs;
    for (auto& It : UnboundPlayers) {
        Pairs.Add(It.Key, It.Value);
    }
    for (auto& It : BoundPlayers) {
        check(Pairs.Contains(It.Key) == false);
        Pairs.Add(It.Key, It.Value);
    }
    Pairs.KeySort([](const int32 L, const int32 R) {
        return L < R;
    });

    if (Pairs.Num() > 0) {
        int Panel = 0;
        FIntPoint Min(0, 0);
        for (auto& Pair : Pairs) {
            FAirVRPlayerCameraRigMap::Item Item = Pair.Value;
            if (CalculateNextScreenViewport(SplitScreenType, ScreenSize, Panel, Item.ScreenViewport) == false) {
                Item.ScreenViewport = FIntRect();
            }
            Item.CameraRig->UpdateViewInfo(Item.ScreenViewport, Item.bEncode, Item.bIsStereoscopic);
            if (Item.CameraRig->GetVideoWidth() > 0 && Item.CameraRig->GetVideoHeight()) {
                Item.RenderViewport = Item.bIsStereoscopic ?
                    FIntRect(Min, Min + FIntPoint(Item.CameraRig->GetVideoWidth(), Item.CameraRig->GetVideoHeight())) :
                    FIntRect(Min, Min + FIntPoint(Item.CameraRig->GetVideoWidth() * 2, Item.CameraRig->GetVideoHeight()));
            }
            else {
                Item.RenderViewport = FIntRect();
            }
            
            if (UnboundPlayers.Contains(Pair.Key)) {
                UnboundPlayers.Add(Pair.Key, Item);
            }
            else if (BoundPlayers.Contains(Pair.Key)) {
                BoundPlayers.Add(Pair.Key, Item);
            }

            MaxRenderTargetSize.X = FMath::Max<uint32>(MaxRenderTargetSize.X, (uint32)Item.RenderViewport.Width());
            MaxRenderTargetSize.Y = FMath::Max<uint32>(MaxRenderTargetSize.Y, (uint32)(Min.Y + Item.RenderViewport.Height()));
            
            Min.Y += Item.RenderViewport.Height() + Padding;
        }
    }
}

bool FAirVRPlayerCameraRigMap::CalculateNextScreenViewport(ESplitScreenType::Type SplitScreenType, FVector2D ScreenSize, int& InOutPanel, FIntRect& Result)
{
    if (InOutPanel < 0 ||
        (SplitScreenType == ESplitScreenType::None && InOutPanel != 0) ||
        ((SplitScreenType == ESplitScreenType::TwoPlayer_Horizontal || SplitScreenType == ESplitScreenType::TwoPlayer_Vertical) && InOutPanel >= 2) ||
        ((SplitScreenType == ESplitScreenType::ThreePlayer_FavorTop || SplitScreenType == ESplitScreenType::ThreePlayer_FavorBottom) && InOutPanel >= 3) ||
        ((SplitScreenType == ESplitScreenType::FourPlayer_Grid || SplitScreenType == ESplitScreenType::FourPlayer_Vertical) && InOutPanel >= 4)) {
        return false;
    }

    int ScreenWidth = (int)ScreenSize.X;
    int ScreenHeight = (int)ScreenSize.Y;

    switch (SplitScreenType) {
        case ESplitScreenType::None:
            Result = FIntRect(0, 0, ScreenWidth, ScreenHeight);
            break;
        case ESplitScreenType::TwoPlayer_Horizontal:
            Result = FIntRect(0, InOutPanel * ScreenHeight / 2, ScreenWidth, (InOutPanel + 1) * ScreenHeight / 2);
            break;
        case ESplitScreenType::TwoPlayer_Vertical:
            Result = FIntRect(InOutPanel * ScreenWidth / 2, 0, (InOutPanel + 1) * ScreenWidth / 2, ScreenHeight);
            break;
        case ESplitScreenType::ThreePlayer_FavorTop:
            Result = FIntRect(InOutPanel != 2 ? 0 : ScreenWidth / 2, InOutPanel == 0 ? 0 : ScreenHeight / 2,
                              InOutPanel == 1 ? ScreenWidth / 2 : ScreenWidth, InOutPanel == 0 ? ScreenHeight / 2 : ScreenHeight);
            break;
        case ESplitScreenType::ThreePlayer_FavorBottom:
            Result = FIntRect(InOutPanel != 1 ? 0 : ScreenWidth / 2, InOutPanel != 2 ? 0 : ScreenHeight / 2,
                              InOutPanel == 0 ? ScreenWidth / 2 : ScreenWidth, InOutPanel != 2 ? ScreenHeight / 2 : ScreenHeight);
            break;
        case ESplitScreenType::FourPlayer_Grid:
        case ESplitScreenType::FourPlayer_Vertical:
            Result = FIntRect((InOutPanel % 2) * ScreenWidth / 2, (InOutPanel / 2) * ScreenHeight / 2,
                              (InOutPanel % 2 + 1) * ScreenWidth / 2, (InOutPanel / 2 + 1) * ScreenHeight / 2);
            break;
        default:
            return false;
    }
    
    InOutPanel++;
    return true;
}
