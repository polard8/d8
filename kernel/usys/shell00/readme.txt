
bash clone:
    Isso � apenas um shell.
	Ele deve rodar nos limites da tela do terminal virtual.
	Isso n�o deve criar janelas.


  shell log:

   #todo: Incluir o suporte a lexer e parser, semelhante ao que acontece com os compiladores.
          Isso � necess�rio para interpretar os scripts;

na vers�o 2.1, o sistema de enviar mensagens para o procedimento da janela com o foco de entrada foi implementado. e funcionou corretamente. Ou seja.
o procedimento de janela de um processo em user mode � ativado toda vez que
pressionamos uma tecla do teclado.
O driver de teclado envia uma mensagem para o kernel, que coloca a mensagem
na estrutura da janela com o foco de entrada. O processo cliente, faz uma chamada
ao kernel solicitando uma mensagem. o processo quer saber se tem uma mensagem
na estrutura de determinda janela. O kernel envia a mensagem que est� na estrutura enviada pelo processo. O processo recebe essa mensagem e envia para o procedimento de janela dentro do programa.
:)


nem todo caractere digitado est� aparecendo na tela.
mas todo caractere que aparece na tela, tamb�m foi para o buffer de digita��es.
� poss�vel que seja algo com escalonamento do processo shell, estamos digitando
no momento em que a cpu est� sendo usada por outro processo.

O escalonador do kernel tem que selecionar a thread � qual a janela com o foco de entrada esta associada no momento em que recebe uma digita��o de teclado.

