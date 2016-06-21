# Peruse
A comic book reader app

This is an application for reading comic books distributed in various digital
formats. It is based on KDE's Frameworks 5, and the UI is built using Qt Quick 2.

File discovery is done using Baloo when available and enabled, and otherwise
through a simple file system digger. The Baloo method is both faster and more
featureful, and you should be using that whenever possible.

## Features

This is a list of features which currently exist in Peruse. It does not include
the planned features, which can be found in [the accompanying PLAN.md file](PLAN.md).

* Multiple file format support, in two tiers
  * Native support with optimised viewing for comic books
    * cbz or cbr
  * Okular based generic view support
    * other cb* formats with archive types not supported by KArchive
    * pdf
    * deja-vu
    * epub
* Touch optimised user interface (nb: mouse/keyboard UI also planned)
  * All the standard stuff like kinetic scrolling, pinch-zoom and so on
  * Plasma Mobile components based experience
* Baloo based file discovery with simple filesystem scraper fallback
