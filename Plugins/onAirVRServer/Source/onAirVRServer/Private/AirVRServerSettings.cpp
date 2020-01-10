/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRServerSettings.h"
#include "AirVRServerPrivate.h"

#include "Misc/DefaultValueHelper.h"
#include "GenericPlatform/GenericPlatformHttp.h"

UAirVRServerSettings::UAirVRServerSettings()
    : LicenseFilePath(TEXT("onairvr.license")),
      MaxClientCount(1),
      PortSTAP(9090),
      VideoBitrate(24000000),
      MaxFrameRate(90.0f),
      DefaultFrameRate(30.0f),
      BroadcastAudioToAllPlayers(true),
      PortAMP(0),
      LoopbackOnlyForSTAP(false),
      UserData(TEXT("")),
      GroupServer(TEXT(""))
{}

void UAirVRServerSettings::ParseCommandLineArgs()
{
    TArray<FString> Tokens;
    TArray<FString> Switches;

    FCommandLine::Parse(FCommandLine::Get(), Tokens, Switches);

    for (auto& Token : Tokens) {
        FString Name;
        FString Value;
        if (Token.Split(TEXT("="), &Name, &Value)) {
            if (Name.Equals(TEXT("onairvr_stap_port"))) {
                int32 parsed = 0;
                if (FDefaultValueHelper::ParseInt(Value, parsed) && 0 <= parsed && parsed <= 65535) {
                    PortSTAP = parsed;
                }
                else {
                    UE_LOG(LogonAirVRServer, Warning, TEXT("STAP port number of the command line argument is invalid : %s"), *Value);
                }
            }
            else if (Name.Equals(TEXT("onairvr_amp_port"))) {
                int32 parsed = 0;
                if (FDefaultValueHelper::ParseInt(Value, parsed) && 0 <= parsed && parsed <= 65535) {
                    PortAMP = parsed;
                }
                else {
                    UE_LOG(LogonAirVRServer, Warning, TEXT("AMP port number of the command line argument is invalid : %s"), *Value);
                }
            }
            else if (Name.Equals(TEXT("onairvr_loopback_only"))) {
                LoopbackOnlyForSTAP = Value.Equals(TEXT("true"));
            }
            else if (Name.Equals(TEXT("onairvr_license"))) {
                LicenseFilePath = Value;
            }
            else if (Name.Equals(TEXT("onairvr_video_bitrate"))) {
                int32 parsed = 0;
                VideoBitrate = FDefaultValueHelper::ParseInt(Value, parsed) && parsed > 0 ? parsed : VideoBitrate;
            }
            else if (Name.Equals(TEXT("onairvr_user_data"))) {
                UserData = FGenericPlatformHttp::UrlDecode(Value);
            }
            else if (Name.Equals(TEXT("onairvr_group_server"))) {
                GroupServer = Value;
            }
        }
    }
}
