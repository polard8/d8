/*
 * File: parser.c
 *
 * 2018 - Created by Fred Nora.
 */
 

#include  "as.h" 



//expression
unsigned long parse_expression ( int token );
unsigned long parse_sizeof( int token );

int parse_function ( int token );
int parse_return ( int token );


int parse_if ( int token );
int parse_do ( int token );
int parse_while ( int token );
int parse_for ( int token );

int parse_asm ( int token );

//...




//#test
/*
int parse_number (int olen){
	
    //pointer #todo	
    //register char *p = lexptr;
    register char *p = token_buffer;
  
  register long n = 0;
  register int c;
  register int base = 10;
  register len = olen;
  char *err_copy;

    //#todo função importada.
	extern double atof ();

    for (c = 0; c < len; c++)
	{
        //se encontrarmos um ponto no meio dos números.
		//pois não tem ponto no meio de uma expressões no  
		//#if (ainda não sei se gcc está falando de diretivas ou statement)
		if ( p[c] == '.' )
		{
            // It's a float since it contains a point.  
            //yyerror ("floating point numbers not allowed in #if expressions");
			printf ("floating point numbers not allowed in #if expressions");
            return ERROR;
      
            // ****************
	        //yylval.dval = atof (p);
	       // lexptr += len;
	       // return FLOAT;
		   // ****************  
        }		
	}
  
    //se o comprimeiro for mior que 3.
	//se começar com '0x', ou '0X', indicando se um número heaxadecimal.
	//então a base será 16.
	if ( len >= 3 && 
	     ( !strncmp (p, "0x", 2) || !strncmp (p, "0X", 2)) ) 
	{
		//baes hexa.
		base = 16;
        
		//pegaremos depois do x.
		p += 2;
		
		//atualiza o comprimeito.
		len -= 2;
    
	
	//Se o número começar com '0', então temos octal.
	
	}else if ( *p == '0' )
	      {
			base = 8;  
		  };
            
  
   
	while (len-- > 0) 
	{
        c = *p++;
        n *= base;
        
		//se o caractere for um dígito decimal.
		if (c >= '0' && c <= '9')
		{
            n += c - '0';
        
		}else{
			
            if (c >= 'A' && c <= 'Z') 
				c += 'a' - 'A';
            
			if (base == 16 && c >= 'a' && c <= 'f'){
				
	            n += c - 'a' + 10;
				
            }else if (len == 0 && c == 'l'){
				
	            ; //nothing
				
            }else{
	            //yyerror ("Invalid number in #if expression");
				printf ("Invalid number in #if expression");
	            return ERROR;
            }
        };
    };

    lexptr = p;
    yylval.lval = n;
    return INT;
};
*/



unsigned long parse_sizeof ( int token ){
	
	unsigned long Result = 0;
	
    int c = token;	
	
	if ( c != TOKENKEYWORD )
	{
		printf("parse_sizeof: fail");
		exit(1);
	}
 
    int State = 1;
	int running = 1;
	
	while (running)
	{
		c = yylex (); 
		
		again:
		
	    switch (State)
        {
		    //#todo
			//(	
		    case 1:
			    printf("parse_sizeof: State 1\n");
			    if ( c == TOKENSEPARATOR )
				{
					State = 2;
				    break;	
				}
				printf("parse_sizeof: State 1 fail");
		        exit(1);
				break;
			   
			//#todo   
		    //tipo, símbolo, etc ...	
		    case 2:
				printf("parse_sizeof: State 2\n");
				//#todo modifier 
				//if ...
			    if ( c == TOKENTYPE )
				{
					switch (type_found)
					{
						case TNULL:
						    Result = sizeof(0);
						    break;
							
						case TINT:
						    Result = sizeof(int);
							break;

						case TVOID:
						    Result = sizeof(void);
						    break;

						case TCHAR:
						    Result = sizeof(char);
						    break;

						case TSHORT:
						    Result = sizeof(short);
						    break;
							
						case TLONG:
						    Result = sizeof(long);
                            break;						

							
						default:
				        printf("parse_sizeof: State 2 unexpected type found in line %d", 
				           lineno);						
						   break;
					}
					State = 3;
					break;
				};
				    
				printf("parse_sizeof: #TODO State 2 unexpected element on sizeof in line %d", 
				    lineno);
		        exit(1);				              					
		        break;

		    //#todo
			//)	
		    case 3:
			    printf("parse_sizeof: State 3\n");
			    if ( c == TOKENSEPARATOR )
				{
					goto done;
					//State = 2;
				    break;	
				}
				printf("parse_sizeof: State 3 fail");
		        exit(1);		        
				goto done;
				break;

			default:
		        printf("parse_sizeof: default State");
		        exit(1);                
				break;			
		};		
	};
	
	
done:	
    return (unsigned long) Result;	
};



//analizando uma expressão.
//temos a questão de predecessores para os operadores.

unsigned long parse_expression ( int token ){

    //#todo tree

	int eCount = 0;
	
	//operator
	int Op;
	unsigned long sizeof_constant;
	int sizeof_flag = 0;
	
	// e1=x  e2=y
	unsigned long e[2];	
	unsigned long Result = 0;
	
	
	
	int c;
	
	c = token;
	
	
    
	//primeiro analizamos o que veio via argumento 
	//assim checaremos a validade da expressão e
	//abortaremos logo cedo.
	
	//uma expressão pode começar com (, operador, número ...
	//então vamos ver qual é o primeiro token da expressão.
	
	
    switch (c)
	{
		case TOKENSEPARATOR:
	        if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
			{
				
				
#ifdef PARSER_EXPRESSION_VERBOSE				
			    printf("parse_expression: TOKENSEPARATOR={%s} in line %d\n", 
				    real_token_buffer, lineno ); 
#endif 

				//State = 2;	
			}
		    break;
	    default:

#ifdef PARSER_EXPRESSION_VERBOSE		
            printf ("parse_expression: State 1 Missed '(' separator in line %d \n", 
			    lineno );
#endif			
			exit(1);
			break;					
	}	
	
	
	//#todo:
	//estamos suspendendo a parte de baixo e chamado uma rotina 
	//em tree para calcular a expressão.
	return (unsigned long) tree_eval ();
	
	//===========================================================
	
	/*
	int running = 1;
	int State = 1;
	
	//agora pegaremos os outros elementos da expressão.
	
	while (running == 1)
	{
	    c = yylex ();
		
		//voltando sem pegar.
		again:
		
		
		if( c == TOKENEOF )
		{
            printf("parse_expression: #error EOF \n");
            exit(1);			
		}		
	    
		switch (State)   
	    {
			
			//===================
			//State 1
			//Aceitamos constantes ou identificadores.
			//não vamos começar com operadores e sim com operandos,
			//que pdoem ser números ou identificadores.
			case 1:
			    switch(c)
				{
					
					case TOKENSEPARATOR:
	                    if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
						{
#ifdef PARSER_EXPRESSION_VERBOSE								
						    printf("parse_expression: TOKENSEPARATOR={%s} in line %d\n", 
							    real_token_buffer, lineno ); 
#endif							
							//State = 1;	
						    break;
						}
	                    if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
						{
							
#ifdef PARSER_EXPRESSION_VERBOSE								
						    printf("parse_expression: TOKENSEPARATOR={%s} in line %d\n", 
							    real_token_buffer, lineno );
#endif								
							//State = 1;
                            //provisório
							//retornaremos, mas falta o corpo {...};
							goto expression_exit;
							break;							
						}						
					    break;
					
					
					
					case TOKENCONSTANT:					
						//ok;
#ifdef PARSER_EXPRESSION_VERBOSE							
						printf ("parse_expression: State1 TOKENCONSTANT={%s} line %d\n", 
						    real_token_buffer, lineno );
#endif
						
						switch(eCount)
						{
							//primeiro elemento
							//esperamos uma constante
							case 0:
							
#ifdef PARSER_EXPRESSION_VERBOSE								
							    printf("eCount-case-0:\n");
#endif								
								//se estamos lidando com a contante gerada pelo sizeof.
								if( sizeof_flag == 1 )
								{
									e[0] = sizeof_constant;
									sizeof_flag = 0;
									
								}else{
									e[0] = (unsigned long) atoi (real_token_buffer);
								};								
								
								eCount++;
								//vamos para o tratador de operadores.
								State = 2;
								break;
								
							//segundo elemento	
							//error: o operador é tratado em outro case.
							case 1:
#ifdef PARSER_EXPRESSION_VERBOSE								
						        printf("eCount-case-1:\n");
								printf ("parse_expression: State1 error unexpected operator element TOKENCONSTANT={%s} line %d\n", 
						            real_token_buffer, lineno );
#endif                                
								exit(1); 									
							    break;
								
							//terceito elemento	
							//esperamos uma constante
							case 2:
#ifdef PARSER_EXPRESSION_VERBOSE								
							    printf("eCount-case-2:\n");
#endif							    
								//se estamos lidando com a contante gerada pelo sizeof.
								if( sizeof_flag == 1 )
								{
									e[1] = sizeof_constant;
									sizeof_flag = 0;
									
								}else{
									e[1] = (unsigned long) atoi (real_token_buffer);
								};
								eCount = 0;
								switch (Op)
								{
								    case PLUS_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] + (unsigned long) e[1] ); 									
								        break;
										
								    case MINUS_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] - (unsigned long) e[1] ); 									
								        break;

								    case BIT_AND_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] & (unsigned long) e[1] ); 									
								        break;

								    case BIT_IOR_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] | (unsigned long) e[1] ); 									
								        break;	

								    case MULT_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] * (unsigned long) e[1] ); 									
								        break;
										
								    case TRUNC_DIV_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] / (unsigned long) e[1] ); 									
								        break;
										
								    case TRUNC_MOD_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] % (unsigned long) e[1] ); 									
								        break;
										
								    case BIT_XOR_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] ^ (unsigned long) e[1] ); 									
								        break;
										
								    case LSHIFT_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] << (unsigned long) e[1] ); 									
								        break;
										
										
								    case RSHIFT_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] >> (unsigned long) e[1] ); 									
								        break;
										
								    case LE_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] <= (unsigned long) e[1] ); 									
								        break;
										
								    case GE_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] >= (unsigned long) e[1] ); 									
								        break;
										
									//...	
									
									default:
                                        printf("parse_expression: State1 default Op={%d}",Op);
										exit(1);             
										break;									
								};
								
								// ( 1 + 2 ')'
								//vamos ver se o que vem depois da segunda constante é um esperado separador 
								//ou outracoisa.
								c = yylex ();
								
								//Ok temos um sepador
								//vamos tratá-lo
								if ( c == TOKENSEPARATOR )
								{
									State = 1;
								    goto again;	
								}else{
									printf("expected separator in line %d", lineno);
									exit(1);
								}
								
								
								//vamos ver se tem um separador ou não
								//que finalize a expressão.
								//então continuaremos no State 1.
								//State = 1;							
							    break;
								
							default:
						        printf ("parse_expression: State1 error #default TOKENCONSTANT={%s} line %d\n", 
						            real_token_buffer, lineno );
								exit(1);
							    break;
						};
						break;
						
					case TOKENIDENTIFIER:
						printf ("parse_expression: State1 #todo TOKENIDENTIFIER={%s} line %d\n", 
						    real_token_buffer, lineno );					
					    //State = 2;
						exit(1);
						break;
						
					//sizeof	
					case TOKENKEYWORD:
					    if (keyword_found == KWSIZEOF)
						{
							
#ifdef PARSER_EXPRESSION_VERBOSE								
						   printf("parse_expression: State1 sizeof found in line %d\n", lineno); 	
#endif						   
						    //#bugbug
							//não sabemos se é a primeira ou a segunda constante.
							sizeof_constant = (unsigned long) parse_sizeof (TOKENKEYWORD);
							sizeof_flag = 1;
							
#ifdef PARSER_EXPRESSION_VERBOSE								
							printf("SIZEOF={%d}\n", sizeof_constant );
#endif 
							
							c = TOKENCONSTANT; //transformamos a keyword em constant.
							State = 1;
							goto again;
							//eCount++;
							
							//c = yylex();
							//if ( c == TOKENSEPARATOR )
							//{
							//	State = 1;
							//    goto again;
							//}
							
							//se o próximo não for um separador então temos mais operadores e 
							//constantes.
							//depois de um sizeof tem operadores ou separador.
							//vamos para o 2 porque esperamos por operador.
							State = 2;						   
						    break;
						    //exit(1);	
						};
					    break;	
					//...
					
					default:
					    //printf("parse_expression: default case in State 1");
						//exit(1); //die();
					    //vamos permitir operadores, indo para o state2, 
						//ele voltará par o state 1 se for um número ou identificador.
						State = 2;
						goto again;
						break;
				}
			   break;
			   
			//=================
			//State 2
            //operadores			
			case 2:
			    switch (c)
			    {
					//#importante
					//o lexer lida com alguns tokens usados por expressões 
					//usar esse tokens. 
					//lexer_code = o operador encontrado, que pode ser simples ou duplo. 
					
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
					
#ifdef PARSER_EXPRESSION_VERBOSE						
					    printf("parse_expression: State 2 simple operator{%s} lexer_code=%d \n", 
						    real_token_buffer, lexer_code );
#endif					    
						
						if ( eCount != 1 )
						{
							printf ("parse_expression: State 2 eCount error %d", eCount);
							exit(1);
						}
						//avançamos para pegarmos constantes.
						eCount++; 
						
						switch (lexer_code)
						{
                            case PLUS_EXPR:
							    Op = PLUS_EXPR;
                                break;
								
							case MINUS_EXPR:
							    Op = MINUS_EXPR;
								break;
								
                            case BIT_AND_EXPR:
							    Op = BIT_AND_EXPR;
								break;
								
                            case BIT_IOR_EXPR:
							    Op = BIT_IOR_EXPR;
								break;
								
                            case MULT_EXPR:
							    Op = MULT_EXPR;
								break;
								
                            case TRUNC_DIV_EXPR:
                                Op = TRUNC_DIV_EXPR;
								break;
								
							case TRUNC_MOD_EXPR:
                                Op = TRUNC_MOD_EXPR;
								break;
								
							case BIT_XOR_EXPR:
                                Op = BIT_XOR_EXPR;
								break;
								
							case LSHIFT_EXPR:
                                Op = LSHIFT_EXPR;
								break;
								
							case RSHIFT_EXPR:							
							    Op = RSHIFT_EXPR;
								break;
								
							//...	
								
							default:
                                printf ("parse_expression: State 2 default lexer code %d", lexer_code );
								exit (1); 
								break;							
							//...
						}
						//voltamos para pegarmos constantes.
						State = 1;
						break;		
					
					case ARITHCOMPARE:
					
#ifdef PARSER_EXPRESSION_VERBOSE	
					    printf("parse_expression: State 2 ARITHCOMPARE{%s} lexer_code=%d \n", 
						    real_token_buffer, lexer_code );
#endif					    
						if ( eCount != 1 )
						{
							printf ("parse_expression: State 2 eCount error %d", eCount);
							exit(1);
						}
						//avançamos para pegarmos constantes.
						eCount++; 						
						
						switch (lexer_code)
						{
							case LE_EXPR:
							    Op = LE_EXPR;

#ifdef PARSER_EXPRESSION_VERBOSE									
								printf("parse_expression: LE_EXPR lexer_code=%d \n", lexer_code );
#endif							    
								//State = 1;
								break;
								
							case GE_EXPR:
							    Op = GE_EXPR;
#ifdef PARSER_EXPRESSION_VERBOSE									
								printf("parse_expression: GE_EXPR lexer_code=%d \n", lexer_code );
#endif							    
								//State = 1;
								break;
	
								
							///...	
								
							default:
							    printf("parse_expression: State 2 error ARITHCOMPARE default lexer_code=%d in line %d \n", 
								    lexer_code, lineno );
							    exit(1); //die();
								//State = 1;
								break;
						};
						State = 1;
						break;
					   
				    
					case EQCOMPARE:
#ifdef PARSER_EXPRESSION_VERBOSE						
					    printf("parse_expression: State 2 EQCOMPARE{%s} lexer_code=%d \n", 
						    real_token_buffer, lexer_code );
#endif						
						switch (lexer_code)
						{
							
							case NE_EXPR:
#ifdef PARSER_EXPRESSION_VERBOSE								
							    printf("parse_expression: NE_EXPR lexer_code=%d \n", lexer_code );
#endif							    
								State = 1;
								break;
								
							case EQ_EXPR:
#ifdef PARSER_EXPRESSION_VERBOSE								
							    printf("parse_expression: EQ_EXPR lexer_code=%d \n", lexer_code );
#endif							    
								State = 1;
								break;							
							
							default:
							    printf("parse_expression: State 2 error EQCOMPARE default lexer_code=%d in line %d \n", 
								    lexer_code, lineno );
							    //exit(1); //die();
								State = 1;
								exit(1);
								break;
                        };						
					    break;
					   
					case ASSIGN:
#ifdef PARSER_EXPRESSION_VERBOSE						
                        printf("parse_expression: State 2 ASSIGN{%s} lexer_code=%d \n",  
						    real_token_buffer, lexer_code );
#endif						
						State = 1;
						break;					
					   
					case PLUSPLUS:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf("parse_expression: State 2 PLUSPLUS{%s} lexer_code=%d \n",  
					       real_token_buffer, lexer_code );
#endif					   
					   State = 1;
					   break;
					   
					case MINUSMINUS:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf("parse_expression: State 2 MINUSMINUS{%s} lexer_code=%d \n", 
					       real_token_buffer, lexer_code );
#endif					   
					   State = 1;
					   break;
					   
					case ANDAND:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf("parse_expression: State 2 ANDAND{%s} lexer_code=%d \n",  
					       real_token_buffer, lexer_code );
#endif					   
					   State = 1;
					   break;
					   
					case OROR:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf("parse_expression: State 2 OROR{%s} lexer_code=%d \n",  
					       real_token_buffer, lexer_code );
#endif					   
					   State = 1;
					   break;
					   
					   
					case POINTSAT:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf("parse_expression: State 2 POINTSAT{%s} lexer_code=%d \n", 
					       real_token_buffer, lexer_code );
#endif
					   State = 1;
					   break;
					   
					case LSHIFT:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf("parse_expression: State 2 LSHIFT{%s} lexer_code=%d \n", 
					       real_token_buffer, lexer_code );
#endif
					   State = 1;
					   break;
					   
					   
					case RSHIFT:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf ("parse_expression: State 2 RSHIFT{%s} lexer_code=%d \n", 
					       real_token_buffer, lexer_code );
#endif					   
					   State = 1;
					   break;
					   
					//...
					
					default:
					    printf("parse_expression: default case in State 2 {%s} lexer_code=%d \n",real_token_buffer, lexer_code );
						State = 1;
						goto again;
					    break;
				};
           	    break;		 
            
            //...
			
			//State default
            default:	
                printf("parse_expression: default State {%s} lexer_code=%d \n",real_token_buffer, lexer_code );
                exit(1); //die();				
		        break;
	    };
	};
	
	*/
	
expression_exit:
    
	return (unsigned long) Result;
};



int parse_asm ( int token )
{
	
	int c;
	
	int running = 1;

	int State = 1;	
	
#ifdef PARSER_ASM_VERBOSE	
	//debug
	printf("parse_asm: Initializing ...\n");	
#endif	

	//se entramos errado.
	if ( token != TOKENKEYWORD)
	{
		printf("parse_asm: token error\n");
		exit(1);
	}
	
    if ( token == TOKENKEYWORD )	
	{	
#ifdef PARSER_ASM_VERBOSE	
		printf("parse_asm: TOKENKEYWORD={%s} in line %d\n", 
		    real_token_buffer, lineno );  
#endif		
	};

	
	int inside = 0;
	
	//
	// (
	//
	
	c = yylex ();
	
	if ( c == TOKENSEPARATOR )
	{
	    if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
        {
			
#ifdef PARSER_ASM_VERBOSE				
			printf("parse_asm: TOKENKEYWORD={%s} in line %d\n", 
			    real_token_buffer, lineno ); 
#endif			
			//ok
			inside = 1;
			
		} 
		
	}else{
		//fail
		printf("parse_asm: expected (");
		exit(1);
	}
	
	//
	// " .... "
	//
	
	c = yylex ();
	
	if ( c == TOKENSTRING )
	{
	    //if ( strncmp( (char *) real_token_buffer, "\"", 1 ) == 0  )
        //{
			//ok
			//inside = 1;
		//} 
		
		//coloca a string no arquivo de saída.
		strcat( outfile, real_token_buffer );
		
			//ao fim da string vamos para a próxima linha do output file
		strcat( outfile,"\n");		
		
		c = yylex ();
		
		
			//)
		    if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
			{
				inside = 0;
				
				c = yylex ();
				
				//;
				if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
				{
					//ok
					return c;
				}
			    printf("parse_asm: expected ; in asm string");
			    exit(1);	
			}
			
			printf("parse_asm: expected ) in asm string");
			exit(1);	
			
		
		
	}else{
		//fail
		printf("parse_asm: expected string in asm("") ");
		exit(1);
	}
	

    printf("parse_asm: todo unexpected error in asm string");
	exit(1);		
		   
	return 0;
};


int parse_function ( int token )
{
	int c;
	
	int running = 1;
	int State = 1;
	
#ifdef PARSER_FUNCTION_VERBOSE	
	//debug
	printf("parse_function: Initializing ...\n");	
#endif	
	
	//se entramos errado.
	if ( token != TOKENIDENTIFIER )
	{
		printf("parse_function: Can't initialize function statement\n");
		exit(1);
	}
	
    if ( token == TOKENIDENTIFIER )	
	{
	
		id[ID_TOKEN] = TOKENIDENTIFIER;
		id[ID_STACK_OFFSET] = stack_index;
		
#ifdef PARSER_FUNCTION_VERBOSE			
		printf ("parse_function: TOKENIDENTIFIER={%s} in line %d\n", 
		    real_token_buffer, lineno );    			
#endif	

	};

	while (running)
	{
		c = yylex ();
		
		switch ( State )
		{
			//state1 
			//esperamos um separador '('
			//pois isso é uma função e não uma definação de variável.
			case 1:
			    switch (c)
				{
					case TOKENSEPARATOR:
	                    if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
						{
#ifdef PARSER_FUNCTION_VERBOSE							
						    printf ("parse_function: TOKENSEPARATOR={%s} in line %d\n", 
							    real_token_buffer, lineno ); 
#endif							
							State = 2;	
						}
					    break;
						
                    default:
                        printf("parse_function: State1 Missed '(' separator in line %d ", lineno );
						exit(1);
						break;					
				}
			    break;
				
			//esperamos aqui tipos, simbolos , separador ',' ou o separador ')'	
			case 2:
			    switch (c)
				{
					//')'
					case TOKENSEPARATOR:
	                    if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
						{
#ifdef PARSER_FUNCTION_VERBOSE							
						    printf ("parse_function: TOKENSEPARATOR={%s} in line %d\n", 
							    real_token_buffer, lineno ); 
#endif							
							State = 3;	
						}
					    break;
						
					//case type 
					//case symbol 
					//case ','
					//...
					
                    default:
                        printf("parse_function: State2 something wrong in line %d ", lineno );
						exit(1);
						break;					
				}
			    break;
				
			//esperamos aqui o separador final ';'
            //isso finaliza o statement 'function'			
			case 3:
			    switch (c)
				{
					//';'
					//terminamos o statement function.
					case TOKENSEPARATOR:
	                    if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
						{
#ifdef PARSER_FUNCTION_VERBOSE							
						    printf ("parse_function: TOKENSEPARATOR={%s} in line %d\n", 
							    real_token_buffer, lineno ); 
#endif							
							return (int) TOKENSEPARATOR;	
						}
					    break;
						
                    default:
                        printf("parse_function: State3 Expected separator ';' in line %d ", lineno );
						exit(1);
						break;										
				}
			    break;
			
			default:
				printf("parse_function: default statement\n");
				exit(1);				    
			    break;
		};
	};	
};

// #return statement
// >> termina com  ';'
// return 0;
// return (int) 1;
// return 1+2;
// return (1+2);
// return (int) (1+2);
// return function();
// return (int) function();
//interna 
int parse_return ( int token ){
	
	
	int c;
	
	int running = 1;
	int State = 1;
	
	int open = 0;
	

#ifdef PARSER_RETURN_VERBOSE
	//debug
	printf("parse_return: Initializing ...\n");	
#endif	
	
	//se entramos errado.
	if ( token != TOKENKEYWORD || keyword_found != KWRETURN )
	{
		printf("parse_return: Can't initialize return statement\n");
		exit(1);
	}
	
	unsigned long eval_ret;
	//char buffer[20];
	
	   
	eval_ret = (unsigned long) tree_eval ();
	
	//char *buffer;
	char buffer[32];
	
	//buffer = (char *) itoa ( (int) eval_ret);
	
	itoa ( (int) eval_ret, buffer );
	
	strcat( outfile,"  mov eax, ");
	strcat( outfile, buffer );
	strcat( outfile,"\n  ret \n\n");

	//o ultimo token em um return statement foi ';'
	//vamos conferir
    if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
    {
		//printf("; OK ");
		c = TOKENSEPARATOR;
		return c;
	}	

	//#debug
	printf("parse_return: debug *hang");
	while(1){}
	
	//#obs 
	//supendemos todo o resto abaixo por enquanto.
	
    /*	
	while (running)
	{
		c = yylex ();
		
		switch ( State )
		{
			
			//#state1
		    //esperamos constante, expressão, tipo ou função.
			case 1:
			    switch(c)
				{
					//se encontramos a constante, o que segue é o separador ';' ou o separador ')'
					case TOKENCONSTANT:					
#ifdef PARSER_VERBOSE							
						//ok;
						printf ("parse_return: State1 TOKENCONSTANT={%s} line %d\n", 
						    real_token_buffer, lineno );
#endif

					    constant[CONSTANT_TOKEN] = TOKENCONSTANT;
						constant[CONSTANT_TYPE] = constant_type_found;
						constant[CONSTANT_BASE] = constant_base_found;

						//#todo: fazer um switch para tipos válidos.
						strcat( outfile,"  mov eax, ");
						strcat( outfile, real_token_buffer );
						strcat( outfile,"\n  ret \n\n");
                        //strcat( outfile,"");							
						
						// se parênteses aberto.
						//if ( open == 1 )
						//{
						//    struct tree_node *n = (struct tree_node *) create_tree_node ( ?, NULL, NULL );	
						//}
						
						//Se não temos um parêntese aberto então vamos para o próximo
						//que deverá ser um ';'
						
						if ( open == 0 )
						{
						    State = 2;	
						}
						
						//mas se ainda temos um parentese aberto então
						//encontramos uma constante dentro do parêntese,
						//indicando que estamos um uma expressão.
						break;
					
					
                     
					case TOKENSEPARATOR:
	                    
						//iniciamos uma expressão ou condicional.
						//uma árvore.
						if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
                        {
						    open = 1;
						}
						
						//se fecharmos, o que segue pode ser um separador ';',
						//uma função, uma constante ou uma expressão.
	                    if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
						{
						    open = 0;							
						}
						
						//se o separador for ';' é porque estamos num retorno do tipo void.
						//se o retorno não for do tipo void então foi erro de sintaxe..
						if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
						{
							
#ifdef PARSER_RETURN_VERBOSE							
							printf("parse_return: separator ';' in line %d\n", 
							    lineno );
#endif							
							
						    if (open == 1)
                            {
								printf("parse_return: State1 wrong separator ';' in line %d\n", lineno);
								exit(1);
							}
							
                            //retorno do tipo void.
							if (open == 0)
                            {
								strcat( outfile,"\n  ret \n\n");
								return (int) TOKENSEPARATOR;
                            }								
						}
						break;
					
				    //case identificador. (símbolo)
					//significa que o return foi seguido de uma função.
					case TOKENIDENTIFIER:
					    //#todo Nesse momento podemos chamar a rotian qu trata uma função 
						//function statement. function_parser
						
						// ';' foi encontrado.
						//finalizado o statement de função então vamos sair do statemente de return.
						//pois o ';' da função é o mesmo do return.
						//se estivermos após a keyword 'return' ou após o tipo '(int)'.
						if ( open == 0 )
						{
                            //vamos analizar uma chamada de função dentro de um statement de return.							
						    return (int) parse_function ( TOKENIDENTIFIER );
						}
						
						//#importante
						//temos um identificador dentro do parênteses.
						//Se uma função foi chamada dentro do parênteses não devemos esperar 
						//por um separador ';'
						if ( open == 1 )
						{
						    //todo	
						}
						break;		
						
					default:
				        printf("parse_return: State1 default\n");
				        exit(1);
					    break;
				}
                break;

			
			//#state2
			// separador ';'
			case 2:
			    switch(c)
				{
					case TOKENSEPARATOR:
	                    if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
                        {
#ifdef PARSER_RETURN_VERBOSE							
			                //deu certo.
			                printf ("parse_return: State2 do_separator TOKENSEPARATOR={%s} line %d \n", 
							    real_token_buffer, lineno );
#endif		                    
							//Se encontramos o separador que finaliza o statement
							//então podemos retornar.
							return (int) TOKENSEPARATOR;
		                }	
					    break;
						
					default:
				        printf("parse_return: State2 default\n");
				        exit(1);
					    break;
				}
			    break;
				
			//Statemente não enumerado.
			default:
				printf("parse_return: default statement\n");
				exit(1);			
			    break;
		};
	};
	
	//printf("parse_var:\n");
	
	//pegaremos o tipo e a exp.
	
do_constant:

	c = yylex ();
	

	if ( c == TOKENCONSTANT)
	{
		
#ifdef PARSER_RETURN_VERBOSE		
	    printf ("parse_return: do_constant TOKENCONSTANT={%s} line %d\n", 
		    real_token_buffer, lineno );	
#endif
		constant[CONSTANT_TOKEN] = TOKENCONSTANT;
		constant[CONSTANT_TYPE] = constant_type_found;
		constant[CONSTANT_BASE] = constant_base_found;	
	
	} else {
			//falhou.
			printf("parse_return: do_constant Expacted constant on line %d", lineno);
			exit(1);
			//while(1){}		
	};
	
	
	
do_separator:

    c = yylex ();
    
	//separador ';'
	//isso finaliza o statement.
	if( c == TOKENSEPARATOR )
    {
	    if( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
        {

#ifdef PARSER_RETURN_VERBOSE
			//deu certo.
			printf("parse_return: do_separator TOKENSEPARATOR={%s} line %d \n", 
			    real_token_buffer, lineno );
#endif		  
		  
			goto done;
		}else{
			//falhou.
			printf("parse_return: do_separator: fail");
			exit(1);
			//while(1){}
		};	
		
	}else{
			//falhou.
			printf("parse_return: do_separator Expected ';' on line %d",lineno);
			exit(1);
			//while(1){}		
	};

    */	
	
done:		
   return c;
};


//## if ##
int parse_if (int token){
	
	int If_Result = -1;
	
	unsigned long Exp_Result = 0;
	
	printf("todo: parse_if in line %d \n", lineno );
	
	//#todo
	//conferir se o token do argumento é um if 
	
	//#todo 
	//temos que criar State para if.
	
	//pega o próximo que deve ser um (
	int c = yylex();
	
	
	if ( c != TOKENSEPARATOR )
	{
		printf ("parse_if separator missed\n");
		exit(1);
	}
	//testando chamar uma análise de expressão dentro do statement de if.
	Exp_Result = parse_expression ( c );
	
	//#importante 
	//retornamos 1 ou 0 da análise da expressão,
	//quem chamou o if vai armazenar esse valor para 
	//chamar o else.
	If_Result = (int) Exp_Result;
	
	printf ("EXP={%d}\n",Exp_Result);
	
	c = yylex();
	if ( c != TOKENSEPARATOR )
	{
		printf ("parse_if separator { missed\n");
		exit(1);
	}
	
	c = yylex();
	if ( c != TOKENSEPARATOR )
	{
	printf ("parse_if separator } missed\n");
		exit(1);
	}
	
	c = yylex();
	if ( c != TOKENSEPARATOR )
	{
		printf ("parse_if separator ; missed\n");
		exit(1);
	}
	

	
	//exit(1);
    return (int) If_Result;	
};



int parse_while (int token){
	
	int While_Result = -1;
	unsigned long Exp_Result = 0;
	
	printf("todo: parse_while in line %d\n ", lineno );
	
	//#todo
	//conferir se o token do argumento é um while

	//pega o próximo que deve ser um (
	int c = yylex();

	if ( c != TOKENSEPARATOR )
	{
		printf ("parse_while separator missed\n");
		exit(1);
	}
	
	//testando chamar uma análise de expressão dentro do statement de while.
	Exp_Result = parse_expression ( c );
	While_Result = (int) Exp_Result;
	
	c = yylex();
	if ( c != TOKENSEPARATOR )
	{
		printf ("parse_while separator { missed\n");
		exit(1);
	}
	
	c = yylex();
	if ( c != TOKENSEPARATOR )
	{
	printf ("parse_while separator } missed\n");
		exit(1);
	}
	
	c = yylex();
	if ( c != TOKENSEPARATOR )
	{
		printf ("parse_while separator ; missed\n");
		exit(1);
	}
	
	printf ("EXP={%d}\n",Exp_Result);
	//exit(1);
    return (int) While_Result;	
};

int parse_do ( int token )
{
	printf("todo: parse_do in line %d\n ", lineno );
	exit(1);
    return -1;	
};

int parse_for ( int token )
{
	printf("todo: parse_while in line %d \n", lineno );
	exit(1);
    return -1;	
};


/*
 ************************************************************************
 * parse:
 *     Função principal.
 *     Pegando tokens com o lexer e fazendo coisas ...
 */
 
int parse (){
	
	register int c;
	int i;
	
	char save_symbol[32];
	
	int running = 1;
	size_t size = (size_t) strlen ( (const char *) stdin->_base );
	
	
	//{([
	//se entramos em um desses corpos.
	int braces_inside = 0;
	int parentheses_inside = 0;
	int square_brackets_inside = 0;
	
	
	int If_Result = -1;
	int While_Result = -1;
	//...
	
	//steps;
	int State = 1;
	
    printf("\n");
	
#ifdef GRAMCC_VERBOSE	
    printf("parse: Initializing ...\n");
#endif		
	
	
	//#obs
	//Aqui podemos usar um while(running) até que se encontre o fim do arquivo.

	while (running == 1)
	{
	    //pega o char.
		//#todo: trocar c por TOKEN.
		
	    c = yylex ();
		
		again:
		
		//#debug
	    //printf("%c", c );
		
		
        //#importante		
        //Estamos começando um arquivo. No começo do arquivo 
		//apenas alguns tokens são válidos, são eles:
        // >>'#' do preprocessador
        // >>modificadores 
        // >>qualificadores 
        // >>tipos   		
		
		switch (State)
		{
            //
			//  ## MODIFIER,  TYPE and SEPARATOR ##
			//
						
			//################################
			// #State 1
			// Esperamos um MODIFIER,  TYPE and SEPARATOR
			//
			case 1:
#ifdef PARSER_VERBOSE						
			    printf("<1> ");
#endif
				switch (c)
				{
	                case TOKENMODIFIER:
					
#ifdef PARSER_VERBOSE							
						//continua pois precisamos pegar um tipo.
						//#bugbug ??mas e se o modificar vir seguido de um simbolo ???
						printf("State1: TOKENMODIFIER={%s} line %d\n", 
						    real_token_buffer, lineno );
#endif
						
						State = 1;
						//goto again;
						break;
					
					//TYPE
					// >>> peekChar=) significa marcação de tipagem.
					// >>> peekSymbol=symbol  significa declaração de variável ou função.
			        case TOKENTYPE:
					
#ifdef PARSER_VERBOSE						
			            printf("State1: TOKENTYPE={%s} line %d\n", real_token_buffer, lineno );
#endif						
						
						id[ID_TYPE] = type_found;
				        
						//depois de um type vem um identificador.
						State = 2;
			            break;
						
						
					// #bugbug	
					// e se o arquivo começar com um separador, então teremos problema.	
					
					case TOKENSEPARATOR:
					
#ifdef PARSER_VERBOSE						
					     printf("State1: TOKENSEPARATOR={%s} line %d\n", real_token_buffer, lineno );
#endif						 

					
						//; função
						if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
                        {
							
#ifdef PARSER_VERBOSE								
						    printf ("State2: SEP={%s} line %d\n", 
							    real_token_buffer, lineno );
#endif 
								
								//incrementamos
								//pois podemos estar no primeiro, no segundo etc ...
							parentheses_inside++;
							
#ifdef PARSER_VERBOSE										
							printf ("[PAR] line %d\n", lineno );  //debug par open
#endif

								//#test
								//peekChar = c;
								
						        //tentando mandar alguma coisa para o arquivo de output 
						        //pra ter o que salvar, pra construir o assembly file;	
						       // strcat( outfile,"\n segment .text \n");
						    strcat( outfile,"_");
						    strcat( outfile,save_symbol);
						    strcat( outfile,":\n");
								
								//recomeçar a lista. 
								//#bugbug desconsiderando o modificador.
							State = 1;	
							break;
						};							
						
						//entramos no corpo da função.
						if ( strncmp( (char *) real_token_buffer, "{", 1 ) == 0  )
						{
							
#ifdef PARSER_VERBOSE								
							printf ("State1: separator={%s} line %d\n", 
							    real_token_buffer, lineno );  
#endif						
						
							braces_inside++;
							
//#ifdef PARSER_VERBOSE										
						    printf ("[BRACE] line %d\n", lineno);  //debug par close
//#endif									
									
									//isso vai para o 1 onde procura-se por modificadores e tipos,
									//mas se estivermos com o corpo da função aberto ele avançará para o próximo state.
							State = 1;
							break;
						};

					    //fechando UM parênteses, provavelmente sem nada dentro.
					    if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
						{
						    if ( parentheses_inside > 0 )
							{
#ifdef PARSER_VERBOSE											
								printf("[/PAR] line %d\n", lineno);
#endif                                
								parentheses_inside--;
                                State = 1;
								break;
								//State++;
                                //goto again; 								
							};
						};
						
                        //fechando UM corpo de função. 					
				        if ( strncmp( (char *) real_token_buffer, "}", 1 ) == 0  )
                        {
						    if ( braces_inside > 0 )
							{
//#ifdef PARSER_VERBOSE											
								printf("[/BRACE] line %d\n", lineno);
//#endif                                
								
								braces_inside--;
                                State = 1;
								break;							
							};							
						};
						
						if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
						{
							printf("; separator found!\n");
							State = 1;
						}
					    break;




                    //#bugbug						
					//Não devemos aceitar diretivas do preprocessador nesse momento,
					//pois antes de tudo devemos rodar um preprocessador para expandir as 
					//diretivas.
					
                    //Se o corpo da função estiver aberto podemos ter identificadores sem tipo,
					//que são label: ou chamada de função (call). ou ainda keywords.
					
					//significa que dentro do um corpo de função não encontramos um tipo,
					//então vamos ver do que se trata o token e configurar a variável c.
					//loga após vamos reiniciar o running mas sem pegarmos o próximo token.
					//poderíamos realizar um ungetchar, mas não testamos isso direito.
					
					//Pode ser que não encontramos um token válido no início do documento.
	
					default:
					    //se estamos dentro do parênteses e não encontramos nenhum case acima.
					    if ( parentheses_inside > 0 )
					    {
						    State++;
						    goto again;
					    }	
					    //se estamos dentro de uma chave e não encontramos nenhum case acima.
				        if ( braces_inside > 0 )
					    {
						    State++;
						    goto again;
					    }
						//EOF 
						if( c == TOKENEOF )
						{
						    printf("State1: eof\n");
                            goto debug_output; 							
						}
					    //printf("State1: default error\n");
						printf("State1: default. MODIFIER,  TYPE or SEPARATOR expected on line %d \n", 
						    lineno);
						printf(">>>token={%d}",c);
						exit(1);
                        break;
						
				}
		        break;
				
				
            //
			//  ## IDENTIFIER ##
			//				
				
			//################################
			// #State 2	
			// Esperamos um identificador, pos estamos logo após um tipo.
			// Pode ser uma função ou uma declaração de variável, isso depende do peekChar.
			case 2:
#ifdef PARSER_VERBOSE						
			    printf("<2> ");
#endif
			    switch (c)
				{
				    //identificador. (símbolo)
					//peekChar=: significa que o identificador é uma label. acontece no case.
					//peekChar=( estamos chamando uma função
					//peekChar=; estamos finalizando um goto ou um return.
					//peekChar=, estamos listando variáveis.
					//
					case TOKENIDENTIFIER:
			            
#ifdef PARSER_VERBOSE							
				        printf("State2: TOKENIDENTIFIER={%s} line %d\n", real_token_buffer, lineno );    
#endif 
						
						id[ID_TOKEN] = TOKENIDENTIFIER;
				        id[ID_STACK_OFFSET] = stack_index;
						
                        //salva o símbolo. #isso funciona.						
						sprintf ( save_symbol, real_token_buffer );
						
						//?? não sabemos se real_token_buffer é código ou dados ?? 
						
						
                        c = yylex ();
						
						//printf("test={%s} line %d\n", real_token_buffer, lineno ); 
						
						//: para label 
                        //( para função 
                        //; para declaração de variável.
                        //, para sequência de variável.
                        // = para atribuição de valor à uma variável.
						// vários operadores se estivermos em uma expressão == <= ...
						//... 						
						if ( c == TOKENSEPARATOR )
						{
							//printf("sep \n");
							
						    //: label
							if( strncmp ( (char *) real_token_buffer, ":", 1 ) == 0  )
                            {
								
#ifdef PARSER_VERBOSE									
						        printf ("State2: SEP={%s} line %d\n", 
								    real_token_buffer, lineno );
#endif 

								//tentando mandar alguma coisa para o arquivo de output 
						        //pra ter o que salvar, pra construir o assembly file;	
						       // strcat( outfile,"\n segment .text \n");
						        strcat( outfile,"_");
						        strcat( outfile,save_symbol);
						        strcat( outfile,":\n");
								
								//recomeçar.
								State = 1;
								break;
							}
							
						    //; função
							if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
                            {
								
#ifdef PARSER_VERBOSE										
								printf ("State2: SEP={%s} line %d\n", 
								    real_token_buffer, lineno );
#endif								
								//incrementamos
								//pois podemos estar no primeiro, no segundo etc ...
								parentheses_inside++;
								
#ifdef PARSER_VERBOSE											
								printf ("[PAR] line %d\n", lineno );  //debug par open
#endif								
								
								//#test
								//peekChar = c;
								
						        //tentando mandar alguma coisa para o arquivo de output 
						        //pra ter o que salvar, pra construir o assembly file;	
						       // strcat( outfile,"\n segment .text \n");
						        strcat( outfile,"_");
						        strcat( outfile,save_symbol);
						        strcat( outfile,":\n");
								
								//recomeçar a lista. 
								//#bugbug desconsiderando o modificador.
								State = 1;
								
								break;
							}	

						    //Se encontramos um separador ')' 
						    //entao esperaremos um separador '{'.
						    if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
					 	    {
#ifdef PARSER_VERBOSE										
								printf ("State2: SEP={%s} line %d\n", 
								    real_token_buffer, lineno );
#endif								
								//se não tem parênteses aberto.
								if ( parentheses_inside < 1 )
								{
									printf("state2: Error trying to close a not opened parentheses in line %d ", 
									  lineno );
									exit(1);
								}
								
								//fechamos um dos parênteses.
								parentheses_inside--;
								
#ifdef PARSER_VERBOSE											
								printf ("[/PAR] line %d\n", lineno);  //debug par close
#endif								
								
							    //é bss porque não foi inicializada.
								//strcat( BSS,"\n segment .bss \n");
						        strcat( BSS,"_");
						        strcat( BSS,save_symbol);
						        strcat( BSS,": dd 0 \n");								
						        
								//?? depois de ) podemos ter o corpo da função.
								//ou outra coisa caso estivermos parentese aberto.
								
								//peek next
								c = yylex ();
								
								//entramos no corpo da função.
								if ( strncmp( (char *) real_token_buffer, "{", 1 ) == 0  )
								{
									
#ifdef PARSER_VERBOSE										
									printf ("State2: separator={%s} line %d\n", 
									    real_token_buffer, lineno );  
#endif	
								 
									braces_inside++;
#ifdef PARSER_VERBOSE												
								    printf ("\n[BRACE] line %d\n", lineno);  //debug par close
#endif									
									
									//isso vai para o 1 onde procura-se por modificadores e tipos,
									//mas se estivermos com o corpo da função aberto ele avançará para o próximo state.
									State = 1;
									break;
								}	
									
							    break;
						    }									

						    //; var
							if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
                            {
#ifdef PARSER_VERBOSE									
						        printf ("State2: SEP={%s} line %d\n", 
								    real_token_buffer, lineno );
#endif								
								
								//tentando mandar alguma coisa para o arquivo de output 
						        //pra ter o que salvar, pra construir o assembly file;	
						        
								//é bss porque não foi inicializada.
								//strcat( BSS,"\n segment .bss \n");
						        strcat( BSS,"_");
						        strcat( BSS,save_symbol);
						        strcat( BSS,": dd 0 \n");
								
								//recomeçar, vai que tem mais variável...
								State = 1;
								break;
							}
							
							
						    //, var (listando) tirando da pilha
							if ( strncmp( (char *) real_token_buffer, ",", 1 ) == 0  )
                            {
#ifdef PARSER_VERBOSE									
						        printf ("State2: SEP={%s} line %d\n", 
								    real_token_buffer, lineno );
#endif								
								//tentando mandar alguma coisa para o arquivo de output 
						        //pra ter o que salvar, pra construir o assembly file;	
						        
								//é bss porque não foi inicializada.
								//strcat( BSS,"\n segment .bss \n");
						        strcat( BSS,"_");
						        strcat( BSS,save_symbol);
						        strcat( BSS,": dd 0 \n");
								
								//recomeçar, vai que tem mais variável...
								State = 1;
								break;
							}							
							
							//printf("...");
							//goto debug_output;
							
						}//else{
							//printf("sep fail");
						//}
						
						printf("state2: TOKENIDENTIFIER fail");
						exit(1);
						//tentando mandar alguma coisa para o arquivo de output 
						//pra ter o que salvar, pra construir o assembly file;	
						//strcat( outfile,"\n");
						//strcat( outfile,"_");
						//strcat( outfile,real_token_buffer);
						//strcat( outfile,":\n");
						
						//depois do identificador deve vir um '(' ou um '=';
						//State = 3;
						break;	

                    //Se o corpo da função estiver aberto podemos ter identificadores sem tipo,
					//que são label: ou chamada de função (call). ou ainda keywords.
					
					//significa que dentro do um corpo de função não encontramos um tipo,
					//então vamos ver do que se trata o token e configurar a variável c.
					//loga após vamos reiniciar o running mas sem pegarmos o próximo token.
					//poderíamos realizar um ungetchar, mas não testamos isso direito.
					

 				
                
				    default:
					    //se estamos dentro do parênteses e não encontramos nenhum case acima.
					    if ( parentheses_inside > 0 )
					    {
						    State++;
						    goto again;
					    }	
					    //se estamos dentro de uma chave e não encontramos nenhum case acima.
				        if ( braces_inside > 0 )
					    {
						    State++;
						    goto again;
					    }						
					    //printf("State2: default Error.\n");
						printf("State2: default. expected identifier on line %d.\n", lineno);
						exit(1);
					    break;
				};
                break;	


            //
			//  ## KEYWORD ##
			//			
            
            case 3:
#ifdef PARSER_VERBOSE				
			    printf("<3> ");
#endif
			    switch(c)
				{
					//KEYWORD.
                    //peekChar=; depois do break, ou continue.(obrigatório)
                    //peekChar=: depois do default.(obrigatório)
                    //peekChar=( depois do switch, if, while ...					
				    case TOKENKEYWORD:
					
#ifdef PARSER_VERBOSE						
					    printf("line %d TOKENKEYWORD={%s} \n", lineno, real_token_buffer );
#endif 
						
						// # return #
						//return. Chamaremos o tratador do stmt parse_return() 
				        if( keyword_found == KWRETURN )
				        {
							
#ifdef PARSER_VERBOSE								
					        printf("State3: TOKENKEYWORD={%s} KWRETURN line %d  \n", 
							    real_token_buffer, lineno );
#endif					        
							// parse deve retornar o '(', quando encontrá-lo faz o tratamento até chegar  no ';'
						    // a função deve retornar o separador ';'
				            //indicando que tudo deu certo no parser.
						    //continuaremos nesse state até pegarmos o separador '}'.
							//printf ("\n");
					        c = parse_return ( TOKENKEYWORD );
					        //printf ("\n");
							
							//esperávamos o separador ';', se não veio então falhou o parser.
							if ( c != TOKENSEPARATOR )
					        {
							    printf("State3: TOKENKEYWORD TOKENSEPARATOR fail");
                                exit(1);								
					        }
							
							//reinicia
							//porque depois de um return podemos ter várias outras coisas.
							//inclusive apenas terminarmos um corpo.
							State = 1;
							break;
				        }

                        if( keyword_found == KWGOTO )
						{
#ifdef PARSER_VERBOSE								
					        printf("State3: TOKENKEYWORD={%s} KWGOTO in line %d \n", 
							    real_token_buffer, lineno );
#endif								
						    //parse_goto??
							//Esperamos um identificador logo após o goto.
							State = 2;
							break;
						}
						
                        if( keyword_found == KWIF )
						{
#ifdef PARSER_VERBOSE								
					        printf("State3: TOKENKEYWORD={%s} KWIF in line %d \n", 
							    real_token_buffer, lineno );
#endif								
						    //parse_if??
							If_Result = (int) parse_if  (TOKENKEYWORD);
							
#ifdef PARSER_VERBOSE								
							printf("IF-RESULT={%d}\n",If_Result);
#endif							
							
							//recomeçamos
							State = 1;
							break;
						}
						
                        if( keyword_found == KWWHILE )
						{
						    While_Result = (int) parse_while (TOKENKEYWORD);
#ifdef PARSER_VERBOSE								
							printf("WHILE-RESULT={%d}\n",While_Result);
#endif	
						
							State = 1;
							break;
						}	
						
						// asm (" ... ");
                        if( keyword_found == KWASM )
						{
#ifdef PARSER_VERBOSE								
					        printf ("State3: TOKENKEYWORD={%s} KWASM in line %d \n", 
							    real_token_buffer, lineno );
#endif								
						     
							parse_asm (TOKENKEYWORD);
							//recomeçamos
							State = 1;
							break;
						}	

						 

                        //...							
						
						break;
						
					
					//estamos dentro do corpo da função e não encontramos uma keyword.	
					default:
					    if ( parentheses_inside > 0 )
					    {
						    printf("State3: bugbug searching for keyword inside parentheses");
						    exit(1);
					    }
					    if ( braces_inside > 0 )
					    {
						    printf("State3: bugbug searching for keyword inside braces");
						    exit(1);
					    }
						//??em que momento espera-se por uma keyword e não encontra ??
					    printf("State3: default. keyword expected in line %d",lineno);
						exit(1);
						break;
				};
                break;	

            //###################################
            //state4 ';'			
			case 4:
#ifdef PARSER_VERBOSE						
			    printf("<4> ");
#endif
				switch (c)
				{
		            case TOKENSEPARATOR:
				        // ';'
                        if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )			 
      	                {
				            //ok #todo
			            }else{
					        printf(" State4: expected ; in line %d", lineno);
					        exit(1);
				        };	
					    break;
					
				        default:
					        printf(" State4: default. expected ; in line %d", lineno);
					        exit(1);				
				            break;
				};	
			    break;
				
		    default:
#ifdef PARSER_VERBOSE				
			    printf("<default>State default: Error.\n");
#endif				
				if ( parentheses_inside > 0 )
				{
				    printf("default: expected ) in line %d \n", lineno);
                }
				if ( braces_inside > 0 )
				{
				    printf("default: expected } in line %d \n", lineno);
			    }				
				goto debug_output;
				//exit(1);
                break;				
 
	




	    };//switch State	
		
				
		size--;
		
		//sai do while
        if (size <= 0)
		{
			printf("parser: End of size cont\n");
            running = 0;
        }			
	};
	
	//
	// Saimos do while running.
	//
	
	//#debug
	//printf("\n INPUT: \n");
	//printf("%s\n",stdin->_base);
	//printf("number of lines: %d \n",lineno);
	//...
debug_output:

    //incluimos no arquivo de output os segmentos de dados,
	strcat( outfile,DATA);
    strcat( outfile,BSS);
	
#ifdef PARSER_OUTPUT_VERBOSE	
	//exibimos o arquivo de output.
	printf("\n OUTPUT: \n");
	printf("%s\n",outfile);
	printf("number of lines: %d \n",lineno);
#endif	

    goto parse_exit;	
	
hang:	

	printf("parse: *hang");
	//goto done;    
		while (1){
			
			asm ("pause");
		}
		
		
syntax:	
    printf("parser: systax error in line %d \n",lineno);	
	exit(1);
	
parse_exit:

#ifdef GRAMCC_VERBOSE	
    printf("parse: done\n");
#endif		
  	
	return 0;
}; 


/*
 ***********************************
 * parserInit:
 *     Initializing parser.
 */
int parserInit (){
	
	register int i;
	
	
#ifdef GRAMCC_VERBOSE	
    printf("parserInit: Initializing ...\n");
#endif		
	
	//infile_size = 0;
	//outfile_size = 0;
	
    //stack support
	stack_flag = 0;
	stack_count = 0;
	stack_index = 0;
	

	    
	for ( i=0; i<8; i++ )
		id[i] = 0;
	
	for ( i=0; i<8; i++ )
		constant[i] = 0;
	
	for ( i=0; i<8; i++ )
		return_info[i] = 0;

	for ( i=0; i<512; i++ )
		stack[i] = 0;
	
	//...
	
	//esses endereços vão depender do arquivo de configuração do 
	//linker ...
	//#test: default em 0.
    program_header_address =     0;
    program_text_address   =   100;
    program_data_address   = 2*100;
    program_bss_address    = 3*100;	
	
	//...
	
	return (int) 0;
};

