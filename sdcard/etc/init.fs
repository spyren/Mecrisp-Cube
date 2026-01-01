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
\      For details see copyright.txt

CR .( init.fs Loading started) CR 

include /fsr/utils.fs
include /fsr/ansification.fs
include /fsr/dump.fs
include /fsr/disassembler-m3.fs
\ include /fsr/float.fs
include /fsr/words.fs
include /fsr/rtos.fs
[IFDEF] block include /fsr/blocks.fs [THEN]
[IFDEF] f_chdrive include /fsr/fat.fs  [THEN]
include /fsr/redirection.fs
include /fsr/memory.fs
\ include /fsr/struct.fs
include /fsr/control.fs
\ include /fsr/ppp.fs
[IFDEF] button include /fsr/calculator.fs [THEN]

CR 
compiletoram? [IF] .( RAM Dictionary: ) [ELSE] .( FLASH Dictionary: ) [THEN]
unused 1024 / . .( KiB)
CR .( init.fs finished )
