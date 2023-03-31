# Optimized M4A Converter

Uses wxWidgets to create a GUI for leveraging [FFMPEG](http://ffmpeg.org/)'s executable (Windows) or `afconvert` (MacOS), in order to create **<u>optimized</u> M4A (AAC) files** from WAVs or other M4A files. For **MacOS 10.13+** and **Windows**.

## Prerequisites

Requires [FFMPEG executable](http://ffmpeg.org/download.html) to be downloaded and placed in specific folders:

- For **MacOS**: `Optimized_m4a_converter.app/Contents/Resources/`
- For **Windows**: A folder named `Resources` in the same directory as the executable.

## Roadmap

- Output files in their own `M4A` folder **--- DONE**
- Ability to cancel entire batch **--- DONE**
- Ability to drag and drop folders **--- DONE**
- Progress bar **--- DONE**
- Aesthetic improvements **--- DONE**(?)
- Settings menu (eg. configure bit rate, etc) **--- DONE** _(Needs to be tested on Windows)_
- Change destination folder
- (?) "Quick mode" - dragging and droping files automatically converts them to predefined settings

## Acknowledgments

- Built using [lszl84](https://github.com/lszl84)'s [wx_cmake_template](https://github.com/lszl84/wx_cmake_template). His [tutorials](https://www.youtube.com/channel/UC4Q-KGKCeFbBpaAqwllCDqQ) are awesome.
