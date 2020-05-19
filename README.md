## tresf

Objeto em Pd que calcula lista de frequencias, amplitudes (dB 0 a 100) e dampings (0 a 100) com base no processo de síntese 3f criado por Miller Puckette e explorado em algumas peças de Phillip Manoury (ver: http://www.philippemanoury.com/?p=6506, seção 3.2)

Em linhas gerais, o objeto recebe listas de 3 frequencias e criar listas resultantes de N frequencias, amplitudes e "dampings" calculados com base no particionamento de multiplicadores das frequencias originais e em suas somas. Além disso, um fator de probabilidade (limitado entre 0.03 e 1) permite controlar o comportamento de filtragem de frequencias (o que afeta a harmonicidade/inarmonicidade de um resultado sonoro baseado em síntese aditiva, por exemplo).

O objeto ainda precisa ser aperfeiçoado para permitir a alteração do número máximo de frequencias geradas. Em breve pretendo subir um exemplo prático de uso e binários para Linux e Windows.
