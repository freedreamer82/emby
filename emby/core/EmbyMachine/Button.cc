#include "Button.hh"
#include <vector>
#include <algorithm>
#include <EmbyDebug/assert.hh>

namespace
{
    std::vector<Button *> s_buttons;
}


extern "C" __attribute__((weak))
void
Button_BSP_ConfigGpioIrq(Button::ButtonId id, Button::ButtonIrqCb irqFn, Button *instance,bool enable)
{
    EmbyDebug_ASSERT_FAIL();
}

extern "C" __attribute__((weak))
bool Button_BSP_ReadState(Button::ButtonId id)
{
    EmbyDebug_ASSERT_FAIL();
    return false;
}


Button::~Button()
{
    unregisterButton(this);
}

Button::Button(ButtonId
               uuid,
               ButtonEventCb cb, CallbackContext
               context,
               bool enable ,
               ButtonEventMask eventMask,
               bool activeLow
)
        : m_uuid{
        uuid}, m_cb{
        cb}, m_context{
        context},

          m_state(ButtonEvent::Released),
          m_debounceMs(DEFAULT_DEBOUNCE_MS), m_longPressMs(DEFAULT_LONGPRESS_MS),
          m_lastPhysicalState(false),
          m_debounceTimer(DEFAULT_DEBOUNCE_MS, false,  EmbySystem::Timer::Type::Software),
          m_longPressTimer(DEFAULT_LONGPRESS_MS, false, EmbySystem::Timer::Type::Software),
          m_eventMask(eventMask),
          m_activeLow(activeLow)
{
    m_debounceTimer.attach(this, &Button::onDebounceTimer);
    m_longPressTimer.attach(this, &Button::onLongPressTimer);
    registerButton(this);
    if( enable )
    {
        Button_BSP_ConfigGpioIrq(m_uuid, &Button::StaticIrqHandler, this,enable);
    }

}

bool Button::enable(bool status)
{
    if( status != m_isEnabled )
    {
        m_isEnabled = status;
        Button_BSP_ConfigGpioIrq(m_uuid, &Button::StaticIrqHandler, this, m_isEnabled);
    }
    return m_isEnabled;
}


void Button::registerButton(Button *btn)
{
    auto it = std::find(s_buttons.begin(), s_buttons.end(), btn);
    if (it == s_buttons.end())
        s_buttons.push_back(btn);
}

void Button::unregisterButton(Button *btn)
{
    auto it = std::find(s_buttons.begin(), s_buttons.end(), btn);
    if (it != s_buttons.end())
        s_buttons.erase(it);
}

void Button::irqHandler()
{
    if (!m_debouncing)
    {
        m_debouncing = true;
        m_debounceTimer.reset(m_debounceMs);
        m_debounceTimer.start();
    }
}

void Button::onDebounceTimer(EmbySystem::Timer &timer)
{
    bool physical = readPhysicalState();
    bool pressed = isButtonPressed(physical);
    m_debouncing = false;
    if (pressed != m_lastPhysicalState)
    {
        m_lastPhysicalState = pressed;
        if (pressed)
        {
            m_state = ButtonEvent::Pressed;
            notifyEvent(ButtonEvent::Pressed);
            m_longPressTimer.reset(m_longPressMs);
            m_longPressTimer.start();
        } else
        {
            m_state = ButtonEvent::Released;
            notifyEvent(ButtonEvent::Released);
            m_longPressTimer.stop();
        }
    } else
    {
        m_state = ButtonEvent::Released;
    }
}

bool Button::readPhysicalState()
{
    return Button_BSP_ReadState(m_uuid);
}

void Button::onLongPressTimer(EmbySystem::Timer &timer)
{
    bool physical = readPhysicalState();
    bool pressed = isButtonPressed(physical);
    if (pressed)
    {
        m_state = ButtonEvent::LongPressed;
        notifyEvent(ButtonEvent::LongPressed);
    } else
    {
        m_state = ButtonEvent::Released;
        notifyEvent(ButtonEvent::Released);
    }
}

void Button::setEventMask(ButtonEventMask mask)
{
    m_eventMask = mask;
}

Button::ButtonEventMask Button::getEventMask() const
{
    return m_eventMask;
}

void Button::notifyEvent(ButtonEvent ev)
{
    if (m_cb.isAttached() && (m_eventMask & static_cast<ButtonEventMask>(ev)))
    {
        m_cb(ev, m_context);
    }
}

bool Button::isPressed() const
{
    return m_state == ButtonEvent::Pressed || m_state == ButtonEvent::LongPressed;
}

void Button::setDebounceMs(uint32_t ms)
{
    m_debounceMs = ms;
    m_debounceTimer.reset(ms);
}

uint32_t Button::getDebounceMs() const
{
    return m_debounceMs;
}

void Button::setLongPressMs(uint32_t ms)
{
    m_longPressMs = ms;
    m_longPressTimer.reset(ms);
}

uint32_t Button::getLongPressMs() const
{
    return m_longPressMs;
}


void Button::StaticIrqHandler(Button *btn)
{
    if (btn)
    {
        for (auto b: s_buttons)
        {
            if (b == btn)
            {
                btn->irqHandler();
                return;
            }
        }
    }
}