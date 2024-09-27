
// tree.c 
// bst - calculando express�es.

#include "gramcnf.h"

// #importante
// #express�o em ordem!
// Os tokens ser�o colocados aqui como uma express�o em ordem.
int exp_buffer[32];
int exp_offset=0;

//====================================================================
// Buffer pra fazer conta usando pos order.
int POS_BUFFER[32];
int buffer_offset = 0;
//====================================================================

struct stack
{
    int top;
    int items[32];
};

struct node 
{ 
    int key;
    struct node *left;
    struct node *right;
}; 

// ==============================================

static int bst_initialize(void);
static int eval(int *str);
static int my_isdigit(char ch);
static struct node *newNode(int item);
static void inorder(struct node *root);
static void exibirEmOrdem (struct node *root);
static void exibirPreOrdem(struct node *root);
static void exibirPosOrdem (struct node *root);
static struct node *insert( struct node* node, int key );
static void push( struct stack *s, int x );
static int pop (struct stack *s);
static int oper(char c, int opnd1, int opnd2);


// ==============================================

static int my_isdigit(char ch)
{
    return ( ch >= '0' && ch <= '9' );
}

// A utility function to create a new BST node.
static struct node *newNode(int item)
{
    struct node *tmp;

    tmp = (struct node *) malloc( sizeof(struct node) );
    if( (void*) tmp == NULL ){
        return NULL;
    }
    tmp->key = (int) item; 
    tmp->left = NULL;
    tmp->right = NULL;

    return (struct node *) tmp;
}


// A utility function to do inorder traversal of BST.
static void inorder(struct node *root)
{
    if ( (void*) root != NULL )
    { 
        inorder(root->left); 
        printf("%d \n", root->key); 
        inorder(root->right); 
    }
}

// # same as 'inorder()'
// Em ordem  a+b.
// Desce at� o �ltimo pela esquerda, 
// n�o havendo esquerda vai pra direita.
// Visita a esquerda do pr�ximo
// s� retorna no �ltimo ent�o printf funciona 
// mostrando o conte�do do �ltimo 
// ai visita a direita do �ltimo e desce pela esquerda,
// n�o havendo esquerda vai pra direita.
static void exibirEmOrdem (struct node *root)
{
    if ( (void*) root != NULL )
    {
        exibirEmOrdem (root->left);
        printf("%d ", root->key);
        exibirEmOrdem (root->right);
    }
}


// Pr�-ordem +ab.
// Imprime o conte�do
// desce at� o �ltimo pela esquerda
// visita a direita e desce at� o �ltimo pela esquerda.
static void exibirPreOrdem(struct node *root)
{
    if ((void*)root != NULL)
    {
        printf("%d ", root->key);
        exibirPreOrdem(root->left);
        exibirPreOrdem(root->right);
    }
}

// P�s-ordem ab+.
// #importante
// Exibe em n�veis. de baixo para cima.
// desce at� o ultimo pela esquerda
// visita o da direita e imprime;
static void exibirPosOrdem (struct node *root)
{
    if ((void*)root != NULL)
    {
        exibirPosOrdem(root->left);
        exibirPosOrdem(root->right);
        printf("%d ", root->key);

        //?? # what is that?
        if( buffer_offset < 0 || 
            buffer_offset > 32 )
        {
            printf("exibirPosOrdem: buffer_offset\n");
            return;
        }

        // #importante
        // Colocar num buffer pra usar no c�lculo 
        // isso simula uma digita��o

        POS_BUFFER[buffer_offset] = 
            (int) (root->key + '0');

        // next
        buffer_offset++;
    };
}

// An utility function to insert 
// a new node with given key in BST.
static struct node *insert( struct node* node, int key )
{

// If the tree is empty, return a new node.
    if ( (void*) node == NULL ){
        return (struct node *) newNode(key); 
    }

// Otherwise, recur down the tree.

    // Se for menor, inclui na esquerda.
    if (key < node->key){
        node->left = (struct node *) insert(node->left, key); 
    // Se for maior, inclui na direita.
    }else if (key > node->key){
        node->right = (struct node *) insert(node->right, key); 
    };

// return the (unchanged) node pointer.
    return (struct node *) node; 
} 

/*
 bst_main:
 Called by tree_eval();
 Inicializa �rvore bin�ria.
 Ela pega uma express�o que est� em um buffer e 
 prepara o buffer POS_BUFFER para eval usar.
 Driver Program to test above functions 
 C program to demonstrate insert operation in binary search tree 
 Let us create following BST.

           - 
          /  \ 
         +     * 
        / \   / \ 
       4   3 2   5 
*/
     //4+3 - 2*5 = 12

//==================================================

static int bst_initialize(void)
{
// Inicializa �rvore bin�ria.

    buffer_offset = 0;
    struct node *root = NULL; 
    register int i=0;
    int buffer1[32];
    int buffer2[32];
    int buffer1_offset=0;
    int buffer2_offset=0;

    int c=0;

    printf ("bst_main:\n");
    printf ("for\n");

// #IMPORTANTE:
// ESSE � O BUFFER USADO PARA COLOCAR A EXPRESS�O EM ORDEM 
// VAMOS FAZER ELE GLOBAL PARA SER PREENCHIDO PELOS TOKENS.
    //int exp[] = { 4, '+', 3, '-', 2, '*', 5, '?' };

// Colocamos nos buffers em ordem.
    for ( 
        i=0; 
        (c = exp_buffer[i]) != '?';  // Se ainda n�o chegou ao fim.
        i++ )
    {
        // Numbers
        if ( c >= 0 && c <= 9 ){
            //printf(">");  //#debug
            // d�gito
            buffer1[buffer1_offset] = (int) c;
            buffer1_offset++; 
        // Operators
        }else{
            //printf("$");  //#debug
            // operadores
            buffer2[buffer2_offset] = (int) c;
            buffer2_offset++;
        }
    };

// Visualizar os buffer,
// pra depois manipular eles.

    buffer1[buffer1_offset] = (int) '?';
    buffer2[buffer2_offset] = (int) '?';

// ===================================================================
// #todo: 
// NESSA HORA TEM QUE AJUSTAR A 
// PRECED�NCIA DOS OPERADORES

// Inserindo root.
    root = insert(root,'?'); 

// Operadores +-*
    for ( 
        i=0; 
        (c = buffer2[i]) != '?'; i++ )
    {
        //if ( c>= 0 && c<= 9 )
        //{
        //    printf ("%d", c);
        //    continue;
        //}
        printf ("%c", c);
        
        insert(root,buffer2[i]);
    };

// Ajustando para o �ltimo v�lido.
    buffer1_offset--; 

    //for ( i=0; (c = buffer1[i]) != '?'; i++ )
    for ( 
        i=buffer1_offset; 
        (c = buffer1[i]); 
        i-- )
    {
        c = buffer1[i];    // Redundante

        if ( c>= 0 && c<= 9 )
        {
            printf ("%d", c);
            insert ( root, c );
            //continue;
        }
        //printf ("%c", c);
    };

// #OK 
// Nos buffers est�o na mesma ordem que na express�o.
// agora vamos inserir na ordem inversa dos buffers.

// ### root ##
//insert 111. 
// � um finalizador, representa o igual
//depois vamos usar o igual =
// x = 4+3 - 2*5

	//root = insert ( root, '?' ); 	
	//os operadores precisar sem inseridos na ordem da express�o.
	//insert(root, '+'); //
	//insert(root, '-'); //
	//insert(root, '*'); //
	//insert(root, 5);   // 
	//insert(root, 2);   //
	//insert(root, 3);   // 
	//insert(root, 4);   //

//
// Print
//

    printf(":: em ordem: \n");
    exibirEmOrdem(root);

    printf(":: pre ordem: \n");
    exibirPreOrdem(root);

    printf(":: pos ordem: \n");
    exibirPosOrdem(root);

    return 0; 
} 


//====================================================================

static void push( struct stack *s, int x )
{
    if( (void*) s == NULL ){
        printf("push: [FAIL] s\n");
        exit(1);
    }

    if ( s->top > 32 ){
        printf("Stack Overflow!\n");
        return;
    }else{
        s->items[ ++s->top ] = x;
    };
}

static int pop (struct stack *s)
{
    if( (void*) s == NULL ){
        printf("pop: [FAIL] s\n");
        exit(1);
    }

    if ( s->top == -1){
        printf("Stack Underflow !\n");
        return 0;  //??
    }else{
        return ( s->items[ s->top-- ] );
    };
}

static int oper(char c, int opnd1, int opnd2)
{
    switch (c){

    //case '*': 
    case 90:
        return (opnd1*opnd2);
        break;

    //case '+': 
    case 91:    
        return (opnd1+opnd2);
        break;

    //case '-': 
    case 93:
        return (opnd1-opnd2);
        break;

    //case '/': 
    case 95:
        return (opnd1/opnd2);
        break;

    //#todo
    case '^': 
        return 0; //return(pow(opnd1,opnd2));
        break;

    //...

    default: 
        printf("oper: Invalid operator! {%d}\n", c);
        return 0;
        break;
    };
}

static int eval(int *str)
{
    register int i=0;
    int opnd1=0;
    int opnd2=0; 
    int val=0;
    char c=0;
    struct stack stk;

    stk.top = -1;

    printf("\n");
    printf("eval:\n");

    //for ( i=0; (c = str[i]) != '?'; i++ )
    for ( 
        i=0; 
        (c = str[i]) != 111; 
        i++ )
    {
        // Push numbers.
        if ( c>='0' && c<='9' ){
            push( &stk, (int)( c - '0' ) );
        // Quando encontrar um operador, Faz push do result.
        // O problema � a ordem em que os operandos aparecem 
        // o �ltimo � a raiz.
        // e aqui a o operando raiz aparece no meio da express�o.
        }else{
            opnd2 = pop(&stk);
            opnd1 = pop(&stk);
            val = oper( c, opnd1, opnd2 );
            // Push result.
            push( &stk, val );
        }
    }

// O resultado � o que sobrou na pilha.
    return ( pop(&stk) );
}

// tree_eval:
// Calcula a express�o e retorna o valor.
// #todo:
// prepara o buffer contendo a express�o em ordem. 
// pra isso precisamos pegar os tokens e colocar no buffer. 
// #exemplo
// tem que pegar os tokens e colocar assim no buffer.
// +os n�meros s�o n�meros mesmo 
// +os operadores s�o chars ou strings.
// tem que finalizar com '?'
// exp[] = { 4, '+', 3, '-', 2, '*', 5, '?' };
// #todo
// vamos copiar a fun��o no parser que pega os tokens de express�es.
// mas por enquanto s� os operadores b�sicos.

unsigned long tree_eval(void)
{
// Global function.
// Pegamos os pr�ximos tokens e colocamos no buffer exp_buffer[].
// Initializa a �rvore bin�ria chamando bst_initialize(),
// os dados s�o transferidos para o buffer POS_BUFFER[].
// Calcula o resultado chamando eval();

    int running = 1;
    int State = 1;
    register int c=0;
    int j=0;
    int v=0;

    printf ("tree_eval:\n");

    while (running == 1){

    c = yylex();

    // EOF was found
    if (c == TOKENEOF){
        printf ("tree_eval: #error EOF in line %d\n", lineno);
        exit(1);
    }

    // ';' was found. 
    // End of statement.
    if (c == TOKENSEPARATOR)
    {
        if ( strncmp ( (char *) real_token_buffer, ";", 1 ) == 0  )
        {
            printf("tree_eval: ';' was found!\n");
            goto done;
        }
    }

    switch (State){
    
    // State1: Numbers.
    case 1:
        switch (c){

        // Constants: N�meros ou separadores.
        case TOKENCONSTANT:
            exp_buffer[exp_offset] = (int) atoi(real_token_buffer);
            exp_offset++;
            // Depois de um n�mero espera-se 
            // um operador ou um separador.
            State=2; 
            break;

        // ';' separador no caso de return void.
        // para quando a express�o � depois do return.
        case TOKENSEPARATOR:
            if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
            {
                goto done;
            }
            //if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
            //{}
        // #todo
        // Temos que tratar as aberturas e fechamentos (),{}	
        default:
            printf("tree_eval: State1 default\n");
            exit(1);
            break;
        }
        break;

    // State2: Operators and separators.
    case 2:
        switch (c){
        
        // Operators
        case '+':  case '-':  case '*':  case '/':
        case '&':  case '|':
        case '<':  case '>':
        case '%':
        case '^':
        case '!':
        case '=':
            exp_buffer[exp_offset] = (int) c;
            exp_offset++;
            // Depois do operador esperamos 
            // um n�mero ou um separador ')' ou 
            // finalizador provis�rio?.
            State=1; 
            break;

        // Separators
        // ')' provis�rio para terminar a express�o,
        // da� incluimos o finalizador provis�rio '?'
        case TOKENSEPARATOR:
            // ')'
            if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
            {
                exp_buffer[exp_offset] = (int) '?';
                exp_offset++;
                goto do_bst;  // #done
            }
            // ';'
            if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
            {
                 printf("tree_eval: ';' was found\n");
                 exp_buffer[exp_offset] = (int) '?';
                 exp_offset++;
                 goto do_bst;  // #done
            }
            break;
        
        // State2 default
        default:
            break;  
        
        } 
        break;

    default:
        printf("tree_eval: Default State\n");
        break;
    };
    };  // While end.

do_bst:

// #debug
// Visualizando o buffer.
    printf("\n");
    printf("tree_eval: do_bst: show buffer:\n");
    for (j=0; j<32; j++){
        v = exp_buffer[j];
        if ( v >= 0 && v <= 9 ){
            printf("exp_buffer: %d", exp_buffer[j]);
        }else{
            printf("exp_buffer: %c", exp_buffer[j]);
        }
    };
    //#debug 
    //hang
    //printf("do_bst: *debug breakpoint");
    //while(1){}    

//==================================================
// Inicializa �rvore bin�ria.
// ela pega uma express�o que est� em um buffer e 
// prepara o buffer POS_BUFFER para eval usar.

    bst_initialize(); 

//#debug
//ok funcionou
    //printf ("\n tree_eval: result={%d} \n", eval ( (int*) &POS_BUFFER[0] ) );   	

//#debug 
//hang
    //printf("*debug breakpoint");
    //while(1){}    

//
// Eval
//

    unsigned long ret_val=0;
    ret_val = (unsigned long) eval( (int *) &POS_BUFFER[0] ); 
    printf("result: %d\n",ret_val);
    return (unsigned long) ret_val; 
done:
    return (unsigned long) ret_val;
}

//
// End.
//

