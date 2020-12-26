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
        intervalBox.applyFontToAllText (juce::Font (intervalFontSize, juce::Font::plain));
        intervalBox.setSize (static_cast<int>(romanNumeralBox.getTextHeight() * INTERVAL_WIDTH_TO_HEIGHT_RATIO), romanNumeralBox.getTextHeight());
        intervalBox.setTopLeftPosition (romanNumeralBox.getRight(), romanNumeralBox.getY());
    };
    
    // set up left box to display any accidentals the bass has
    initBox (&accidentalBox);
    addAndMakeVisible (accidentalBox);
    accidentalBox.onTextChange = [this]
    {
        accidentalBox.setSize (accidentalBox.getTextWidth(), accidentalBox.getTextHeight());
        accidentalBox.setTopRightPosition (romanNumeralBox.getX(), romanNumeralBox.getY());
    };
    
    // set up second right box in between intervalBox and romanNumeralBox to display diminished/augmented status
    initBox (&diminishedBox);
    addAndMakeVisible (diminishedBox);
    diminishedBox.onTextChange = [this]
    {
        diminishedBox.setSize (diminishedBox.getTextWidth(), diminishedBox.getTextHeight());
        diminishedBox.setTopLeftPosition (romanNumeralBox.getRight(), romanNumeralBox.getY());
        intervalBox.setTopLeftPosition (diminishedBox.getRight(), diminishedBox.getY());
    };
}

ChordComponent::~ChordComponent() {}

void ChordComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    // set chordFontSize to be proportional to the size of the parent (chordBox)
    // but be careful not to have a font size too big that doesn't fit inside the parent
    chordFontSize = static_cast<float>(romanNumeralBox.getParentHeight() - FONT_SIZE_AND_HEIGHT_DIFF);
    int totalWidthOfBoxes = static_cast<int>(romanNumeralBox.getParentHeight() * WIDTH_TO_HEIGHT_RATIO);
    if (totalWidthOfBoxes > romanNumeralBox.getParentWidth())
    {
        chordFontSize = romanNumeralBox.getParentWidth() / WIDTH_TO_HEIGHT_RATIO - FONT_SIZE_AND_HEIGHT_DIFF;
    }
    
    // set font sizes before setting the sizes and positions of the boxes
    setFonts();
    
    romanNumeralBox.setSize (romanNumeralBox.getTextWidth(), romanNumeralBox.getTextHeight());
    romanNumeralBox.setCentrePosition (romanNumeralBox.getParentWidth() / 2, romanNumeralBox.getParentHeight() / 2);
    
    intervalBox.setSize (static_cast<int>(romanNumeralBox.getTextHeight() * INTERVAL_WIDTH_TO_HEIGHT_RATIO), romanNumeralBox.getTextHeight());
    if (diminishedBox.getWidth() == 0)
    {
        intervalBox.setTopLeftPosition (romanNumeralBox.getRight(), romanNumeralBox.getY());
    } else
    {
        diminishedBox.setSize (diminishedBox.getTextWidth(), diminishedBox.getTextHeight());
        diminishedBox.setTopLeftPosition (romanNumeralBox.getRight(), romanNumeralBox.getY());
        intervalBox.setTopLeftPosition (diminishedBox.getRight(), diminishedBox.getY());
    }
    
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
    romanNumeralBox.applyFontToAllText (juce::Font (chordFontSize, juce::Font::plain));
    
    // intervalFontSize set up to divide the height of the box into equal proportions of 2, 3, or 4
    float intervalFontSize = (chordFontSize + FONT_SIZE_AND_HEIGHT_DIFF)/numIntervals - FONT_SIZE_AND_HEIGHT_DIFF;
    intervalBox.applyFontToAllText (juce::Font (intervalFontSize, juce::Font::plain));
    
    accidentalBox.applyFontToAllText (juce::Font (chordFontSize, juce::Font::plain));
    
    float diminishedFontSize = (chordFontSize + FONT_SIZE_AND_HEIGHT_DIFF)/2 - FONT_SIZE_AND_HEIGHT_DIFF;
    diminishedBox.applyFontToAllText (juce::Font (diminishedFontSize, juce::Font::plain));
}

void ChordComponent::clearAll()
{
    intervals.clear();
    numIntervals = 2;
    
    romanNumeralBox.setText (" ");
    intervalBox.setText (" ");
    accidentalBox.setText (" ");
    diminishedBox.setText (" ");
    romanNumeralBox.setSize (0, 0);
    intervalBox.setSize (0, 0);
    accidentalBox.setSize (0, 0);
    diminishedBox.setSize (0, 0);
}

void ChordComponent::constructIntervals()
{
    // clear all intervals and boxes to erase any previous chord data
    clearAll();

    // return if chord has less than 3 notes
    if (chord.size() < 3)
    {
        return;
    }
    
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
    
    int chromaticDegree = chord[0] + 12 - keyToScaleDegree[key - 1];
    switch (it->second)
    {
        case Chord::MajTriadRoot:
            drawRomanNum (chromaticDegree % 12, true);
            break;
        case Chord::MajTriadFirst:
            drawRomanNum ((chromaticDegree + 8) % 12, true);
            intervalBox.setText ("6");
            break;
        case Chord::MajTriadSecond:
            if ((chromaticDegree + 5) % 12 == 0)
            {
                // cadential 6-4 is a V chord
                drawRomanNum (7, true);
            }
            else
            {
                drawRomanNum ((chromaticDegree + 5) % 12, true);
            }
            intervalBox.setText ("6\n4");
            break;
        case Chord::MinTriadRoot:
            drawRomanNum (chromaticDegree % 12, false);
            break;
        case Chord::MinTriadFirst:
            drawRomanNum ((chromaticDegree + 9) % 12, false);
            intervalBox.setText ("6");
            break;
        case Chord::AugTriadRoot:
            drawRomanNum (chromaticDegree % 12, true);
            diminishedBox.setText ("+");
            break;
        case Chord::DimTriadRoot:
            drawRomanNum (chromaticDegree % 12, false);
            diminishedBox.setText ("0");
            break;
        case Chord::DimTriadFirst:
            drawRomanNum ((chromaticDegree + 9) % 12, false);
            intervalBox.setText ("6");
            diminishedBox.setText ("0");
            break;
        case Chord::SeventhRoot:
            drawRomanNum (chromaticDegree % 12, true);
            intervalBox.setText ("7");
            break;
        case Chord::SeventhFirst:
            drawRomanNum ((chromaticDegree + 8) % 12, true);
            intervalBox.setText ("6\n5");
            break;
        case Chord::SeventhSecond:
            drawRomanNum ((chromaticDegree + 5) % 12, true);
            intervalBox.setText ("4\n3");
            break;
        case Chord::SeventhThird:
            drawRomanNum ((chromaticDegree + 2) % 12, true);
            intervalBox.setText ("4\n2");
            break;
        case Chord::DimSeventh:
            switch (chromaticDegree % 12)
            {
                case 11:
                    intervalBox.setText ("7");
                    break;
                case 2:
                    intervalBox.setText ("6\n5");
                    break;
                case 5:
                    intervalBox.setText ("4\n3");
                    break;
                case 8:
                    intervalBox.setText ("4\n2");
                    break;
                default:
                    return;
            }
            drawRomanNum (11, false);
            diminishedBox.setText ("0");
            break;
        case Chord::HalfDimSeventhRoot:
            drawRomanNum (chromaticDegree % 12, false);
            intervalBox.setText ("7");
            diminishedBox.setText (juce::CharPointer_UTF8 ("\xc3\xb8"));
            break;
        case Chord::HalfDimSeventhFirst:
            drawRomanNum ((chromaticDegree + 9) % 12, false);
            intervalBox.setText ("6\n5");
            diminishedBox.setText (juce::CharPointer_UTF8 ("\xc3\xb8"));
            break;
        case Chord::HalfDimSeventhSecond:
            drawRomanNum ((chromaticDegree + 6) % 12, false);
            intervalBox.setText ("4\n3");
            diminishedBox.setText (juce::CharPointer_UTF8 ("\xc3\xb8"));
            break;
        case Chord::HalfDimSeventhThird:
            drawRomanNum ((chromaticDegree + 2) % 12, false);
            intervalBox.setText ("4\n2");
            diminishedBox.setText (juce::CharPointer_UTF8 ("\xc3\xb8"));
            break;
        case Chord::MinSeventhRoot:
            drawRomanNum (chromaticDegree % 12, false);
            intervalBox.setText ("7");
            break;
        case Chord::MinSeventhFirst:
            drawRomanNum ((chromaticDegree + 9) % 12, false);
            intervalBox.setText ("6\n5");
            break;
        case Chord::MinSeventhSecond:
            drawRomanNum ((chromaticDegree + 5) % 12, false);
            intervalBox.setText ("4\n3");
            break;
        case Chord::MinSeventhThird:
            drawRomanNum ((chromaticDegree + 2) % 12, false);
            intervalBox.setText ("4\n2");
            break;
        default:
            // should not be run
            std::cerr << "Hash error";
            exit(0);
            break;
    }
}

void ChordComponent::drawRomanNum (const int chromaticDegree, const bool capital)
{
    // draws a capital roman numeral depending on chord having major or minor third
    // for 3, 8 semitones (minor third and sixth), draw a flat if key is major
    // for 4, 9 semitones (major third and sixth), draw a sharp if key is minor
    // for 6 semitones, it forms a dim. fifth for sharp keys (including C major/a minor),
    // otherwise for flat keys it forms a aug. fourth
    
    const bool major = key % 2;
    
    switch (chromaticDegree)
    {
        case 0:
            if (capital)
            {
                romanNumeralBox.setText ("I");
            } else
            {
                romanNumeralBox.setText ("i");
            }
            break;
        case 1:
            if (capital)
            {
                romanNumeralBox.setText ("II");
            } else
            {
                romanNumeralBox.setText ("ii");
            }
            accidentalBox.setText (juce::CharPointer_UTF8 ("\xe2\x99\xad"));
            break;
        case 2:
            if (capital)
            {
                romanNumeralBox.setText ("II");
            } else
            {
                romanNumeralBox.setText ("ii");
            }
            break;
        case 3:
            if (capital)
            {
                romanNumeralBox.setText ("III");
            } else
            {
                romanNumeralBox.setText ("iii");
            }
            if (major)
            {
                accidentalBox.setText (juce::CharPointer_UTF8 ("\xe2\x99\xad"));
                
            }
            break;
        case 4:
            if (capital)
            {
                romanNumeralBox.setText ("III");
            } else
            {
                romanNumeralBox.setText ("iii");
            }
            if (!major)
            {
                accidentalBox.setText (juce::CharPointer_UTF8 ("\xe2\x99\xaf"));
            }
            break;
        case 5:
            if (capital)
            {
                romanNumeralBox.setText ("IV");
            } else
            {
                romanNumeralBox.setText ("iv");
            }
            break;
        case 6:
            if (key > 15)  // flat keys
            {
                if (capital)
                {
                    romanNumeralBox.setText ("IV");
                } else
                {
                    romanNumeralBox.setText ("iv");
                }
                accidentalBox.setText (juce::CharPointer_UTF8 ("\xe2\x99\xaf"));
            } else  // sharp keys
            {
                if (capital)
                {
                    romanNumeralBox.setText ("V");
                } else
                {
                    romanNumeralBox.setText ("v");
                }
                accidentalBox.setText (juce::CharPointer_UTF8 ("\xe2\x99\xad"));
            }
            break;
        case 7:
            if (capital)
            {
                romanNumeralBox.setText ("V");
            } else
            {
                romanNumeralBox.setText ("v");
            }
            break;
        case 8:
            if (capital)
            {
                romanNumeralBox.setText ("VI");
            } else
            {
                romanNumeralBox.setText ("vi");
            }
            if (major)
            {
                accidentalBox.setText (juce::CharPointer_UTF8 ("\xe2\x99\xad"));
                
            }
            break;
        case 9:
            if (capital)
            {
                romanNumeralBox.setText ("VI");
            } else
            {
                romanNumeralBox.setText ("vi");
            }
            if (!major)
            {
                accidentalBox.setText (juce::CharPointer_UTF8 ("\xe2\x99\xaf"));
            }
            break;
        case 10:
            if (capital)
            {
                romanNumeralBox.setText ("VII");
            } else
            {
                romanNumeralBox.setText ("vii");
            }
            accidentalBox.setText (juce::CharPointer_UTF8 ("\xe2\x99\xad"));
            break;
        case 11:
            if (capital)
            {
                romanNumeralBox.setText ("VII");
            } else
            {
                romanNumeralBox.setText ("vii");
            }
            break;
    }
}
