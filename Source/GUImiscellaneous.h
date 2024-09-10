#pragma once

#define LED_ON_COLOUR           0xffeb685c
#define LED_OFF_COLOUR          0xff781a13

class LEDComponent : public juce::Component
{
public:
    
    LEDComponent(juce::ToggleButton& toggleButton) : associatedToggleButton(toggleButton)
    {
        associatedToggleButton.onClick = [this]() { repaint(); };
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(4.0f);
        auto ledRadius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;

        auto ledColor = associatedToggleButton.getToggleState() ? juce::Colour(LED_ON_COLOUR) : juce::Colour(LED_OFF_COLOUR);

        g.setColour(ledColor);
        g.fillEllipse(bounds.withSizeKeepingCentre(ledRadius * 2.0f, ledRadius * 2.0f));

        g.setColour(juce::Colours::black);
        g.drawEllipse(bounds.withSizeKeepingCentre(ledRadius * 2.0f, ledRadius * 2.0f), 2.0f); // Bordo di 2 pixel
    }

private:
    juce::ToggleButton& associatedToggleButton;
};

