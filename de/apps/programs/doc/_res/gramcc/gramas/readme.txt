
//gramas
//vamos aproveitar o c�digo do compilador para criar uma assembler ...
//vamos usar o mesmo esquema de lexer e parser. 

------------------------
old stuff
 /gramcc/c 

 Esse � o compilador de C da cole��o gramcc.
 Ele poder� ser chamado pelo wrapper gramcc.bin eu diretamente atrav�s do comando gramc;


//gramado C Compiler. 

Esse � um aplicativo para rodar no sistema operacional Gramado 0.4.
Roda apenas no ambiente gramado core, no processo init.

� um editor de testes para testar funcionalidades do sistema.
No momento usamos ele para testar o envio de uma string do shell
para o editor atrav�s do kernel via mem�ria compartilhada.
O envio da mensagem funcionou.

crt0.asm cont�m o entry point, chamando main em seguida.


entry point em crt0.asm
__crt0Main:
  


//fun��o principal em main.c
mainGetMessage()


