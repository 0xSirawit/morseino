class DebouncedButton {
  private:
    uint8_t pin;
    bool activeState;
    bool lastReading;
    bool stableState;
    uint32_t lastDebounceTime;

    bool wasPressedState;

  public:
    DebouncedButton(uint8_t p, bool active)
      : pin(p),
        activeState(active),
        lastReading(!active),
        stableState(!active),
        lastDebounceTime(0),
        wasPressedState(false) {}

    void begin() {
      pinMode(pin, INPUT);
    }

    bool isHeld() {
      update();

      return (stableState == activeState);
    }

    bool wasPressed() {
      update();

      bool pressed = (stableState == activeState);

      if (pressed) {
        wasPressedState = true;
      }

      if (!pressed && wasPressedState) {
        wasPressedState = false;
        return true;
      }

      return false;
    }

  private:
    void update() {
      bool reading = digitalRead(pin);
      uint32_t now = xTaskGetTickCount();

      if (reading != lastReading) {
        lastDebounceTime = now;
      }

      if ((now - lastDebounceTime) > pdMS_TO_TICKS(15)) {
        if (reading != stableState) {
          stableState = reading;
        }
      }

      lastReading = reading;
    }
};