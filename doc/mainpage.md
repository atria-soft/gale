GALE library                                {#mainpage}
===============

@tableofcontents

What is GALE?         {#gale_mainpage_what}
=============

GALE, or "Graphic Abstraction Layer for Ewol" is a simple Interface with the OS Gui interface (openGL) and basic interface.
For example, on android we use java interface, on linux we use X11, ....
Gale abstract all of this and provide an inetrface in C++ on all of these platform.
Gale abstact the Keybord, mouse and touch event on all of these platform (when availlable: no touch on linux X11)

Gale is a low level abstraction interface. you can user upper interface:
  - [EWOL](http://atria-soft.github.io/ewol) It provide some basic widgets and gui management

With Gale you have the Graphyc abstraction. We provide some abstraction for the audio interface:
  - [audio-orchastra](https://musicdsp.github.io/audio-orchestra) That is the dual of gale but for audio.
  - [audio-river](https://musicdsp.github.io/audio-river) That is the dual of ewol but for audio.


Where can I use it?         {#gale_mainpage_where}
-------------------

Everywhere! GALE is cross-platform devolopped to support bases OS:
- Linux (X11) (mouse)
- Windows (mouse) (build on linux...)
- MacOs (mouse)
- Android (mouse + touch)
- IOs (touch)

What languages are supported?                   {#gale_mainpage_language}
=============================

GALE is written in C++.


Are there any licensing restrictions?           {#gale_mainpage_restriction}
=====================================

GALE is **FREE software** and _all sub-library are FREE and staticly linkable !!!_


License (MPL v2.0)                              {#gale_mainpage_license}
==================

Copyright gale Edouard DUPIN

Licensed under the Mozilla Public License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

<https://www.mozilla.org/MPL/2.0>

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


Other pages                                      {#gale_mainpage_sub_page}
===========

  - @ref gale_build
  - @ref gale_tutorial
  - [**ewol coding style**](http://atria-soft.github.io/ewol/ewol_coding_style.html)

