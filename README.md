# SDG2
Es es el proyecto base del curso 2022-2023 para los alumnos de la asignatura de Sistemas Digitales II. 
Los códigos están parcialmente documentados para que pueda ser interpretado por Doxygen a fin de que pueda tener ejemplos de cómo hacer la documentación de su propio código.

Este proyecto base no compila y debe seguir los pasos del enunciado de la asignatura. 

Puedes acceder al vídeo del demostrador del proyecto pinchando en la imagen:
[![Demostrador del proyecto Retina](https://github.com/sdg2DieUpm/retina/blob/assets/images/rgb_led_system_midjourney.png?raw=true)](https://youtu.be/7yMZnfwStgs "Demostrador del proyecto Retina. Imagen generada con Midjourney.")

Puedes descargar los códigos desde la página de **[Github de SDG2](https://github.com/sdg2DieUpm/retina)**.

Incluya la carpeta `retina` en su directorio `projects` como se muestra a continuación. Recuerde que esta es la estructura de los proyectos de SDG2 para **compilación multiplataforma** usando VSCode. La diferencia fundamental con la estructura de los [proyectos para SDG1](https://sdg1dieupm.github.io/c_basics/) es que **contiene** las carpetas `common` y `port`. También son necesarias las carpetas `drivers` y `svd` a la misma altura de `projects`. 

```
📂sdg
┣ 📂drivers
┃ ┣ 📂stm32f4xx
┃ ┃ ┃ ┣ 📂CMSIS
┃ ┃ ┃ ┗ 📂Drivers
┃ ┗ ┗...
┣ 📂projects
┃ ┣ 📦projects.code-workspace
┃ ┣ 📂hello
┃ ┣ 📂blink
┃ ┣ 📂retina
┃ ┃ ┣ 📂.vscode    
┃ ┃ ┃ ┣ 📜launch.json
┃ ┃ ┃ ┗ 📜tasks.json
┃ ┃ ┣ 📂common
┃ ┃ ┃ ┣ 📂include
┃ ┃ ┃ ┣ 📂src
┃ ┃ ┃ ┗ 📜Makefile.common
┃ ┃ ┣ 📂port
┃ ┃ ┃ ┃ ┣ 📂nucleo_stm32f446re
┃ ┃ ┃ ┃ ┃ ┣ 📂include
┃ ┃ ┃ ┃ ┃ ┣ 📂src
┃ ┃ ┃ ┃ ┃ ┣ 📜STM32F446RETx_FLASH.ld
┃ ┃ ┃ ┃ ┃ ┣ 📜Makefile.port
┃ ┃ ┃ ┃ ┃ ┗ 📜openocd.cfg
┃ ┃ ┣ 📂lib
┃ ┃ ┗ 📜Makefile
┃ ┃ ┗ ...
┃ ┗ ...
┗
```
Puede navegar a través del menú lateral por parte de la API generada. El resto de la API del proyecto debe crearla usted. Siga los pasos de la *Guía de instalación de herramientas para compilación multiplataforma en C* donde se le indica cómo generar la API usando Doxygen.

Puede modificar este fichero README.md para documentar su proyecto. Este README.md será la portada de su API. Puede poner imágenes, enlaces a vídeos y URLs, hacer tablas, formatear el texto, hacer listas, introducir emojis, etc. Todo debe hacerlo con estilo **Markdown**. Puede buscar muchos ejemplos en la red y ver cómo usarlo en el [sitio web](https://www.markdownguide.org/basic-syntax/).

