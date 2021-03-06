/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVREventDispatcher.h"
#include "AirVRServerPrivate.h"

#include "Misc/Base64.h"

static const char* KeyType              = "Type";
static const char* TypeEvent            = "Event";
static const char* TypeUserData         = "userdata";
static const char* KeyFrom              = "From";
static const char* KeyName              = "Name";

static const char* FromSession          = "Session";
static const char* NameConnected        = "Connected";
static const char* NameDisconnected     = "Disconnected";

static const char* FromPlayer           = "Player";
static const char* NameCreated          = "Created";
static const char* NameActivated        = "Activated";
static const char* NameDeactivated      = "Deactivated";
static const char* NameDestroyed        = "Destroyed";
static const char* NameShowCopyright    = "ShowCopyright";

static const char* FromMediaStream          = "MediaStream";
static const char* NameInitialized          = "Initialized";
static const char* NameStarted              = "Started";
static const char* NameEncodeVideoFrame     = "EncodeVideoFrame";
static const char* NameStopped              = "Stopped";
static const char* NameCleanedUp            = "CleanedUp";
static const char* NameSetCameraProjection  = "SetCameraProjection";
static const char* KeyCameraProjection      = "CameraProjection";

static const char* FromInputStream                      = "InputStream";
static const char* NameRemoteInputDeviceRegistered      = "RemoteInputDeviceRegistered";
static const char* NameRemoteInputDeviceUnregistered    = "RemoteInputDeviceUnregistered";
static const char* KeyDeviceID                          = "DeviceID";
static const char* KeyDeviceName                        = "DeviceName";

void FAirVREventDispatcher::AddListener(IAirVREventListener* Listener)
{
    Listeners.AddUnique(Listener);
}

void FAirVREventDispatcher::RemoveListener(IAirVREventListener* Listener)
{
    Listeners.Remove(Listener);
}

void FAirVREventDispatcher::DispatchMessages()
{
    int SrcPlayerID = -1;
    const char* Message = nullptr;
    while (ocs_CheckMessageQueue(SrcPlayerID, &Message)) {
        TSharedPtr<FJsonObject> Parsed;
        TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Message);
        if (FJsonSerializer::Deserialize<TCHAR>(Reader, Parsed) && Parsed.IsValid()) {
            check(Parsed->HasField(KeyType));
            if (Parsed->GetStringField(KeyType).Equals(TypeEvent)) {
                check(Parsed->HasField(KeyFrom));
                check(Parsed->HasField(KeyName));

                FString From = Parsed->GetStringField(KeyFrom);
                if (From.Equals(FromSession)) {
                    DispatchSessionMessage(SrcPlayerID, Parsed);
                }
                else if (From.Equals(FromPlayer)) {
                    DispatchPlayerMessage(SrcPlayerID, Parsed);
                }
                else if (From.Equals(FromMediaStream)) {
                    DispatchMediaStreamMessage(SrcPlayerID, Parsed);
                }
            }
        }
        ocs_RemoveFirstMessage();
    }
}

void FAirVREventDispatcher::DispatchSessionMessage(int PlayerID, const TSharedPtr<FJsonObject>& Message)
{
    if (Message->GetStringField(KeyName).Equals(NameConnected)) {
        NotifySessionConnected(PlayerID);
    }
    else if (Message->GetStringField(KeyName).Equals(NameDisconnected)) {
        NotifySessionDisconnected(PlayerID);
    }
}

void FAirVREventDispatcher::DispatchPlayerMessage(int PlayerID, const TSharedPtr<FJsonObject>& Message)
{
    if (Message->GetStringField(KeyName).Equals(NameCreated)) {
        NotifyPlayerCreated(PlayerID);
    }
    else if (Message->GetStringField(KeyName).Equals(NameActivated)) {
        NotifyPlayerActivated(PlayerID);
    }
    else if (Message->GetStringField(KeyName).Equals(NameDeactivated)) {
        NotifyPlayerDeactivated(PlayerID);
    }
    else if (Message->GetStringField(KeyName).Equals(NameDestroyed)) {
        NotifyPlayerDestroyed(PlayerID);
    }
    else if (Message->GetStringField(KeyName).Equals(NameShowCopyright)) {
        NotifyPlayerShowCopyright(PlayerID);
    }
}

void FAirVREventDispatcher::DispatchMediaStreamMessage(int PlayerID, const TSharedPtr<FJsonObject>& Message)
{
    if (Message->GetStringField(KeyName).Equals(NameInitialized)) {
        NotifyMediaStreamInitialized(PlayerID);
    }
    else if (Message->GetStringField(KeyName).Equals(NameStarted)) {
        NotifyMediaStreamStarted(PlayerID);
    }
    else if (Message->GetStringField(KeyName).Equals(NameEncodeVideoFrame)) {
        NotifyMediaStreamEncodeVideoFrame(PlayerID);
    }
    else if (Message->GetStringField(KeyName).Equals(NameStopped)) {
        NotifyMediaStreamStopped(PlayerID);
    }
    else if (Message->GetStringField(KeyName).Equals(NameCleanedUp)) {
        NotifyMediaStreamCleanedUp(PlayerID);
    }
    else if (Message->GetStringField(KeyName).Equals(NameSetCameraProjection)) {
        check(Message->HasField(KeyCameraProjection));

        auto CameraProjection = Message->GetArrayField(KeyCameraProjection);
        check(CameraProjection.Num() == 4);

        float Projection[4];
        for (int i = 0; i < 4; i++) {
            Projection[i] = CameraProjection[i].Get()->AsNumber();
        }

        NotifyMediaStreamSetCameraProjection(PlayerID, Projection);
    }
}

void FAirVREventDispatcher::NotifySessionConnected(int PlayerID) const
{
    for (auto Listener : Listeners) {
        Listener->AirVREventSessionConnected(PlayerID);
    }
}

void FAirVREventDispatcher::NotifySessionDisconnected(int PlayerID) const
{
    for (auto Listener : Listeners) {
        Listener->AirVREventSessionDisconnected(PlayerID);
    }
}

void FAirVREventDispatcher::NotifyPlayerCreated(int PlayerID) const
{
    for (auto Listener : Listeners) {
        Listener->AirVREventPlayerCreated(PlayerID);
    }
}

void FAirVREventDispatcher::NotifyPlayerActivated(int PlayerID) const
{
    for (auto Listener : Listeners) {
        Listener->AirVREventPlayerActivated(PlayerID);
    }
}

void FAirVREventDispatcher::NotifyPlayerDeactivated(int PlayerID) const
{
    for (auto Listener : Listeners) {
        Listener->AirVREventPlayerDeactivated(PlayerID);
    }
}

void FAirVREventDispatcher::NotifyPlayerDestroyed(int PlayerID) const
{
    for (auto Listener : Listeners) {
        Listener->AirVREventPlayerDestroyed(PlayerID);
    }
}

void FAirVREventDispatcher::NotifyPlayerShowCopyright(int PlayerID) const
{
    for (auto Listener : Listeners) {
        Listener->AirVREventPlayerShowCopyright(PlayerID);
    }
}

void FAirVREventDispatcher::NotifyMediaStreamInitialized(int PlayerID) const
{
    for (auto Listener : Listeners) {
        Listener->AirVREventMediaStreamInitialized(PlayerID);
    }
}

void FAirVREventDispatcher::NotifyMediaStreamStarted(int PlayerID) const
{
    for (auto Listener : Listeners) {
        Listener->AirVREventMediaStreamStarted(PlayerID);
    }
}

void FAirVREventDispatcher::NotifyMediaStreamEncodeVideoFrame(int PlayerID) const
{
    for (auto Listener : Listeners) {
        Listener->AirVREventMediaStreamEncodeVideoFrame(PlayerID);
    }
}

void FAirVREventDispatcher::NotifyMediaStreamStopped(int PlayerID) const
{
    for (auto Listener : Listeners) {
        Listener->AirVREventMediaStreamStopped(PlayerID);
    }
}

void FAirVREventDispatcher::NotifyMediaStreamCleanedUp(int PlayerID) const
{
    for (auto Listener : Listeners) {
        Listener->AirVREventMediaStreamCleanedUp(PlayerID);
    }
}

void FAirVREventDispatcher::NotifyMediaStreamSetCameraProjection(int PlayerID, const float* Projection) const 
{
    for (auto Listener : Listeners) {
        Listener->AirVREventMediaStreamSetCameraProjection(PlayerID, Projection);
    }
}
