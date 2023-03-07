// from http://www.codeproject.com/Articles/4225/Unix-ucontext_t-Operations-on-Windows-Platforms

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

// contexts
ucontext_t auc, // context for thread a
  buc,              // context for thread b
  mainuc;         // context for main

void a()
{
    int i;

    for (i = 0; i < 10; i++)
    {
        printf("a");
        swapcontext(&auc, &buc);        /* switch to thread B */
    }

    printf("\nswitching to main\n");
    swapcontext(&auc, &mainuc);         /* switch to main thread */
}

void b()
{
    int i;

    for (i = 0; i < 10; i++)
    {
        printf("b");
        swapcontext(&buc, &auc);        /* switch to thread A */
    }
}

int main(void)
{
    printf("start\n");                  /* main thread starts */

    /* Set up context for thread A (Unix code, see manpages) */
    getcontext(&auc);
    auc.uc_stack.ss_size = 16 * 1024;

    if ((auc.uc_stack.ss_sp = malloc(auc.uc_stack.ss_size)) == NULL)
        perror("malloc"), exit(1);

    auc.uc_stack.ss_flags = 0;
    makecontext(&auc, a, 0);

    /* Set up context for thread B */
    getcontext(&buc);
    buc.uc_stack.ss_size = 16 * 1024;

    if ((buc.uc_stack.ss_sp = malloc(buc.uc_stack.ss_size)) == NULL)
        perror("malloc"), exit(1);

    buc.uc_stack.ss_flags = 0;
    makecontext(&buc, b, 0);

    /* Switch to A */
    //getcontext(&mainuc);           /* Save the context of main thread */
    swapcontext(&mainuc, &auc);    /* Switch to thread A */

    printf("\ndone\n");  /* Execution control returned to main thread */
    return 0;
}