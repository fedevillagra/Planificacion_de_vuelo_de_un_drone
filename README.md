# Planificación de Vuelo de un Drone

Este proyecto consiste en la planificación del vuelo de un drone sobre un campo de siembra para aplicar herbicida en áreas con maleza significativa. La información del campo se obtiene de una imagen satelital procesada, representada en una matriz que se carga desde un archivo de texto (`mapa.txt`). 

El drone debe identificar áreas de maleza significativas (3x3), evitar obstáculos representados por filas o columnas de barreras, y planificar un recorrido óptimo para cubrir todas las manchas detectadas, volviendo al punto de partida.

## Uso

1. **Preparar el archivo de mapa**:
   - El archivo `mapa.txt` debe contener la representación del campo en forma de matriz con valores `1` (maleza), `0` (sin maleza), y `X` (barreras).
   - Asegúrate de que los valores de `MAX_Y` y `MAX_X` al principio del programa coincidan con las dimensiones de la matriz del archivo `mapa.txt`.

2. **Ejecutar el programa**:
   - Ejecuta el archivo `main.cpp`.
   - El programa leerá el archivo `mapa.txt`, identificará las áreas de maleza significativas, y planificará el vuelo del drone.

## Resultados

- El programa mostrará en consola:
  - Las coordenadas de las manchas de maleza significativas que requieren aplicación de herbicida.
  - El recorrido óptimo del drone para cubrir todas las manchas y regresar al punto de partida.

## Ejemplo de Entrada

Supongamos que `mapa.txt` contiene la siguiente matriz de 12x28:

```
0000000111000110000010001110
0111000111001000011100001110
0111000111000110011100001110
0111000000001000011100000000
00000000000000000000xxxxxxx1
00000111000x0001110000000000
00000111000x0001110000000000
00000111000x0001110000000000
00000000000x0000000000000000
1110000000000000000000000111
1110000000000000000000000111
1110000000000000000000000111
```

En este ejemplo:
- Las celdas con `1` representan maleza.
- Las celdas con `X` representan barreras.

El programa detectará las manchas de maleza y calculará el recorrido óptimo del drone para cubrir todas ellas.

## Notas

- **Dimensiones del mapa**: Es crucial que los valores de `MAX_Y` y `MAX_X` se ajusten a las dimensiones del archivo `mapa.txt` antes de ejecutar el programa.
- **Limitación de manchas**: Se recomienda que el mapa no contenga más de 10 manchas a recorrer, debido a la complejidad computacional del problema del ciclo hamiltoneano.
- **Barreras**: Las barreras, representadas por `X`, impedirán el paso del drone entre manchas, y se tendrán en cuenta al calcular el recorrido óptimo.
- **Costo computacional**: Este programa utiliza un costo computacional de orden factorial O(n!), ya que es un problema similar al del viajero (Travelling Salesman Problem).