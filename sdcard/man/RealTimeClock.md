Real Time Clock
===============

Epoch is 1.1.1970 (Unix time stamp). But valid dates are from 1.1.2000
to 31.12.2099 because of the STM32WB RTC peripheral.

    time!          (  -- u )      Returns the unix time stamp. 
    time@          ( u --  )      Sets the real time clock.
    .time          ( -- )         Prints the current date and time (YYYY-MM-DDTHH:MM:SS ISO 8601).

    date

How to set the current date and time?
-------------------------------------

Copy the current time stamp from a Unix machine (GNU/Linux box) or from
<https://www.unixtimestamp.com/>:

    $ date +%s
    1602430522

Paste the time stamp in the Mecrisp-Cube console:

     ok.
    1602430522 time![RET]  ok.
    .time[RET] 2020-10-11T15:35:27 ok.
    date[RET] 
    Sun Oct 11 15:36:02 2020

\-- [PeterSchmid - 2020-10-11]
