#ifndef FiveButtonManager_h
#define FiveButtonManager_h
#include "Arduino.h"
#include "MultiClickButton.h"
#include "ButtonClickEvent.h"

class FiveButtonManager {
public:

    enum ButtonEventTypes : uint8_t {
        Button1SingleClick,
        Button1DoubleClick,
        Button1Hold,
        Button2SingleClick,
        Button2DoubleClick,
        Button2Hold,
        Button3SingleClick,
        Button3DoubleClick,
        Button3Hold,
        Button4SingleClick,
        Button4DoubleClick,
        Button4Hold,
        Button5SingleClick,
        Button5DoubleClick,
        Button5Hold,
        AllButtonsSingleClick,
        AllButtonsDoubleClick,
        AllButtonsHold,
        MultiButtonEvent
    };

    enum MultiButtonEvents : uint8_t {
        Error,
        Multi01,
        Multi02,
        Multi03,
        Multi04,
        Multi12,
        Multi13,
        Multi14,
        Multi23,
        Multi24,
        Multi34
    };

    FiveButtonManager(uint8_t multiButtonEventTimeout = 250);

    void Initialize(uint8_t* buttonPins);
    void Initialize(uint8_t commonPin, ButtonClickEvent multiplexerCallback);
    void RegisterButtonEventCallback(ButtonEventTypes event, ButtonClickEvent callback);
    void CheckButtons();

private:

    struct QueuedButton {
        uint8_t ButtonNum;
        long TimeAdded;

        QueuedButton(uint8_t num, long time) {
            ButtonNum = num;
            TimeAdded = time;
        }
    };

    bool _isUsingMux = false;
    bool _isMultiEventActive = false;
    uint8_t _numButtons = 5;
    uint8_t _debounce = 20;
    uint8_t _doubleClickTime = 350;
    uint8_t _holdTime = 1000;
    uint8_t _multiButtonEventTimeout = 250;
    QueuedButton _simultaneousButtons[2] = { QueuedButton(5, 0), QueuedButton(5, 0) };

    ButtonClickEvent _multiplexerCallback;
    ButtonClickEvent _multiButtonEvent;

    MultiClickButton _buttons[5] = {
        MultiClickButton(_debounce, _doubleClickTime, _holdTime),
        MultiClickButton(_debounce, _doubleClickTime, _holdTime),
        MultiClickButton(_debounce, _doubleClickTime, _holdTime),
        MultiClickButton(_debounce, _doubleClickTime, _holdTime),
        MultiClickButton(_debounce, _doubleClickTime, _holdTime)
    };

    void GetMultiButtonEvent();
    void FireMultiButtonEvent();
    bool HasMultiEventExpired() const;

    void AddButtonToQueue(uint8_t buttonNum);
    bool IsQueueFull() const;
    void ClearButtonQueue();
};

#endif