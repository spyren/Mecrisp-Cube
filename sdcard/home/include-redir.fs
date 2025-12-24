

create fil /FIL allot
create fn 50 allot
fn dup 20 accept /home/letter-f.fs
str0term

fil fn <f_open
<file

: read-stdin 
  begin
    fs-key dup emit
  04 = until
;

