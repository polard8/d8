
 /gramcc/gramc

 Esse � o compilador de C da cole��o gramcc.
 Ele poder� ser chamado pelo wrapper gramcc.bin ou diretamente 
 atrav�s do comando gramc;
 
 
 gramcc usage:



$gramcc.bin [flag] [filename]


flags:
    -s
    -c
    ...

filename:
    test.c
    ... 

ex:
    gramcc -c file.c
    gramc -c file.
