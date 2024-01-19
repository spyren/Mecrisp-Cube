Real Time Clock
===============

Epoch is 1.1.1970 (Unix time stamp). But valid dates are from 1.1.2000
to 31.12.2099 because of the STM32WB RTC peripheral.
```
time@          (  -- u )      Returns the unix time stamp. 
time!          ( u --  )      Sets the real time clock.
.time          ( -- )         Prints the current date and time (YYYY-MM-DDTHH:MM:SS ISO 8601).

date           ( -- )         Prints date unix style e.g. Mon Jan  1 01:57:52 2000
```

How to set the current date and time?
-------------------------------------

Copy the current time stamp from a Unix machine (GNU/Linux box) or from
<https://www.unixtimestamp.com/>:
```
$ date +%s
1602430522
```

Paste the time stamp in the Mecrisp-Cube console:
```
1602430522 time![RET]  ok.
.time[RET] 2020-10-11T15:35:27 ok.
date[RET] 
Sun Oct 11 15:36:02 2020
```

Set Alarms
----------

Wake me at 06:00
<pre>
<b>6 0 0 alarm! wait-alarm .( Get Up!)[RET]</b> Get Up! ok.
</pre>

If you want an alarm every second:
<pre>
<b>-1 -1 -1 alarm![RET]</b>  ok.
</pre>

for every minute:
<pre>
<b>-1 -1 0 alarm![RET]</b>  ok.
</pre>

Stop the alarms:
<pre>
<b>0 0 0 alarm![RET]</b>  ok.
</pre>


```forth
: count-down ( -- )
  -1 -1 -1 alarm!  \ an alarm every second
  10 0 do
    cr 10 i - .
    wait-alarm  
  loop
  cr ." 0 Launch!"
;
```

<pre>
<b>count-down[RET]</b>
10 
9 
8 
7 
6 
5 
4 
3 
2 
1 
0 Launch! ok. 
</pre>
