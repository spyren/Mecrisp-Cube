\  @brief
\      Includes essential tools for the Mecrisp-Cube Forth system.
\
\      To make the tools persistent, compile the tools into the flash dictionary, e.g.
\      compiletoflash
\      include /etc/init.fs
\      compiletoram
\  @file
\      init.fs
\  @author
\      Peter Schmid, peter@spyr.ch
\  @date
\      2021-05-16
\  @remark
\      Language: Mecrisp-Stellaris Forth
\  @copyright
\      Peter Schmid, Switzerland
\
\      This project Mecrsip-Cube is free software: you can redistribute it
\      and/or modify it under the terms of the GNU General Public License
\      as published by the Free Software Foundation, either version 3 of
\      the License, or (at your option) any later version.
\
\      Mecrsip-Cube is distributed in the hope that it will be useful, but
\      WITHOUT ANY WARRANTY; without even the implied warranty of
\      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
\      General Public License for more details.
\
\      You should have received a copy of the GNU General Public License
\      along with Mecrsip-Cube. If not, see http://www.gnu.org/licenses/.

CR .( init.fs Loading started) CR 

include /fsr/utils.fs
include /fsr/conditional.fs
include /fsr/ansification.fs
include /fsr/dump.fs
include /fsr/disassembler-m3.fs
\ include /fsr/float.fs
include /fsr/words.fs
include /fsr/rtos.fs
\ include /fsr/blocks.fs
\ include /fsr/fat.fs
include /fsr/redirection.fs
include /fsr/memory.fs
\ include /fsr/struct.fs
include /fsr/control.fs
include /fsr/ppp.fs

CR 
compiletoram? [IF] .( RAM Dictionary: ) [ELSE] .( FLASH Dictionary: ) [THEN]
unused 1024 / . .( KiB)
CR .( init.fs finished )
