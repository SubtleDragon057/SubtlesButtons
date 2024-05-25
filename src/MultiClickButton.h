#ifndef MultiClickButton_h
#define MultiClickButton_h
#include "Arduino.h"
#include "ButtonClickEvent.h"

class MultiClickButton {
public:

    enum ButtonEvents : uint8_t {
        SingleClickEvent,
        DoubleClickEvent,
        HoldEvent,
        NoEvent
    };

    MultiClickButton(uint8_t debounce, uint8_t dcTimeout, uint8_t holdTime);

    uint8_t Number() const { return _number; }
    ButtonEvents CurrentEvent() const { return _queuedEvent; }
    void DequeueEvent() { _queuedEvent = ButtonEvents::NoEvent; }

    void Initialize(uint8_t buttonNum, uint8_t buttonPin, bool isUsingManager = false);
    void RegisterButtonEventCallback(ButtonEvents buttonEvent, ButtonClickEvent callback);
    void CheckButtonForEvent();
    void FireQueuedEvent();

private:

    enum ButtonState : uint8_t {
        PressedState = 0x0, // Arduino define LOW
        ReleasedState = 0x1 // Arduino definte HIGH
    };

    uint8_t _number;
    uint8_t _pin;
    uint8_t _debounceTime;
    uint8_t _dblClickTimeout;
    uint8_t _holdTime;
    uint8_t _currentState = ButtonState::ReleasedState;
    uint8_t _previousState = ButtonState::ReleasedState;
    bool _isUsingManager = false;
    bool _dblClickEnabled = false;
    bool _dblClickOnRelease = false;
    ButtonEvents _queuedEvent = ButtonEvents::NoEvent;
    uint16_t _timeSincePressed = 0;
    uint16_t _timeSinceReleased = 0;

    ButtonClickEvent _singleClickCallback;
    ButtonClickEvent _doubleClickCallback;
    ButtonClickEvent _holdCallback;

    void HandleButtonEvent(ButtonEvents buttonEvent);
    
    bool ButtonIsPressed(long& currentMillis) const;
    bool ButtonIsReleased(long& currentMillis) const;
    bool DoubleClickExpired(long& currentMillis) const;
};

#endif