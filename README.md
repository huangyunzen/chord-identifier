# Chord Identifier
Chord Identifier displays chords using roman numeral and figured bass notation in real-time when played on a MIDI keyboard. Supports macOS and Windows.
![screenshot](https://github.com/huangyunzen/chord-identifier/blob/master/Assets/screenshot.png)
## Usage
This app is meant to be a tool for music theory instruction. Simply plug in a MIDI keyboard (before launching the app) and choose a key from the drop-down list. The chords you play will then be displayed using roman numeral and figured bass notation in real-time.

*Note that this app uses "case-sensitive" roman numerals, i.e. uppercase indicate major triads and lowercase indicate minor triads.*
### Currently supported chords
Major/minor triads, Diminished, Augmented, Seventh, with all their respective inversions.
If you want to add more chords (or other features), either head over to the [developers](#developers) section or send me an email at huangyunzen@gmail.com.
## Download
Visit the [releases](https://github.com/huangyunzen/chord-identifier/releases) page to download the latest release.
## Developers
Steps to contribute:
1. Download Juce/Projucer and a supported IDE (Xcode for macOS and Visual Studio for Windows)
2. Clone the repository
3. In Projucer, open ChordIdentifier.jucer and select "Save and Open in IDE"
4. Build
5. After you are done with your changes, submit a pull request to the master branch
