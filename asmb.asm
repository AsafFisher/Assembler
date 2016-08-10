.extern A
  prn     KAASDA[1-2]
MAIN: mov K[2-4],LENGTH
add r2,STR
LOOP: jmp END
prn #-5
sub r1, r4
inc KS
mov LOOP[1-13],r3
cmp #5, r0
bne LOOP
END: stop
NAME: .string "Asaf Fisher"
STR: .string "a bc def"
LENGTH: .data  9,-15,6
 KS: .data 22
   KAASDA: .data 12
