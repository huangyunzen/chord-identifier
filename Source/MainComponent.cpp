#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
  : keyboardComponent (keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setOpaque (true);
    
    addAndMakeVisible (midiInputListLabel);
    midiInputListLabel.setText ("MIDI Input:", juce::dontSendNotification);
    midiInputListLabel.attachToComponent (&midiInputList, true);
    
    addAndMakeVisible (midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    auto midiInputs = juce::MidiInput::getAvailableDevices();
    // add available midi devices to list
    juce::StringArray midiInputNames;
    for (auto input : midiInputs)
    {
        midiInputNames.add (input.name);
    }
    midiInputList.addItemList (midiInputNames, 1);
    midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };
    // find the first enabled device and use that by default
    for (auto input : midiInputs)
    {
        if (deviceManager.isMidiInputDeviceEnabled (input.identifier))
        {
            setMidiInput (midiInputs.indexOf (input));
            break;
        }
    }
    // if no enabled devices were found just use the first one in the list
    if (midiInputList.getSelectedId() == 0)
    {
        setMidiInput (0);
    }

    addAndMakeVisible (keyListLabel);
    keyListLabel.setText ("Key:", juce::dontSendNotification);
    keyListLabel.attachToComponent (&keyList, true);
    
    addAndMakeVisible (keyList);
    keyList.setTextWhenNothingSelected ("--");
    // add all keys to list
    for (int i = 0; i < keyArray.size(); ++i)
    {
        keyList.addItem (keyArray[i], i + 1);
        if (i == 15)
        {
            keyList.addSeparator();
        }
    }
    
    addAndMakeVisible (chordBox);
    keyList.onChange = [this] { chordBox.setKey(keyList.getSelectedId()); };
    
    addAndMakeVisible (keyboardComponent);
    keyboardComponent.setOctaveForMiddleC (4);
    keyboardState.addListener (this);
    
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
    keyboardState.removeListener (this);
    deviceManager.removeMidiInputDeviceCallback (juce::MidiInput::getAvailableDevices()[midiInputList.getSelectedItemIndex()].identifier, this);
}

void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    auto area = getLocalBounds();

    midiInputList.setBounds (80, 0, 120, 25);
    keyList.setBounds (250, 0, 80, 25);
    /*scaleDegreeBox   .setBoundsRelative (0.4f, 0.2f, 0.2f, 0.43f);
    intervalBox      .setBoundsRelative (0.6f, 0.08f, 0.15f, 0.65f);*/
    chordBox.setBoundsRelative (0.35f, 0.08f, 0.425f, 0.65f);
    keyboardComponent.setBounds (area.removeFromBottom (80));
}

//==============================================================================

void MainComponent::setMidiInput (int index)
{
    auto list = juce::MidiInput::getAvailableDevices();

    deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier, this);

    auto newInput = list[index];

    if (! deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
    {
        deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);
    }

    deviceManager.addMidiInputDeviceCallback (newInput.identifier, this);
    midiInputList.setSelectedId (index + 1, juce::dontSendNotification);

    lastInputIndex = index;
}

// These methods handle callbacks from the midi device + on-screen keyboard..
void MainComponent::handleIncomingMidiMessage (juce::MidiInput* source, const juce::MidiMessage& message)
{
    const juce::ScopedValueSetter<bool> scopedInputFlag (isAddingFromMidiInput, true);
    keyboardState.processNextMidiEvent (message);
    postMessage (message);
}

void MainComponent::handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    if (! isAddingFromMidiInput)
    {
        auto m = juce::MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity);
        postMessage (m);
    }
}

void MainComponent::handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/)
{
    if (! isAddingFromMidiInput)
    {
        auto m = juce::MidiMessage::noteOff (midiChannel, midiNoteNumber);
        postMessage (m);
    }
}

MainComponent::IncomingMessageCallback::IncomingMessageCallback (MainComponent* o, const juce::MidiMessage& m)
  : owner (o), message (m)
{}

void MainComponent::IncomingMessageCallback::messageCallback()
{
    if (owner != nullptr)
    {
        owner->addMessage (message);
    }
}

void MainComponent::postMessage (const juce::MidiMessage& message)
{
    (new IncomingMessageCallback (this, message))->post();
}

void MainComponent::addMessage (const juce::MidiMessage& message)
{
    // return if key is not set
    if (chordBox.getKey() == 0)
    {
        return;
    }
    
    if (message.isNoteOn())
    {
        chordBox.addNote (message.getNoteNumber());
    } else if (message.isNoteOff())
    {
        chordBox.removeNote (message.getNoteNumber());
    }
}

