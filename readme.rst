██               ██       ████                  ████     ████         ████
██               ██     ██       ██████       ██           ██       ██
██████       ██████   ████████         ██   ████████       ██     ████████
██    ██   ██    ██     ██           ██       ██           ██       ██
██    ██   ██    ██     ██         ██         ██           ██       ██
██████       ██████     ██       ████████     ██         ██████     ██


Convert BDF bitmap fonts to flf FIGfonts for figlet.


Usage
-----

bdf2flf simply takes BDF data from stdin and outputs flf data to stdout:

$ bdf2flf < myfont.bdf > myfont.flf

By default, the pixels that form the letters are represented by '#', with the
spaces represented  by ' '. This can be changed using other tools:

$ bdf2flf < Tamzen5x9r.bdf | sed 's/#/██/g; s/ /  /g' > Tamzen5x9r.flf

This last example is what was used the make the header above (Tamzen font:
https://github.com/sunaku/tamzen-font).


To install
----------

$ make
$ sudo make install


To uninstall
------------

$ sudo make uninstall


Contact
-------
Email me @ mcol@posteo.net for questions or patches.
