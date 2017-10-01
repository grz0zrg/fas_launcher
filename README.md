# Fragment Audio Server launcher

A [wxWidgets](https://www.wxwidgets.org/) launcher for [Fragment Audio Server](https://github.com/grz0zrg/fas) built for the [Fragment Synthesizer](https://github.com/grz0zrg/fsynth), a [web-based Collaborative Spectral Synthesizer](https://www.fsynth.com)

This program should compile on most platforms.

The [Fragment Audio Server](https://github.com/grz0zrg/fas) executable should be put alongside the launcher application.

## Description

This program is a simple native launcher which provide an easy interface to start the Fragment Audio Server, the launcher also provide a convenient way to configure the audio server for individual sessions and provide a direct way to launch the web. application pre-linked to the native audio server.

## Package

A .deb package is available in the **package** folder along with the [Debreate](https://antumdeluge.github.io/debreate-web/) file project used to build the package for amd64 platform.

## Build

This program was made with the [CodeLite IDE](https://codelite.org/) to ease the process of creating the wxWidgets UI with [wxCrafter](https://wxcrafter.codelite.org/).

Requirements :

 * [PortAudio](http://www.portaudio.com/download.html)
 * [wxWidgets](https://www.wxwidgets.org/)
 * [tiny-process-library](https://github.com/eidheim/tiny-process-library)

This also make use of [SQLite](https://www.sqlite.org/) which is bundled.

This was only compiled under Linux for now.

## Screenshots

[![FAS launcher](https://www.fsynth.com/data/fas_launcher.png)](https://github.com/grz0zrg/fas_launcher)
