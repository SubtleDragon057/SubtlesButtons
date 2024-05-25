#include "MultiClickButton.h"

MultiClickButton::MultiClickButton(uint8_t debounce, uint8_t dblClickTimeout, uint8_t holdTime) {    
    _debounceTime = debounce;
    _dblClickTimeout = dblClickTimeout;
    _holdTime = holdTime;
}

// TODO: Add variables to allow for buttons to be run high or low, and using pullup resistors or not
void MultiClickButton::Initialize(uint8_t buttonNum, uint8_t buttonPin, bool isUsingManager) {    
    _number = buttonNum;
    _pin = buttonPin;
    _isUsingManager = isUsingManager;

    pinMode(_pin, INPUT);
}

void MultiClickButton::RegisterButtonEventCallback(ButtonEvents buttonEvent, ButtonClickEvent callback) {    
    switch (buttonEvent) {
        case MultiClickButton::SingleClickEvent:
            _singleClickCallback = callback;
            break;
        case MultiClickButton::DoubleClickEvent:
            _doubleClickCallback = callback;
            break;
        case MultiClickButton::HoldEvent:
            _holdCallback = callback;
            break;
        default:
            break;
    }
}

void MultiClickButton::CheckButtonForEvent() {

    _currentState = digitalRead(_pin);
    long currentMillis = millis();

    if (DoubleClickExpired(currentMillis)) {
        _dblClickEnabled = false;
        HandleButtonEvent(ButtonEvents::SingleClickEvent);
        return;
    }

    if (ButtonIsPressed(currentMillis)) {
        _timeSincePressed = currentMillis;
        _previousState = _currentState;

        _dblClickOnRelease = _dblClickEnabled && 
            (currentMillis - _timeSinceReleased) <= _dblClickTimeout;
    }

    if (ButtonIsReleased(currentMillis)) {
        _timeSinceReleased = currentMillis;
        _previousState = _currentState;
        
        if ((_timeSinceReleased - _timeSincePressed) > _holdTime) {
            HandleButtonEvent(ButtonEvents::HoldEvent);
            return;
        }

        if (_dblClickEnabled) {
            HandleButtonEvent(ButtonEvents::DoubleClickEvent);
        }

        _dblClickEnabled = true;

        if (_dblClickOnRelease) {
            _dblClickOnRelease = false;
            _dblClickEnabled = false;
        }
    }
}

void MultiClickButton::FireQueuedEvent() {

    switch (_queuedEvent) {
        case ButtonEvents::SingleClickEvent:
            _singleClickCallback(_number);
            break;
        case ButtonEvents::DoubleClickEvent:
            _doubleClickCallback(_number);
            break;
        case ButtonEvents::HoldEvent:
            _holdCallback(_number);
            break;
        default:
            break;
    }

    _queuedEvent = ButtonEvents::NoEvent;
}

void MultiClickButton::HandleButtonEvent(ButtonEvents buttonEvent) {

    _queuedEvent = buttonEvent;

    if (!_isUsingManager) {
        FireQueuedEvent();
    }
}

bool MultiClickButton::ButtonIsPressed(long& currentMillis) const {
    if ((currentMillis - _timeSinceReleased) < _debounceTime) {
        return false;
    }

    return _currentState == ButtonState::PressedState && _previousState == ButtonState::ReleasedState;
}

bool MultiClickButton::ButtonIsReleased(long& currentMillis) const {
    if ((currentMillis - _timeSincePressed) < _debounceTime) {
        return false;
    }

    return _currentState == ButtonState::ReleasedState && _previousState == ButtonState::PressedState;
}

bool MultiClickButton::DoubleClickExpired(long& currentMillis) const {
    return (currentMillis - _timeSinceReleased) > _dblClickTimeout && _dblClickEnabled && !_dblClickOnRelease;
}