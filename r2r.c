/* 
 * Range -> Regex Converter
 * by Florian Wesch <fw@dividuum.de>
 *
 * This is free software released into the public domain.
 * 
 * TODO: 
 *  - Ranges like  10000 11000 should yield 1(0[0-9]{3}|1000)
 *  - optimize brackets
 *
 * Compile using:
 * $ gcc s2r.c -o s2r
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int base = 10;

void *xmalloc(size_t size) {
    void *bla = malloc(size);
    if (!bla) {
        fprintf(stderr, "no memory\n");
        exit(1);
    }
    return bla;
}

struct node {
    int l;
    int r;
    struct node *ln;
    struct node *rn;
};

struct node *newnode() {
    struct node *n = xmalloc(sizeof(struct node));
    return n;
}

struct tree {
    int l1;
    int l2;
    char *num1;
    char *num2;
    struct node *root;
};

struct tree *newtree() {
    struct tree *t = xmalloc(sizeof(struct tree));
    return t;
}

int char2num(char c) {
    static int char2num[256] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
         0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, 
        -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 
        25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1, 
        -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 
        25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
    };
    if (char2num[c] == -1) {
        fprintf(stderr, "invalid character %c\n", c);
        exit(1);
    }
    return char2num[c];
}

char num2char(int num) {
    static char num2char[36] = {
        '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
        'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
        'w','x','y','z'
    };
    if (num >= 36) {
        fprintf(stderr, "invalid num %d. wtf?\n", num);
        exit(1);
    }
    return num2char[num];
}


struct node *buildtreerec(char *num1, char *num2, int pos, int l1, int l2, int bound) {
    char l;
    char r;
    struct node *n;

    if (pos <= 0)
        return NULL;

    n = newnode();

    if (pos > l1)
        l = 0;
    else 
        l = char2num(*(num1 + l1 - pos));

    r = char2num(*(num2 + l2 - pos));
        
    if (bound == 0 && l == r) {
        n->l  = n->r  = l;
        n->ln = n->rn = buildtreerec(num1, num2, pos - 1, l1, l2, 0);
        
    } else {
        if (bound <= 0) {
            n->l  = l;
            n->ln = buildtreerec(num1, num2, pos - 1, l1, l2, -1);
        } else {
            n->l  = 0;
            n->ln = NULL;
        }

        if (bound >= 0) {
            n->r  = r;
            n->rn = buildtreerec(num1, num2, pos - 1, l1, l2,  1);
        } else {
            n->r  = base - 1;
            n->rn = NULL;
        }
    }

    return n;
}

struct tree *buildtree(char *num1, char *num2) {
    int swap = 0;
    struct tree *t = newtree();
    int l1 = strlen(num1);
    int l2 = strlen(num2);

    if (l1 > l2)
        swap = 1;
    else if (l1 == l2) {
        char *ptr1, *ptr2;
        for (ptr1 = num1, ptr2 = num2; *ptr1; ptr1++, ptr2++) {
            int n1 = char2num(*ptr1);
            int n2 = char2num(*ptr2);
            if (n1 != n2) {
                if (n1 > n2) swap = 1;
                break;
            }
        }
    }

    if (swap) {
        t->num1 = num2;
        t->num2 = num1;
        t->l1 = l2;
        t->l2 = l1;
    } else {
        t->num1 = num1;
        t->num2 = num2;
        t->l1 = l1;
        t->l2 = l2;
    }

    t->root = buildtreerec(t->num1, t->num2, t->l2, t->l1, t->l2, 0);
    return t;
}

void range(int from, int to) {
    if (to == from)
        printf("%c", num2char(to));
    else if (to - from == 1)
        printf("%c%c", num2char(from), num2char(to));
    else 
        printf("%c-%c", num2char(from), num2char(to));
}

void repeat(int from, int to, int num) {
    if (num <= 0) return;
    if (from > to) return;
#if 1
    printf("(");
    while (num-- > 0) {
        if (from == to)
            printf("%c", num2char(from));
        else {
            printf("[");
            if ((from <  10 && to  < 10) || 
                (from >= 10 && to >= 10))
                range(from, to);
            else {
                range(from, 9);
                range(10, to);
            }
            printf("]");
        }
    }
    printf(")");
#else
    if (from == to)
        printf("%c", num2char(from));
    else {
        printf("[");
        if ((from <  10 && to  < 10) || 
            (from >= 10 && to >= 10))
            range(from, to);
        else {
            range(from, 9);
            range(10, to);
        }
        printf("]");
    }
    if (num > 1)
        printf("{%d}", num);
#endif
}

void buildregexrec(struct node *node, int pos, int killzero) {
    printf("(");
    if (node->ln && node->rn) {
        if (node->l == node->r) {
            printf("%c", num2char(node->l));
            buildregexrec(node->ln, pos - 1, 0);
        } else {
            if (node->l != 0 || !killzero) {
                killzero = 0;
                printf("%c", num2char(node->l));
            }
            buildregexrec(node->ln, pos - 1, killzero);
            printf("|");
            if (node->r - node->l >= 2) {
                repeat(node->l + 1, node->r - 1, 1);
                repeat(0, base - 1, pos-1);
                printf("|");
            }
            printf("%c", num2char(node->r));
            buildregexrec(node->rn, pos - 1, 0);
        }
    } else if (node->ln) {
        if (node->l != 0 || !killzero) {
            killzero = 0;
            printf("%c", num2char(node->l));
        }
        buildregexrec(node->ln, pos - 1, killzero);
        if (node->l < base - 1) {
            printf("|");
            repeat(node->l + 1, base - 1, 1);
            repeat(0, base - 1, pos-1);
        }
    } else if (node->rn) {
        if (node->r > 0) {
            repeat(0, node->r - 1, 1);
            repeat(0, base - 1, pos-1);
            printf("|");
        }
        printf("%c", num2char(node->r));
        buildregexrec(node->rn, pos - 1, 0);
    } else {
        repeat(node->l, node->r, 1);
    }
    printf(")");
    free(node);
}

void buildregex(struct tree *tree, int killzero) { 
    buildregexrec(tree->root, tree->l2, killzero);
    free(tree);
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "S2R: builds regex matching numbers in range from <num1> to <num2>\n");
        fprintf(stderr, "usage: %s <num1> <num2> [base]\n", argv[0]);
        return 1;
    }
    
    if (argc == 4)
        base = atoi(argv[3]);

    if (base < 2 || base > 36) {
        fprintf(stderr, "invalid base\n");
        return 1;
    }

    buildregex(buildtree(argv[1], argv[2]), 1);
    return 0;
}
