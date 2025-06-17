
#include "LedControlller.h"

LEDController::LEDController() {
  FastLED.addLeds<WS2812B, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(LED_BRIGHTNESS);

  for (int i = 0; i < 256; i++) {
    fill_rainbow(leds, NUM_LEDS, i % 256, 16);
    FastLED.show();
    delay(10);
  }

  clear();
}

void LEDController::setState(const CRGB* colors) {
  for (int i = 0; i < NUM_LEDS; i++) currentState[i] = colors[i];
}

void LEDController::reset() {
  for (int i = 0; i < NUM_LEDS; i++) currentState[i] = CRGB::White;
  render();
  beatPos = -1;
  for (int i = 0; i < NUM_LEDS; i++) currentState[i] = CRGB::Black;
  render();
}

void LEDController::setLED(int index, CRGB color = CRGB::Red) {
  if (index >= 0 && index < NUM_LEDS) {
    currentState[index] = color;
  }
}

void LEDController::clear() {
  for (int i = 0; i < NUM_LEDS; i++) currentState[i] = CRGB::Black;
}

void LEDController::setBeatPos(int pos) { beatPos = pos % NUM_LEDS; }

void LEDController::next_beat() { beatPos = (beatPos + 1) % NUM_LEDS; }

void LEDController::render() {
  for (int i = 0; i < NUM_LEDS; i++) leds[i] = currentState[i];
  leds[beatPos] =
      currentState[beatPos] == CRGB::Black ? CRGB::Blue : CRGB::White;
  FastLED.show();
}