gale
====

`gale` is an abtracter of the low level graphic windowing (like the SDL but in C++11)

It is designed to abstract systems
  - Linux
  - MacOs
  - Windows
  - Android
  - Ios

Release (master)
----------------

[![Build Status](https://travis-ci.org/atria-soft/gale.svg?branch=master)](https://travis-ci.org/atria-soft/gale)
[![Coverage Status](http://atria-soft.com/ci/coverage/atria-soft/gale.svg?branch=master)](http://atria-soft.com/ci/atria-soft/gale)
[![Test Status](http://atria-soft.com/ci/test/atria-soft/gale.svg?branch=master)](http://atria-soft.com/ci/atria-soft/gale)
[![Warning Status](http://atria-soft.com/ci/warning/atria-soft/gale.svg?branch=master)](http://atria-soft.com/ci/atria-soft/gale)

[![Build Status](http://atria-soft.com/ci/build/atria-soft/gale.svg?branch=master&tag=Linux)](http://atria-soft.com/ci/atria-soft/gale)
[![Build Status](http://atria-soft.com/ci/build/atria-soft/gale.svg?branch=master&tag=MacOs)](http://atria-soft.com/ci/atria-soft/gale)
[![Build Status](http://atria-soft.com/ci/build/atria-soft/gale.svg?branch=master&tag=Mingw)](http://atria-soft.com/ci/atria-soft/gale)

[![Build Status](http://atria-soft.com/ci/build/atria-soft/gale.svg?branch=master&tag=Android)](http://atria-soft.com/ci/atria-soft/gale)
[![Build Status](http://atria-soft.com/ci/build/atria-soft/gale.svg?branch=master&tag=IOs)](http://atria-soft.com/ci/atria-soft/gale)

Developement (dev)
------------------

[![Build Status](https://travis-ci.org/atria-soft/gale.svg?branch=dev)](https://travis-ci.org/atria-soft/gale)
[![Coverage Status](http://atria-soft.com/ci/coverage/atria-soft/gale.svg?branch=dev)](http://atria-soft.com/ci/atria-soft/gale)
[![Test Status](http://atria-soft.com/ci/test/atria-soft/gale.svg?branch=dev)](http://atria-soft.com/ci/atria-soft/gale)
[![Warning Status](http://atria-soft.com/ci/warning/atria-soft/gale.svg?branch=dev)](http://atria-soft.com/ci/atria-soft/gale)

[![Build Status](http://atria-soft.com/ci/build/atria-soft/gale.svg?branch=dev&tag=Linux)](http://atria-soft.com/ci/atria-soft/gale)
[![Build Status](http://atria-soft.com/ci/build/atria-soft/gale.svg?branch=dev&tag=MacOs)](http://atria-soft.com/ci/atria-soft/gale)
[![Build Status](http://atria-soft.com/ci/build/atria-soft/gale.svg?branch=dev&tag=Mingw)](http://atria-soft.com/ci/atria-soft/gale)

[![Build Status](http://atria-soft.com/ci/build/atria-soft/gale.svg?branch=dev&tag=Android)](http://atria-soft.com/ci/atria-soft/gale)
[![Build Status](http://atria-soft.com/ci/build/atria-soft/gale.svg?branch=dev&tag=IOs)](http://atria-soft.com/ci/atria-soft/gale)



Instructions
============

download Build system:
----------------------

	sudo pip install lutin
	sudo pip install pillow

need google repo:
-----------------

see: http://source.android.com/source/downloading.html#installing-repo

	mkdir ~/.bin
	PATH=~/.bin:$PATH
	curl https://storage.googleapis.com/git-repo-downloads/repo > ~/.bin/repo
	chmod a+x ~/.bin/repo


download the software:
----------------------

	mkdir WORKING_DIRECTORY
	cd WORKING_DIRECTORY
	repo init -u git://github.com/atria-soft/manifest.git
	repo sync -j8

Compile software:
-----------------

	cd WORKING_DIRECTORY
	lutin gale-sample-basic


License (MPL v2.0)
=====================
Copyright gale Edouard DUPIN

Licensed under the Mozilla Public License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.mozilla.org/MPL/2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


# gale
(MPL v2.0) Graphic Abstraction Layer for Ewol: abstract the OpenGL/... interface and mouse/touch/keyboard for multiple system (Linux, Android, MacOs, Windows, Ios)
