#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int readInstruction() {
    unsigned int counter;
    char buffer[MAX_READLINE];
    FILE* f;
    if(!(f = fopen(INSTRUCTION_PATH, "r"))) Error("Failed to read file\n");
    counter = 0;
    while(fgets(buffer, MAX_READLINE, f)
            && buffer[0] != '\n') {
        char addr_s[24];
        int addr;
        int ins;
        printf("%s", buffer);
        if(buffer[0] == '#') continue;
        sscanf(buffer, "%s%x", addr_s, &ins);
        if(addr_s[1] == 'x')
					addr_s[10] = '\0';
				else
					addr_s[8] = '\0';
        sscanf(addr_s, "%x", &addr);
        if(addr > INS_END_POS) Error("Out of Bound\n");
        memory[addr>>2] = ins;
        //printf("[%s]\tchunk[%d], 0x%x [0x%x]\n", buffer, addr>>2, ins, memory[addr>>2]);
        counter++;
    }
    fclose(f);
    return counter;
}

int loadInstructions() {
	int count;
	const unsigned int instructions[] = {

0x00000fa0,		//   $sp = 4000
0x00000fa0,		//   $fp = 4000
0x00000000,
0x00000000,
0x00000000,
0x00000032,		//	$pc = 50
0x00000000,		//	reserved for answer
0x00000000,		//	reserved for answer
0x00000000,		//	reserved for answer
0x00000000,		//	reserved for answer
0x00001025,     // 	move	v0,zero    <load_arrays>:
0x24060064,     // 	li	a2,100
0x8c830000,     // 	lw	v1,0(a0)
0x00621821,     // 	addu	v1,v1,v0
0xac830000,     // 	sw	v1,0(a0)
0x8ca30000,     // 	lw	v1,0(a1)
0x00621821,     // 	addu	v1,v1,v0
0xaca30000,     // 	sw	v1,0(a1)
0x24420001,     // 	addiu	v0,v0,1
0x24840004,     // 	addiu	a0,a0,4
0x1446fff7,     // 	bne	v0,a2,400838 <load_arrays+0x8>
0x24a50004,     // 	addiu	a1,a1,4
0x8d090f90,		//lw $t1, 3576($t0)
0xad090018,		//sw $t1, 24($t0)
0x8d080418,		//lw $t0, 1048($t0)
0x03e00008,     // 	jr	ra
0xac030018,     // 	sw v1, 24($zer0)
0x00000000,     // 	nop
0x00000000,     // 	nop
0x00000000,     // 	nop
0x00801825,    //  	move	v1,a0		<check_result>:
0x24870190,    // 	addiu	a3,a0,400
0x00001025,    // 	move	v0,zero
0x24080001,    // 	li	t0,1
0x8c640000,    // 	lw	a0,0(v1)
0x8ca60000,    // 	lw	a2,0(a1)
0x00862026,    // 	xor	a0,a0,a2
0x0104100b,    // 	movn	v0,t0,a0
0x24630004,    // 	addiu	v1,v1,4
0x1467fffa,    // 	bne	v1,a3,400878 <check_result+0x10>
0x24a50004,    // 	addiu	a1,a1,4
0xac02001c,		//sw $v0, 28($zero)
0x8c18041c,		//lw $t8, 1052($zero)
0xac030020,		//sw $v1, 32($zero)
0x8c180420,		//lw $t8, 1056($zero)
0x03e00008,    // 	jr	ra
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x00000000,    // 	nop
0x27bdfcb0,    // 	addiu	sp,sp,-848   <main>:
0xafbf034c,    // 	sw	ra,844(sp)
0xafb20348,    // 	sw	s2,840(sp)
0xafb10344,    // 	sw	s1,836(sp)
0xafb00340,    // 	sw	s0,832(sp)
0xafa2033c,    // 	sw	v0,828(sp)
0x27b101ac,    // 	addiu	s1,sp,428
0x02202825,    // 	move	a1,s1
0x27b0001c,    // 	addiu	s0,sp,28
0x02002025,    // 	move	a0,s0
0x0c00000a,    // 	jal	10 <load_arrays>
0x00000000,    // 	nop
0x02202825,    // 	move	a1,s1
0x02002025,    // 	move	a0,s0
0x0c00001e,    // 	jal	30 <check_result>
0x00000000,    // 	nop
0x8fa4033c,    // 	lw	a0,828(sp)
0x8e430000,    // 	lw	v1,0(s2)
0x10830004,    // 	beq	a0,v1,400914 <main+0x78>
0x8fbf034c,    // 	lw	ra,844(sp)
0x00000000,    // 	nop
0x8fb20348,    // 	lw	s2,840(sp)
0x8fb10344,    // 	lw	s1,836(sp)
0x8fb00340,    // 	lw	s0,832(sp)
0x00000008,    // 	jr	$zero
0x00000000,	   //
0x00000000   //
		};
		count = (int)sizeof(instructions)/sizeof(unsigned int);
		if(!memcpy(&memory, &instructions, count*sizeof(unsigned int))) exit(1);
		printf("Loaded[%d]\n", count);
		return count;
}



op_format getInsFormat(int ins) {
    if((ins&OP_MASK) == 2 || (ins&OP_MASK) == 3){
      // 0x02 0x03
      printf("0x%x is J Format\n",ins);
      return FORMAT_J;
    }else if(ins&OP_MASK){
        // non-zero I Type
        printf("0x%x is I Format\n", ins);
        return FORMAT_I;
    }
    printf("0x%x is R Format\n", ins);
    return FORMAT_R;
}

unsigned getPartNum(int ins, part_type ptype) {
	switch(ptype) {
		case PART_OP:
			return (ins&OP_MASK)>>OP_SHF;
		case PART_RS:
			return (ins&RS_MASK)>>RS_SHF;
		case PART_RT:
			return (ins&RT_MASK)>>RT_SHF;
		case PART_RD:
			return (ins&RD_MASK)>>RD_SHF;
		case PART_SHM:
			return (ins&SH_MASK)>>SHM_SHF;
		case PART_FUNC:
			return (ins&FN_MASK);
		case PART_IMM:
			return (ins&IM_MASK);
		default:
			Error("Instruction: Wrong format\n");
			exit(1);
	}
}

void Error(const char* msg) {
    printf("%s", msg);
    exit(1);
}

void Log(const char* format, ...) {
  // log
  /*
  va_list argptr = NULL;
  va_start(argptr, format);
  vfprintf(format, argptr);
  va_end(argptr);
  */
  return;
}

void printRegisters() {
	int i;
	printf("PC[0x%04d]\n", PC);
	for( i = 0; i < REGISTER_NUM; i+=4) {
		printf("$%02d[0x%08x], ", i	, register_file[i]);
		printf("$%02d[0x%08x], ", i+1, register_file[i+1]);
		printf("$%02d[0x%08x], ", i+2, register_file[i+2]);
		printf("$%02d[0x%08x]\n", i+3, register_file[i+3]);
	}

}
