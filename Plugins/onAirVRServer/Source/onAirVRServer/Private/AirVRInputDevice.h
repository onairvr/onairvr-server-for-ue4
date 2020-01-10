/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRInputStream.h"

class FAirVRInputDevice : public FAirVRInputReceiver {
private:
    class Axis3D;
    class Axis2D;
    class Axis;
    class Button;
    class Orientation;
    class Transform;
    class Touch;

    class Control 
    {
    public:
        virtual Axis3D* AsAxis3D()              { check(false); return nullptr; }
        virtual Axis2D* AsAxis2D()              { check(false); return nullptr; }
        virtual Axis* AsAxis()                  { check(false); return nullptr; }
        virtual Button* AsButton()              { check(false); return nullptr; }
        virtual Orientation* AsOrientation()    { check(false); return nullptr; }
        virtual Transform* AsTransform()        { check(false); return nullptr; }
        virtual Touch* AsTouch()                { check(false); return nullptr; }

        virtual void PollInput(FAirVRInputDevice* Device, FAirVRInputStream* InputStream, uint8 Id) = 0;
    };

    class Axis3D : public Control 
    {
    public:
        Axis3D()
        {
            Val[0] = Val[1] = Val[2] = 0.0f;
        }

    private:
        float Val[3];

    public:
        ONAIRVR_VECTOR3D Value() const { return ONAIRVR_VECTOR3D(Val[2], Val[0], Val[1]); }

        void SetValue(const ONAIRVR_VECTOR3D& InValue) 
        {
            Val[0] = InValue.y;
            Val[1] = InValue.z;
            Val[2] = InValue.x;
        }
        
        // implements Control
        virtual Axis3D* AsAxis3D() override { return this; }

        virtual void PollInput(FAirVRInputDevice* Device, FAirVRInputStream* InputStream, uint8 Id) override
        {
            if (InputStream->GetInput(Device, Id, Val, 3) == false) {
                SetValue(ONAIRVR_VECTOR3D());
            }
        }
    };

    class Axis2D : public Control
    {
    public:
        Axis2D()
        {
            Val[0] = Val[1] = 0.0f;
        }

    private:
        float Val[2];

    public:
        ONAIRVR_VECTOR2D Value() const { return ONAIRVR_VECTOR2D(Val[0], Val[1]); }

        void SetValue(const ONAIRVR_VECTOR2D& InValue)
        {
            Val[0] = InValue.x;
            Val[1] = InValue.y;
        }

        // implements Control
        virtual Axis2D* AsAxis2D() override { return this; }

        virtual void PollInput(FAirVRInputDevice* Device, FAirVRInputStream* InputStream, uint8 Id) override
        {
            if (InputStream->GetInput(Device, Id, Val, 2) == false) {
                SetValue(ONAIRVR_VECTOR2D());
            }
        }
    };

    class Axis : public Control
    {
    public:
        Axis() : Val(0.0f) {}

    private:
        float Val;   

    public:
        float Value() const { return Val; }
        void SetValue(float InValue) { Val = InValue; }

        // implements Control
        virtual Axis* AsAxis() override { return this; }

        virtual void PollInput(FAirVRInputDevice* Device, FAirVRInputStream* InputStream, uint8 Id) override
        {
            if (InputStream->GetInput(Device, Id, &Val, 1) == false) {
                Val = 0.0f;
            }
        }
    };

    class Button : public Control
    {
    public:
        Button() : Prev(0.0f), Val(0.0f) {}

    private:
        float Prev;
        float Val;

    public:
        float Value() const { return Val; }

        void SetValue(float InValue)
        {
            Prev = Val;
            Val = InValue;
        }

        bool IsDown() const
        {
            return Prev == 0.0f && Val != 0.0f;
        }

        bool IsUp() const
        {
            return Prev != 0.0f && Val == 0.0f;
        }

        // implements Control
        virtual Button* AsButton() override { return this; }

        virtual void PollInput(FAirVRInputDevice* Device, FAirVRInputStream* InputStream, uint8 Id) override
        {
            float InputVal = 0.0f;
            if (InputStream->GetInput(Device, Id, &InputVal, 1)) {
                SetValue(InputVal);
            }
            else {
                SetValue(0.0f);
            }
        }
    };

    class Orientation : public Control
    {
    public:
        Orientation()
        {
            Val[0] = Val[1] = Val[2] = 0.0f;
            Val[3] = 1.0f;
        }

    private:
        float Val[4];

    public:
        ONAIRVR_QUATERNION Value() const { return ONAIRVR_QUATERNION(Val[2], Val[0], Val[1], Val[3]); }

        void SetValue(const ONAIRVR_QUATERNION& InValue)
        {
            Val[0] = InValue.y;
            Val[1] = InValue.z;
            Val[2] = InValue.x;
            Val[3] = InValue.w;
        }

        // implements Control
        virtual Orientation* AsOrientation() override { return this; }

        virtual void PollInput(FAirVRInputDevice* Device, FAirVRInputStream* InputStream, uint8 Id) override
        {
            if (InputStream->GetInput(Device, Id, Val, 4) == false) {
                SetValue(ONAIRVR_QUATERNION());
            }
        }
    };

    class Transform : public Control
    {
    public:
        Transform()
        {
			TimeStamp_ = 0.0;
            for (int i = 0; i < 6; i++) {
                Value[i] = 0.0f;
            }
            Value[6] = 1.0f;
        }

    private:
		double TimeStamp_;
        float Value[7];

    public:
		double TimeStamp() const				{ return TimeStamp_; }
        ONAIRVR_VECTOR3D Position() const       { return ONAIRVR_VECTOR3D(Value[2], Value[0], Value[1]); }
        ONAIRVR_QUATERNION Orientation() const  { return ONAIRVR_QUATERNION(Value[5], Value[3], Value[4], Value[6]); }

        void SetValue(double TimeStamp, const ONAIRVR_VECTOR3D& Position, const ONAIRVR_QUATERNION& Orientation)
        {
			TimeStamp_ = TimeStamp;
            Value[0] = Position.y;
            Value[1] = Position.z;
            Value[2] = Position.x;
            Value[3] = Orientation.y;
            Value[4] = Orientation.z;
            Value[5] = Orientation.x;
            Value[6] = Orientation.w;
        }

        // implements Control
        virtual Transform* AsTransform() override { return this; }

        virtual void PollInput(FAirVRInputDevice* Device, FAirVRInputStream* InputStream, uint8 Id) override
        {
            if (InputStream->GetInputWithTimeStamp(Device, Id, Value, 7, &TimeStamp_) == false) {
                SetValue(0.0, ONAIRVR_VECTOR3D(), ONAIRVR_QUATERNION());
            }
        }
    };

    class Touch : public Control
    {
    public:
        Touch()
        {
            Value[0] = Value[1] = Value[2] = 0.0f;
        }

    private:
        float Value[3];

    public:
        ONAIRVR_VECTOR2D Position() const   { return ONAIRVR_VECTOR2D(Value[0], Value[1]); }
        bool IsTouched() const              { return Value[2] != 0.0f; }

        void SetValue(const ONAIRVR_VECTOR2D& Position, bool bTouch)
        {
            Value[0] = Position.x;
            Value[1] = Position.y;
            Value[2] = bTouch ? 1.0f : 0.0f;
        }

        // implements Control
        virtual Touch* AsTouch() override { return this; }

        virtual void PollInput(FAirVRInputDevice* Device, FAirVRInputStream* InputStream, uint8 Id) override
        {
            if (InputStream->GetInput(Device, Id, Value, 3) == false) {
                SetValue(ONAIRVR_VECTOR2D(), false);
            }
        }
    };

public:
    FAirVRInputDevice();
    virtual ~FAirVRInputDevice();

public:
    void AddControlTouch(uint8 ControlID);
    void AddControlTransform(uint8 ControlID);
    void AddControlOrientation(uint8 ControlID);
    void AddControlAxis3D(uint8 ControlID);
    void AddControlAxis2D(uint8 ControlID);
    void AddControlAxis(uint8 ControlID);
    void AddControlButton(uint8 ControlID);
    void AddExtControlAxis2D(uint8 ControlID);
    void AddExtControlButton(uint8 ControlID);

    void SetExtControlAxis2D(uint8 ControlID, const ONAIRVR_VECTOR2D& Value);
    void SetExtControlButton(uint8 ControlID, float Value);

    void GetTouch(uint8 ControlID, ONAIRVR_VECTOR2D* Position, bool* bTouch);
	void GetTransform(uint8 ControlID, ONAIRVR_VECTOR3D* Position, ONAIRVR_QUATERNION* Orientation);
    void GetTransform(uint8 ControlID, double& TimeStamp, ONAIRVR_VECTOR3D* Position, ONAIRVR_QUATERNION* Orientation);
    ONAIRVR_QUATERNION GetOrientation(uint8 ControlID);
    ONAIRVR_VECTOR3D GetAxis3D(uint8 ControlID);
    ONAIRVR_VECTOR2D GetAxis2D(uint8 ControlID);
    float GetAxis(uint8 ControlID);
    float GetButtonRaw(uint8 ControlID);
    bool GetButton(uint8 ControlID);
    bool GetButtonDown(uint8 ControlID);
    bool GetButtonUp(uint8 ControlID);

public:
    // implements FAirVRInputReceiver
    virtual void PollInputsPerFrame(class FAirVRInputStream* InputStream) override;

protected:
    virtual void UpdateExtendedControls() {}

private:
    Control* FindControl(uint8 ControlID);

private:
    TMap<uint8, Control*> Controls;
    TMap<uint8, Control*> ExtControls;
};