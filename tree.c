#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "tree.h"

jmp_buf begin;

void error(char *err, int line)
{
    printf("error: %s\nin line %d in file %s\n", err, line, __FILE__);
    longjmp(begin, 1);
}

static void add_argv(struct cmd_inf *unit, char *string)
{
    int size = ++unit->argc;
    unit->argv = realloc(unit->argv, (size + 1) * sizeof(char *));
    unit->argv[size - 1] = string;
    unit->argv[size] = NULL;
}

char is_name(const char *string)
{
    for (int i = 0; i < strlen((string)); ++i)
    {
        switch (string[i])
        {
        case '>':
        case '<':
        case '&':
        case '#':
        case '(':
        case ')':
        case '|':
        case ';':
            return 0;
        default:
            continue;

        }
    }
    return 1;
}

static struct cmd_inf *create_unit1(struct iterator *iter)
{
    struct cmd_inf *unit = cmd_inf_constructor();
    int unit_filled = 0;
    for (const char *lexeme = iter->next_string(iter); lexeme != NULL; lexeme = iter->next_string(iter))
    {

        if (strcmp(lexeme, ">") == 0)
        {
            if (unit->outfile != NULL)
            {
                delete_tree(unit);
                error("two input files", __LINE__);
            }
            if (!is_name(lexeme = iter->next_string(iter)))
            {
                delete_tree(unit);
                error("parse error near `>'", __LINE__);
            }
            unit->outfile = (char *) lexeme;
            unit->append = 0;
        }
        else if (strcmp(lexeme, ">>") == 0)
        {
            if (unit->outfile != NULL)
            {
                delete_tree(unit);
                error("two input files", __LINE__);
            }
            if (!is_name(lexeme = iter->next_string(iter)))
            {
                delete_tree(unit);
                error("parse error near `>>'", __LINE__);
            }
            unit->outfile = (char *) lexeme;
            unit->append = 1;
        }
        else if (strcmp(lexeme, "<") == 0)
        {
            if (unit->infile != NULL)
            {
                delete_tree(unit);
                error("two output files", __LINE__);
            }
            if (!is_name(lexeme = iter->next_string(iter)))
            {
                delete_tree(unit);
                error("parse error near `<'", __LINE__);
            }
            unit->infile = (char *) lexeme;
        }
        else if (strcmp(lexeme, ";") == 0)
        {
            if (!unit_filled)
            {
                delete_tree(unit);
                error("parse error near `;;'", __LINE__);
            }
            unit->type = NXT;
            //return NULL;
            //if (flag == 1)
            //unit->next = create_unit1(iter);
            //break;
            return unit;
        }
        else if (strcmp(lexeme, "&&") == 0)
        {
            unit->type = AND;
            unit->next = create_unit1(iter);
            break;

        }
        else if (strcmp(lexeme, "||") == 0)
        {
            unit->type = OR;
            unit->next = create_unit1(iter);
            break;

        }
        else if (strcmp(lexeme, "&") == 0)
        {
            unit->backgrnd = 1;
            unit->next = create_unit1(iter);
            break;

        }
        else if (strcmp(lexeme, "|") == 0)
        {
            unit->pipe = create_unit1(iter);
            unit->next = create_unit1(iter);
            break;

        }
        else if (strcmp(lexeme, "(") == 0)
        {
            if (unit->argc != 0)
            {
                delete_tree(unit);
                error("Unexpected arg before '('", __LINE__);
            }
            unit->psubcmd = create_unit1(iter);

        }
        else if (strcmp(lexeme, ")") == 0)
        {
            if (!unit_filled)
            {
                delete_tree(unit);
                return NULL;
            }
            else
                return unit;

        }
        else
        {
            char *string = malloc(sizeof(char) * (strlen(lexeme) + 1));
            strcpy(string, lexeme);
            add_argv(unit, string);
        }
        unit_filled = 1;
    }
    if (!unit_filled)
    {
        delete_tree(unit);
        return NULL;
    }
    else
        return unit;
}

static struct cmd_inf *create_unit(struct iterator *iter)
{
    struct cmd_inf *unit = cmd_inf_constructor();
    int unit_filled = 0;
    for (const char *lexeme = iter->next_string(iter); lexeme != NULL; lexeme = iter->next_string(iter))
    {

        if (strcmp(lexeme, ">") == 0)
        {
            if (unit->outfile != NULL)
            {
                delete_tree(unit);
                error("two input files", __LINE__);
            }
            if (!is_name(lexeme = iter->next_string(iter)))
            {
                delete_tree(unit);
                error("parse error near `>'", __LINE__);
            }
            unit->outfile = (char *) lexeme;
            unit->append = 0;
        }
        else if (strcmp(lexeme, ">>") == 0)
        {
            if (unit->outfile != NULL)
            {
                delete_tree(unit);
                error("two input files", __LINE__);
            }
            if (!is_name(lexeme = iter->next_string(iter)))
            {
                delete_tree(unit);
                error("parse error near `>>'", __LINE__);
            }
            unit->outfile = (char *) lexeme;
            unit->append = 1;
        }
        else if (strcmp(lexeme, "<") == 0)
        {
            if (unit->infile != NULL)
            {
                delete_tree(unit);
                error("two output files", __LINE__);
            }
            if (!is_name(lexeme = iter->next_string(iter)))
            {
                delete_tree(unit);
                error("parse error near `<'", __LINE__);
            }
            unit->infile = (char *) lexeme;
        }
        else if (strcmp(lexeme, ";") == 0)
        {
            if (!unit_filled)
            {
                delete_tree(unit);
                error("parse error near `;;'", __LINE__);
            }
            unit->type = NXT;
            //return NULL;
            //if (flag == 1)
            unit->next = create_unit(iter);
            break;
        }
        else if (strcmp(lexeme, "&&") == 0)
        {
            unit->type = AND;
            unit->next = create_unit(iter);
            break;

        }
        else if (strcmp(lexeme, "||") == 0)
        {
            unit->type = OR;
            unit->next = create_unit(iter);
            break;

        }
        else if (strcmp(lexeme, "&") == 0)
        {
            unit->backgrnd = 1;
            unit->next = create_unit(iter);
            break;

        }
        else if (strcmp(lexeme, "|") == 0)
        {
            unit->pipe = create_unit(iter);
            //unit->next = create_unit1(iter);
            break;

        }
        else if (strcmp(lexeme, "(") == 0)
        {
            if (unit->argc != 0)
            {
                delete_tree(unit);
                error("Unexpected arg before '('", __LINE__);
            }
            unit->psubcmd = create_unit(iter);

        }
        else if (strcmp(lexeme, ")") == 0)
        {
            if (!unit_filled)
            {
                delete_tree(unit);
                return NULL;
            }
            else
                return unit;

        }
        else
        {
            char *string = malloc(sizeof(char) * (strlen(lexeme) + 1));
            strcpy(string, lexeme);
            add_argv(unit, string);
        }
        unit_filled = 1;
    }
    if (!unit_filled)
    {
        delete_tree(unit);
        return NULL;
    }
    else
        return unit;
}


extern struct cmd_inf *list_to_tree(string_list list)
{
    struct iterator *iter = iterator_constructor(list);
    struct cmd_inf *root;
    if (!setjmp(begin))
    {
        root = create_unit(iter);
    }
    else
    {
        printf("Syntax error, pleas try again\n");
        root = NULL;
    }
    free(iter);
    return root;
}

extern void delete_tree(struct cmd_inf *tree)
{
    if (tree == NULL)
        return;
    delete_tree(tree->psubcmd);
    delete_tree(tree->pipe);
    delete_tree(tree->next);
    for (int i = 0; tree->argv[i] != NULL; free(tree->argv[i]), ++i);
    free(tree->argv);
    free(tree);
}

void make_shift(int n)
{
    while(n--)
        putc(' ', stderr);
}

void print_argv(char **p, int shift)
{
    char **q = p;
    if(p != NULL)
    {
        while(*p!=NULL)
        {
            make_shift(shift);
            fprintf(stderr, "argv[%d] = %s\n",(int) (p-q), *p);
            p++;
        }
    }
}

extern void print_tree(struct cmd_inf *t, int shift)
{
    char **p;
    if(t==NULL)
        return;
    p=t->argv;
    if(p!=NULL)
        print_argv(p, shift);
    else
    {
        make_shift(shift);
        fprintf(stderr, "psubshell\n");
    }
    make_shift(shift);
    if(t->infile==NULL)
        fprintf(stderr, "infile = NULL\n");
    else
        fprintf(stderr, "infile = %s\n", t->infile);
    make_shift(shift);
    if(t->outfile==NULL)
        fprintf(stderr, "outfile = NULL\n");
    else
        fprintf(stderr, "outfile = %s\n", t->outfile);
    make_shift(shift);
    fprintf(stderr, "append = %d\n", t->append);
    make_shift(shift);
    fprintf(stderr, "background = %d\n", t->backgrnd);
    make_shift(shift);
    fprintf(stderr, "type = %s\n", t->type==NXT?"NXT": t->type==OR?"OR": "AND" );
    make_shift(shift);
    if(t->psubcmd==NULL)
        fprintf(stderr, "psubcmd = NULL \n");
    else
    {
        fprintf(stderr, "psubcmd---> \n");
        print_tree(t->psubcmd, shift+5);
    }
    make_shift(shift);
    if(t->pipe==NULL)
        fprintf(stderr, "pipe = NULL \n");
    else
    {
        fprintf(stderr, "pipe---> \n");
        print_tree(t->pipe, shift+5);
    }
    make_shift(shift);
    if(t->next==NULL)
        fprintf(stderr, "next = NULL \n");
    else
    {
        fprintf(stderr, "next---> \n");
        print_tree(t->next, shift+5);
    }
}

struct cmd_inf *cmd_inf_constructor()
{
    struct cmd_inf *unit = malloc(sizeof(struct cmd_inf));
    unit->argc = 0;
    unit->argv = malloc(sizeof(char *));
    unit->argv[0] = NULL;
    unit->infile = NULL;
    unit->outfile = NULL;
    unit->append = 0;
    unit->backgrnd = 0;
    unit->psubcmd = NULL;
    unit->pipe = NULL;
    unit->next = NULL;
    unit->type = NXT;
    return unit;
}

