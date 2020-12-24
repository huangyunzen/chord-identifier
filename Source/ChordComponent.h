#pragma once

#include <JuceHeader.h>
#include <algorithm>
#include <unordered_map>

// multiline TextEditor doesn't support getTextWidth(), so we need INTERVAL_WIDTH_TO_HEIGHT_RATIO
// as an estimate for the interval width
#define INTERVAL_WIDTH_TO_HEIGHT_RATIO 0.38f

// estimate for the width of the parent component with relation to the height
#define WIDTH_TO_HEIGHT_RATIO 1.5715f

// font size and TextEditor height has a difference of 5
#define FONT_SIZE_AND_HEIGHT_DIFF 5

//==============================================================================

//-----Chord Names-----
enum class Chord : char
{
    MajTriadRoot,
    MajTriadFirst,
    MajTriadSecond,
    MinTriadRoot,
    MinTriadFirst,
    MinTriadSecond,
    AugTriadRoot,
    DimTriadRoot,
    Sus542
};

// taken from https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
struct VectorHasher {
    std::size_t operator() (const std::vector<int> &v) const
    {
        std::size_t seed = v.size();
        for (auto& i : v)
        {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

//==============================================================================
class ChordComponent : public juce::Component
{
public:
    ChordComponent();
    ~ChordComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    int getKey() const;
    
    void setKey (int k);
    
    void addNote (int note);
    
    void removeNote (int note);

private:
    void initBox (juce::TextEditor* box);
    
    void setFonts();
    
    void clearAll();
    
    void constructIntervals();
    
    void identify();
    
    void drawRomanNum (const int scaleDegree, const bool capital);
    
    //=======================================
    juce::TextEditor romanNumeralBox;
    juce::TextEditor intervalBox;
    juce::TextEditor accidentalBox;
    
    // any chord would be in the context of a key
    // default is 0 (no key is set)
    int key = 0;
    
    // array used to convert key number to a scale degree that is easier to work with
    const int keyToScaleDegree[30] = {0, 9, 7, 4, 2, 11, 9, 6, 4, 1, 11, 8, 6, 3, 1, 10, 5, 2, 10, 7, 3, 0, 8, 5, 1, 10, 6, 3, 11, 8};
    
    // font size of the roman numeral
    // default is 135.0 for a window of 600x400
    float chordFontSize = 135.0;
    
    // tracks how many numbers are to be displayed in the interval box
    // default is 2 as that is most common
    int numIntervals = 2;
    
    // chord vector stores midi note numbers in order
    std::vector<int> chord;
    
    // intervals vector stores unique intervals in semitones, also in order
    std::vector<int> intervals;
    
    //-----------------------------Chord Database-----------------------------
    const std::unordered_map<std::vector<int>, Chord, VectorHasher> chordDb =
    {
        {std::vector<int> {4, 7}, Chord::MajTriadRoot},
        {std::vector<int> {3, 8}, Chord::MajTriadFirst},
        {std::vector<int> {5, 9}, Chord::MajTriadSecond},
        {std::vector<int> {3, 7}, Chord::MinTriadRoot},
        {std::vector<int> {4, 9}, Chord::MinTriadFirst},
        {std::vector<int> {5, 8}, Chord::MinTriadSecond},
        {std::vector<int> {4, 8}, Chord::AugTriadRoot},
        {std::vector<int> {3, 6}, Chord::DimTriadRoot},
        {std::vector<int> {2, 5, 7}, Chord::Sus542}
    };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordComponent)
};
