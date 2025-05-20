; demo.asm
START:
  LDI  r0, 10
  LDI  r1, 20
  ADD  r2, r0, r1
  CMP  r2, r1
  JEQ  equal
  HLT
equal:
  MOV  r3, r2
  HLT