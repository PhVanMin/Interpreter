#include <stdio.h>
#include "common.h"
#include "vm.h"
#include "debug.h"
#include "compiler.h"

VM vm;

static void resetStack()
{
    vm.stackCount = 0;
}

void initVM()
{
    vm.stackCapacity = 0;
    vm.stack = NULL;
    resetStack();
}

void freeVM()
{
    FREE_ARRAY(Value, vm.stack, vm.stackCapacity);
    initVM();
}

static InterpretResult run() {
    #define READ_BYTE() (*vm.ip++)
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    #define BINARY_OP(op) \
    do { \
      double b = pop(); \
      double a = pop(); \
      push(a op b); \
    } while (false)
    for (;;) {
        #ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = vm.stack; slot < vm.stack + vm.stackCount; slot++)
        {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
        #endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT:
            {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_RETURN:
            {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
            case OP_ADD:      BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE:   BINARY_OP(/); break;
            case OP_NEGATE:
            {
                vm.stack[vm.stackCount - 1] *= -1;
                break;
            }
            default:
                return INTERPRET_RUNTIME_ERROR;
        }
    }

    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}

void push(Value value)
{
    if (vm.stackCapacity < vm.stackCount + 1)
    {
        int oldCapacity = vm.stackCapacity;
        vm.stackCapacity = GROW_CAPACITY(oldCapacity);
        vm.stack = GROW_ARRAY(Value, vm.stack,
                              oldCapacity, vm.stackCapacity);
    }

    vm.stack[vm.stackCount] = value;
    vm.stackCount++;
}

Value pop()
{
    vm.stackCount--;
    return vm.stack[vm.stackCount];
}

//InterpretResult interpret(Chunk* chunk)
//{
//    vm.chunk = chunk;
//    vm.ip = vm.chunk->code;
//    return run();
//}

InterpretResult interpret(const char* source)
{
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk))
    {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}