# qt-native-clock-40087-40096

This workspace contains a native Qt6 clock application (qt_clock_native) with a retro theme styled using the Ocean Professional palette.

Features
- Analog and digital clock with smooth seconds hand animation
- Date display (auto-updates daily)
- 12/24-hour toggle (persists for the session)
- Timezone selection using QTimeZone (updates all displays)
- Start/Pause control to halt/resume ticking
- Retro styling with segmented-like digital readout, round gauge face, and subtle glow
- Ocean Professional colors:
  - Primary: #2563EB
  - Secondary: #F59E0B
  - Background: #f9fafb
  - Surface: #ffffff
  - Text: #111827

Project Structure
- qt_clock_native/
  - CMakeLists.txt
  - include/mainApp.h
  - src/mainApp.cpp
  - resources/app.qrc (reserved for fonts/icons if added later)

Build and Run
1) Create build directory and configure:
   - mkdir -p qt_clock_native/build
   - cd qt_clock_native/build
   - cmake .. 

2) Build:
   - cmake --build .

3) Run the application:
   - cmake --build . --target run
   OR run the generated binary directly from the build folder.

Notes
- Requires Qt6 with Core, Widgets, and Gui components available in your environment.
- The resource file is included and ready for future fonts/icons if needed.
