#include <stdio.h>

int main(void)
{
    long bank_balance = 500;
    long new_balance = 1000000;
    // create the lens
    int *lens = (int *)bank_balance;
    // use the lens to write the value
    // THIS will not change the bank_balance 
    // but copy the address of the new_balance 
    // to the lens pointer
    lens = (int *)&new_balance;
    // dereference the lens to get the value 
    printf("Final Balance: %d\n", *lens);
    
    // The new_balance is still 500 
    printf("Old Balance: %ld\n", bank_balance);
   

    // use the lens to change the value of the new_balance
    long *new_lens = &bank_balance;
  
    printf("Lens pointer address: %p\n", new_lens);
    printf("bank_balance address: %p\n", &bank_balance);
   
    // they have the same memory address so we 
    // can dereference the new_lens and assign 
    // the value directly using the pointer
    *new_lens = 1500000;

    printf("Old Balance: %ld\n", bank_balance);

    return 0;
}
