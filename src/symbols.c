#include "include/symbols.h"
#include "include/hashmap.h"
#include "include/list.h"

const unsigned long max_long = 0 - 1;

hashmap_T *symbol_table = NULL;
list_T *global_memory_assignment = NULL; 
unsigned long global_memory_assignment_start = 0;

void init_symbols() {
    symbol_table = new_hashmap(65535);
    global_memory_assignment = init_list(8);
}

int memory_offset(list_T *memory_assignment, unsigned long *memory_assignment_start, unsigned char amount) {
    unsigned int memory_start = *memory_assignment_start;
    unsigned int res = sizeof(long) * memory_start;
    unsigned char mask = (1 << amount) - 1;

    char finished = 0;
    for (int i = memory_start; finished == 0; i++) {
        if (memory_assignment->top <= i) {
            unsigned long *n = calloc(1, sizeof(unsigned long));
            list_push(memory_assignment, n);
        }
        
        unsigned long *current_memory_block = (unsigned long *)(memory_assignment->array[i]);
        if (*current_memory_block == max_long) {
            memory_start += 1;
            continue;
        }

        for (int j = 0; j < sizeof(unsigned long) * 8 - amount; j += amount) {
            if (((*current_memory_block >> j) & mask) == 0) {
                *current_memory_block += mask << j;
                finished = 1;
                break;
            }

            res += amount;
        }
    }

    *memory_assignment_start = memory_start;
    return res;
}

symbol_T *get_symbol(char *name, char *context) {

    if (context) {
        symbol_T *sym = hashmap_get(((symbol_T *)hashmap_get(symbol_table, context))->function->variables, name);
        if (sym) return sym;
    }

    return (symbol_T *)hashmap_get(symbol_table, name);
}

void set_symbol(char *name, char *context, symbol_T *sym) {
    if (context) {
        function_T *context_function = ((symbol_T *)hashmap_get(symbol_table, context))->function;
        if (hashmap_get(context_function->variables, name)) {
            printf("Symbol %s is already defined within the context of function %s\n", name, context);
            exit(1);
        }
        hashmap_set(context_function->variables, name, sym);
    } else {
        if (hashmap_get(symbol_table, name)) {
            printf("Symbol %s is already defined within the global context\n", name);
            exit(1);
        }

        hashmap_set(symbol_table, name, sym);
    }
}

symbol_T *init_function(char *name, unsigned char return_type, unsigned int line, unsigned int param_count, token_T **params) {
    function_T *fun = malloc(sizeof(struct FUNCTION_STRUCT));
    fun->return_type = return_type;
    fun->param_count = param_count;
    fun->variables = new_hashmap(128);
    fun->memory_assignment_start = 0;
    fun->memory_assignment = init_list(4);
    fun->memory_assignment_end = 0;
    fun->param_datatypes = malloc(sizeof(unsigned int) * param_count);

    symbol_T *sym = malloc(sizeof(struct SYMBOL_STRUCT));
    sym->type = function;
    sym->function = fun;

    set_symbol(name, NULL, sym);

    for (int i = 0; i < param_count; i++) {
        unsigned char data_type_id = params[2 * i]->data_type_id;
        fun->param_datatypes[i] = data_type_id;
        variable_T *var = init_variable(params[2 * i + 1]->value, data_type_id, line, name)->variable;
    }

    return sym;
}

symbol_T *init_variable(char *name, unsigned char type, unsigned int line, char *context) {
    variable_T *var = malloc(sizeof(struct VARIABLE_STRUCT));
    var->line = line;
    var->type = type;
    
    if (context) {
        var->global = 0;
        function_T *context_function = get_symbol(context, NULL)->function;
        var->context = context_function;
        var->offset = memory_offset(context_function->memory_assignment,
                                    &context_function->memory_assignment_start,
                                    data_type_sizes[type]);
        int end_res = var->offset + data_type_sizes[type] - context_function->memory_assignment_end;
        if (end_res > 0)
            context_function->memory_assignment_end += ((end_res - 1) / 4 + 1) * 4;
    } else {
        var->global = 1;
        var->context = NULL;
        var->address = memory_offset(global_memory_assignment,
                                     &global_memory_assignment_start,
                                     data_type_sizes[type]);
    }

    symbol_T *sym = malloc(sizeof(struct SYMBOL_STRUCT));
    sym->type = variable;
    sym->variable = var;

    set_symbol(name, context, sym);

    return sym;
}

symbol_T *init_constant(char *name, void *expression, unsigned char type, unsigned int line, char *context) {
    constant_T *cons = malloc(sizeof(struct CONSTANT_STRUCT));
    cons->line = line;
    cons->type = type;
    cons->expression = expression;

    symbol_T *sym = malloc(sizeof(struct SYMBOL_STRUCT));
    sym->type = constant;
    sym->constant = cons;

    set_symbol(name, context, sym);

    return sym;
}
