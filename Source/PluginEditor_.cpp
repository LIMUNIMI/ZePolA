#include "PluginEditor_.h"

// =============================================================================
const int PolesAndZerosEQAudioProcessorEditor::originalWidth  = 1200;
const int PolesAndZerosEQAudioProcessorEditor::originalHeight = 790;
typedef PolesAndZerosEQColourScheme::Label ColourLabels;

// =============================================================================
PolesAndZerosEQAudioProcessorEditor::PolesAndZerosEQAudioProcessorEditor(
    PolesAndZerosEQAudioProcessor& p)
    : juce::AudioProcessorEditor(&p)
    , processor(p)
    , aspectRatioConstrainer()
    , palette(PolesAndZerosEQColourScheme::getEditorTheme())
    , sizeRatio(1.0f)
{
    sizeSetup();
    addAndMakeVisible(slidersPanel);
    addAndMakeVisible(plotsPanel);
    addAndMakeVisible(designPanel);
    addAndMakeVisible(masterPanel);
}
void PolesAndZerosEQAudioProcessorEditor::sizeSetup()
{
    aspectRatioConstrainer.setFixedAspectRatio(
        static_cast<double>(originalWidth) / originalHeight);
    setConstrainer(&aspectRatioConstrainer);

    juce::PropertiesFile::Options options;
    options.applicationName     = ProjectInfo::projectName;
    options.commonToAllUsers    = true;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";

    applicationProperties.setStorageParameters(options);
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        sizeRatio = pf->getDoubleValue("sizeRatio", sizeRatio);

    setResizable(true, true);
    setSize(static_cast<int>(originalWidth * sizeRatio),
            static_cast<int>(originalHeight * sizeRatio));
}

// =============================================================================
void PolesAndZerosEQAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(palette[ColourLabels::BACKGROUND]);
}
void PolesAndZerosEQAudioProcessorEditor::resized()
{
    float wRatio = static_cast<float>(getWidth()) / originalWidth;
    float hRatio = static_cast<float>(getHeight()) / originalHeight;
    sizeRatio    = fminf(wRatio, hRatio);

    slidersPanel.setBounds(
        static_cast<int>(15 * sizeRatio), static_cast<int>(55 * sizeRatio),
        static_cast<int>(510 * sizeRatio), static_cast<int>(720 * sizeRatio));
    plotsPanel.setBounds(
        static_cast<int>(525 * sizeRatio), static_cast<int>(55 * sizeRatio),
        static_cast<int>(480 * sizeRatio), static_cast<int>(720 * sizeRatio));
    designPanel.setBounds(
        static_cast<int>(1005 * sizeRatio), static_cast<int>(55 * sizeRatio),
        static_cast<int>(180 * sizeRatio), static_cast<int>(396 * sizeRatio));
    masterPanel.setBounds(
        static_cast<int>(1005 * sizeRatio), static_cast<int>(451 * sizeRatio),
        static_cast<int>(180 * sizeRatio), static_cast<int>(324 * sizeRatio));

    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        pf->setValue("sizeRatio", sizeRatio);
}

// =============================================================================
juce::Colour PolesAndZerosEQAudioProcessorEditor::getColour(ColourLabels label)
{
    return palette[label];
}
float PolesAndZerosEQAudioProcessorEditor::getRectCornerSize()
{
    return 14.5f * sizeRatio;
}
float PolesAndZerosEQAudioProcessorEditor::getRectThickness()
{
    return 1.5f * sizeRatio;
}

// =============================================================================
Panel::Panel() {}
void Panel::paint(juce::Graphics& g)
{
    juce::Rectangle<float> r = getLocalBounds().toFloat();

    if (PolesAndZerosEQAudioProcessorEditor* ape
        = findParentComponentOfClass<PolesAndZerosEQAudioProcessorEditor>())
    {
        float t                       = ape->getRectThickness();
        juce::Rectangle<float> r_trim = r.reduced(t);

        g.setColour(ape->getColour(ColourLabels::BOX_FILL));
        g.fillRoundedRectangle(r_trim, ape->getRectCornerSize());
        g.setColour(ape->getColour(ColourLabels::BOX_STROKE));
        g.drawRoundedRectangle(r_trim, ape->getRectCornerSize(), t);
    }
}
