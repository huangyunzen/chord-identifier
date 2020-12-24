#include "ChordComponent.h"

//==============================================================================
ChordComponent::ChordComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    setOpaque (false);
    
    // set up background and outline of TextEditors to be transparent
    getLookAndFeel().setColour (juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    getLookAndFeel().setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    
    setFonts();
    
    // set up middle box to display scale degree in roman numerals
    initBox (&romanNumeralBox);
    addAndMakeVisible (romanNumeralBox);
    romanNumeralBox.onTextChange = [this]
    {
        romanNumeralBox.setSize (romanNumeralBox.getTextWidth(), romanNumeralBox.getTextHeight());
        romanNumeralBox.setCentrePosition (romanNumeralBox.getParentWidth() / 2, romanNumeralBox.getParentHeight() / 2);
    };
    
    // set up right box to display intervals of the chord in figured bass notation
    initBox (&intervalBox);
    // multiline so that we can display multiple interval numbers horizontally
    intervalBox.setMultiLine (true);
    addAndMakeVisible (intervalBox);
    intervalBox.onTextChange = [this]
    {
        // set new font size for interalBox according to the numbers needed to be displayed
        float intervalFontSize = (chordFontSize + FONT_SIZE_AND_HEIGHT_DIFF)/numIntervals - FONT_SIZE_AND_HEIGHT_DIFF;
        intervalBox.applyFontToAllText (juce::Font ("Arial Unicode MS", intervalFontSize, juce::Font::plain));
        intervalBox.setSize (romanNumeralBox.getTextHeight() * INTERVAL_WIDTH_TO_HEIGHT_RATIO, romanNumeralBox.getTextHeight());
        intervalBox.setTopLeftPosition (romanNumeralBox.getRight(), romanNumeralBox.getY());
    };
    
    //set up left box to display any accidentals the bass has
    initBox (&accidentalBox);
    addAndMakeVisible (accidentalBox);
    accidentalBox.onTextChange = [this]
    {
        accidentalBox.setSize (accidentalBox.getTextWidth(), accidentalBox.getTextHeight());
        accidentalBox.setTopRightPosition (romanNumeralBox.getX(), romanNumeralBox.getY());
    };
}

ChordComponent::~ChordComponent() {}

void ChordComponent::paint (juce::Graphics& g) {}

void ChordComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    // set chordFontSize to be proportional to the size of the parent (chordBox)
    // but be careful not to have a font size too big that doesn't fit inside the parent
    chordFontSize = romanNumeralBox.getParentHeight() - FONT_SIZE_AND_HEIGHT_DIFF;
    int totalWidthOfBoxes = romanNumeralBox.getParentHeight() * WIDTH_TO_HEIGHT_RATIO;
    if (totalWidthOfBoxes > romanNumeralBox.getParentWidth())
    {
        chordFontSize = romanNumeralBox.getParentWidth() / WIDTH_TO_HEIGHT_RATIO - FONT_SIZE_AND_HEIGHT_DIFF;
    }
    
    // set font sizes before setting the sizes and positions of the boxes
    setFonts();
    
    romanNumeralBox.setSize (romanNumeralBox.getTextWidth(), romanNumeralBox.getTextHeight());
    romanNumeralBox.setCentrePosition (romanNumeralBox.getParentWidth() / 2, romanNumeralBox.getParentHeight() / 2);
    
    intervalBox.setSize (romanNumeralBox.getTextHeight() * INTERVAL_WIDTH_TO_HEIGHT_RATIO, romanNumeralBox.getTextHeight());
    intervalBox.setTopLeftPosition (romanNumeralBox.getRight(), romanNumeralBox.getY());
    
    accidentalBox.setSize (accidentalBox.getTextWidth(), accidentalBox.getTextHeight());
    accidentalBox.setTopRightPosition (romanNumeralBox.getX(), romanNumeralBox.getY());
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
    // removes note in a way that retains order
    auto range = std::equal_range (chord.begin(), chord.end(), note);
    chord.erase (range.first, range.second);
    constructIntervals();
}

//===============================================================================

void ChordComponent::initBox(juce::TextEditor *box)
{
    box->setReadOnly (true);
    box->setScrollbarsShown (false);
    box->setCaretVisible (false);
    box->setPopupMenuEnabled (true);
}

void ChordComponent::setFonts()
{
    romanNumeralBox.applyFontToAllText (juce::Font ("Arial Unicode MS", chordFontSize, juce::Font::plain));
    
    // intervalFontSize set up to divide the height of the box into equal proportions of 2, 3, or 4
    float intervalFontSize = (chordFontSize + FONT_SIZE_AND_HEIGHT_DIFF)/numIntervals - FONT_SIZE_AND_HEIGHT_DIFF;
    intervalBox.applyFontToAllText (juce::Font ("Arial Unicode MS", intervalFontSize, juce::Font::plain));
    
    accidentalBox.applyFontToAllText (juce::Font ("Arial Unicode MS", chordFontSize, juce::Font::plain));
}

void ChordComponent::clearAll()
{
    intervals.clear();
    numIntervals = 2;
    
    romanNumeralBox.setText (juce::String (" "));
    intervalBox.setText (juce::String (" "));
    accidentalBox.setText (juce::String (" "));
    romanNumeralBox.setSize (0, 0);
    intervalBox.setSize (0, 0);
    accidentalBox.setSize (0, 0);
}

void ChordComponent::constructIntervals()
{
    // clear all intervals and boxes to erase any previous chord data
    clearAll();
    
    for (int bassNote = chord[0], i = 1; i < chord.size(); ++i)
    {
        int interval = (chord[i] - bassNote) % 12;
        // add unique intervals to vector, excluding the unison (which forms an interval of 0)
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
        return;
    }
    auto it = chordDb.find (intervals);
    if (it == chordDb.end())
    {
        return;
    }
    
    int scaleDegreeOfKey = keyToScaleDegree[key - 1] + 12;
    switch (it->second)
    {
        case Chord::MajTriadRoot:
            drawRomanNum ((chord[0] - scaleDegreeOfKey) % 12, true);
            break;
        case Chord::MajTriadFirst:
            drawRomanNum ((chord[0] + 8 - scaleDegreeOfKey) % 12, true);
            intervalBox.setText (juce::String ("6"));
            break;
        case Chord::MajTriadSecond:
            if ((chord[0] + 5 - scaleDegreeOfKey) % 12 == 0)
            {
                // cadential 6-4 is a V chord
                drawRomanNum (7, true);
            } else
            {
                drawRomanNum ((chord[0] + 5 - scaleDegreeOfKey) % 12, true);
            }
            intervalBox.setText (juce::String ("6\n4"));
            break;
        case Chord::MinTriadRoot:
            drawRomanNum ((chord[0] - scaleDegreeOfKey) % 12, false);
            break;
        case Chord::MinTriadFirst:
            drawRomanNum ((chord[0] + 9 - scaleDegreeOfKey) % 12, false);
            intervalBox.setText (juce::String ("6"));
            break;
        case Chord::MinTriadSecond:
            drawRomanNum ((chord[0] + 5 - scaleDegreeOfKey) % 12, false);
            intervalBox.setText (juce::String ("6\n4"));
            break;
        case Chord::AugTriadRoot:
            drawRomanNum ((chord[0] - scaleDegreeOfKey) % 12, true);
            intervalBox.setText (juce::String ("+"));
            break;
        case Chord::DimTriadRoot:
            drawRomanNum ((chord[0] - scaleDegreeOfKey) % 12, false);
            intervalBox.setText (juce::String ("o"));
            break;
        case Chord::Sus542:
            drawRomanNum ((chord[0] - scaleDegreeOfKey) % 12, true);
            numIntervals = 3;
            intervalBox.setText (juce::String ("5\n4\n2"));
            break;
        default:
            // should not be run
            std::cerr << "Hash error";
            exit(0);
            break;
    }
}

void ChordComponent::drawRomanNum (const int romanNumeral, const bool capital)
{
    // draws a capital roman numeral depending on major or minor third
    // if key is flat (16-30), draw a sharp key with the lower roman numeral
    // if key is sharp (0-15), including C maj/a min, draw a flat key with the higher roman numeral
    // e.g. B-D-F in the key of F (flat) would have a sharp beside the roman numeral iv,
    // instead of a flat beside the roman numeral v
    
    switch (romanNumeral)
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
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa0")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb0")));
                }
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xaf")));
            } else
            {
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa1")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb1")));
                }
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xad")));
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
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa1")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb1")));
                }
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xaf")));
            } else
            {
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa2")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb2")));
                }
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xad")));
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
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa3")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb3")));
                }
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xaf")));
            } else
            {
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa4")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb4")));
                }
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xad")));
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
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa4")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb4")));
                }
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xaf")));
            } else
            {
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa5")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb5")));
                }
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xad")));
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
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa5")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb5")));
                }
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xaf")));
            } else
            {
                if (capital)
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xa6")));

                } else
                {
                    romanNumeralBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x85\xb6")));
                }
                accidentalBox.setText (juce::String (juce::CharPointer_UTF8 ("\xe2\x99\xad")));
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
