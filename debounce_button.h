class DebouncedButton {
  private:
    uint8_t pin;
    bool activeState;
    bool lastReading;
    bool stableState;
    uint32_t lastDebounceTime;

  public:
    DebouncedButton(uint8_t p, bool active)
      : pin(p), activeState(active), lastReading(!active), stableState(!active), lastDebounceTime(0) {}

    void begin() {
      pinMode(pin, INPUT);
    }

    bool isPressed() {
      bool reading = digitalRead(pin);

      if (reading != lastReading) {
        lastDebounceTime = xTaskGetTickCount();
      }

      if ((xTaskGetTickCount() - lastDebounceTime) > pdMS_TO_TICKS(15)) {
        if (reading != stableState) {
          stableState = reading;
        }
      }

      lastReading = reading;
      return (stableState == activeState);
    }
};