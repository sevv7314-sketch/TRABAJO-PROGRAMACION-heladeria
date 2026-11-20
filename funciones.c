#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "funciones.h"

static void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static void leerTextoNoVacio(const char *mensaje, char *destino, int tam) {
    while (1) {
        printf("%s", mensaje);
        if (fgets(destino, tam, stdin) != NULL) {
            destino[strcspn(destino, "\n")] = '\0';
            if (strlen(destino) > 0) {
                return;
            }
        }
        printf("[ERROR] El texto no puede estar vacio. Intente de nuevo.\n");
    }
}

static int leerEnteroEnRango(const char *mensaje, int min, int max) {
    int valor;
    while (1) {
        printf("%s", mensaje);
        if (scanf("%d", &valor) == 1) {
            limpiarBuffer();
            if (valor >= min && valor <= max) {
                return valor;
            }
            printf("[ERROR] Debe ingresar un numero entre %d y %d. Intente de nuevo.\n", min, max);
        } else {
            limpiarBuffer();
            printf("[ERROR] Entrada invalida. Ingrese solo numeros enteros.\n");
        }
    }
}

static float leerFloatMinimo(const char *mensaje, float min, int estricto) {
    float valor;
    while (1) {
        printf("%s", mensaje);
        if (scanf("%f", &valor) == 1) {
            limpiarBuffer();
            if ((estricto && valor > min) || (!estricto && valor >= min)) {
                return valor;
            }
            if (estricto) {
                printf("[ERROR] El valor debe ser mayor a %.2f. Intente de nuevo.\n", min);
            } else {
                printf("[ERROR] El valor debe ser mayor o igual a %.2f. Intente de nuevo.\n", min);
            }
        } else {
            limpiarBuffer();
            printf("[ERROR] Entrada invalida. Ingrese un numero valido.\n");
        }
    }
}

static float leerFloatEnRango(const char *mensaje, float min, float max) {
    float valor;
    while (1) {
        printf("%s", mensaje);
        if (scanf("%f", &valor) == 1) {
            limpiarBuffer();
            if (valor > min && valor <= max) {
                return valor;
            }
            printf("[ERROR] Debe ingresar un valor mayor a %.2f y menor o igual a %.2f.\n", min, max);
        } else {
            limpiarBuffer();
            printf("[ERROR] Entrada invalida. Ingrese un numero valido.\n");
        }
    }
}

static Fecha leerFecha(const char *titulo) {
    Fecha f;
    printf("\n--- %s ---\n", titulo);
    f.dia = leerEnteroEnRango("Dia (1-31): ", 1, 31);
    f.mes = leerEnteroEnRango("Mes (1-12): ", 1, 12);
    f.anio = leerEnteroEnRango("Anio (2020-2100): ", 2020, 2100);
    return f;
}

static int estaVencido(Fecha ingreso, Fecha actual) {
    int diffMeses = (actual.anio - ingreso.anio) * 12 + (actual.mes - ingreso.mes);
    if (diffMeses > 2) {
        return 1;
    }
    if (diffMeses == 2 && actual.dia > ingreso.dia) {
        return 1;
    }
    return 0;
}

static void recalcularStockTotal(MateriaPrima *mp) {
    float total = 0.0f;
    for (int i = 0; i < mp->numLotes; i++) {
        total += mp->lotes[i].cantidadKg;
    }
    mp->cantidadKg = total;
}

static void descontarDeLotesFIFO(MateriaPrima *mp, float cantidadADescontar) {
    float restante = cantidadADescontar;
    int i = 0;

    while (i < mp->numLotes && restante > 0.0001f) {
        if (mp->lotes[i].cantidadKg <= restante) {
            restante -= mp->lotes[i].cantidadKg;
            for (int j = i; j < mp->numLotes - 1; j++) {
                mp->lotes[j] = mp->lotes[j + 1];
            }
            mp->numLotes--;
        } else {
            mp->lotes[i].cantidadKg -= restante;
            restante = 0.0f;
            i++;
        }
    }
    recalcularStockTotal(mp);
}

static void agregarALotes(MateriaPrima *mp, float cantidad, Fecha fecha) {
    for (int i = 0; i < mp->numLotes; i++) {
        if (mp->lotes[i].fechaIngreso.dia == fecha.dia &&
            mp->lotes[i].fechaIngreso.mes == fecha.mes &&
            mp->lotes[i].fechaIngreso.anio == fecha.anio) {
            mp->lotes[i].cantidadKg += cantidad;
            recalcularStockTotal(mp);
            return;
        }
    }

    if (mp->numLotes < MAX_LOTES) {
        mp->lotes[mp->numLotes].fechaIngreso = fecha;
        mp->lotes[mp->numLotes].cantidadKg = cantidad;
        mp->numLotes++;
    } else {
        mp->lotes[MAX_LOTES - 1].cantidadKg += cantidad;
    }
    recalcularStockTotal(mp);
}

static int contieneTextoInsensible(const char *texto, const char *busqueda) {
    char tLower[MAX_NOMBRE];
    char bLower[MAX_NOMBRE];
    size_t i;

    for (i = 0; texto[i] != '\0' && i < MAX_NOMBRE - 1; i++) {
        tLower[i] = (char)tolower((unsigned char)texto[i]);
    }
    tLower[i] = '\0';

    for (i = 0; busqueda[i] != '\0' && i < MAX_NOMBRE - 1; i++) {
        bLower[i] = (char)tolower((unsigned char)busqueda[i]);
    }
    bLower[i] = '\0';

    return strstr(tLower, bLower) != NULL;
}

void inicializarDatos(MateriaPrima inventario[], RecetaHelado recetas[], ProductoTerminado productos[]) {
    const char *nombresMaterias[NUM_MATERIAS] = {
        "Base lactea",       // 0
        "Azucar",            // 1
        "Pulpa de fresa",    // 2
        "Pulpa de mora",     // 3
        "Pulpa de banano",   // 4
        "Pulpa de maracuya", // 5
        "Pasta de cacao"     // 6
    };

    for (int i = 0; i < NUM_MATERIAS; i++) {
        strcpy(inventario[i].nombre, nombresMaterias[i]);
        strcpy(inventario[i].proveedor, "Sin asignar");
        inventario[i].cantidadKg = 0.0f;
        inventario[i].costoPorKg = 0.0f;
        inventario[i].numLotes = 0;
    }

    // 0: Fresa
    strcpy(recetas[0].nombre, "Fresa");
    recetas[0].indicesMateria[0] = 0; recetas[0].cantidadesPorKg[0] = 0.50f;
    recetas[0].indicesMateria[1] = 2; recetas[0].cantidadesPorKg[1] = 0.35f;
    recetas[0].indicesMateria[2] = 1; recetas[0].cantidadesPorKg[2] = 0.15f;
    recetas[0].tiempoPorKgMin = 15.0f;

    // 1: Mora
    strcpy(recetas[1].nombre, "Mora");
    recetas[1].indicesMateria[0] = 0; recetas[1].cantidadesPorKg[0] = 0.50f;
    recetas[1].indicesMateria[1] = 3; recetas[1].cantidadesPorKg[1] = 0.34f;
    recetas[1].indicesMateria[2] = 1; recetas[1].cantidadesPorKg[2] = 0.16f;
    recetas[1].tiempoPorKgMin = 15.0f;

    // 2: Banano
    strcpy(recetas[2].nombre, "Banano");
    recetas[2].indicesMateria[0] = 0; recetas[2].cantidadesPorKg[0] = 0.58f;
    recetas[2].indicesMateria[1] = 4; recetas[2].cantidadesPorKg[1] = 0.30f;
    recetas[2].indicesMateria[2] = 1; recetas[2].cantidadesPorKg[2] = 0.12f;
    recetas[2].tiempoPorKgMin = 15.0f;

    // 3: Maracuya
    strcpy(recetas[3].nombre, "Maracuya");
    recetas[3].indicesMateria[0] = 0; recetas[3].cantidadesPorKg[0] = 0.55f;
    recetas[3].indicesMateria[1] = 5; recetas[3].cantidadesPorKg[1] = 0.28f;
    recetas[3].indicesMateria[2] = 1; recetas[3].cantidadesPorKg[2] = 0.17f;
    recetas[3].tiempoPorKgMin = 15.0f;

    // 4: Chocolate
    strcpy(recetas[4].nombre, "Chocolate");
    recetas[4].indicesMateria[0] = 0; recetas[4].cantidadesPorKg[0] = 0.75f;
    recetas[4].indicesMateria[1] = 6; recetas[4].cantidadesPorKg[1] = 0.10f;
    recetas[4].indicesMateria[2] = 1; recetas[4].cantidadesPorKg[2] = 0.15f;
    recetas[4].tiempoPorKgMin = 20.0f;

    for (int i = 0; i < NUM_RECETAS; i++) {
        strcpy(productos[i].nombre, recetas[i].nombre);
        productos[i].cantidadProducidaKg = 0.0f;
    }
}

void mostrarBienvenida(void) {
    printf("==================================================\n");
    printf("   BIENVENIDO AL SISTEMA DE PRODUCCION DE HELADOS \n");
    printf("==================================================\n");
    printf("Todas las cantidades se manejan en kilogramos (kg).\n");
    printf("Nota: La materia prima vence y se desecha a los 2 meses.\n");
}

Fecha solicitarFechaActual(void) {
    return leerFecha("INGRESE LA FECHA ACTUAL DEL SISTEMA");
}

void mostrarMenuPrincipal(Fecha fechaActual) {
    printf("\n--- MENU PRINCIPAL (Fecha: %02d/%02d/%04d) ---\n",
           fechaActual.dia, fechaActual.mes, fechaActual.anio);
    printf("1. CREACION\n");
    printf("2. INVENTARIO\n");
    printf("3. FACTURACION\n");
    printf("4. Salir\n");
}

void menuCreacion(MateriaPrima inventario[], const RecetaHelado recetas[],
                  ProductoTerminado productos[], Fecha fechaActual) {
    int opcion;
    do {
        printf("\n--- CREACION ---\n");
        printf("1. Ingresar proveedor, costo y cantidad de materias primas (en kg)\n");
        printf("2. Producto terminado (producir helado en kg y validar tiempo)\n");
        printf("3. Volver al menu principal\n");

        opcion = leerEnteroEnRango("Seleccione una opcion: ", 1, 3);

        switch (opcion) {
            case 1:
                ingresarStockMateriaPrima(inventario, fechaActual);
                break;
            case 2:
                producirProductoTerminado(inventario, recetas, productos, fechaActual);
                break;
            case 3:
                printf("\nVolviendo al menu principal...\n");
                break;
        }
    } while (opcion != 3);
}

void ingresarStockMateriaPrima(MateriaPrima inventario[], Fecha fechaActual) {
    int opcion;
    do {
        printf("\n--- INGRESO DE MATERIA PRIMA (Fecha: %02d/%02d/%04d) ---\n",
               fechaActual.dia, fechaActual.mes, fechaActual.anio);
        mostrarInventario(inventario);
        printf("Seleccione el numero de la materia prima (1 a %d) o 0 para volver: ", NUM_MATERIAS);

        opcion = leerEnteroEnRango("", 0, NUM_MATERIAS);

        if (opcion == 0) {
            break;
        }

        int idx = opcion - 1;
        char prompt[150];

        printf("\n--- Ingresando stock para: %s ---\n", inventario[idx].nombre);
        leerTextoNoVacio("Nombre del proveedor: ", inventario[idx].proveedor, MAX_NOMBRE);

        snprintf(prompt, sizeof(prompt), "Costo por kilogramo de %s ($): ", inventario[idx].nombre);
        inventario[idx].costoPorKg = leerFloatMinimo(prompt, 0.0f, 0);

        snprintf(prompt, sizeof(prompt), "Cantidad que llego de %s (kg): ", inventario[idx].nombre);
        float cantidadLlegada = leerFloatMinimo(prompt, 0.0f, 1);

        agregarALotes(&inventario[idx], cantidadLlegada, fechaActual);

        printf("\n[OK] Se agregaron %.2f kg de %s (Fecha: %02d/%02d/%04d | Proveedor: %s | Costo: $%.2f/kg).\n",
               cantidadLlegada, inventario[idx].nombre,
               fechaActual.dia, fechaActual.mes, fechaActual.anio,
               inventario[idx].proveedor, inventario[idx].costoPorKg);
        printf("Stock total acumulado de %s: %.2f kg.\n",
               inventario[idx].nombre, inventario[idx].cantidadKg);
    } while (opcion != 0);
}

void producirProductoTerminado(MateriaPrima inventario[], const RecetaHelado recetas[],
                               ProductoTerminado productos[], Fecha fechaActual) {
    revisarYDesecharVencidos(inventario, fechaActual);

    printf("\n--- PRODUCTO TERMINADO ---\n");
    printf("Sabores disponibles (recetas base por 1 kg):\n");
    for (int i = 0; i < NUM_RECETAS; i++) {
        printf("%d. %-10s (Tiempo: %.0f min por kg)\n",
               i + 1, recetas[i].nombre, recetas[i].tiempoPorKgMin);
    }

    char prompt[150];
    snprintf(prompt, sizeof(prompt), "\nSeleccione el sabor a producir (1 a %d): ", NUM_RECETAS);
    int sel = leerEnteroEnRango(prompt, 1, NUM_RECETAS) - 1;

    snprintf(prompt, sizeof(prompt), "Cuantos kilogramos (kg) de helado de '%s' deseas producir?: ",
             recetas[sel].nombre);
    float cantidadKg = leerFloatMinimo(prompt, 0.0f, 1);

    float tiempoRequeridoMin = recetas[sel].tiempoPorKgMin * cantidadKg;
    float tiempoDisponibleMin = leerFloatMinimo("Cuanto tiempo disponible tienes para producir (en minutos)?: ", 0.0f, 1);

    if (tiempoRequeridoMin > tiempoDisponibleMin) {
        printf("\n[NO VIABLE] Produccion cancelada por falta de tiempo.\n");
        printf("Tiempo requerido: %.2f min | Tiempo disponible: %.2f min.\n",
               tiempoRequeridoMin, tiempoDisponibleMin);
        return;
    }

    printf("\n[VIABLE] El tiempo alcanza (Requerido: %.2f min / Disponible: %.2f min).\n",
           tiempoRequeridoMin, tiempoDisponibleMin);

    float costoTotalProduccion = 0.0f;
    for (int i = 0; i < 3; i++) {
        int idx = recetas[sel].indicesMateria[i];
        float requeridoKg = recetas[sel].cantidadesPorKg[i] * cantidadKg;
        if (requeridoKg > inventario[idx].cantidadKg) {
            printf("\n[ERROR] Stock insuficiente de %s.\n", inventario[idx].nombre);
            printf("Requerido: %.2f kg | Disponible: %.2f kg. Produccion cancelada.\n",
                   requeridoKg, inventario[idx].cantidadKg);
            return;
        }
        costoTotalProduccion += requeridoKg * inventario[idx].costoPorKg;
    }

    for (int i = 0; i < 3; i++) {
        int idx = recetas[sel].indicesMateria[i];
        float requeridoKg = recetas[sel].cantidadesPorKg[i] * cantidadKg;
        descontarDeLotesFIFO(&inventario[idx], requeridoKg);
    }

    productos[sel].cantidadProducidaKg += cantidadKg;

    printf("\n[EXITO] Se produjeron %.2f kg de helado de '%s' en %.2f min!\n",
           cantidadKg, recetas[sel].nombre, tiempoRequeridoMin);
    printf("Costo estimado de esta produccion: $%.2f ($%.2f por kg).\n",
           costoTotalProduccion, costoTotalProduccion / cantidadKg);
    mostrarProductosTerminados(productos);
}

void menuInventario(MateriaPrima inventario[], const RecetaHelado recetas[],
                    ProductoTerminado productos[], Fecha *fechaActual) {
    int opcion;
    do {
        printf("\n--- INVENTARIO (Fecha: %02d/%02d/%04d) ---\n",
               fechaActual->dia, fechaActual->mes, fechaActual->anio);
        printf("1. Ver inventario de materia prima (en kg)\n");
        printf("2. Ver productos terminados (en kg)\n");
        printf("3. Ver recetas predefinidas y costos por kg\n");
        printf("4. Calculadora de capacidad maxima\n");
        printf("5. Busqueda por proveedor\n");
        printf("6. Volver al menu principal\n");

        opcion = leerEnteroEnRango("Seleccione una opcion: ", 1, 6);

        switch (opcion) {
            case 1:
                submenuInventarioMateriaPrima(inventario, fechaActual);
                break;
            case 2:
                submenuProductosTerminados(inventario, recetas, productos, *fechaActual);
                break;
            case 3:
                mostrarRecetas(recetas, inventario);
                break;
            case 4:
                calcularCapacidadMaxima(inventario, recetas);
                break;
            case 5:
                buscarPorProveedor(inventario);
                break;
            case 6:
                printf("\nVolviendo al menu principal...\n");
                break;
        }
    } while (opcion != 6);
}

void submenuInventarioMateriaPrima(MateriaPrima inventario[], Fecha *fechaActual) {
    int opcion;
    do {
        printf("\n--- INVENTARIO DE MATERIA PRIMA (EN KG) ---\n");
        mostrarInventario(inventario);
        printf("1. Editar proveedor, costo y cantidad de materia prima\n");
        printf("2. Eliminar proveedor y cantidad de materia prima\n");
        printf("3. Actualizar fecha actual y desechar vencidos (> 2 meses)\n");
        printf("4. Volver a INVENTARIO\n");

        opcion = leerEnteroEnRango("Seleccione una opcion: ", 1, 4);

        switch (opcion) {
            case 1:
                editarMateriaPrima(inventario, *fechaActual);
                break;
            case 2:
                eliminarMateriaPrima(inventario);
                break;
            case 3:
                *fechaActual = leerFecha("NUEVA FECHA ACTUAL DEL SISTEMA");
                revisarYDesecharVencidos(inventario, *fechaActual);
                break;
            case 4:
                break;
        }
    } while (opcion != 4);
}

void submenuProductosTerminados(MateriaPrima inventario[], const RecetaHelado recetas[],
                                ProductoTerminado productos[], Fecha fechaActual) {
    int opcion;
    do {
        printf("\n--- PRODUCTOS TERMINADOS (EN KG) ---\n");
        mostrarProductosTerminados(productos);
        printf("1. Editar productos terminados (ajusta la materia prima)\n");
        printf("2. Eliminar producto (devuelve la materia prima al inventario)\n");
        printf("3. Volver a INVENTARIO\n");

        opcion = leerEnteroEnRango("Seleccione una opcion: ", 1, 3);

        switch (opcion) {
            case 1:
                editarProductoTerminado(inventario, recetas, productos, fechaActual);
                break;
            case 2:
                eliminarProductoTerminado(inventario, recetas, productos, fechaActual);
                break;
            case 3:
                break;
        }
    } while (opcion != 3);
}

void editarMateriaPrima(MateriaPrima inventario[], Fecha fechaActual) {
    char prompt[150];
    snprintf(prompt, sizeof(prompt),
             "\nSeleccione el numero de materia prima a editar (1 a %d) o 0 para cancelar: ", NUM_MATERIAS);
    int opcion = leerEnteroEnRango(prompt, 0, NUM_MATERIAS);

    if (opcion == 0) {
        return;
    }

    int idx = opcion - 1;
    printf("\n--- Editando: %s ---\n", inventario[idx].nombre);
    leerTextoNoVacio("Nuevo nombre del proveedor: ", inventario[idx].proveedor, MAX_NOMBRE);

    snprintf(prompt, sizeof(prompt), "Nuevo costo por kg de %s ($): ", inventario[idx].nombre);
    inventario[idx].costoPorKg = leerFloatMinimo(prompt, 0.0f, 0);

    snprintf(prompt, sizeof(prompt), "Nueva cantidad total en stock de %s (kg): ", inventario[idx].nombre);
    float nuevaCantidad = leerFloatMinimo(prompt, 0.0f, 0);

    inventario[idx].numLotes = 0;
    if (nuevaCantidad > 0.0f) {
        agregarALotes(&inventario[idx], nuevaCantidad, fechaActual);
    } else {
        inventario[idx].cantidadKg = 0.0f;
    }

    printf("\n[OK] %s actualizado -> Proveedor: %s | Costo: $%.2f/kg | Stock: %.2f kg.\n",
           inventario[idx].nombre, inventario[idx].proveedor,
           inventario[idx].costoPorKg, inventario[idx].cantidadKg);
}

void eliminarMateriaPrima(MateriaPrima inventario[]) {
    char prompt[150];
    snprintf(prompt, sizeof(prompt),
             "\nSeleccione el numero de materia prima a eliminar (1 a %d) o 0 para cancelar: ", NUM_MATERIAS);
    int opcion = leerEnteroEnRango(prompt, 0, NUM_MATERIAS);

    if (opcion == 0) {
        return;
    }

    int idx = opcion - 1;
    strcpy(inventario[idx].proveedor, "Sin asignar");
    inventario[idx].cantidadKg = 0.0f;
    inventario[idx].costoPorKg = 0.0f;
    inventario[idx].numLotes = 0;

    printf("\n[OK] Se elimino el proveedor, costo y stock de '%s' (Stock: 0.00 kg).\n", inventario[idx].nombre);
}

void revisarYDesecharVencidos(MateriaPrima inventario[], Fecha fechaActual) {
    int totalDesechados = 0;
    for (int i = 0; i < NUM_MATERIAS; i++) {
        int j = 0;
        while (j < inventario[i].numLotes) {
            if (estaVencido(inventario[i].lotes[j].fechaIngreso, fechaActual)) {
                printf("\n[DESECHADO] %.2f kg de %s (ingresado el %02d/%02d/%04d) supero los 2 meses.\n",
                       inventario[i].lotes[j].cantidadKg,
                       inventario[i].nombre,
                       inventario[i].lotes[j].fechaIngreso.dia,
                       inventario[i].lotes[j].fechaIngreso.mes,
                       inventario[i].lotes[j].fechaIngreso.anio);
                for (int k = j; k < inventario[i].numLotes - 1; k++) {
                    inventario[i].lotes[k] = inventario[i].lotes[k + 1];
                }
                inventario[i].numLotes--;
                totalDesechados++;
            } else {
                j++;
            }
        }
        recalcularStockTotal(&inventario[i]);
    }

    if (totalDesechados == 0) {
        printf("\n[OK] Revision de caducidad completa: no hay lotes vencidos (> 2 meses).\n");
    }
}

void editarProductoTerminado(MateriaPrima inventario[], const RecetaHelado recetas[],
                             ProductoTerminado productos[], Fecha fechaActual) {
    char prompt[150];
    snprintf(prompt, sizeof(prompt),
             "\nSeleccione el numero del helado a editar (1 a %d) o 0 para cancelar: ", NUM_RECETAS);
    int opcion = leerEnteroEnRango(prompt, 0, NUM_RECETAS);

    if (opcion == 0) {
        return;
    }

    int idx = opcion - 1;
    float cantidadAnterior = productos[idx].cantidadProducidaKg;

    snprintf(prompt, sizeof(prompt),
             "Nueva cantidad producida para '%s' (actual: %.2f kg): ",
             productos[idx].nombre, cantidadAnterior);
    float nuevaCantidad = leerFloatMinimo(prompt, 0.0f, 0);

    float diferencia = nuevaCantidad - cantidadAnterior;

    if (diferencia > 0.0f) {
        for (int i = 0; i < 3; i++) {
            int idxMateria = recetas[idx].indicesMateria[i];
            float requeridoExtra = recetas[idx].cantidadesPorKg[i] * diferencia;
            if (requeridoExtra > inventario[idxMateria].cantidadKg) {
                printf("\n[ERROR] Stock insuficiente de %s para aumentar %.2f kg.\n",
                       inventario[idxMateria].nombre, diferencia);
                printf("Requerido adicional: %.2f kg | Disponible: %.2f kg. Edicion cancelada.\n",
                       requeridoExtra, inventario[idxMateria].cantidadKg);
                return;
            }
        }
        for (int i = 0; i < 3; i++) {
            int idxMateria = recetas[idx].indicesMateria[i];
            float requeridoExtra = recetas[idx].cantidadesPorKg[i] * diferencia;
            descontarDeLotesFIFO(&inventario[idxMateria], requeridoExtra);
        }
        productos[idx].cantidadProducidaKg = nuevaCantidad;
        printf("\n[OK] '%s' aumentado a %.2f kg y se desconto la materia prima adicional.\n",
               productos[idx].nombre, nuevaCantidad);

    } else if (diferencia < 0.0f) {
        float cantidadDevuelta = -diferencia;
        for (int i = 0; i < 3; i++) {
            int idxMateria = recetas[idx].indicesMateria[i];
            float devolverKg = recetas[idx].cantidadesPorKg[i] * cantidadDevuelta;
            agregarALotes(&inventario[idxMateria], devolverKg, fechaActual);
        }
        productos[idx].cantidadProducidaKg = nuevaCantidad;
        printf("\n[OK] '%s' reducido a %.2f kg y se devolvio la materia prima sobrante al inventario.\n",
               productos[idx].nombre, nuevaCantidad);
    } else {
        printf("\n[!] La cantidad no cambio (%.2f kg).\n", nuevaCantidad);
    }
}

void eliminarProductoTerminado(MateriaPrima inventario[], const RecetaHelado recetas[],
                               ProductoTerminado productos[], Fecha fechaActual) {
    char prompt[150];
    snprintf(prompt, sizeof(prompt),
             "\nSeleccione el numero del helado a eliminar (1 a %d) o 0 para cancelar: ", NUM_RECETAS);
    int opcion = leerEnteroEnRango(prompt, 0, NUM_RECETAS);

    if (opcion == 0) {
        return;
    }

    int idx = opcion - 1;
    float cantidadADevolver = productos[idx].cantidadProducidaKg;

    if (cantidadADevolver <= 0.0f) {
        printf("\n[!] El helado '%s' ya tiene 0.00 kg. No hay nada que eliminar.\n", productos[idx].nombre);
        return;
    }

    for (int i = 0; i < 3; i++) {
        int idxMateria = recetas[idx].indicesMateria[i];
        float devolverKg = recetas[idx].cantidadesPorKg[i] * cantidadADevolver;
        agregarALotes(&inventario[idxMateria], devolverKg, fechaActual);
    }

    productos[idx].cantidadProducidaKg = 0.0f;

    printf("\n[OK] Se eliminaron %.2f kg de '%s' y se devolvio su materia prima al inventario.\n",
           cantidadADevolver, productos[idx].nombre);
}

void calcularCapacidadMaxima(const MateriaPrima inventario[], const RecetaHelado recetas[]) {
    printf("\n--- CALCULADORA DE CAPACIDAD MAXIMA DE PRODUCCION ---\n");
    printf("+------------+-----------------+----------------------+-----------------+\n");
    printf("| Sabor      | Maximo (kg)     | Ingrediente limitante| Costo Max ($)   |\n");
    printf("+------------+-----------------+----------------------+-----------------+\n");

    for (int i = 0; i < NUM_RECETAS; i++) {
        float maxKg = -1.0f;
        int idxLimitante = recetas[i].indicesMateria[0];
        float costoPorKg = 0.0f;

        for (int j = 0; j < 3; j++) {
            int idxMat = recetas[i].indicesMateria[j];
            float posible = inventario[idxMat].cantidadKg / recetas[i].cantidadesPorKg[j];
            costoPorKg += recetas[i].cantidadesPorKg[j] * inventario[idxMat].costoPorKg;

            if (maxKg < 0.0f || posible < maxKg) {
                maxKg = posible;
                idxLimitante = idxMat;
            }
        }

        printf("| %-10s | %11.2f kg  | %-20s | $%13.2f  |\n",
               recetas[i].nombre,
               maxKg,
               inventario[idxLimitante].nombre,
               maxKg * costoPorKg);
    }
    printf("+------------+-----------------+----------------------+-----------------+\n");
}

void buscarPorProveedor(const MateriaPrima inventario[]) {
    char busqueda[MAX_NOMBRE];
    printf("\n--- BUSQUEDA POR PROVEEDOR ---\n");
    leerTextoNoVacio("Ingrese el nombre del proveedor a buscar: ", busqueda, MAX_NOMBRE);

    int encontrados = 0;
    printf("\n+-----+----------------------+----------------------+---------------+------------+\n");
    printf("| #   | Materia Prima        | Proveedor            | Stock (kg)    | Costo/kg   |\n");
    printf("+-----+----------------------+----------------------+---------------+------------+\n");

    for (int i = 0; i < NUM_MATERIAS; i++) {
        if (contieneTextoInsensible(inventario[i].proveedor, busqueda)) {
            printf("| %-3d | %-20s | %-20s | %9.2f kg  | $%8.2f  |\n",
                   i + 1,
                   inventario[i].nombre,
                   inventario[i].proveedor,
                   inventario[i].cantidadKg,
                   inventario[i].costoPorKg);
            encontrados++;
        }
    }

    printf("+-----+----------------------+----------------------+---------------+------------+\n");
    if (encontrados == 0) {
        printf("[!] No se encontraron materias primas para el proveedor '%s'.\n", busqueda);
    }
}

void mostrarInventario(const MateriaPrima inventario[]) {
    float valorTotalInventario = 0.0f;

    printf("\n+-----+----------------------+----------------------+---------------+------------+-------------+------------+\n");
    printf("| #   | Materia Prima        | Proveedor            | Stock (kg)    | Costo/kg   | Valor Total | Ult. Fecha |\n");
    printf("+-----+----------------------+----------------------+---------------+------------+-------------+------------+\n");
    for (int i = 0; i < NUM_MATERIAS; i++) {
        float valor = inventario[i].cantidadKg * inventario[i].costoPorKg;
        valorTotalInventario += valor;

        if (inventario[i].numLotes > 0) {
            Fecha ult = inventario[i].lotes[inventario[i].numLotes - 1].fechaIngreso;
            printf("| %-3d | %-20s | %-20s | %9.2f kg  | $%8.2f  | $%9.2f  | %02d/%02d/%04d |\n",
                   i + 1, inventario[i].nombre, inventario[i].proveedor,
                   inventario[i].cantidadKg, inventario[i].costoPorKg, valor,
                   ult.dia, ult.mes, ult.anio);
        } else {
            printf("| %-3d | %-20s | %-20s | %9.2f kg  | $%8.2f  | $%9.2f  | Sin fecha  |\n",
                   i + 1, inventario[i].nombre, inventario[i].proveedor,
                   inventario[i].cantidadKg, inventario[i].costoPorKg, valor);
        }
    }
    printf("+-----+----------------------+----------------------+---------------+------------+-------------+------------+\n");
    printf("| %-79s | $%9.2f  |            |\n", "VALOR TOTAL DEL INVENTARIO", valorTotalInventario);
    printf("+---------------------------------------------------------------------------------+-------------+------------+\n");
}

void mostrarRecetas(const RecetaHelado recetas[], const MateriaPrima inventario[]) {
    printf("\n+------------+---------------+------------------------------+-----------+-----------+------------+----------+\n");
    printf("| Sabor      | Base lactea   | Pulpa / Pasta                | Azucar    | Tiempo/kg | Costo/kg   | Total    |\n");
    printf("+------------+---------------+------------------------------+-----------+-----------+------------+----------+\n");
    for (int i = 0; i < NUM_RECETAS; i++) {
        int idxPulpa = recetas[i].indicesMateria[1];
        float costoPorKg = 0.0f;

        for (int j = 0; j < 3; j++) {
            int idxMat = recetas[i].indicesMateria[j];
            costoPorKg += recetas[i].cantidadesPorKg[j] * inventario[idxMat].costoPorKg;
        }

        printf("| %-10s | %9.2f kg  | %5.2f kg %-19s | %5.2f kg  | %5.0f min | $%8.2f  | 1.00 kg  |\n",
               recetas[i].nombre,
               recetas[i].cantidadesPorKg[0],
               recetas[i].cantidadesPorKg[1],
               inventario[idxPulpa].nombre,
               recetas[i].cantidadesPorKg[2],
               recetas[i].tiempoPorKgMin,
               costoPorKg);
    }
    printf("+------------+---------------+------------------------------+-----------+-----------+------------+----------+\n");
}

void mostrarProductosTerminados(const ProductoTerminado productos[]) {
    printf("\n+-----+----------------------+--------------------+\n");
    printf("| #   | Helado               | Producido (kg)     |\n");
    printf("+-----+----------------------+--------------------+\n");
    for (int i = 0; i < NUM_RECETAS; i++) {
        printf("| %-3d | %-20s | %14.2f kg  |\n",
               i + 1, productos[i].nombre, productos[i].cantidadProducidaKg);
    }
    printf("+-----+----------------------+--------------------+\n");
}

// ==================== 3. FACTURACION ====================

void menuFacturacion(Cliente clientes[], int *totalClientes,
                     ProductoTerminado productos[],
                     Factura facturas[], int *totalFacturas,
                     Fecha fechaActual) {
    int opcion;
    do {
        printf("\n--- FACTURACION (Fecha: %02d/%02d/%04d) ---\n",
               fechaActual.dia, fechaActual.mes, fechaActual.anio);
        printf("1. Registrar cliente (ID y nombre)\n");
        printf("2. Egresos (descontar producto terminado y generar factura)\n");
        printf("3. Ver clientes registrados\n");
        printf("4. Ver facturas / egresos emitidos\n");
        printf("5. Volver al menu principal\n");

        opcion = leerEnteroEnRango("Seleccione una opcion: ", 1, 5);

        switch (opcion) {
            case 1:
                registrarCliente(clientes, totalClientes);
                break;
            case 2:
                registrarEgresoFactura(clientes, *totalClientes, productos, facturas, totalFacturas, fechaActual);
                break;
            case 3:
                mostrarClientes(clientes, *totalClientes);
                break;
            case 4:
                mostrarFacturas(facturas, *totalFacturas);
                break;
            case 5:
                printf("\nVolviendo al menu principal...\n");
                break;
        }
    } while (opcion != 5);
}

void registrarCliente(Cliente clientes[], int *totalClientes) {
    if (*totalClientes >= MAX_CLIENTES) {
        printf("\n[!] Limite maximo de clientes alcanzado.\n");
        return;
    }

    Cliente nuevo;
    printf("\n--- REGISTRO DE CLIENTE ---\n");

    while (1) {
        leerTextoNoVacio("Ingrese el ID del cliente (ej. cedula o codigo): ", nuevo.id, sizeof(nuevo.id));

        int duplicado = 0;
        for (int i = 0; i < *totalClientes; i++) {
            if (strcmp(clientes[i].id, nuevo.id) == 0) {
                duplicado = 1;
                break;
            }
        }

        if (duplicado) {
            printf("[ERROR] Ya existe un cliente registrado con el ID '%s'. Ingrese otro.\n", nuevo.id);
        } else {
            break;
        }
    }

    leerTextoNoVacio("Nombre del cliente: ", nuevo.nombre, sizeof(nuevo.nombre));

    clientes[*totalClientes] = nuevo;
    (*totalClientes)++;

    printf("\n[OK] Cliente '%s' (ID: %s) registrado correctamente.\n", nuevo.nombre, nuevo.id);
}

void mostrarClientes(const Cliente clientes[], int totalClientes) {
    if (totalClientes == 0) {
        printf("\n[!] No hay clientes registrados todavia.\n");
        return;
    }

    printf("\n+-----+--------------------+------------------------------+\n");
    printf("| #   | ID Cliente         | Nombre                       |\n");
    printf("+-----+--------------------+------------------------------+\n");
    for (int i = 0; i < totalClientes; i++) {
        printf("| %-3d | %-18s | %-28s |\n", i + 1, clientes[i].id, clientes[i].nombre);
    }
    printf("+-----+--------------------+------------------------------+\n");
}

void registrarEgresoFactura(const Cliente clientes[], int totalClientes,
                            ProductoTerminado productos[],
                            Factura facturas[], int *totalFacturas,
                            Fecha fechaActual) {
    if (totalClientes == 0) {
        printf("\n[ERROR] Debes registrar al menos un cliente antes de emitir un egreso/factura.\n");
        return;
    }

    if (*totalFacturas >= MAX_FACTURAS) {
        printf("\n[!] Limite maximo de facturas alcanzado.\n");
        return;
    }

    printf("\n--- EGRESOS Y FACTURACION ---\n");
    mostrarClientes(clientes, totalClientes);

    char prompt[150];
    snprintf(prompt, sizeof(prompt),
             "Seleccione el numero del cliente (1 a %d) o 0 para cancelar: ", totalClientes);
    int selCliente = leerEnteroEnRango(prompt, 0, totalClientes);
    if (selCliente == 0) {
        return;
    }
    selCliente--;

    mostrarProductosTerminados(productos);
    snprintf(prompt, sizeof(prompt),
             "Seleccione el numero del helado a egresar (1 a %d) o 0 para cancelar: ", NUM_RECETAS);
    int selProd = leerEnteroEnRango(prompt, 0, NUM_RECETAS);
    if (selProd == 0) {
        return;
    }
    selProd--;

    if (productos[selProd].cantidadProducidaKg <= 0.0f) {
        printf("\n[ERROR] No hay stock disponible de helado de '%s' (0.00 kg).\n",
               productos[selProd].nombre);
        return;
    }

    snprintf(prompt, sizeof(prompt),
             "Cantidad de '%s' a egresar en kg (disponible: %.2f kg): ",
             productos[selProd].nombre, productos[selProd].cantidadProducidaKg);
    float cantidadKg = leerFloatEnRango(prompt, 0.0f, productos[selProd].cantidadProducidaKg);

    snprintf(prompt, sizeof(prompt),
             "Precio de venta por kg de '%s' ($): ", productos[selProd].nombre);
    float precioPorKg = leerFloatMinimo(prompt, 0.0f, 1);

    // Descontar de productos terminados
    productos[selProd].cantidadProducidaKg -= cantidadKg;

    // Generar factura con codigo automatico
    Factura nueva;
    snprintf(nueva.codigo, sizeof(nueva.codigo), "FAC-%04d", *totalFacturas + 1);
    nueva.fecha = fechaActual;
    strcpy(nueva.idCliente, clientes[selCliente].id);
    strcpy(nueva.nombreCliente, clientes[selCliente].nombre);
    strcpy(nueva.sabor, productos[selProd].nombre);
    nueva.cantidadKg = cantidadKg;
    nueva.precioPorKg = precioPorKg;
    nueva.total = cantidadKg * precioPorKg;

    facturas[*totalFacturas] = nueva;
    (*totalFacturas)++;

    printf("\n==================================================\n");
    printf("               COMPROBANTE DE FACTURA             \n");
    printf("==================================================\n");
    printf("Codigo de facturacion : %s\n", nueva.codigo);
    printf("Fecha                 : %02d/%02d/%04d\n", nueva.fecha.dia, nueva.fecha.mes, nueva.fecha.anio);
    printf("Cliente               : %s (ID: %s)\n", nueva.nombreCliente, nueva.idCliente);
    printf("Producto              : Helado de %s\n", nueva.sabor);
    printf("Cantidad egresada     : %.2f kg\n", nueva.cantidadKg);
    printf("Precio por kg         : $%.2f\n", nueva.precioPorKg);
    printf("TOTAL A PAGAR         : $%.2f\n", nueva.total);
    printf("==================================================\n");
    printf("Stock restante de '%s': %.2f kg.\n",
           productos[selProd].nombre, productos[selProd].cantidadProducidaKg);
}

void mostrarFacturas(const Factura facturas[], int totalFacturas) {
    if (totalFacturas == 0) {
        printf("\n[!] Aun no se ha emitido ninguna factura.\n");
        return;
    }

    float totalVendido = 0.0f;
    printf("\n+----------+------------+--------------+--------------------+------------+-----------+------------+\n");
    printf("| Codigo   | Fecha      | ID Cliente   | Cliente            | Sabor      | Cant (kg) | Total ($)  |\n");
    printf("+----------+------------+--------------+--------------------+------------+-----------+------------+\n");
    for (int i = 0; i < totalFacturas; i++) {
        totalVendido += facturas[i].total;
        printf("| %-8s | %02d/%02d/%04d | %-12s | %-18s | %-10s | %6.2f kg | $%8.2f  |\n",
               facturas[i].codigo,
               facturas[i].fecha.dia, facturas[i].fecha.mes, facturas[i].fecha.anio,
               facturas[i].idCliente,
               facturas[i].nombreCliente,
               facturas[i].sabor,
               facturas[i].cantidadKg,
               facturas[i].total);
    }
    printf("+----------+------------+--------------+--------------------+------------+-----------+------------+\n");
    printf("| %-84s | $%8.2f  |\n", "TOTAL FACTURADO", totalVendido);
    printf("+--------------------------------------------------------------------------------------+------------+\n");
}