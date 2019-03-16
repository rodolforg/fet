TEMPLATE = subdirs
SUBDIRS = src/libfet.pro \
          src/src.pro \
          src/src-cl.pro \
          tests

# With C++11 support
greaterThan(QT_MAJOR_VERSION, 4){
CONFIG += c++11
} else {
QMAKE_CXXFLAGS += -std=c++11
}

unix {
translations.path = /usr/share/fet/translations
translations.files = translations/*.qm

examples.path = /usr/share/fet
examples.files = examples/

desktop.path = /usr/share/applications
desktop.files = fet.desktop

documentation.path = /usr/share/doc/fet
documentation.files = doc/* AUTHORS COPYING ChangeLog README REFERENCES THANKS TODO TRANSLATORS

manual.path = /usr/share/man/man1
manual.files = man/*

icon_svg.path = /usr/share/icons/hicolor/scalable/apps
icon_svg.files = icons/fet.svg

icon_png.path = /usr/share/pixmaps
icon_png.files = icons/fet.png

INSTALLS += translations examples desktop manual documentation icon_svg icon_png
}
