#include <threading.h>
#include <stdio.h>

void t_init() // TODO
{        
        for (int i = 0; i < NUM_CTX; i++) {
                contexts[i].state = INVALID; 
        }
        current_context_idx = 0;
        contexts[current_context_idx].state = VALID;        
}

int32_t t_create(fptr foo, int32_t arg1, int32_t arg2) // TODO
{
        for (uint8_t i = 0; i < NUM_CTX; i++) {
                if (contexts[i].state == INVALID) {
                        ucontext_t* ptr = &contexts[i].context;
                        contexts[i].state = VALID; 
                        getcontext(ptr);
                        ptr->uc_stack.ss_sp = (char*) malloc(STK_SZ);
                        ptr->uc_stack.ss_size = STK_SZ;
                        ptr->uc_stack.ss_flags = 0;
                        ptr->uc_link = NULL;
                        makecontext(ptr, (void (*)()) foo, 2, arg1, arg2);                        
                        return 0; // success
                }
        }
        return 1; // unsucessful        
}

int32_t t_yield() // TODO
{   
        getcontext(&contexts[current_context_idx].context);
        uint8_t nextValidIdx = 255;
        int32_t found = 0;
        for (uint8_t i = 0; i < NUM_CTX; i++) {
                if (i == current_context_idx) {
                        continue; 
                }
                if (contexts[i].state == VALID) {
                        if (nextValidIdx == 255) {
                                nextValidIdx = i;
                        }
                        found++;
                }                
        }
        if (nextValidIdx == 255) { return -1; }        
        uint8_t old_id = current_context_idx;
        current_context_idx = nextValidIdx;        
        swapcontext(&contexts[old_id].context, &contexts[nextValidIdx].context);                                       
        current_context_idx = old_id;
        return found;
}

void t_finish() // TODO
{        
        free(contexts[current_context_idx].context.uc_stack.ss_sp);
        memset(&contexts[current_context_idx].context, 0, sizeof(ucontext_t));
        contexts[current_context_idx].state = DONE;
        t_yield();
}
