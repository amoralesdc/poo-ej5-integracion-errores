# Ejercicio 5 - Integración con manejo de errores

Combina un **archivo de texto** y una **base de datos SQLite** con manejo de excepciones:

1. `LectorTareas` lee las tareas de un archivo `.txt`. Si el archivo no existe, lanza `ArchivoNoEncontrado`.
2. `BaseDatos` se conecta a SQLite y guarda las tareas. Si la conexión o una consulta falla, lanza `ErrorBaseDatos`.
3. En `main` se atrapa cada excepción y se muestra un mensaje claro en lugar de que el programa termine de golpe.

## Requisitos
```bash
sudo apt install libsqlite3-dev
```

## Compilar y ejecutar
```bash
make
./integracion                                  # caso correcto (tareas.txt -> tareas.db)
./integracion no_existe.txt                    # error: el archivo no existe
./integracion tareas.txt carpeta_falsa/x.db    # error: falla la conexión a la BD
```
