#include "ChordComponent.h"

//==============================================================================
ChordComponent::ChordComponent()
  : key (0)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    setOpaque (false);
    
    // set up background and outline of TextEditors to be transparent
    getLookAndFeel().setColour (juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    getLookAndFeel().setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    
    // set up middle box to display scale degree in roman numerals
    initBox (&romanNumeralBox, 150);
    //romanNumeralBox.setText (juce::String ("abcde"));
    /*int fontHeight = romanNumeralBox.getFont().getHeight();
    int fontWidth = romanNumeralBox.getFont().getStringWidth(romanNumeralBox.getText());
    int topIndent = (romanNumeralBox.getHeight() - fontHeight) / 2;
    int leftIndent = (romanNumeralBox.getWidth() - fontWidth) / 2;
    romanNumeralBox.setIndents (leftIndent, topIndent);*/
    addAndMakeVisible (romanNumeralBox);
    
    // set up right box to display intervals of the chord in figured bass notation
    initBox (&intervalBox, 65);
    intervalBox.setMultiLine (true);
    //intervalBox.setText (juce::String ("\n6\n4"));
    addAndMakeVisible (intervalBox);
    
    //set up left box to display any accidentals the bass has
    initBox (&accidentalBox, 100);
    //accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xad")));
    addAndMakeVisible (accidentalBox);
}

ChordComponent::~ChordComponent()
{
    clearAll();
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

int ChordComponent::getKey() const
{
    return key;
}

void ChordComponent::setKey (int k)
{
    clearAll();
    key = k;
}

void ChordComponent::addNote (int note)
{
    chord.emplace_back (note);
    std::sort (chord.begin(), chord.end());
    constructIntervals();
}

void ChordComponent::removeNote (int note)
{
    auto range = std::equal_range (chord.begin(), chord.end(), note);
    chord.erase (range.first, range.second);
    constructIntervals();
}

//======================================================================

void ChordComponent::initBox(juce::TextEditor *box, const int fontSize)
{
    box->setFont (juce::Font ("Arial Unicode MS", fontSize, juce::Font::plain));
    box->setReadOnly (true);
    box->setScrollbarsShown (false);
    box->setCaretVisible (false);
    box->setPopupMenuEnabled (true);
}

void ChordComponent::clearAll()
{
    romanNumeralBox.setText (juce::String (" "));
    intervalBox.setText (juce::String (" "));
    accidentalBox.setText (juce::String (" "));
}

void ChordComponent::constructIntervals()
{
    // clear all intervals
    intervals.clear();
    
    for (int bassNote = chord[0], i = 1; i < chord.size(); ++i)
    {
        int interval = (chord[i] - bassNote) % 12;
        if (interval != 0 && std::find (intervals.begin(), intervals.end(), interval) == intervals.end())
        {
            intervals.emplace_back (interval);
        }
    }
    std::sort (intervals.begin(), intervals.end());
    identify();
}

void ChordComponent::identify()
{
    // return if chord has less than 2 unique intervals, or if we cannot find the chord
    if (intervals.size() < 2)
    {
        clearAll();
        return;
    }
    auto it = chordMap.find (intervals);
    if (it == chordMap.end())
    {
        clearAll();
        return;
    }
    switch (it->second)
    {
        case Chord::MajTriadRoot:
            drawRomanNum ((chord[0] - scaleDegreeOfKey[key - 1] + 12) % 12, true);
            break;
        case Chord::MajTriadFirst:
            drawRomanNum ((chord[0] + 8 - scaleDegreeOfKey[key - 1] + 12) % 12, true);
            intervalBox.setText (juce::String ("\n6"));
            break;
        case Chord::MajTriadSecond:
            drawRomanNum ((chord[0] + 5 - scaleDegreeOfKey[key - 1] + 12) % 12, true);
            intervalBox.setText (juce::String ("\n6\n4"));
            break;
        case Chord::MinTriadRoot:
            drawRomanNum ((chord[0] - scaleDegreeOfKey[key - 1] + 12) % 12, false);
            break;
        case Chord::MinTriadFirst:
            drawRomanNum ((chord[0] + 9 - scaleDegreeOfKey[key - 1] + 12) % 12, false);
            intervalBox.setText (juce::String ("\n6"));
            break;
        case Chord::MinTriadSecond:
            drawRomanNum ((chord[0] + 5 - scaleDegreeOfKey[key - 1] + 12) % 12, false);
            intervalBox.setText (juce::String ("\n6\n4"));
            break;
        default:
            clearAll();
            break;
    }
}

void ChordComponent::drawRomanNum (const int scaleDegree, const bool capital)
{
    switch (scaleDegree)
    {
        case 0:
            if (capital)
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa0")));
            } else
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb0")));
            }
            break;
        case 1:
            if (key > 15)
            {
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xaf")));
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa0")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb0")));
                }
            } else
            {
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xad")));
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa1")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb1")));
                }
            }
            break;
        case 2:
            if (capital)
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa1")));
            } else
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb1")));
            }
            break;
        case 3:
            if (key > 15)
            {
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xaf")));
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa1")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb1")));
                }
            } else
            {
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xad")));
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa2")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb2")));
                }
            }
            break;
        case 4:
            if (capital)
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa2")));
            } else
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb2")));
            }
            break;
        case 5:
            if (capital)
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa3")));
            } else
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb3")));
            }
            break;
        case 6:
            if (key > 15)
            {
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xaf")));
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa3")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb3")));
                }
            } else
            {
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xad")));
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa4")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb4")));
                }
            }
            break;
        case 7:
            if (capital)
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa4")));
            } else
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb4")));
            }
            break;
        case 8:
            if (key > 15)
            {
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xaf")));
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa4")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb4")));
                }
            } else
            {
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xad")));
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa5")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb5")));
                }
            }
            break;
        case 9:
            if (capital)
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa5")));
            } else
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb5")));
            }
            break;
        case 10:
            if (key > 15)
            {
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xaf")));
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa5")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb5")));
                }
            } else
            {
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xad")));
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa6")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb6")));
                }
            }
            break;
        case 11:
            if (capital)
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa6")));
            } else
            {
                romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb6")));
            }
            break;
    }
}
