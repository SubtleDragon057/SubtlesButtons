#include "FiveButtonManager.h"

FiveButtonManager::FiveButtonManager(uint8_t multiButtonEventTimeout) {
    _multiButtonEventTimeout = multiButtonEventTimeout;
}

void FiveButtonManager::Initialize(uint8_t* buttonPins) {
    
    _numButtons = sizeof(_buttons) / sizeof(_buttons[0]);
    for (uint8_t i = 0; i < _numButtons; i++) {
        _buttons[i].Initialize(i, buttonPins[i], true);
    }

    ClearButtonQueue();
}

void FiveButtonManager::Initialize(uint8_t commonPin, ButtonClickEvent multiplexerCallback) {

    _isUsingMux = true;
    _multiplexerCallback = multiplexerCallback;
    _numButtons = sizeof(_buttons) / sizeof(_buttons[0]);

    for (uint8_t i = 0; i < _numButtons; i++) {
        _buttons[i].Initialize(i, commonPin, true);
    }

    ClearButtonQueue();
}

void FiveButtonManager::RegisterButtonEventCallback(ButtonEventTypes event, ButtonClickEvent callback) {
    switch (event) {
        case ButtonEventTypes::MultiButtonEvent: 
            _multiButtonEvent = callback;
            break;
        case FiveButtonManager::Button1SingleClick:
            _buttons[0].RegisterButtonEventCallback(MultiClickButton::SingleClickEvent, callback);
            break;
        case FiveButtonManager::Button1DoubleClick:
            _buttons[0].RegisterButtonEventCallback(MultiClickButton::DoubleClickEvent, callback);
            break;
        case FiveButtonManager::Button1Hold:
            _buttons[0].RegisterButtonEventCallback(MultiClickButton::HoldEvent, callback);
            break;
        case FiveButtonManager::Button2SingleClick:
            _buttons[1].RegisterButtonEventCallback(MultiClickButton::SingleClickEvent, callback);
            break;
        case FiveButtonManager::Button2DoubleClick:
            _buttons[1].RegisterButtonEventCallback(MultiClickButton::DoubleClickEvent, callback);
            break;
        case FiveButtonManager::Button2Hold:
            _buttons[1].RegisterButtonEventCallback(MultiClickButton::HoldEvent, callback);
            break;
        case FiveButtonManager::Button3SingleClick:
            _buttons[2].RegisterButtonEventCallback(MultiClickButton::SingleClickEvent, callback);
            break;
        case FiveButtonManager::Button3DoubleClick:
            _buttons[2].RegisterButtonEventCallback(MultiClickButton::DoubleClickEvent, callback);
            break;
        case FiveButtonManager::Button3Hold:
            _buttons[2].RegisterButtonEventCallback(MultiClickButton::HoldEvent, callback);
            break;
        case FiveButtonManager::Button4SingleClick:
            _buttons[3].RegisterButtonEventCallback(MultiClickButton::SingleClickEvent, callback);
            break;
        case FiveButtonManager::Button4DoubleClick:
            _buttons[3].RegisterButtonEventCallback(MultiClickButton::DoubleClickEvent, callback);
            break;
        case FiveButtonManager::Button4Hold:
            _buttons[3].RegisterButtonEventCallback(MultiClickButton::HoldEvent, callback);
            break;
        case FiveButtonManager::Button5SingleClick:
            _buttons[4].RegisterButtonEventCallback(MultiClickButton::SingleClickEvent, callback);
            break;
        case FiveButtonManager::Button5DoubleClick:
            _buttons[4].RegisterButtonEventCallback(MultiClickButton::DoubleClickEvent, callback);
            break;
        case FiveButtonManager::Button5Hold:
            _buttons[4].RegisterButtonEventCallback(MultiClickButton::HoldEvent, callback);
            break;
        case FiveButtonManager::AllButtonsSingleClick:
            for (uint8_t i = 0; i < _numButtons; i++) {
                _buttons[i].RegisterButtonEventCallback(MultiClickButton::SingleClickEvent, callback);
            }
            break;
        case FiveButtonManager::AllButtonsDoubleClick:
            for (uint8_t i = 0; i < _numButtons; i++) {
                _buttons[i].RegisterButtonEventCallback(MultiClickButton::DoubleClickEvent, callback);
            }
            break;
        case FiveButtonManager::AllButtonsHold:
            for (uint8_t i = 0; i < _numButtons; i++) {
                _buttons[i].RegisterButtonEventCallback(MultiClickButton::HoldEvent, callback);
            }
            break;
        default:
            break;
    }
}

void FiveButtonManager::CheckButtons() {
    
    for (uint8_t i = 0; i < _numButtons; i++) {
        if (_isUsingMux) _multiplexerCallback(_buttons[i].Number());

        _buttons[i].CheckButtonForEvent();
    }

    GetMultiButtonEvent();
    if (_isMultiEventActive) return;

    for (uint8_t i = 0; i < _numButtons; i++) {
        if (_isUsingMux) _multiplexerCallback(_buttons[i].Number());

        _buttons[i].FireQueuedEvent();
    }
}

void FiveButtonManager::GetMultiButtonEvent() {

    for (uint8_t i = 0; i < _numButtons; i++) {
        if (_buttons[i].CurrentEvent() != MultiClickButton::HoldEvent) continue;

        AddButtonToQueue(i);
        _isMultiEventActive = true;
    }

    if (HasMultiEventExpired()) {
        ClearButtonQueue();
        _isMultiEventActive = false;
    }

    if (IsQueueFull()) {
        FireMultiButtonEvent();
        ClearButtonQueue();
    }
}

void FiveButtonManager::FireMultiButtonEvent() {

    _buttons[_simultaneousButtons[0].ButtonNum].DequeueEvent();
    _buttons[_simultaneousButtons[1].ButtonNum].DequeueEvent();

    const uint8_t lookupTable[5][5] = {
        {Error, Multi01, Multi02, Multi03, Multi04},
        {Multi01, Error, Multi12, Multi13, Multi14},
        {Multi02, Multi12, Error, Multi23, Multi24},
        {Multi03, Multi13, Multi23, Error, Multi34},
        {Multi04, Multi14, Multi24, Multi34, Error}
    };

    uint8_t buttonEvent = lookupTable[_simultaneousButtons[0].ButtonNum][_simultaneousButtons[1].ButtonNum];
    _multiButtonEvent(buttonEvent);
}

bool FiveButtonManager::HasMultiEventExpired() const {
    long currentTime = millis();
    bool button1Expired = (_simultaneousButtons[0].ButtonNum < _numButtons) && ((currentTime - _simultaneousButtons[0].TimeAdded) > _multiButtonEventTimeout);
    bool button2Expired = (_simultaneousButtons[1].ButtonNum < _numButtons) && ((currentTime - _simultaneousButtons[1].TimeAdded) > _multiButtonEventTimeout);
    
    return button1Expired || button2Expired;
}

void FiveButtonManager::AddButtonToQueue(uint8_t buttonNum) {

    if (_simultaneousButtons[0].ButtonNum == buttonNum || _simultaneousButtons[1].ButtonNum == buttonNum) return;

    _simultaneousButtons[0] = _simultaneousButtons[1];
    _simultaneousButtons[1].ButtonNum = buttonNum;
    _simultaneousButtons[1].TimeAdded = millis();
}

bool FiveButtonManager::IsQueueFull() const {
    return _simultaneousButtons[0].ButtonNum < _numButtons && _simultaneousButtons[1].ButtonNum < _numButtons;
}

void FiveButtonManager::ClearButtonQueue() {
    _simultaneousButtons[0].ButtonNum = _numButtons;
    _simultaneousButtons[0].TimeAdded = 0;
    _simultaneousButtons[1].ButtonNum = _numButtons;
    _simultaneousButtons[1].TimeAdded = 0;
}