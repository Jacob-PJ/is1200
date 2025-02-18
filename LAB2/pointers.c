#include <stdio.h>

int count = 0;
char* text1 = "This is a string.";
int list1[80];

char* text2 = "Yet another thing.";
int list2[80];

void printlist(const int* lst){
  printf("ASCII codes and corresponding characters.\n");
  while(*lst != 0){
    printf("0x%03X '%c' ", *lst, (char)*lst);
    lst++;
  }
  printf("\n");
}

void endian_proof(const char* c){
  printf("\nEndian experiment: 0x%02x,0x%02x,0x%02x,0x%02x\n", 
         (int)*c,(int)*(c+1), (int)*(c+2), (int)*(c+3));
  
}


void copycodes(const char *a0, int *a1) {
    while(*a0!='\0')
    {
      *(int *)a1=*a0;
      a0++;
      a1= (int *)((char*)a1+4);
      count++;
      
    }

}

//copycodes:
//loop:
//lb	$t0, 0($a0)   //what does 0(dollar a0 ) mean, i forgor = input
//beq	$t0, $0, done
//sw	$t0, 0($a1)
//
//addi	$a0, $a0, 1
//addi	$a1, $a1, 4
//
//lw	$t1, 0($a2)
//addi	$t1, $t1, 1
//sw	$t1, 0($a2)
//j	loop
//done :
//jr	$ra

void work() {
     
    copycodes(text1, list1);

    
    copycodes(text2, list2);

}

//PUSH($ra)
//la 	$a0, text1
//la	$a1, list1
//la	$a2, count
//jal	copycodes
//
//la 	$a0, text2
//la	$a1, list2
//la	$a2, count
//jal	copycodes
//POP($ra)



int main(void) {
    work();

    printf("\nlist1: ");
    printlist(list1);
    printf("\nlist2: ");
    printlist(list2);
    printf("\nCount = %d\n", count);

    endian_proof((char*)&count);
}