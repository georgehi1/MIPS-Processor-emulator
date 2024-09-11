# Functional Test Program

# Load immediate value 0x10 into register $1
addi $1, $0, 0x10
# Load immediate value 0x5 into register $2
addi $2, $0, 0x5

# Test ADD: Add the values in registers $1 and $2, store result in register $3
add $3, $1, $2
# Test SUB: Subtract the value in register $2 from register $1, store result in register $4
sub $4, $1, $2
# Test AND: Bitwise AND between the values in registers $1 and $2, store result in register $5
and $5, $1, $2
# Test OR: Bitwise OR between the values in registers $1 and $2, store result in register $6
or $6, $1, $2
# Test XOR: Bitwise XOR between the values in registers $1 and $2, store result in register $7
xor $7, $1, $2
# Test NOR: Bitwise NOR between the values in registers $1 and $2, store result in register $8
nor $8, $1, $2

# Store the value in register $8 to memory address 0x7f04
sw $8, 0x7f04($0)

# Test BEQ: Branch to label if $1 and $2 are equal
beq $1, $2, equal
# Load immediate value 0x01 into register $9 if BEQ failed
addi $9, $0, 0x01
# Test BNE: Branch to label if $1 and $2 are not equal
bne $1, $2, not_equal
# Load immediate value 0x01 into register $10 if BNE failed
addi $10, $0, 0x01
# Continue to the next test
beq $0, $0, end

# Label for BEQ
equal:
# Load immediate value 0x00 into register $9 if BEQ succeeded
addi $9, $0, 0x00
# Continue to the next test
beq $0, $0, end

# Label for BNE
not_equal:
# Load immediate value 0x00 into register $10 if BNE succeeded
addi $10, $0, 0x00
# Continue to the next test
beq $0, $0, end

# Test SLT: Set $11 to 1 if $1 is less than $2, else set to 0
slt $11, $1, $2
# Continue to the next test
beq $0, $0, end

# Test SLLV: Shift left the value in register $2 by the amount in register $1, store result in register $12
sllv $12, $1, $2
# Test SRLV: Shift right the value in register $2 by the amount in register $1, store result in register $13
srlv $13, $1, $2

# Infinite loop to hang the processor
beq $0, $0, end

end: 