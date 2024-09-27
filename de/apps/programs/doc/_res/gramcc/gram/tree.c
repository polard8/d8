/*
 * File: tree.c 
 *
 * tbst - calculando express�es. isso deu meio certo.
 *     ...
 */
 
 
//
// Includes.
// 
 

#include "gramc.h"

//#importante
//#express�o em ordem!!!
//os tokens ser�o colocados aqui como uma express�o em ordem.
int exp_buffer[32];
int exp_offset = 0;

//===============================================================================

//buffer pra fazer conta usando pos order
int POS_BUFFER[32];

int buffer_offset = 0;

//===============================================================================



 
//
//  ## STACK ##
//

struct stack
{
	int top;
    int items[32];
};



struct node 
{ 
	int key; 
	struct node *left, *right; 
}; 


int my_isdigit (char ch){
	
    return ( ch >= '0' && ch <= '9' );
}


// A utility function to create a new BST node 
struct node *newNode (int item){
	
	struct node *temp = (struct node *)malloc ( sizeof(struct node) );
    
	//#todo: check validation
	
	temp->key = item; 
	temp->left = temp->right = NULL;
	
	return temp; 
}; 


// A utility function to do inorder traversal of BST 
void inorder (struct node *root){
	
	if (root != NULL) 
	{ 
		inorder(root->left); 
		printf("%d \n", root->key); 
		inorder(root->right); 
	} 
}; 


//Em ordem  a+b
//desce at� o �ltimo pela esquerda, n�o havendo esquerda vai pra direita.
void exibirEmOrdem (struct node *root){
	
    if (root != NULL)
	{
		//visita a esquerda do pr�ximo
		//s� retorna no �ltimo ent�o printf funciona 
		//mostrando o conte�do do �ltimo 
		//ai visita a direita do �ltimo e desce pela esquerda,
		//n�o havendo esquerda vai pra direita.
        exibirEmOrdem (root->left);
        printf("%d ", root->key);
        
		exibirEmOrdem (root->right);
    }
};



//Pr�-ordem  +ab
void exibirPreOrdem (struct node *root){
    
	if (root != NULL)
	{
        //imprime o conte�do
		//desce at� o �ltimo pela esquerda
		//visita a direita e desce at� o �ltimo pela esquerda.
		printf("%d ", root->key);
        exibirPreOrdem(root->left);
        exibirPreOrdem(root->right);
    }
};


//P�s-ordem ab+
void exibirPosOrdem (struct node *root){
	
	//#importante
	//exibe em n�veis. de baixo para cima.
    
	if (root != NULL)
	{
		//desce at� o ultimo pela esquerda
		//visita o da direita e imprime;
        exibirPosOrdem(root->left);
        exibirPosOrdem(root->right);
		printf("%d ", root->key);
		
		if(buffer_offset<0 || buffer_offset >32)
		{
			printf("*buffer fail\n");
			return;
		}
		
		//
		//  ## IMPORTANTE ##
		//
		
		//colocar num buffer pra usar no c�lculo 
		//isso simula uma digita��o
		POS_BUFFER[buffer_offset] = (int) (root->key + '0');
		buffer_offset++;
    };
};


/* A utility function to insert a new node with given key in BST */
struct node* insert ( struct node* node, int key ){
	
	/* If the tree is empty, return a new node */
	if (node == NULL) 
        return newNode(key); 

	/* Otherwise, recur down the tree */
	if (key < node->key) 
		node->left = insert(node->left, key); 
	else if (key > node->key) 
		node->right = insert(node->right, key); 

	/* return the (unchanged) node pointer */
	return node; 
} 



/*
 *********************************************************
 *
 */
 
// Driver Program to test above functions 
// C program to demonstrate insert operation in binary search tree 

	/* Let us create following BST 
           - 
          /  \ 
         +     * 
        / \   / \ 
       4   3 2   5 */
	   
	   //4+3 - 2*5 = 12
	
int bst_main (){
	
	
	printf("bst_main:\n");
	
	buffer_offset = 0;
		
	struct node *root = NULL; 
	
	int i;
	int buffer1[32];
	int buffer2[32];
	
	int buffer1_offset = 0;
	int buffer2_offset = 0;
	
	//#IMPORTANTE:
	//ESSE � O BUFFER USADO PARA COLOCAR A EXPRESS�O EM ORDEM 
	//VAMOS FAZER ELE GLOBAL PARA SER PREENCHIDO PELOS TOKENS.
	
    //int exp[] = { 4, '+', 3, '-', 2, '*', 5, '?' };	
	
	int c;
	
	printf("for\n");
    
	//colocamos nos buffers em ordem.
	for ( i=0; (c = exp_buffer[i]) != '?'; i++ )		
    {
		if ( c>= 0 && c<= 9 )
		{
			printf(">");
			
			//d�gito
			buffer1[buffer1_offset] = (int) c;
            buffer1_offset++; 
			
		}else{
            
			printf("$");
			//operadores.
			buffer2[buffer2_offset] = (int) c;
			buffer2_offset++;
        }
    };
	
	
	//
	//visualizar os buffer,
	//pra depois nmanipular eles.
	buffer1[buffer1_offset] = (int) '?';
	buffer2[buffer2_offset] = (int) '?';
	
	//==========================================================================
	// #todo: NESSA HORA TEM QUE AJUSTAR A PRECED�NCIA DOS OPERADORES ## 
	// 
	//
	
	//inserindo root.
	root = insert ( root, '?' ); 
	

	//operadores +-*
	for ( i=0; (c = buffer2[i]) != '?'; i++ )
	{
		//if ( c>= 0 && c<= 9 )
		//{
		//	printf ("%d", c);
		//	continue;
		//}
		printf ("%c", c);
		insert ( root, buffer2[i] ); 		
	}
	
    buffer1_offset--;//ajustando par ao �ltimo v�lido 
	
	//for ( i=0; (c = buffer1[i]) != '?'; i++ )
	for ( i=buffer1_offset; (c = buffer1[i]); i-- )	
	{
		c = buffer1[i];//redundante
		
		if ( c>= 0 && c<= 9 )
		{
			printf ("%d", c);
			insert ( root, c );
            			
			//continue;
		}
		//printf ("%c", c);
	}	

	
	//#OK 
	//nos buffers est�o na mesma ordem que na express�o.
	//agora vamos inserir na ordem inversa dos buffers.
	
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
	
 
    // ??
	// print inoder traversal of the BST 
	// inorder(root); 
		
	printf("\n\n em ordem: ");
	exibirEmOrdem(root);
	
	printf("\n\n pre ordem: ");
	exibirPreOrdem(root);
	
	printf("\n\n pos ordem: ");
	exibirPosOrdem(root);

	printf("\n\n");
	
	return 0; 
}; 


//===============================================================================


void push ( struct stack *s, int x ){
	
    if ( s->top > 32 )
    {
		printf("Stack Overflow !\n");
        return;
		
    }else{
		
		s->items[ ++s->top ] = x;
    }
};


int pop (struct stack *s){
	
    if ( s->top == -1)
	{
		printf("Stack Underflow !\n");
        return 0; //??
		
    }else{
		
		return ( s->items[ s->top-- ] );
    };
};


int oper(char c,int opnd1,int opnd2){
	
    switch (c)
    {	
		//case '*': 
        case 90:  		
		    return (opnd1*opnd2);
			
        //case '+': 
		case 91:    
			return(opnd1+opnd2);        		
			
		//case '-': 
		case 93:
		    return(opnd1-opnd2);
			
		//case '/': 
		case 95:
		    return(opnd1/opnd2);
        
		//#todo
		case '^': 
		    return 0; //return(pow(opnd1,opnd2));
        
		default: 
		    printf("oper: Invalid operator! %d\n", c);
			return 0;
    };
};


int eval ( int *str ){
	
    int i;
    int opnd1, opnd2, val;
	char c;
	
	struct stack stk;
    
	stk.top = -1;
	
    printf("\n eval:\n");	
	
    //for ( i=0; (c = str[i]) != '?'; i++ )
    for ( i=0; (c = str[i]) != 111; i++ )		
    {
		if ( c>='0' && c<='9' )
		{
            push ( &stk, (int)( c - '0' ) );
        
		}else{
            
			//O problema � a ordem em que os operandos aparecem 
			//o �ltimo � a raiz.
			//e aqui a o operando raiz aparece no meio da express�o.
			
			opnd2 = pop (&stk);
            opnd1 = pop (&stk);
			
            val = oper ( c, opnd1, opnd2 );
            
			push ( &stk, val );
        }
    }
	
	//o resultado � o que sobrou na pilha
    return ( pop(&stk) );
}


/*
 ==================================================================================
 * iNICIALIZA��O GEN�RICA PARA TESTE DE APLICATIVO.
 * ##imortante: provavelmente essa rotina n�o � usada.
 */

int testtest_main (){
	
	printf ("testtest_main: Not used ??");
	while(1){}
	
	/*

	int i;	
	int max;
	
	printf("\n");
	printf("Initilizing TBST.BIN ...\n\n");
	
    libcInitRT();
    stdioInitialize();		

	bst_main(); 
	
	//finalizador
	POS_BUFFER[buffer_offset] = '?';	
	
	max = buffer_offset;
	
	printf("\n the pos order is: "); 
	if(max >32 || max <=0)
		printf("max fail");
	
	for (i=0; i<max; i++)
	{
		printf("%d ",POS_BUFFER[i]);
	}
    
    printf ("\n \n Result after evaluation is: %d \n", eval ( (int*) &POS_BUFFER[0] ) );	
	
	printf("DONE!");	
	
	*/
	
	return 0;
}


//calcula a express�o e retorna o valor;

unsigned long tree_eval (){
	
	//#todo:
	//prepara o buffer contendo a express�o em ordem. 
	//pra isso precisamos pegar os tokens e colocar no buffer. 
	
	//#exemplo
	//tem que pegar os tokens e colocar assim no buffer.
	//+os n�meros s�o n�meros mesmo 
	//+os operadores s�o chars ou strings.
    //tem que finalizar com '?'	
	//exp[] = { 4, '+', 3, '-', 2, '*', 5, '?' };	
	
	//#todo
	//vamos copiar a fun��o no parser que pega os tokens de express�es.
	//mas por enquanto s� os operadores b�sicos.
	
	//===================================================
	
	
	int running = 1;
	int State = 1;
	int c;
	
	while (running == 1)
	{
	    c = yylex ();

		if ( c == TOKENEOF )
		{
            printf ("tree_eval: #error EOF in line %d\n", lineno);
            exit (1);			
		}  
		
		if ( c == TOKENSEPARATOR )
		{
			if ( strncmp ( (char *) real_token_buffer, ";", 1 ) == 0  )
			{
			    goto done;	
			}
		}
		
		switch (State)   
	    {
			//n�meros
            case 1:
			    switch(c)
				{
					//n�meros ou separadores
				    case TOKENCONSTANT:
                        exp_buffer[exp_offset] = (int) atoi (real_token_buffer);
						exp_offset++;
						State = 2; //depois de um n�mero espera-se um operador ou um separador.
						break;	
					
					// ; separador no caso de return void.
					//para quando a express�o � depois do return.
					case TOKENSEPARATOR:
					    if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
					    {
							goto done;
						}
						
						//if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
						//{}
						
					//#todo
					//temos que tratar as aberturas e fechamentos (),{}	
					
				    default:
					    printf("tree_eval: State1 default");
						exit(1);
					    break;
				}
			    break;

			//operadores 	
            case 2:
 		        switch (c)
				{
                    case '+':
                    case '-':
                    case '*':
					case '/':
					case '&':
                    case '|':
                    case '<':
                    case '>':
                    case '%':
                    case '^':
                    case '!':
                    case '=':
                        exp_buffer[exp_offset] = (int) c;
						exp_offset++;
						State = 1; //depois do operador esperamos um n�mero ou um separador ')' ou finalizador provis�rio ?.
						break;

					//')' provis�rio para terminar a express�o,
                    //da� incluimos o finalizador provis�rio '?'					
                    case TOKENSEPARATOR:						
				        if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
						{
                            exp_buffer[exp_offset] = (int) '?';
						    exp_offset++;
                            //
                            // ## DONE !! ##
                            //
                            goto do_bst;							
						}
						
				        if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
						{
                            exp_buffer[exp_offset] = (int) '?';
						    exp_offset++;
                            //
                            // ## DONE !! ##
                            //
                            goto do_bst;							
						}
					    break;						
				} 
			    break;
				
		    default:
			    printf("tree_eval: #error State2 default \n");
			    break;
        };		
		
    };		
	
	
	
	int j;
	int v; //#bugbug estamo lidando somente com d�gitos de 0 � 9.
	
do_bst:
    //#debug
	//visualizando o buffer
    for(j=0; j<32; j++)
    {
		v = exp_buffer[j];
		
		if ( v>= 0 && v<= 9 )
		{
		    printf("%d", exp_buffer[j]);	
		}else{
			printf("%c", exp_buffer[j]);
		}
	}
	
	//#debug 
	//hang
    //printf("do_bst: *debug breakpoint");
	//while(1){}    
	
	//==================================================
	//inicializa �rvore bin�ria.
	//ela pega uma express�o que est� em um buffer e 
	//prepara o buffer POS_BUFFER para eval usar.
	
	bst_main(); 
    
	//#debug
	//ok funcionou
	//printf ("\n tree_eval: result={%d} \n", eval ( (int*) &POS_BUFFER[0] ) );   	

	//#debug 
	//hang
    //printf("*debug breakpoint");
	//while(1){}    
	
	
	unsigned long ret_val;
	ret_val = (unsigned long) eval ( (int *) &POS_BUFFER[0] ); 
	
	printf("result: %d\n",ret_val);
	
	return (ret_val); 
	
done:
    return (ret_val);
}


//
// End.
//

