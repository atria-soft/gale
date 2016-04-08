Tutorial                                         {#gale_tutorial}
========

@tableofcontents

Base:                                            {#gale_declare}
=====

To understand some choise, I designe this software to acces on all system (that provide openGL interface).
But on some embended interface, we have only openGL-es ==> then I limit all the design on OpenGL-ES (need to think to change this maybe)

All is based on the heritage of the single class: gale::Application.

For your fist application you just need to declare an application and create the smallest main that ever exit:

Application declatration:                        {#gale_base_main}
-------------------------

We declare the application:
```{.c}
	class MainApplication : public gale::Application {
		// nothing to do ...
	}
```

We create application on the main()
@snippet basic.cpp gale_declare_main

At this point I need to explain one point on Android:

Android have an internal state engine that can create multiple aplication, then I can call a second time the main() if needed...

Then never forget to not use global value and singleton. We provide other mechanism to do it. (for exemple, you can acces at you Application everywhere you are.




Compleate Sample:                        {#gale_compleate_sample}
=================

@snippet basic.cpp gale_sample_all

