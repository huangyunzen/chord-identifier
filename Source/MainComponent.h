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
    const juce::StringArray keyArray =
    {
        juce::String (juce::CharPointer_UTF8 ("C major")),
        juce::String (juce::CharPointer_UTF8 ("a minor")),
        juce::String (juce::CharPointer_UTF8 ("G major")),
        juce::String (juce::CharPointer_UTF8 ("e minor")),
        juce::String (juce::CharPointer_UTF8 ("D major")),
        juce::String (juce::CharPointer_UTF8 ("b minor")),
        juce::String (juce::CharPointer_UTF8 ("A major")),
        juce::String (juce::CharPointer_UTF8 ("f\xe2\x99\xaf minor")),
        juce::String (juce::CharPointer_UTF8 ("E major")),
        juce::String (juce::CharPointer_UTF8 ("c\xe2\x99\xaf minor")),
        juce::String (juce::CharPointer_UTF8 ("B major")),
        juce::String (juce::CharPointer_UTF8 ("g\xe2\x99\xaf minor")),
        juce::String (juce::CharPointer_UTF8 ("F\xe2\x99\xaf major")),
        juce::String (juce::CharPointer_UTF8 ("d\xe2\x99\xaf minor")),
        juce::String (juce::CharPointer_UTF8 ("C\xe2\x99\xaf major")),
        juce::String (juce::CharPointer_UTF8 ("a\xe2\x99\xaf minor")),
        juce::String (juce::CharPointer_UTF8 ("F major")),
        juce::String (juce::CharPointer_UTF8 ("d minor")),
        juce::String (juce::CharPointer_UTF8 ("B\xe2\x99\xad major")),
        juce::String (juce::CharPointer_UTF8 ("g minor")),
        juce::String (juce::CharPointer_UTF8 ("E\xe2\x99\xad major")),
        juce::String (juce::CharPointer_UTF8 ("c minor")),
        juce::String (juce::CharPointer_UTF8 ("A\xe2\x99\xad major")),
        juce::String (juce::CharPointer_UTF8 ("f minor")),
        juce::String (juce::CharPointer_UTF8 ("D\xe2\x99\xad major")),
        juce::String (juce::CharPointer_UTF8 ("b\xe2\x99\xad minor")),
        juce::String (juce::CharPointer_UTF8 ("G\xe2\x99\xad major")),
        juce::String (juce::CharPointer_UTF8 ("e\xe2\x99\xad minor")),
        juce::String (juce::CharPointer_UTF8 ("C\xe2\x99\xad major")),
        juce::String (juce::CharPointer_UTF8 ("a\xe2\x99\xad minor"))
    };
    
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
    
    ChordComponent chordBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
