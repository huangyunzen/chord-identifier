# Chord Identifier
[![Github All Releases](https://img.shields.io/github/downloads/huangyunzen/chord-identifier/total.svg)](https://github.com/huangyunzen/chord-identifier/releases/latest)

Chord Identifier displays chords using roman numeral and figured bass notation in real-time when played on a MIDI keyboard. Supports macOS, Windows, and Linux.
![screenshot](https://github.com/huangyunzen/chord-identifier/blob/master/Assets/screenshot.png)
## Usage
This app is meant to be a tool for music theory instruction. Simply plug in a MIDI keyboard (before launching the app) and choose a key from the drop-down list. The chords you play will then be displayed using roman numeral and figured bass notation in real-time.

*Note that this app uses "case-sensitive" roman numerals, i.e. uppercase indicate major triads and lowercase indicate minor triads.*
### Currently supported chords
Major/minor triads, Diminished, Augmented, Seventh, with all their respective inversions.
If you want to add more chords (or other features), please create an issue.
## Download
Visit the [releases](https://github.com/huangyunzen/chord-identifier/releases/latest) page to download the latest version. Note that with macOS, since I am not an identified developer, you would need to go to System Preferences > Security & Privacy > General, and click 'Open Anyway'.
## Developers
Steps to contribute:
1. Download Juce/Projucer and a supported IDE (Xcode for macOS and Visual Studio for Windows)
2. Fork the repository
3. In Projucer, open ChordIdentifier.jucer and select "Save and Open in IDE"
4. Build
5. After you are done with your changes, submit a pull request to the master branch
