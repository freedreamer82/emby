#ifndef EMBY_BUTTON_HH
#define EMBY_BUTTON_HH

#include <EmbySystem/Timer.hh>
#include <EmbyLibs/Callback.hh>

class Button
{
public:
    enum class ButtonEvent: uint8_t
    {
        Pressed = 0x01,
        Released = 0x02,
        LongPressed = 0x04
    };
    using ButtonEventMask = uint8_t;
    typedef void *CallbackContext;
    using ButtonEventCb = EmbyLibs::Callback<void, ButtonEvent, CallbackContext>;
    using ButtonId = uint32_t;
    using ButtonIrqCb = void (*)(Button *);

    /**
     * @brief Construct a new Button object.
     *
     * @param uuid Unique identifier for the button instance.
     * @param cb Callback invoked when a button event occurs.
     * @param context User context passed to the callback.
     * @param eventMask Bitmask enabling which events will trigger the callback.
     * @param activeLow True if the physical pin is active low.
     */
    Button(ButtonId uuid, ButtonEventCb cb, CallbackContext context = nullptr, bool enable = true,
           ButtonEventMask eventMask = 0xFF,
           bool activeLow = true);

    /**
     * @brief Get the identifier of this button.
     *
     * @return ButtonId The unique id assigned to the button.
     */
    ButtonId getId() const { return m_uuid; }


    bool enable(bool status);

    bool isEnabled() const { return m_isEnabled; }

    /**
     * @brief Virtual destructor for cleanup in derived classes.
     */
    virtual ~Button();

    /**
     * @brief Query whether the button is currently in the logical "pressed" state.
     *
     * This applies the active-low inversion if configured.
     *
     * @return true if the button is considered pressed, false otherwise.
     */
    bool isPressed() const;

    /**
     * @brief Set the event mask controlling which events will be notified.
     *
     * @param mask Bitmask of ButtonEvent values to enable.
     */
    void setEventMask(ButtonEventMask mask);

    /**
     * @brief Get the current event mask.
     *
     * @return ButtonEventMask Current mask of enabled events.
     */
    ButtonEventMask getEventMask() const;

    /**
     * @brief Set the debounce duration in milliseconds.
     *
     * @param ms Debounce time in milliseconds.
     */
    void setDebounceMs(uint32_t ms);

    /**
     * @brief Get the debounce duration in milliseconds.
     *
     * @return uint32_t Debounce time in milliseconds.
     */
    uint32_t getDebounceMs() const;

    /**
     * @brief Set the duration that qualifies as a long press.
     *
     * @param ms Long press threshold in milliseconds.
     */
    void setLongPressMs(uint32_t ms);

    /**
     * @brief Get the long press duration in milliseconds.
     *
     * @return uint32_t Long press threshold in milliseconds.
     */
    uint32_t getLongPressMs() const;


private:

    /**
 * @brief Static IRQ entry point called from interrupt context.
 *
 * This function adapts the raw IRQ call to the instance method.
 *
 * @param btn Pointer to the Button instance that received the IRQ.
 */
    static void StaticIrqHandler(Button *btn);

    /**
     * @brief Instance IRQ handler called when a hardware interrupt occurs.
     *
     * Should be called only by the class when an actual hardware IRQ is received.
     */
    void irqHandler();

    /**
     * @brief Register a button instance in the internal registry.
     *
     * Used to track buttons that need global handling.
     *
     * @param btn Pointer to the Button instance to register.
     */
    static void registerButton(Button *btn);

    /**
     * @brief Unregister a button instance from the internal registry.
     *
     * @param btn Pointer to the Button instance to unregister.
     */
    static void unregisterButton(Button *btn);


    /**
     * @brief Read the physical state of the pin associated with the button.
     *
     * @return true if the logical level on the pin is high (physical state).
     * Note: this method does not apply inversion for `m_activeLow`; to obtain the
     * logical "pressed" state use `isButtonPressed(readPhysicalState())`.
     * The concrete implementation depends on the hardware and must be provided in the .cc.
     */
    bool readPhysicalState();

    /**
     * @brief Convert a raw physical pin state to the logical "pressed" state.
     *
     * Applies inversion if the button is configured as active low.
     *
     * @param physicalState Raw pin level (true = high).
     * @return true if the button should be considered pressed.
     */
    bool isButtonPressed(bool physicalState) const { return m_activeLow ? !physicalState : physicalState; }

    /**
     * @brief Callback invoked when debounce timer expires.
     *
     * @param timer Reference to the timer that fired.
     */
    void onDebounceTimer(EmbySystem::Timer &timer);

    /**
     * @brief Callback invoked when long-press timer expires.
     *
     * @param timer Reference to the timer that fired.
     */
    void onLongPressTimer(EmbySystem::Timer &timer);

    /**
     * @brief Notify the registered callback about a button event.
     *
     * This will respect the configured event mask and invoke the user callback.
     *
     * @param ev The ButtonEvent to notify.
     */
    void notifyEvent(ButtonEvent ev);

    ButtonEventCb m_cb;
    CallbackContext m_context;
    ButtonEvent m_state;
    uint32_t m_debounceMs;
    uint32_t m_longPressMs;
    bool m_lastPhysicalState;
    EmbySystem::Timer m_debounceTimer;
    EmbySystem::Timer m_longPressTimer;
    ButtonEventMask m_eventMask;
    uint8_t m_debouncing: 1;
    uint8_t m_activeLow: 1;
    uint8_t m_isEnabled: 1;
    ButtonId m_uuid;

    static constexpr uint32_t
    DEFAULT_DEBOUNCE_MS = 50;
    static constexpr uint32_t
    DEFAULT_LONGPRESS_MS = 1000;
};

#endif //PUPS_DETECTOR_BUTTON_HH