#pragma once

#include <JuceHeader.h>
#include "ChordComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::Component,
                      private juce::MidiInputCallback,
                      private juce::MidiKeyboardStateListener
{
public:
    //==============================================================================
    MainComponent();
    
    ~MainComponent() override;

    void paint (juce::Graphics& g) override;

    void resized() override;

private:
    //==============================================================================
    void setMidiInput (int index);
    
    void handleIncomingMidiMessage (juce::MidiInput* source, const juce::MidiMessage& message) override;
    
    void handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    
    void handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) override;
    
    class IncomingMessageCallback : public juce::CallbackMessage
    {
    public:
        IncomingMessageCallback(MainComponent* o, const juce::MidiMessage& m);
        
        void messageCallback() override;
        
        Component::SafePointer<MainComponent> owner;
        juce::MidiMessage message;
    };
    
    void postMessage (const juce::MidiMessage& message);
    
    void addMessage (const juce::MidiMessage& message);
    
    // Member variables
    juce::AudioDeviceManager deviceManager;
    juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;
    bool isAddingFromMidiInput = false;
    
    juce::ComboBox keyList;
    juce::Label keyListLabel;
    const juce::String keyArray[12] = {juce::CharPointer_UTF8 ("\x43"),  // C
                                       juce::CharPointer_UTF8 ("\x43\xe2\x99\xaf\x2f\x44\xe2\x99\xad"),  // C sharp or D flat
                                       juce::CharPointer_UTF8 ("\x44"),  // D
                                       juce::CharPointer_UTF8 ("\x44\xe2\x99\xaf\x2f\x45\xe2\x99\xad"),  // D sharp or E flat
                                       juce::CharPointer_UTF8 ("\x45"),  // E
                                       juce::CharPointer_UTF8 ("\x46"),  // F
                                       juce::CharPointer_UTF8 ("\x46\xe2\x99\xaf\x2f\x47\xe2\x99\xad"),  // F sharp or G flat
                                       juce::CharPointer_UTF8 ("\x47"),  // G
                                       juce::CharPointer_UTF8 ("\x47\xe2\x99\xaf\x2f\x41\xe2\x99\xad"),  // G sharp or A flat
                                       juce::CharPointer_UTF8 ("\x41"),  // A
                                       juce::CharPointer_UTF8 ("\x41\xe2\x99\xaf\x2f\x42\xe2\x99\xad"),  // A sharp or B flat
                                       juce::CharPointer_UTF8 ("\x42\x2f\x43\xe2\x99\xad")};  // B or C flat
    
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
    
    ChordComponent chord;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
