#include "ChordComponent.h"

//==============================================================================
ChordComponent::ChordComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    setOpaque (false);
    
    // set up background and outline of TextEditors to be transparent
    getLookAndFeel().setColour (juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    getLookAndFeel().setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    
    // set up middle box to display scale degree in roman numerals
    initBox (&romanNumeralBox, 150);
    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb6")));
    /*int fontHeight = romanNumeralBox.getFont().getHeight();
    int fontWidth = romanNumeralBox.getFont().getStringWidth(romanNumeralBox.getText());
    int topIndent = (romanNumeralBox.getHeight() - fontHeight) / 2;
    int leftIndent = (romanNumeralBox.getWidth() - fontWidth) / 2;
    romanNumeralBox.setIndents (leftIndent, topIndent);*/
    addAndMakeVisible (romanNumeralBox);
    
    // set up right box to display intervals of the chord in figured bass notation
    initBox (&intervalBox, 65);
    intervalBox.setMultiLine (true);
    intervalBox.setText (juce::String ("\n6\n4"));
    addAndMakeVisible (intervalBox);
    
    //set up left box to display any accidentals the bass has
    initBox (&accidentalBox, 100);
    accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xad")));
    addAndMakeVisible (accidentalBox);
}

ChordComponent::~ChordComponent()
{
}

void ChordComponent::paint (juce::Graphics& g) {}

void ChordComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    accidentalBox.setBoundsRelative (0.0f, 0.25f, 0.15f, 0.5f);
    romanNumeralBox.setBoundsRelative (0.15f, 0.25f, 0.42f, 0.5f);
    intervalBox.setBoundsRelative (0.55f, 0.0f, 0.3f, 1.0f);
}

void ChordComponent::initBox(juce::TextEditor *box, const int fontSize)
{
    box->setFont (juce::Font ("Arial Unicode MS", fontSize, juce::Font::plain));
    box->setReadOnly (true);
    box->setScrollbarsShown (false);
    box->setCaretVisible (false);
    box->setPopupMenuEnabled (true);
}
