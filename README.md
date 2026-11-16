# banker

Implementação em C do algoritmo do Banqueiro (Silberschatz, *Operating System Concepts*, 10ª ed., p. P-45) para evitar deadlock. Atividade de Infraestrutura de Software, CESAR School.

Autor: Rafael Carrilho (`rac4`)

## Como compilar

```
make
```

Gera o executável `banker`. Para limpar: `make clean`. O Makefile compila com `gcc -g -Wall -Wextra -std=c11`.

## Como executar

```
./banker 10 5 7
```

Cada argumento é o número de instâncias disponíveis de um tipo de recurso. O exemplo acima tem 3 tipos de recurso (10, 5 e 7 instâncias). A quantidade de recursos vem dos argumentos, e a quantidade de clientes vem do número de linhas de `customer.txt`. Nenhum dos dois é fixo no código.

O programa lê dois arquivos **na pasta onde é executado**:

| Arquivo | Conteúdo |
|---|---|
| `customer.txt` | uma linha por cliente, com o máximo que ele pode pedir de cada recurso, separado por vírgula (ex.: `7,5,3`) |
| `commands.txt` | um comando por linha: `RQ cliente v1 v2 ...` (pedido), `RL cliente v1 v2 ...` (devolução) ou `*` (imprime o estado) |

E grava o resultado em `result.txt`.

## O que cada comando faz

- **RQ**: confere, nesta ordem, se o cliente existe, se o pedido não excede o que ainda falta ao cliente (`maximum - allocation`) e se há recursos disponíveis. Depois simula a alocação e roda o teste de segurança do Banqueiro. Se o estado resultante não for seguro, desfaz a simulação e nega. As mensagens de `Allocate` e de negação vão para `result.txt`.
- **RL**: devolve recursos, desde que o cliente tenha pelo menos o que quer devolver. Uma devolução maior que o alocado é recusada com mensagem em `stderr`, nada é gravado em `result.txt`, e o programa segue para o próximo comando.
- **\***: grava em `result.txt` a tabela `MAXIMUM | ALLOCATION | NEED` e a linha `AVAILABLE`.

## Tratamento de erros

O programa encerra com código `1` e uma mensagem em `stderr` (começando por `Error:`), liberando a memória e fechando os arquivos antes de sair, nos casos:

- nenhum argumento, argumento que não é número, negativo ou maior que `INT_MAX`;
- `customer.txt` ou `commands.txt` ausente, ou `customer.txt` vazio;
- linha de `customer.txt` com formato inválido ou com quantidade de colunas diferente da quantidade de argumentos;
- linha de `commands.txt` com formato inválido (inclui linha em branco no meio do arquivo);
- comando com cliente que não existe em `customer.txt`;
- falha de alocação de memória.

Termina com código `0` quando todos os comandos são processados.

## Organização do código

| Arquivo | Responsabilidade |
|---|---|
| `banker.c` | `main`: valida os argumentos, lê `customer.txt`, executa os comandos e grava `result.txt` |
| `parser.c` / `parser.h` | `parse` (linha de `customer.txt`) e `parse_comando` (linha de `commands.txt`) |
| `seguranca.c` / `seguranca.h` | `verifica`: teste de segurança do Banqueiro (0 = seguro, 1 = inseguro, 2 = falha de memória) |
| `Makefile` | build do `banker` |
| `evidencias.log` | registro do terminal durante o desenvolvimento (`script`) |

O `need` não é guardado: é recalculado como `maximum - allocation` onde é usado.

## Como foi validado

- Exemplo do enunciado (`./banker 10 5 7`): `result.txt` idêntico ao do professor (`diff` vazio).
- `valgrind --leak-check=full`: sem erros e sem vazamento no caso do exemplo (35 alocações, 35 liberações) e em casos de erro.
- Casos de entrada inválida (argumentos e arquivos) e um cenário alternativo com 2 clientes e 2 recursos.

Detalhes, decisões e limitações estão no relatório.

## Limitações conhecidas

- As linhas dos arquivos são lidas com buffer de 256 bytes. Uma linha maior que 255 caracteres é cortada.
- O alinhamento da tabela do `*` usa espaços fixos, calibrados para 3 recursos. Com outro número de recursos os valores estão corretos, mas as colunas não ficam alinhadas.
- Se um erro ocorre no meio de `commands.txt`, o `result.txt` fica com o que já tinha sido gravado até ali.
- Quando a quantidade de argumentos difere da de colunas de `customer.txt`, a mensagem é genérica (`invalid data for customer 0`).
- O caminho de falha de `malloc` durante o teste de segurança existe no código, mas não foi exercitado.
