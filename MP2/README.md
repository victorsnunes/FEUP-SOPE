# Como Compilar:
    Basta copiar o arquivos e executar a Makefile, não há dependências externas além da biblioteca do C.
    Após clonar o repositório pode usar o seguinte comando:
        cd MP2/a/src && make 
        ou
        cd MP2/b/src && make 

# Como usar:
    Cliente : ./c <-t nsecs> <fifoname>
    Servidor: ./s <-t nsecs> [-l nbufsz] <fifoname>

---

# Detalhes de Implementação (Cliente):
##  Main Thread
   A thread principal lanca thread cliente responsaveis por enviar pedidos ao servidor e receber a sua resposta
## Clientes
   Cada cliente envia o seu pedido ao servido por um FIFO publico , espera pela resposta do servidor por um FIFO privado previamente criado , se o tempo do cliente acabar estes threads desistem fechado os seus respetivos FIFOS privados.
## Logs
  IWANT - o cliente fez o pedido inicial ao servidor
  GOTRS - o cliente recebeu a resposta do servidor
  GAVUP - Acabou o tempo e o cliente fecha
## Pedido ao servidor
  O pedido ao servidor é feito usando a seguinte estrutura : struct msg { int i; int t; pid_t pid; pthread_t tid; int res; };
 

# Detalhes de Implementação (Servidor):
##  Main Thread
   A thread principalrecebe os pedidos do cliente e cria thread produtores
## Produtores
    Cada thread produtor invoca a tarefa pedida a biblbioteca e armazena o resultado obtido no armazem, terminando logo de seguida
## Consumidor
    O thread consumidor retira contunuamente valores do armazem e envia a resposta ao cliente pelo respetivo FIFO privado.
## Sincronizacao
    Utilizacao de 2 mutex 
        - Um para as operacoes relacionadas com o buffer (armazem)
        - Um para o lancamento de threads 
## Buffer
    Foi implementado uma queue de tamanho variável para funcionar como buffer
## Logs
  RECVD - o thread principal recebeu o pedido
  TSKEX - um thread produtor informa que ja tem o resultado da tarefa
  TSKDN - o thread consumidor informa que enviou ao cliente a resposta
  2LATE - o thread consumidor informa que o pedido foi rejeitado
  FAILD -  o thread consumidor informa que nao consegue responder ao pedido




# Auto avaliação:
                                     Work done
    Pedro Carvalho  up201900513      25
    Victor Nunes    up201907226      25
    Emanuel Trigo   up201605389      25
    Leonor Beirão   up201806798      25
