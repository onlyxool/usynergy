# usynergy
============================================

## 0. Introduction

This project is on purpose of implement a compatible synergy client 
on Android.

[Synergy][1] is a mouse and keyboard sharing utility, Synergy lets you
easily share your mouse and keyboard between multiple computers on
your desk, and it’s Free and Open Source.

This project is base on [Micro Synergyi(μSynergy)][2].


## 1. Pre-requirements

usynergy use the uinput module for user space input driver, so you must have
a modded Android with access to /dev/uinput (Like Cyanogen). That means
your android must have /dev/uinput module and this file can be RW by app.

## 2. How to use it

Try to read synerg's [user guide][6]. usynergy is almost same.

## 3. Acknowledgements

* [Alex Evans][3] & [Michiel van der Leeuw][4]  (uSynergy client)
* Tuomas Räsänen [Email][5](suinput)

[1]:http://synergy-foss.org/
[2]:http://synergy-foss.org/wiki/Micro_Synergy
[3]:http://en.wikipedia.org/wiki/Alex_Evans_(video_game_developer)
[4]:http://en.wikipedia.org/wiki/Michiel_van_der_Leeuw
[5]:tuos@codegrove.org
[6]:http://synergy-project.org/wiki/User_Guide
