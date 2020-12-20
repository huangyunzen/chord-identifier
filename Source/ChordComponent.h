#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ChordComponent  : public juce::Component
{
public:
    ChordComponent();
    ~ChordComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void initBox (juce::TextEditor* box, const int fontSize);
    
    //================================
    juce::TextEditor romanNumeralBox;
    juce::TextEditor intervalBox;
    juce::TextEditor accidentalBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordComponent)
};
