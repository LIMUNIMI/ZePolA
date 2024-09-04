/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.12

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "PluginEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
PluginEditor::PluginEditor (PolesAndZerosEQAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), processor (p), valueTreeState (vts)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addZeroButton.reset (new juce::TextButton ("Add Zero"));
    addAndMakeVisible (addZeroButton.get());
    addZeroButton->addListener (this);

    addZeroButton->setBounds (24, 312, 104, 24);

    rmvZeroButton.reset (new juce::TextButton ("Add Zero"));
    addAndMakeVisible (rmvZeroButton.get());
    rmvZeroButton->setButtonText (TRANS ("Remove Zero"));
    rmvZeroButton->addListener (this);

    rmvZeroButton->setBounds (24, 352, 104, 24);

    addPoleButton.reset (new juce::TextButton ("Add Zero"));
    addAndMakeVisible (addPoleButton.get());
    addPoleButton->setButtonText (TRANS ("Add Pole"));
    addPoleButton->addListener (this);

    addPoleButton->setBounds (152, 312, 104, 24);

    rmvPoleButton.reset (new juce::TextButton ("Add Zero"));
    addAndMakeVisible (rmvPoleButton.get());
    rmvPoleButton->setButtonText (TRANS ("Remove Pole"));
    rmvPoleButton->addListener (this);

    rmvPoleButton->setBounds (152, 352, 104, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

PluginEditor::~PluginEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    addZeroButton = nullptr;
    rmvZeroButton = nullptr;
    addPoleButton = nullptr;
    rmvPoleButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PluginEditor::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void PluginEditor::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == addZeroButton.get())
    {
        //[UserButtonCode_addZeroButton] -- add your button handler code here..
        //[/UserButtonCode_addZeroButton]
    }
    else if (buttonThatWasClicked == rmvZeroButton.get())
    {
        //[UserButtonCode_rmvZeroButton] -- add your button handler code here..
        //[/UserButtonCode_rmvZeroButton]
    }
    else if (buttonThatWasClicked == addPoleButton.get())
    {
        //[UserButtonCode_addPoleButton] -- add your button handler code here..
        //[/UserButtonCode_addPoleButton]
    }
    else if (buttonThatWasClicked == rmvPoleButton.get())
    {
        //[UserButtonCode_rmvPoleButton] -- add your button handler code here..
        //[/UserButtonCode_rmvPoleButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PluginEditor" componentName=""
                 parentClasses="public juce::AudioProcessorEditor" constructorParams="PolesAndZerosEQAudioProcessor&amp; p, AudioProcessorValueTreeState&amp; vts"
                 variableInitialisers="AudioProcessorEditor(&amp;p), processor (p), valueTreeState (vts)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <TEXTBUTTON name="Add Zero" id="86b517ba70a15962" memberName="addZeroButton"
              virtualName="" explicitFocusOrder="0" pos="24 312 104 24" buttonText="Add Zero"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Add Zero" id="69591ec43ac1b757" memberName="rmvZeroButton"
              virtualName="" explicitFocusOrder="0" pos="24 352 104 24" buttonText="Remove Zero"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Add Zero" id="ef210bafba4dad7" memberName="addPoleButton"
              virtualName="" explicitFocusOrder="0" pos="152 312 104 24" buttonText="Add Pole"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Add Zero" id="8998cdab90d99df6" memberName="rmvPoleButton"
              virtualName="" explicitFocusOrder="0" pos="152 352 104 24" buttonText="Remove Pole"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

