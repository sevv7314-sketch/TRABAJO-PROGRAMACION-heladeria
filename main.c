#include <stdio.h>
#include "funciones.h"

int main(void) {
    MateriaPrima inventario[NUM_MATERIAS];
    RecetaHelado recetas[NUM_RECETAS];
    ProductoTerminado productos[NUM_RECETAS];
    Cliente clientes[MAX_CLIENTES];
    Factura facturas[MAX_FACTURAS];
    int totalClientes = 0;
    int totalFacturas = 0;
    int opcion;

    inicializarDatos(inventario, recetas, productos);
    mostrarBienvenida();
    Fecha fechaActual = solicitarFechaActual();

    do {
        mostrarMenuPrincipal(fechaActual);
        printf("Seleccione una opcion: ");
        if (scanf("%d", &opcion) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {
            }
            printf("\n[!] Entrada no valida. Intente de nuevo.\n");
            opcion = 0;
            continue;
        }

        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }

        switch (opcion) {
            case 1:
                menuCreacion(inventario, recetas, productos, fechaActual);
                break;
            case 2:
                menuInventario(inventario, recetas, productos, &fechaActual);
                break;
            case 3:
                menuFacturacion(clientes, &totalClientes, productos, facturas, &totalFacturas, fechaActual);
                break;
            case 4:
                printf("\nSaliendo del sistema de produccion. Hasta pronto!\n");
                break;
            default:
                printf("\n[!] Opcion no valida. Intente de nuevo.\n");
        }
    } while (opcion != 4);

    return 0;
}