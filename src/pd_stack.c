//
// pd_stack.c
//
// Copyright (c) 2013 Karl-Johan Alm (http://github.com/kallewoof)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "pd_stack.h"
#include "pd_internal.h"
#include "PDEnv.h"
#include "PDState.h"
#include "pd_pdf_implementation.h"

static PDInteger pd_stack_preserve_users = 0;
PDDeallocator pd_stack_dealloc = free;
void pd_stack_preserve(void *ptr)
{}

void pd_stack_set_global_preserve_flag(PDBool preserve)
{
    pd_stack_preserve_users += preserve ? 1 : -1;
    PDAssert(pd_stack_preserve_users >= 0);
    if (pd_stack_preserve_users - preserve < 1)
        pd_stack_dealloc = preserve ? &pd_stack_preserve : &free;
}

void pd_stack_push_identifier(pd_stack *stack, PDID identifier)
{
    pd_stack s = malloc(sizeof(struct pd_stack));
    s->prev = *stack;
    s->info = identifier;
    s->type = pd_stack_ID;
    *stack = s;
}

void pd_stack_push_key(pd_stack *stack, char *key)
{
    pd_stack s = malloc(sizeof(struct pd_stack));
    s->prev = *stack;
    s->info = key;//strdup(key); free(key);
    s->type = pd_stack_STRING;
    *stack = s;
}

void pd_stack_push_freeable(pd_stack *stack, void *freeable)
{
    pd_stack s = malloc(sizeof(struct pd_stack));
    s->prev = *stack;
    s->info = freeable;
    s->type = pd_stack_FREEABL;
    *stack = s;
}

void pd_stack_push_stack(pd_stack *stack, pd_stack pstack)
{
    pd_stack s = malloc(sizeof(struct pd_stack));
    s->prev = *stack;
    s->info = pstack;
    s->type = pd_stack_STACK;
    *stack = s;
}

void pd_stack_unshift_stack(pd_stack *stack, pd_stack sstack) 
{
    pd_stack vtail;
    if (*stack == NULL) {
        pd_stack_push_stack(stack, sstack);
        return;
    }
    
    for (vtail = *stack; vtail->prev; vtail = vtail->prev) ;

    pd_stack s = malloc(sizeof(struct pd_stack));
    s->prev = NULL;
    s->info = sstack;
    s->type = pd_stack_STACK;
    vtail->prev = s;
}

void pd_stack_push_object(pd_stack *stack, void *ob)
{
    PDTYPE_ASSERT(ob);
    pd_stack s = malloc(sizeof(struct pd_stack));
    s->prev = *stack;
    s->info = ob; //PDRetain(ob);
    s->type = pd_stack_PDOB;
    *stack = s;
}

PDID pd_stack_pop_identifier(pd_stack *stack)
{
    if (*stack == NULL) return NULL;
    pd_stack popped = *stack;
    PDAssert(popped->type == pd_stack_ID);
    *stack = popped->prev;
    PDID identifier = popped->info;
    (*pd_stack_dealloc)(popped);
    return identifier;
}

void pd_stack_assert_expected_key(pd_stack *stack, const char *key)
{
    PDAssert(*stack != NULL);
    
    pd_stack popped = *stack;
    PDAssert(popped->type == pd_stack_STRING || popped->type == pd_stack_ID);

    char *got = popped->info;
    if (popped->type == pd_stack_STRING) {
        PDAssert(got == key || !strcmp(got, key));
        (*pd_stack_dealloc)(got);
    } else {
        PDAssert(!strcmp(*(char**)got, key));
    }
    
    *stack = popped->prev;
    (*pd_stack_dealloc)(popped);
}

void pd_stack_assert_expected_int(pd_stack *stack, PDInteger i)
{
    PDAssert(*stack != NULL);
    
    pd_stack popped = *stack;
    PDAssert(popped->type == pd_stack_STRING);
    
    char *got = popped->info;
    PDAssert(i == atoi(got));
    
    *stack = popped->prev;
    (*pd_stack_dealloc)(got);
    (*pd_stack_dealloc)(popped);
}

PDSize pd_stack_pop_size(pd_stack *stack)
{
    if (*stack == NULL) return 0;
    pd_stack popped = *stack;
    PDAssert(popped->type == pd_stack_STRING);
    *stack = popped->prev;
    char *key = popped->info;
    PDSize st = atol(key);
    (*pd_stack_dealloc)(key);
    (*pd_stack_dealloc)(popped);
    return st;
}

PDInteger pd_stack_pop_int(pd_stack *stack)
{
    if (*stack == NULL) return 0;
    pd_stack popped = *stack;
    PDAssert(popped->type == pd_stack_STRING);
    *stack = popped->prev;
    char *key = popped->info;
    PDInteger st = atol(key);
    (*pd_stack_dealloc)(key);
    (*pd_stack_dealloc)(popped);
    return st;
}

PDInteger pd_stack_peek_int(pd_stack popped)
{
    if (popped == NULL) return 0;
    PDAssert(popped->type == pd_stack_STRING);
    return PDIntegerFromString(popped->info);
}

char *pd_stack_pop_key(pd_stack *stack)
{
    if (*stack == NULL) return NULL;
    pd_stack popped = *stack;
    PDAssert(popped->type == pd_stack_STRING);
    *stack = popped->prev;
    char *key = popped->info;
    (*pd_stack_dealloc)(popped);
    return key;
}

pd_stack pd_stack_pop_stack(pd_stack *stack)
{
    if (*stack == NULL) return NULL;
    pd_stack popped = *stack;
    PDAssert(popped->type == pd_stack_STACK);
    *stack = popped->prev;
    pd_stack pstack = popped->info;
    (*pd_stack_dealloc)(popped);
    return pstack;
}

void *pd_stack_pop_object(pd_stack *stack)
{
    if (*stack == NULL) return NULL;
    pd_stack popped = *stack;
    PDAssert(popped->type == pd_stack_PDOB);
    *stack = popped->prev;
    void *ob = popped->info;
    (*pd_stack_dealloc)(popped);
    return ob; //PDAutorelease(ob);
}

void *pd_stack_pop_freeable(pd_stack *stack)
{
    if (*stack == NULL) return NULL;
    pd_stack popped = *stack;
    PDAssert(popped->type == pd_stack_FREEABL);
    *stack = popped->prev;
    void *key = popped->info;
    (*pd_stack_dealloc)(popped);
    return key;
}

void pd_stack_pop_into(pd_stack *dest, pd_stack *source)
{
    if (*source == NULL) {
        PDAssert(*source != NULL);  // must never pop into from a null stack
        return;
    }
    
    pd_stack popped = *source;
    *source = popped->prev;
    popped->prev = *dest;
    *dest = popped;
}

static inline void pd_stack_free_info(pd_stack stack)
{
    switch (stack->type) {
        case pd_stack_STRING:
        case pd_stack_FREEABL:
            free(stack->info);
            break;
        case pd_stack_STACK:
            pd_stack_destroy(stack->info);
            break;
        case pd_stack_PDOB:
            PDRelease(stack->info);
            break;
    }
}

void pd_stack_replace_info_object(pd_stack stack, char type, void *info)
{
    pd_stack_free_info(stack);
    stack->type = type;
    stack->info = info;
}

void pd_stack_destroy(pd_stack stack)
{
    pd_stack p;
    while (stack) {
        //printf("-stack %p\n", stack);
        p = stack->prev;
        pd_stack_free_info(stack);
        free(stack);
        stack = p;
    }
}

pd_stack pd_stack_get_dict_key(pd_stack dictStack, const char *key, PDBool remove)
{
    // dicts are set up (reversedly) as
    // "dict"
    // "entries"
    // [stack]
    if (dictStack == NULL || ! PDIdentifies(dictStack->info, PD_DICT)) 
        return NULL;
    
    pd_stack prev = dictStack->prev->prev;
    pd_stack stack = dictStack->prev->prev->info;
    pd_stack entry;
    while (stack) {
        // entries are stacks, with
        // e
        // ID
        // entry
        entry = stack->info;
        if (! strcmp((char*)entry->prev->info, key)) {
            if (remove) {
                if (prev == dictStack->prev->prev) {
                    // first entry; container stack must reref
                    prev->info = stack->prev;
                } else {
                    // not first entry; simply reref prev
                    prev->prev = stack->prev;
                }
                // disconnect stack from its siblings and from prev (or prev is destroyed), then destroy stack and we can return prev
                stack->info = NULL;
                stack->prev = NULL;
                pd_stack_destroy(stack);
                return entry;
            }
            return entry;
        }
        prev = stack;
        stack = stack->prev;
    }
    return NULL;
}

PDBool pd_stack_get_next_dict_key(pd_stack *iterStack, char **key, char **value)
{
    // dicts are set up (reversedly) as
    // "dict"
    // "entries"
    // [stack]
    pd_stack stack = *iterStack;
    pd_stack entry;
    
    // two instances where we hit falsity; stack is indeed a dictionary, but it's empty: we will initially have a stack but it will be void after below if case (hence, stack truthy is included), otherwise it is the last element, in which case it's NULL on entry
    if (stack && PDIdentifies(stack->info, PD_DICT)) {
        *iterStack = stack = stack->prev->prev->info;
    }
    if (! stack) return false;
    
    // entries are stacks, with
    // de
    // ID
    // entry
    entry = stack->info;
    *key = (char*)entry->prev->info;
    entry = (pd_stack)entry->prev->prev;
    
    // entry is now iterated past e, ID and is now at
    // entry
    // so we see if type is primitive or not
    if (entry->type == pd_stack_STACK) {
        // it's not primitive, so we set the preserve flag and stringify
        pd_stack_set_global_preserve_flag(true);
        entry = (pd_stack)entry->info;
        *value = PDStringFromComplex(&entry);
        pd_stack_set_global_preserve_flag(false);
    } else {
        // it is primitive (we presume)
        PDAssert(entry->type == pd_stack_STRING);
        *value = strdup((char*)entry->info);
    }
    
    *iterStack = stack->prev;
    
    return true;
}

pd_stack pd_stack_create_from_definition(const void **defs)
{
    PDInteger i;
    pd_stack stack = NULL;
    
    for (i = 0; defs[i]; i++) {
        pd_stack_push_identifier(&stack, (const char **)defs[i]);
    }
    
    return stack;
}

//
// debugging
//


static char *sind = NULL;
static PDInteger cind = 0;
void pd_stack_print_(pd_stack stack, PDInteger indent)
{
    PDInteger res = cind;
    sind[cind] = ' ';
    sind[indent] = 0;
    printf("%sstack<%p> {\n", sind, stack);
    sind[indent] = ' ';
    cind = indent + 2;
    sind[cind] = 0;
    while (stack) {
        switch (stack->type) {
            case pd_stack_ID:
                printf("%s %p (\"%s\")\n", sind, stack->info, *(char **)stack->info);
                break;
            case pd_stack_STRING:
                printf("%s %s\n", sind, (char*)stack->info);
                break;
            case pd_stack_FREEABL:
                printf("%s %p\n", sind, stack->info);
                break;
            case pd_stack_STACK:
                pd_stack_print_(stack->info, cind + 2);
                break;
            case pd_stack_PDOB:
                printf("%s object (%p)", sind, stack->info);
                break;
            default:
                printf("%s ?????? %p", sind, stack->info);
                break;
        }
        stack = stack->prev;
    }
    sind[cind] = ' ';
    cind -= 2;
    sind[indent] = 0;
    printf("%s}\n", sind);
    cind = res;
    sind[indent] = ' ';
    sind[cind] = 0;
}

void pd_stack_print(pd_stack stack)
{
    if (sind == NULL) sind = strdup("                                                                                                                                                                                                                                                       ");
    pd_stack_print_(stack, 0);
}

// 
// the "pretty" version (above is debuggy)
//

void pd_stack_show_(pd_stack stack)
{
    PDBool stackLumping = false;
    while (stack) {
        stackLumping &= (stack->type == pd_stack_STACK);
        if (stackLumping) putchar('\t');
        
        switch (stack->type) {
            case pd_stack_ID:
                printf("@%s", *(char **)stack->info);
                break;
            case pd_stack_STRING:
                printf("\"%s\"", (char*)stack->info);
                break;
            case pd_stack_FREEABL:
                printf("%p", stack->info);
                break;
            case pd_stack_STACK:
                if (! stackLumping && (stackLumping |= stack->prev && stack->prev->type == pd_stack_STACK)) 
                    putchar('\n');
                //stackLumping |= stack->prev && stack->prev->type == pd_stack_STACK;
                if (stackLumping) {
                    printf("\t{ ");
                    pd_stack_show_(stack->info);
                    printf(" }");
                } else {
                    printf("{ ");
                    pd_stack_show_(stack->info);
                    printf(" }");
                }
                break;
            case pd_stack_PDOB:
                printf("<%p>", stack->info);
                break;
            default:
                printf("??? %d %p ???", stack->type, stack->info);
                break;
        }
        stack = stack->prev;
        if (stack) printf(stackLumping ? ",\n" : ", ");
    }
}

void pd_stack_show(pd_stack stack)
{
    printf("{ ");
    pd_stack_show_(stack);
    printf(" }\n");
}


