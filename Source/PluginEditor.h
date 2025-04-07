/*
  ==============================================================================

    PluginEditor.h

    Copyright (c) 2025 Laboratorio di Informatica Musicale
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

  ==============================================================================
*/

#pragma once
#include "GUI/DesignerPanel.h"
#include "GUI/LookAndFeel.h"
#include "GUI/MasterPanel.h"
#include "GUI/ParameterPanel.h"
#include "GUI/PlotsPanel.h"
#include "GUI/TopMenuPanel.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>

// =============================================================================
/** Plugin editor for poles and zero eq  */
class ZePolAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    // ===========================================================================
    ZePolAudioProcessorEditor(ZePolAudioProcessor&);
    ~ZePolAudioProcessorEditor();

    //===================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // ===========================================================================
    ZePolAudioProcessor& processor;

    // ===========================================================================
    juce::TooltipWindow tooltipWindow;
    juce::ComponentBoundsConstrainer aspectRatioConstrainer;
    juce::ApplicationProperties applicationProperties;
    CustomLookAndFeel claf;

    // ===========================================================================
    std::unique_ptr<TopMenuPanel> topMenuPanel;
    std::unique_ptr<ParameterPanel> parameterPanel;
    std::unique_ptr<PlotsPanel> plotsPanel;
    std::unique_ptr<DesignerPanel> designerPanel;
    std::unique_ptr<MasterPanel> masterPanel;

    // ===========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ZePolAudioProcessorEditor);
};
