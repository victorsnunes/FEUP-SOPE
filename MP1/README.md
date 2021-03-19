# Xmod Como instalar:
    Basta copiar o arquivos e executar a Makefile, não há dependências externas além da biblioteca do C.
    Após clonar o repositório pode usar o seguinte comando:
        `cd MP1/src && make `
    
# Como usar:
    Xmod é uma cópia de chmod, em que segue o formato **xmod [options] mode FILE/DIR**. As flags implementadas são:
        - r: recursiva, em que chama o xmod para os arquivos dentro de de um DIR
        - v: verboso, detalhes sobre os passos tomados na execução do programa
        - c: parecido com o modo verboso, porém apenas mostra quando a alterações nos ficheiros

---

# Detalhes de Implementação:
    - Modo Recursivo (-r) implementado usando forks e chamadas recurivas na main
