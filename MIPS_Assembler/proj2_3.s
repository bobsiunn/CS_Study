lui $2, 0x1234
ori $2, $2, 0xabcd
lui $3, 0x1000
sw $2, 0($3)
lw $4, 0($3)
lb $5, 0($3)
lb $6, 1($3)
lb $7, 2($3)
lbu $8, 3($3)
addi $9, $3, 4
sb $8, 0($9)
sb $7, 1($9)
sb $6, 2($9)
sb $5, 3($9)
lw $10, 0($9)
sh $0, 0($9)
lw $11, 0($9)
lh $12, -2($9)

