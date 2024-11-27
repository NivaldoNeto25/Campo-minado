# Projeto da cadeira de Programação 1

O objetivo do projeto é desenvolver um jogo de campo minado na linguagem C seguindo as seguintes regras:

1. Ao iniciar o jogo, o/a jogador/a deverá escolher uma de duas opções:
a) Iniciar novo jogo
b) Carregar jogo salvo: nesta opção, o último jogo salvo deve ser carregado de onde parou. Jogos
anteriores devem ser salvos em um ou mais arquivos. O formato do arquivo é livre, porém CSVs
são recomendados.

2. O jogo de campo minado deverá ocorrer em um tabuleiro de 20x20 casas. As regras e formas de interação
devem ser as descritas abaixo:
a) Ao iniciar um novo jogo, 10 bombas devem ser distribuídas aleatoriamente no tabuleiro.
b) Diferente dos jogos tradicionais de campo minado, não haverá casas com números indicando
quantidade de bombas ao redor. Ao invés disso, ao passar o mouse sobre uma casa, deve ser
informado em algum local da tela a soma da quantidade de bombas presentes num raio de 4 casas
acima, abaixo, e ao lado da casa onde o mouse está apontando.
c) Ao clicar em uma casa, deverá ser revelado seu conteúdo (bomba ou terra). Caso seja uma bomba,
o/a jogador/a perde uma vida. Cada jogador/a inicia com 3 vidas. O jogo finaliza quando todas as
casas do tipo terra forem reveladas, ou caso o jogador estoure 3 bombas.

3. Design da interface: cores e/ou texturas devem ser usadas para representar os elementos do jogo.

4. Ao pressionar ESC, o jogo deve ser pausado e um menu com as opções SAIR e RETORNAR deve ser
mostrado. Ao clicar em sair, o status atual do jogo deve ser salvo utilizando arquivos, incluindo estado do
mapa e vidas restantes do/a jogador/a.
