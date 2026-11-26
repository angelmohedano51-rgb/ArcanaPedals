# Arcana Pedals (VST3)

Multiefectos con 7 pedales en forma de carta del tarot. Cadena de senal de izquierda a derecha.

Carta, efecto, perillas
- II Two of Cups, CHORUS: Rate, Depth, Spread
- II Two of Pentacles, DELAY: Time, Feedback, Tone
- VIII Eight of Pentacles, GRANULAR: Size, Density, Pitch (+/- 12 semitonos)
- Page of Cups, LO-FI: Bits, Rate, Wobble
- Page of Swords, NOISE: Level, Color (blanco a marron), Crackle
- XVII The Star, REVERB: Size, Damp, Width
- III Three of Swords, SIDECHAIN: Rate (1 compas a 1/16), Depth, Curve. Sincronizado al tempo del host

Cada pedal tiene su ACTIVE/BYPASS y su MIX. Arriba a la derecha: MIX y OUT globales.

## Compilar e instalar
Requisitos: CMake 3.22+, git, y un compilador (Visual Studio 2022 en Windows, Xcode en macOS, gcc/clang en Linux).

    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release

(En Windows agrega -G "Visual Studio 17 2022".) La primera vez descarga JUCE automaticamente.

El VST3 queda en build/ArcanaPedals_artefacts/Release/VST3/Arcana Pedals.vst3 y se copia solo a:
- Windows: C:\Program Files\Common Files\VST3 (ejecuta la terminal como administrador)
- macOS: ~/Library/Audio/Plug-Ins/VST3
- Linux: ~/.vst3

Tambien genera una version Standalone para probarlo sin DAW.
