# Master preliminary entries

a0: SDC
a1-a7: mission
a8-9: brake sensors
a10: Watchdog

assiy & assib: yellow & blue leds

lwss: input left wheel speed sensor - digital input
rwss: não vem para aqui porque caixa é do lado esquerdo e podia perder-se no ruído do acumulador e outros

d1: sempre em high - Watchdog, se low - SDC ativo (EBS PCB). Questão: D1 ativo em loop ou sempre ativo? 
d2: sempre em low, se problema - por high para ligar o SDC (Master PCB)
d3: ebs desligado - high, ebs ligado - low, (EBS Solenoid Valves - controlar ebs sem abrir o shutdown, início e fim de missões)
d4: começar - high, fechar sd - low. TS on/off para fechar inicialmente relay ebs e fechar o shutdown, é necessário