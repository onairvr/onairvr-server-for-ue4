/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "RHIResources.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "onairvr_server.h"
#include "Windows/HideWindowsPlatformTypes.h"

class FAirVRRenderCameraRigQueue
{
public:
    struct RenderCameraRigRequest
    {
        FQuat Orientation;
		double TrackingTimeStamp;
        FIntRect SrcRectEncode;
        FIntRect SrcRectScreen;
        FIntRect ScreenViewport;
        bool bEncode;
        int PlayerID;

        RenderCameraRigRequest(const FQuat& InOrientation, double InTrackingTimeStamp, const FIntRect& InSrcRectEncode, const FIntRect& InSrcRectScreen, const FIntRect& InScreenViewport, int InPlayerID = -1)
        {
            Orientation = InOrientation;
			TrackingTimeStamp = InTrackingTimeStamp;
            SrcRectEncode = InSrcRectEncode;
            SrcRectScreen = InSrcRectScreen;
            ScreenViewport = InScreenViewport;
            PlayerID = InPlayerID;
        }

        bool ShouldEncode()
        {
            return SrcRectEncode.Area() > 0;
        }
    };

    struct Item {
        TArray<RenderCameraRigRequest> RenderCameraRigRequests;
        FRHITexture2D* RenderTarget;
    };

    void Enqueue(const Item& InItem)
    {
        check(InItem.RenderTarget);
        Queue.Enqueue(InItem);
    }

    bool Dequeue(const FRHITexture2D* RenderTarget, Item& OutItem)
    {
        while (Queue.Dequeue(OutItem)) {
            if (OutItem.RenderTarget == RenderTarget) {
                return true;
            }
        }
        return false;
    }

private:
    TQueue<Item> Queue;
};
