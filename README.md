Chaotic Rage
============

Chaotic Rage is a silly little shooter game. A simple graphical style allows for a focus on gameplay. The game is very customisable with full modding support. There are a variety of game types, with scripting done in Lua. Multiple view modes - top down, behind player and first-person - allow for a unique perspective of the action. Dynamic and scriptable weather and day/night effects create a dynamic environment. The gameplay is rounded out by a number of interesting vehicles including tanks, helicopters and trains. Last but not least is a wide range of weapons including shotguns, machine guns and flame throwers.

At the moment there are a few game types and other game content which is zombie themed. This was mostly because zombie AI is very easy to code. There is [a discussion on the forum](http://chaoticrage.com/forum/topic/6) about actually defining a proper "theme" for the game, because most people agree that zombies is a bit dated.

For more information and screenshots, see our website:
* http://chaoticrage.com

List any issues on GitHub:
* https://github.com/TheJosh/chaotic-rage/issues

You can also talk about your ideas and your issues at our forum:
* http://chaoticrage.com/forum/topics


Compiling
---------

Chaotic Rage is supported on a number of platforms and through various
build environments. For compilation instructions for various platforms,
see the file COMPILE.md


Making mods
-----------

The game is designed with extensability in mind. All of game data
is abstracted into data modules, and multiple mods can be loaded at once.

The mods contain the definitions for everything in the game,
as well as 3D models, sounds, songs, etc. The game also uses the scripting
language Lua, which provides logic for AIs, as well as managed game types.

You can read the official modding documentation at http://chaoticrage.com/modding_docs


Contributing
------------

All contributions (even documentation) are welcome :) Open a pull request and I would be happy to merge them.

If you've got ideas, code or content, but don't know where to start, email me at help@chaoticrage.com.

You can read the technical documentation at http://chaoticrage.com/dev_docs


Build Status
------------
Builds are tested on Travis CI

[![Build Status](https://travis-ci.org/TheJosh/chaotic-rage.png?branch=master)](https://travis-ci.org/TheJosh/chaotic-rage)


License
-------
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


Some content contained within this archive is covered by other
licenses. For full authorship information, see the Git log available
at <https://github.com/TheJosh/chaotic-rage>.

