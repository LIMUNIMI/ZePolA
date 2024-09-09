#pragma once

class LEDComponent : public juce::Component
{
public:
    // Costruttore
    LEDComponent(juce::ToggleButton& toggleButton) : associatedToggleButton(toggleButton)
    {
        associatedToggleButton.onClick = [this]() { repaint(); };
    }

    // Override della funzione di disegno
    void paint(juce::Graphics& g) override
    {
        // Ottieni i limiti dell'area per il LED
        auto bounds = getLocalBounds().toFloat().reduced(5.0f); // Riduce l'area per avere margini
        auto ledRadius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f; // Calcola il raggio del LED

        // Colore del LED: verde acceso se il ToggleButton è attivo, rosso spento se non lo è
        auto ledColor = associatedToggleButton.getToggleState() ? juce::Colours::green : juce::Colours::red;

        // Disegno del LED
        g.setColour(ledColor);
        g.fillEllipse(bounds.withSizeKeepingCentre(ledRadius * 2.0f, ledRadius * 2.0f));

        // Disegno del bordo del LED
        g.setColour(juce::Colours::black);
        g.drawEllipse(bounds.withSizeKeepingCentre(ledRadius * 2.0f, ledRadius * 2.0f), 2.0f); // Bordo di 2 pixel
    }

private:
    juce::ToggleButton& associatedToggleButton;
};

