#include "value.h"
#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include "talloc.h"
#include "tokenizer.h"
#include <assert.h>
#include "parser.h"
#include <string.h>
#ifndef _INTERPRETER
#define _INTERPRETER

void evalationError()
{
    printf("Evaluation error\n");
    texit(0);
}
Value *eval(Value *tree, Frame *frame);
// void printList(Value *tree);

Value *evalIf(Value *args, Frame *frame)
{
    if (length(args) < 3 || length(args) > 3)
    {
        evalationError();
    }
    int boolean = eval(args->c.car, frame)->i;
    // args->c.car;
    if (boolean == 1)
    {
        return eval(args->c.cdr->c.car, frame);
        // args->c.cdr->c.car;
    }
    else if (boolean == 0)
    {
        return eval(args->c.cdr->c.cdr->c.car, frame);
        // args->c.cdr->c.cdr->c.car;
    }
    else
    {
        return NULL;
    }
    // Value *boolean = talloc(sizeof(Value));
}

Value *primitiveSubtract(Value *args, Frame *frame)
{
    if (length(args) != 2)
    {
        evalationError();
    }
    double value;
    bool isDouble = false;
    Value *difference = makeNull();
    if (car(args)->type == INT_TYPE)
    {
        value = car(args)->i;
    }
    else if (car(args)->type == DOUBLE_TYPE)
    {
        isDouble = true;
        value = car(args)->d;
    }
    else if (car(args)->type == SYMBOL_TYPE)
    {
        if (eval(car(args), frame)->type == INT_TYPE)
        {
            value = eval(car(args), frame)->i;
        }
        else if (eval(car(args), frame)->type == DOUBLE_TYPE)
        {
            value = eval(car(args), frame)->d;
        }
    }
    if (car(cdr(args))->type == INT_TYPE)
    {
        value = value - car(cdr(args))->i;
    }
    else if (car(cdr(args))->type == DOUBLE_TYPE)
    {
        isDouble = true;
        value = value - car(cdr(args))->d;
    }
    else if (car(cdr(args))->type == SYMBOL_TYPE)
    {
        if (eval(car(cdr(args)), frame)->type == INT_TYPE)
        {
            value = value - eval(car(cdr(args)), frame)->i;
        }
        else if (eval(car(cdr(args)), frame)->type == DOUBLE_TYPE)
        {
            value = value - eval(car(cdr(args)), frame)->d;
        }
    }
    if (isDouble == false)
    {
        difference->type = INT_TYPE;
        difference->i = (int)value;
        return difference;
    }
    else
    {
        difference->type = DOUBLE_TYPE;
        difference->d = value;
        return difference;
    }
}
Value *lessThan(Value *args, Frame *frame)
{
    Value *result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    double carArgs;
    double cdrArgs;
    if (eval(car(args), frame)->type == DOUBLE_TYPE)
    {
        carArgs = eval(car(args), frame)->d;
    }
    else if (eval(car(args), frame)->type == INT_TYPE)
    {
        carArgs = eval(car(args), frame)->i;
    }
    if (eval(car(cdr(args)), frame)->type == DOUBLE_TYPE)
    {
        cdrArgs = eval(car(cdr(args)), frame)->d;
    }
    else if (eval(car(cdr(args)), frame)->type == INT_TYPE)
    {
        cdrArgs = eval(car(cdr(args)), frame)->i;
    }
    // printf("%f\n", carArgs);
    // printf("%f\n", cdrArgs);
    if (carArgs < cdrArgs)
    {
        result->i = 1;
    }
    else
    {
        result->i = 0;
    }
    return result;
}

Value *greaterThan(Value *args, Frame *frame)
{
    Value *result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    double carArgs;
    double cdrArgs;
    if (eval(car(args), frame)->type == DOUBLE_TYPE)
    {
        carArgs = eval(car(args), frame)->d;
    }
    else if (eval(car(args), frame)->type == INT_TYPE)
    {
        carArgs = eval(car(args), frame)->i;
    }
    if (eval(car(cdr(args)), frame)->type == DOUBLE_TYPE)
    {
        cdrArgs = eval(car(cdr(args)), frame)->d;
    }
    else if (eval(car(cdr(args)), frame)->type == INT_TYPE)
    {
        cdrArgs = eval(car(cdr(args)), frame)->i;
    }
    if (carArgs > cdrArgs)
    {
        result->i = 1;
    }
    else
    {
        result->i = 0;
    }
    return result;
}

Value *Equal(Value *args, Frame *frame)
{
    Value *result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    double carArgs;
    double cdrArgs;
    if (eval(car(args), frame)->type == DOUBLE_TYPE)
    {
        carArgs = eval(car(args), frame)->d;
    }
    else if (eval(car(args), frame)->type == INT_TYPE)
    {
        carArgs = eval(car(args), frame)->i;
    }
    if (eval(car(cdr(args)), frame)->type == DOUBLE_TYPE)
    {
        cdrArgs = eval(car(cdr(args)), frame)->d;
    }
    else if (eval(car(cdr(args)), frame)->type == INT_TYPE)
    {
        cdrArgs = eval(car(cdr(args)), frame)->i;
    }
    if (carArgs == cdrArgs)
    {
        result->i = 1;
    }
    else
    {
        result->i = 0;
    }
    return result;
}
Value *evalLet(Value *args, Frame *frame)
{
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = frame;
    Value *bindings = makeNull();
    Value *cur = car(args);
    while (cur->type != NULL_TYPE)
    {
        Value *newBinding = eval(car(cdr(car(cur))), frame);
        if ((car(cdr(car(cur)))->type == SYMBOL_TYPE) && (newBinding->type == SYMBOL_TYPE))
        {
            Value *currentBinding = bindings;
            while (!isNull(currentBinding))
            {
                if (!strcmp(car(cdr(car(cur)))->s, bindings->c.car->c.car->s))
                {
                    evalationError();
                }
                currentBinding = currentBinding->c.cdr;
            }
        }
        newBinding = cons(newBinding, makeNull());
        newBinding = cons(car(car(cur)), newBinding);
        bindings = cons(newBinding, bindings);
        cur = cdr(cur);
    }
    newFrame->bindings = bindings;
    cur = cdr(args);
    while (cdr(cur)->type != NULL_TYPE)
    {
        eval(car(cur), newFrame);
        cur = cdr(cur);
    }
    return eval(car(cur), newFrame);
}

Value *evalLetStar(Value *args, Frame *frame)
{
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = frame;
    Value *bindings = makeNull();
    Value *newBinding = eval(car(cdr(car(car(args)))), frame);
    newBinding = cons(newBinding, makeNull());
    newBinding = cons(car(car(car(args))), newBinding);
    bindings = cons(newBinding, bindings);
    newFrame->bindings = bindings;
    Frame *follow = newFrame;
    Value *cur = cdr(car(args));
    while (cur->type != NULL_TYPE)
    {
        newBinding = eval(car(cdr(car(cur))), follow);
        newBinding = cons(newBinding, makeNull());
        newBinding = cons(car(car(cur)), newBinding);
        bindings = cons(newBinding, bindings);
        newFrame->parent = follow;
        newFrame->bindings = bindings;
        follow = newFrame;
        cur = cdr(cur);
    }
    cur = cdr(args);
    while (cdr(cur)->type != NULL_TYPE)
    {
        cur = cdr(cur);
    }
    return eval(car(cur), newFrame);
}

Value *letRec(Value *args, Frame *frame)
{
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = frame;
    newFrame->bindings = makeNull();
    newFrame->bindings->type = UNSPECIFIED_TYPE;

    Value *bindings = car(args);
    Value *letBody = cdr(args);
    Value *varList = makeNull();
    Value *valList = makeNull();
    while (bindings->type != NULL_TYPE)
    {
        Value *var = car(car(bindings));
        if (var->type != SYMBOL_TYPE)
        {
            evalationError();
        }

        Value *variables = cons(var, varList);
        varList = variables;
        Value *expression = eval(car(cdr(car(car(args)))), newFrame);
        Value *val = cons(expression, valList);
        valList = val;
        bindings = cdr(bindings);
    }
    while (varList->type != NULL_TYPE)
    {
        Value *newBinding = cons(car(varList), car(valList));
        newFrame->bindings = cons(newBinding, newFrame->bindings);
        varList = cdr(varList);
        valList = cdr(valList);
    }
    letBody = car(letBody);
    return eval(car(cdr(letBody)), newFrame);
}

Value *set(Value *args, Frame *frame)
{
    bool isBound = false;
    Value *voidValue = talloc(sizeof(Value));
    voidValue->type = VOID_TYPE;
    Value *bindings = frame->bindings;
    while (bindings->type != NULL_TYPE)
    {
        Value *binding = car(bindings);
        if (!strcmp(car(binding)->s, car(args)->s))
        {
            isBound = true;
            Value *newVal = eval(car(cdr(args)), frame);
            binding->c.cdr->c.car = newVal;
            return voidValue;
        }
        bindings = cdr(bindings);
        if (bindings->type == NULL_TYPE)
        {
            bindings = frame->parent->bindings;
        }
    }
    return voidValue;
}

Value *begin(Value *args, Frame *frame)
{
    while (args->type != NULL_TYPE)
    {
        Value *evaluation = eval(car(args), frame);
        if (cdr(args)->type == NULL_TYPE)
        {
            return evaluation;
        }
        args = cdr(args);
    }
    Value *voidValue = talloc(sizeof(Value));
    voidValue->type = VOID_TYPE;
    return voidValue;
}

Value *evalDefine(Value *args, Frame *frame)
{
    Value *var = car(args);
    Value *val = car(cdr(args));
    Value *newGlobalBinding = talloc(sizeof(Value));
    newGlobalBinding->type = CONS_TYPE;
    newGlobalBinding->c.car = cons(var, cons(eval(val, frame), makeNull()));
    newGlobalBinding->c.cdr = frame->bindings;
    frame->bindings = newGlobalBinding;
    Value *voidCell = talloc(sizeof(Value));
    voidCell->type = VOID_TYPE;
    return voidCell;
}

Value *evalLambda(Value *tree, Frame *frame)
{
    Value *closure = talloc(sizeof(Value));
    closure->type = CLOSURE_TYPE;
    closure->cl.frame = frame;
    closure->cl.paramNames = tree->c.car;
    closure->cl.functionCode = tree->c.cdr->c.car;
    return closure;
}

Value *apply(Value *function, Value *args)
{
    if (length(args) != length(function->cl.paramNames))
    {
        printf("apply fail\n");
        evalationError();
    }
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = function->cl.frame;
    newFrame->bindings = makeNull();
    Value *cur = function->cl.paramNames;
    while (cur->type != NULL_TYPE)
    {
        // Value *newBinding = cons(eval(args->c.car, function->cl.frame), makeNull());
        Value *newBinding = cons(args->c.car, makeNull());
        newBinding = cons(cur->c.car, newBinding);
        newFrame->bindings = cons(newBinding, newFrame->bindings);
        cur = cur->c.cdr;
        args = args->c.cdr;
    }
    return eval(function->cl.functionCode, newFrame);
}

Value *primitiveNull(Value *args, Frame *frame)
{
    if (length(args) != 1)
    {
        evalationError();
    }
    Value *boolean = makeNull();
    boolean->type = BOOL_TYPE;
    if (args->c.car->type == CONS_TYPE)
    {
        if (args->c.car->c.cdr->c.car->type == NULL_TYPE)
        {
            boolean->i = 1;
        }
        else
        {
            boolean->i = 0;
        }
    }
    else
    {
        if (args->c.car->type == NULL_TYPE)
        {
            boolean->i = 1;
        }
        else
        {
            boolean->i = 0;
        }
    }
    return boolean;
}

Value *primitiveCar(Value *args, Frame *frame)
{
    if (length(args) != 1)
    {
        evalationError();
    }
    return eval(args->c.car, frame)->c.car;
}

Value *primitiveCdr(Value *args, Frame *frame)
{
    if (length(args) != 1)
    {
        evalationError();
    }
    return eval(args->c.car, frame)->c.cdr;
}

Value *primitiveCons(Value *args, Frame *frame)
{
    if (length(args) != 2)
    {
        evalationError();
    }
    Value *consCell = makeNull();
    consCell->type = CONS_TYPE;
    Value *car = eval(args->c.car, frame);
    Value *cdr = eval(args->c.cdr->c.car, frame);
    consCell->c.car = car;
    consCell->c.cdr = cdr;
    return consCell;
}

Value *primitiveAdd(Value *args, Frame *frame)
{
    double value = 0;
    bool isDouble = false;
    Value *sum = makeNull();

    while (args->type != NULL_TYPE)
    {
        if (car(args)->type == INT_TYPE)
        {
            value = value + car(args)->i;
        }
        else if (car(args)->type == DOUBLE_TYPE)
        {
            isDouble = true;
            value = value + car(args)->d;
        }
        else if (car(args)->type == SYMBOL_TYPE)
        {
            if (eval(car(args), frame)->type == INT_TYPE)
            {
                value = value + eval(car(args), frame)->i;
            }
            else if (eval(car(args), frame)->type == DOUBLE_TYPE)
            {
                value = value + eval(car(args), frame)->d;
            }
        }
        else if (car(args)->type == CONS_TYPE)
        {
            if (eval(car(args), frame->parent)->type == INT_TYPE)
            {
                value = value + eval(car(args), frame)->i;
            }
        }
        args = cdr(args);
    }

    if (isDouble == false)
    {
        sum->type = INT_TYPE;
        sum->i = (int)value;
        return sum;
    }
    else
    {
        sum->type = DOUBLE_TYPE;
        sum->d = value;
        return sum;
    }
}
Value *AND(Value *args, Frame *frame)
{
    Value *boolean = talloc(sizeof(Value));

    boolean = eval(car(args), frame);
    if (length(args) == 1)
    {
        return boolean;
    }
    if (boolean->i == 0)
    {
        return boolean;
    }
    boolean = eval(car(cdr(args)), frame);
    return boolean;
}

Value *OR(Value *args, Frame *frame)
{
    Value *boolean = talloc(sizeof(Value));

    boolean = eval(car(args), frame);
    if (length(args) == 1)
    {
        return boolean;
    }
    if (boolean->i == 1)
    {
        return boolean;
    }
    boolean = eval(car(cdr(args)), frame);
    return boolean;
}
Value *eval(Value *tree, Frame *frame)
{
    Value *result = talloc(sizeof(Value));
    switch (tree->type)
    {
    case INT_TYPE:
    {
        result->type = INT_TYPE;
        result->i = tree->i;
        break;
    }
    case SYMBOL_TYPE:
    {
        if (isNull(frame->bindings))
        {
            Frame *recursiveFrame = talloc(sizeof(Frame));
            if (frame->parent != NULL)
            {
                if (!isNull(frame->parent->bindings))
                {
                    recursiveFrame = frame->parent;
                    recursiveFrame->bindings = frame->parent->bindings;
                    return eval(tree, recursiveFrame);
                }
            }
            result->type = SYMBOL_TYPE;
            result->s = tree->s;
            break;
        }
        else
        {
            if (frame->bindings->type != CONS_TYPE)
            {
                printf("test1\n");
                evalationError();
            }
            else if (frame->bindings->c.car->type != CONS_TYPE)
            {
                printf("test2\n");
                evalationError();
            }
            else if (frame->bindings->c.car->c.car->type != SYMBOL_TYPE)
            {
                printf("test3\n");
                evalationError();
            }
            if (strcmp(frame->bindings->c.car->c.car->s, tree->s) == 0)
            {
                // printf("%i\n", frame->bindings->c.car->c.cdr->c.car->i);
                Frame *nullframe = talloc(sizeof(Frame));
                nullframe->parent = NULL;
                nullframe->bindings = makeNull();
                return eval(frame->bindings->c.car->c.cdr->c.car, nullframe);
            }
            else
            {
                Frame *recursiveFrame = talloc(sizeof(Frame));
                if (!isNull(frame->bindings->c.cdr))
                {
                    recursiveFrame->bindings = frame->bindings->c.cdr;
                    recursiveFrame->parent = frame->parent;
                    return eval(tree, recursiveFrame);
                }
                else if (!isNull(frame->parent->bindings))
                {
                    recursiveFrame = frame->parent;
                    recursiveFrame->bindings = frame->parent->bindings;
                    return eval(tree, recursiveFrame);
                }
                // evalationError();
            }
        }
    }
    case CONS_TYPE:
    {
        Value *first = car(tree);
        Value *args = cdr(tree);
        // Sanity and error checking on first...
        // printf("%u", args->type);
        // printf("%i", length(args));
        if (first->type != SYMBOL_TYPE)
        {
            if (first->type != CONS_TYPE)
            {
                evalationError();
            }
            if (eval(first, frame)->type == CLOSURE_TYPE)
            {
                result = apply(eval(first, frame), args);
            }
            else
            {
                result = eval(cons(eval(first, frame), args), frame);
            }
        }
        else if (!strcmp(first->s, "if"))
        {
            result = evalIf(args, frame);
        }
        else if (!strcmp(first->s, "let"))
        {
            result = evalLet(args, frame);
        }
        else if (!strcmp(first->s, "let*"))
        {
            result = evalLetStar(args, frame);
        }
        else if (!strcmp(first->s, "quote"))
        {
            if (length(args) != 1)
            {
                evalationError();
            }
            result = car(args);
        }
        else if (!strcmp(first->s, "define"))
        {
            if (length(args) != 2)
            {
                evalationError();
            }
            result = evalDefine(args, frame);
        }
        else if (!strcmp(first->s, "lambda"))
        {
            result = evalLambda(cdr(tree), frame);
        }
        else if (!strcmp(first->s, "begin"))
        {
            result = begin(args, frame);
        }
        else if (!strcmp(first->s, "letrec"))
        {
            result = letRec(args, frame);
        }
        else if (!strcmp(first->s, "null?"))
        {
            result = primitiveNull(cdr(tree), frame);
        }
        else if (!strcmp(first->s, "="))
        {
            result = Equal(args, frame);
        }
        else if (!strcmp(first->s, "-"))
        {
            result = primitiveSubtract(args, frame);
        }
        else if (!strcmp(first->s, "cons"))
        {
            result = primitiveCons(cdr(tree), frame);
        }
        else if (!strcmp(first->s, "<"))
        {
            result = lessThan(args, frame);
        }
        else if (!strcmp(first->s, "set!"))
        {
            result = set(args, frame);
        }
        else if (!strcmp(first->s, ">"))
        {
            result = greaterThan(args, frame);
        }
        else if (!strcmp(first->s, "car"))
        {
            result = primitiveCar(cdr(tree), frame);
        }
        else if (!strcmp(first->s, "cdr"))
        {
            result = primitiveCdr(cdr(tree), frame);
        }
        else if (!strcmp(first->s, "and"))
        {
            result = AND(args, frame);
        }
        else if (!strcmp(first->s, "or"))
        {
            result = OR(args, frame);
        }
        else if (!strcmp(first->s, "+"))
        {
            result = primitiveAdd(cdr(tree), frame);
        }
        else
        {
            if (first->type == SYMBOL_TYPE)
            {
                if (!isNull(frame->bindings))
                {
                    if (strcmp(first->s, frame->bindings->c.car->c.car->s) == 0)
                    {
                        if (frame->bindings->c.car->c.cdr->c.car->type != CLOSURE_TYPE)
                        {
                            evalationError();
                        }
                        result = apply(frame->bindings->c.car->c.cdr->c.car, args);
                    }
                    else
                    {
                        Frame *recursiveFrame = talloc(sizeof(Frame));
                        recursiveFrame->bindings = frame->bindings;
                        while (!isNull(frame->bindings->c.cdr))
                        {
                            if (strcmp(first->s, recursiveFrame->bindings->c.car->c.car->s) == 0)
                            {
                                result = apply(recursiveFrame->bindings->c.car->c.cdr->c.car, args);
                            }
                            recursiveFrame->bindings = recursiveFrame->bindings->c.cdr;
                        };
                    }
                }
            }
        }
    }
    case OPENBRACKET_TYPE:
        break;
    case CLOSEBRACKET_TYPE:
        break;
    case DOT_TYPE:
        break;
    case SINGLEQUOTE_TYPE:
        break;
    case VOID_TYPE:
        break;
    case DOUBLE_TYPE:
        result->type = DOUBLE_TYPE;
        result->d = tree->d;
        break;
    case STR_TYPE:
        result->type = STR_TYPE;
        result->s = tree->s;
        break;
    case NULL_TYPE:
        break;
    case PTR_TYPE:
        break;
    case OPEN_TYPE:
        break;
    case CLOSE_TYPE:
        break;
    case BOOL_TYPE:
    {
        result->type = BOOL_TYPE;
        result->i = tree->i;
    }
    case CLOSURE_TYPE:
    {
        break;
    }
    case PRIMITIVE_TYPE:
        break;
    case UNSPECIFIED_TYPE:
        break;
    }
    return result;
}
void printCons(Value *tree)
{
    switch (tree->type)
    {
    case INT_TYPE:
    {
        printf("%i ", tree->i);
        break;
    }
    case SYMBOL_TYPE:
    {
        printf("%s ", tree->s);
        break;
    }
    case CONS_TYPE:
        printf("(");
        Value *cur = tree;
        while (!isNull(cur))
        {
            printCons(cur->c.car);
            cur = cur->c.cdr;
        }
        printf(")");
        break;
    case OPENBRACKET_TYPE:
        break;
    case CLOSEBRACKET_TYPE:
        break;
    case DOT_TYPE:
        break;
    case SINGLEQUOTE_TYPE:
        break;
    case VOID_TYPE:
        break;
    case UNSPECIFIED_TYPE:
        break;
    case DOUBLE_TYPE:
        printf("%.6f ", tree->d);
        break;
    case STR_TYPE:
        printf("%s ", tree->s);
        break;
    case NULL_TYPE:
        break;
    case PTR_TYPE:
        break;
    case OPEN_TYPE:
        break;
    case CLOSE_TYPE:
        break;
    case PRIMITIVE_TYPE:
        break;
    case BOOL_TYPE:
    {
        if (tree->i == 1)
            printf("#t ");
        if (tree->i == 0)
            printf("#f ");
    }
    case CLOSURE_TYPE:
    {
        break;
    }
    }
}
void printList(Value *tree)
{
    switch (tree->type)
    {
    case INT_TYPE:
    {
        printf("%i\n", tree->i);
        break;
    }
    case SYMBOL_TYPE:
    {
        printf("%s\n", tree->s);
        break;
    }
    case CONS_TYPE:
        printf("(");
        Value *cur = tree;
        if (tree->c.cdr->type != CONS_TYPE)
        {
            printCons(tree->c.car);
            printf(". ");
            printCons(tree->c.cdr);
        }
        else
        {
            while (!isNull(cur))
            {
                printCons(cur->c.car);
                cur = cur->c.cdr;
            }
        }
        printf(")\n");
        break;
    case OPENBRACKET_TYPE:
        break;
    case CLOSEBRACKET_TYPE:
        break;
    case DOT_TYPE:
        break;
    case SINGLEQUOTE_TYPE:
        break;
    case UNSPECIFIED_TYPE:
        break;
    case VOID_TYPE:
        break;
    case DOUBLE_TYPE:
        printf("%.6f\n", tree->d);
        break;
    case STR_TYPE:
        printf("%s\n", tree->s);
        break;
    case NULL_TYPE:
        printf("()\n");
        break;
    case PTR_TYPE:
        break;
    case OPEN_TYPE:
        break;
    case CLOSE_TYPE:
        break;
    case PRIMITIVE_TYPE:
        break;
    case CLOSURE_TYPE:
    {
        printf("#<procedure>\n");
        break;
    }
    case BOOL_TYPE:
    {
        if (tree->i == 1)
            printf("#t\n");
        if (tree->i == 0)
            printf("#f\n");
    }
    }
}
void interpret(Value *tree)
{
    Frame *frame = talloc(sizeof(Frame));
    frame->parent = NULL;
    frame->bindings = makeNull();
    while (tree->type != NULL_TYPE)
    {
        Value *evaluation = eval(tree->c.car, frame);
        printList(evaluation);
        tree = cdr(tree);
    }
}

#endif