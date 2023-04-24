# SO Zoo
Este repositório contém a implementação do trabalho final da cadeira de Sistemas Operacionais. </br>

# Descrição
Em um zoológico existem leões, suricatos e avestruzes, que devem ser alimentados por dois veterinários. </br>
Cada animal come um tipo específico de comida e possui um ciclo diário que consiste em comer, dormir, comer e se exibir, e os veterinários podem servir do estoque do zoológico para o comedouro de cada animal de modo a manter os comedouros sempre cheios.<br>
Quando o estoque de comida de um animal termina o veterinário pode requisitar ao fornecedor a reposição do estoque. O fornecedor reabastece o estoque de todos os animais. <br>
Deve ser utilizada um thread, para cada animal, para cada veterinário e para o fornecedor.<br>

# Implementação
A implementação foi realizada utilizando o princípio de atores e troca de mensagens entre eles. <br>
O zoológico possui um canal de comunicação com cada ator, e envia mensagens de atualização de hora, e eventualmente de término da simulação. <br>
Os animais recebem as atualizações das horas, e de acordo com seu estado atual, tratam o evento. Quando os animais vão comer, eles enviam uma mensagem para os veterinários com a quantidade de alimento que irão comer. <br>
Os veterinários recebem mensagens de término da simulação e de requisição de alimento por parte dos animais. Quando os veterinários recebem a mensagem de alimentação dos animais, eles irão atender no comedouro da espécie correspondente o quanto será comido informado na mensagem. Quando termina o suprimento do zoológico, o veterinário envia uma mensagem requisitando reabastecimento do estoque para o fornecedor.<br>
O fornecedor recebe mensagens de término da simulação e de requisição de reabastecimento. <br>


# Executar
Para compilar, existe um arquivo `Makefile`. <br>
Como é utilizado a biblioteca `pthread` para lançamento de threads, será necessário utilizar o sistema operacional `linux` com a biblioteca disponível<br>
```
git clone https://github.com/JelsonRodrigues/SO_zoo
cd SO_Zoo
make run
```