/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include <cassert>
#include "Engine.h"
#include "Runtime/Launch/Resources/Version.h"

#include "IAirVRServerPlugin.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "ocs_server.h"
#include "Windows/HideWindowsPlatformTypes.h"

DECLARE_LOG_CATEGORY_EXTERN(LogonAirVRServer, Verbose, All);

inline FVector VectorFrom(const OCS_VECTOR3D& Value)
{
    return FVector(Value.z, Value.x, Value.y);
}

inline OCS_VECTOR3D OCSVector3DFrom(const FVector& Value)
{
    return OCS_VECTOR3D(Value.Y, Value.Z, Value.X);
}

inline FVector2D Vector2DFrom(const OCS_VECTOR2D& Value) 
{
    return FVector2D(Value.x, Value.y);
}

inline OCS_VECTOR2D OCSVector2DFrom(const FVector2D& Value) 
{
    return OCS_VECTOR2D(Value.X, Value.Y);
}

inline FQuat QuatFrom(const OCS_QUATERNION& Value)
{
    return FQuat(Value.z, Value.x, Value.y, Value.w);
}

inline OCS_QUATERNION OCSQuaternionFrom(const FQuat& Value)
{
    return OCS_QUATERNION(Value.Y, Value.Z, Value.X, Value.W);
}
