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
    keyboardComponent.setColour (juce::MidiKeyboardComponent::keyDownOverlayColourId, juce::Colours::cornflowerblue);
    keyboardComponent.setColour (juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, juce::Colours::lightsteelblue);
    keyboardState.addListener (this);
    
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
    setLookAndFeel (nullptr);
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

    midiInputList.setBounds (80, 0, 120, 24);
    keyList.setBounds (250, 0, 85, 24);
    
    // keyboardComponent takes up 20% of the window
    keyboardComponent.setBoundsRelative (0.0f, 0.8f, 1.0f, 0.2f);
    if (keyboardComponent.getParentWidth() > DEFAULT_KEYBOARD_WIDTH_PIXELS)
    {
        keyboardComponent.setKeyWidth ((float)keyboardComponent.getParentWidth() / (float)DEFAULT_NUM_WHITE_KEYS);
    }
    
    auto area = getBounds();
    int startWidth = static_cast<int>(area.getWidth() * 0.2867);
    int startHeight = static_cast<int>(area.getHeight() * 0.225f + 12);
    int boxWidth = static_cast<int>(area.getWidth() * 0.4267);
    int boxHeight = static_cast<int>(area.getHeight() * 0.35);
    chordBox.setBounds (startWidth, startHeight, boxWidth, boxHeight);
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
void MainComponent::handleIncomingMidiMessage (juce::MidiInput* /*source*/, const juce::MidiMessage& message)
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
