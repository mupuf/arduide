#!/bin/sh
cat <<EOF
[Desktop Entry]
Name=Arduino IDE
GenericName=Arduino IDE
Comment=Qt-based Arduino IDE
Comment[fr]=IDE Arduino basée sur Qt
Exec=$1/bin/arduino-ide
Icon=arduide
Type=Application
Terminal=false
Categories=Development;IDE;
EOF
