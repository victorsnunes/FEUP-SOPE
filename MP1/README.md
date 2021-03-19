# Xmod Como Compilar:
    Basta copiar o arquivos e executar a Makefile, não há dependências externas além da biblioteca do C.
    Após clonar o repositório pode usar o seguinte comando:
        `cd MP1/src && make `
    
# Como usar:
    Xmod é uma cópia de chmod, em que segue o formato **xmod [options] mode FILE/DIR**. As flags implementadas são:
        - r: recursiva, em que chama o xmod para os arquivos dentro de de um DIR
        - v: verboso, detalhes sobre os passos tomados na execução do programa
        - c: parecido com o modo verboso, porém apenas mostra quando à alterações nos ficheiros

---

# Detalhes de Implementação:
##  Modo Recursivo (-r)
   Implementado usando forks e chamadas recurivas na main
## Sinais 
   Para criar o prompt (Are you sure...) quando se é recebido o sinal de interrupção (SIGINT) o pai se encarrega de perguntar enquanto os filhos e netos esperam por um sinal com pause(), é enviado ao grupo SIGUSR1 caso a resposta no prompt seja negativa e SIGTERM no caso de uma resposta positiva
## Logs
   Endereço do log precisa ser especificado pela variável de ambiente LOG_FILENAME, caso não esteja especificada não sera efetuado o log.
   O tempo marcado nos logs é em ms desde o início do progrma, usando como refrência o clock do CPU.
## Modo "normal"
   Foi utilizada uma máquina de estado de modo a poder fazer o parsing dos argumentos do modo "normal" **<u|g|o|a><-|+|=><rwx>** . Depois de lidas as permissoes originais do ficheiro são calculadas novas permissoes e aplicadas recorrendo a máscaras binarias ,**Permission Bits** e **Protection Bits**. 


# Auto avaliação:
    
