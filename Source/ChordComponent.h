#pragma once

#include <JuceHeader.h>
#include <algorithm>
#include <unordered_map>

// taken from https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
struct VectorHasher {
    std::size_t operator() (const std::vector<int> &v) const
    {
        std::size_t seed = v.size();
        for (auto& i : v)
        {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        //std::cout << seed << std::endl;
        return seed;
    }
};

enum class Chord : char
{
    MajTriadRoot,
    MajTriadFirst,
    MajTriadSecond,
    MinTriadRoot,
    MinTriadFirst,
    MinTriadSecond
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
    void initBox (juce::TextEditor* box, const int fontSize);
    
    void clearAll();
    
    void constructIntervals();
    
    void identify();
    
    void drawRomanNum (const int scaleDegree, const bool capital);
    
    //void drawChord (const int );
    
    //=======================================
    juce::TextEditor romanNumeralBox;
    juce::TextEditor intervalBox;
    juce::TextEditor accidentalBox;
    
    // any chord would be in the context of a key
    int key;
    const int scaleDegreeOfKey[30] = {0, 9, 7, 4, 2, 11, 9, 6, 4, 1, 11, 8, 6, 3, 1, 10, 5, 2, 10, 7, 3, 0, 8, 5, 1, 10, 6, 3, 11, 8};
    
    std::vector<int> chord;
    std::vector<int> intervals;
    const std::unordered_map<std::vector<int>, Chord, VectorHasher> chordMap =
    {
        {std::vector<int> {4, 7}, Chord::MajTriadRoot},
        {std::vector<int> {3, 8}, Chord::MajTriadFirst},
        {std::vector<int> {5, 9}, Chord::MajTriadSecond},
        {std::vector<int> {3, 7}, Chord::MinTriadRoot},
        {std::vector<int> {4, 9}, Chord::MinTriadFirst},
        {std::vector<int> {5, 8}, Chord::MinTriadSecond}
    };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordComponent)
};
