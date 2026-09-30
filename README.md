# Compiladores — carpeta reorganizada

Todo el material del curso, ordenado por tipo. El zip original **no se modificó**.

## Estructura

| Carpeta | Contenido |
|---|---|
| `1_documentos/` | Enunciados de las 7 prácticas, guías (2016–2026), notas y el capítulo 8 (`referencia/`) |
| `2_hoc_c/` | `hoc1` … `hoc6` en C (versión de trabajo, cada una con su `Makefile`) |
| `3_hoc_java/` | Versiones en Java de hoc4, hoc5 y hoc6 |
| `4_hoc_cadenas/` | hoc con cadenas/lex (`hoc1`…`hoc6`) y `complejo` |
| `5_practicas/` | `practica1_yacc_basico` (C y Java), `practica2_java_grafico`, `practica3_tabla_simbolos` |
| `6_extras/` | Máquinas virtuales, minilex, minilisp, miniyacc, parserLR, tercetos, prolog |
| `herramientas/yacc/` | Fuente de `oyacc` (yacc portable) para equipos sin yacc/bison |
| `_archivo/` | Respaldos (`lenguajeCP.tar.gz`) |
| `MANIFEST.tsv` | Para **cada** archivo del zip original: a dónde fue, si era duplicado (y de cuál) o por qué se omitió |

Convención: en cualquier proyecto, la subcarpeta `old/` guarda versiones anteriores que **difieren** de la actual.

## Cómo compilar

```
make -C herramientas/yacc        # solo si tu sistema no tiene yacc ni byacc
cd 2_hoc_c/hoc5 && make          # usa yacc/byacc del sistema o, si no hay, ../../herramientas/yacc/oyacc
```
Se puede forzar otro yacc con `make YACC_CMD=/ruta/a/yacc`.

## Calculadora de números grandes
`5_practicas/practica1_yacc_basico/c/numeros_grandes/`
- `v1_arreglos_base10000/` — primera versión (arreglos en base 10000)
- `v2_cadenas/hoc1.y` — tu versión con cadenas de dígitos (sin cambios)
- `v3_cadenas_mejorado/hoc1.y` — versión corregida y simplificada

## Qué se hizo
- **Versión de trabajo de hoc2–hoc6 = la de `planb/`** (la más reciente; usa `div_` e `INDEF` en lugar de `div`/`UNDEF`, que chocaban con `stdlib.h`). Las versiones anteriores están en `old/`.
- `aliceCartman` estaba dos veces (~30 MB cada una, en practica2 y practica3): quedó una sola, con la unión de ambas.
- Se eliminaron duplicados exactos (guías, notas, `capitu8.*`, etc.) y archivos regenerables: `y.tab.c/h`, `lex.yy.c`, binarios (`calc`, `hoc`, `a.out`, `oyacc` ×6…), `.class`, archivo de bloqueo de LibreOffice, archivos y carpetas vacías. Todo está listado en `MANIFEST.tsv`.
- Los `Makefile` de hoc1–hoc6 se reescribieron con una plantilla común (hoc4 no tenía; el de hoc6 limpiaba `com` en vez de `hoc`).
- `herramientas/yacc/Makefile` era en realidad un Makefile de hoc6 copiado por error; se reemplazó por uno que compila `oyacc`.
- Renombrados: `vjo/` → `old/`, `prac1compila.docx` → `practica1_yacc_basico.docx`, etc.

## Pendiente (no se tocó)
- Los proyectos de `5_practicas/.../c/*` (arbol, complejo, polinomio, etc.) conservan sus `Makefile` originales.
- Los `hoc` compilan con *warnings* (`execerror` sin declarar); funcionan, pero conviene declararla en `hoc.h`.
