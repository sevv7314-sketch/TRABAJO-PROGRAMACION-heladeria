#ifndef FUNCIONES_H
#define FUNCIONES_H

#define NUM_MATERIAS 7
#define NUM_RECETAS 5
#define MAX_NOMBRE 50
#define MAX_LOTES 30
#define MAX_CLIENTES 50
#define MAX_FACTURAS 50

typedef struct {
    int dia;
    int mes;
    int anio;
} Fecha;

typedef struct {
    Fecha fechaIngreso;
    float cantidadKg;
} Lote;

typedef struct {
    char nombre[MAX_NOMBRE];
    char proveedor[MAX_NOMBRE];
    float cantidadKg;
    float costoPorKg;
    Lote lotes[MAX_LOTES];
    int numLotes;
} MateriaPrima;

typedef struct {
    char nombre[MAX_NOMBRE];
    int indicesMateria[3];
    float cantidadesPorKg[3];
    float tiempoPorKgMin;
} RecetaHelado;

typedef struct {
    char nombre[MAX_NOMBRE];
    float cantidadProducidaKg;
} ProductoTerminado;

typedef struct {
    char id[20];
    char nombre[MAX_NOMBRE];
} Cliente;

typedef struct {
    char codigo[20];
    Fecha fecha;
    char idCliente[20];
    char nombreCliente[MAX_NOMBRE];
    char sabor[MAX_NOMBRE];
    float cantidadKg;
    float precioPorKg;
    float total;
} Factura;

void inicializarDatos(MateriaPrima inventario[], RecetaHelado recetas[], ProductoTerminado productos[]);
void mostrarBienvenida(void);
Fecha solicitarFechaActual(void);
void mostrarMenuPrincipal(Fecha fechaActual);

// 1. CREACION
void menuCreacion(MateriaPrima inventario[], const RecetaHelado recetas[],
                  ProductoTerminado productos[], Fecha fechaActual);
void ingresarStockMateriaPrima(MateriaPrima inventario[], Fecha fechaActual);
void producirProductoTerminado(MateriaPrima inventario[], const RecetaHelado recetas[],
                               ProductoTerminado productos[], Fecha fechaActual);

// 2. INVENTARIO y sus submenus
void menuInventario(MateriaPrima inventario[], const RecetaHelado recetas[],
                    ProductoTerminado productos[], Fecha *fechaActual);
void submenuInventarioMateriaPrima(MateriaPrima inventario[], Fecha *fechaActual);
void submenuProductosTerminados(MateriaPrima inventario[], const RecetaHelado recetas[],
                                ProductoTerminado productos[], Fecha fechaActual);

// Acciones de inventario y control
void mostrarInventario(const MateriaPrima inventario[]);
void editarMateriaPrima(MateriaPrima inventario[], Fecha fechaActual);
void eliminarMateriaPrima(MateriaPrima inventario[]);
void revisarYDesecharVencidos(MateriaPrima inventario[], Fecha fechaActual);
void mostrarRecetas(const RecetaHelado recetas[], const MateriaPrima inventario[]);
void mostrarProductosTerminados(const ProductoTerminado productos[]);
void editarProductoTerminado(MateriaPrima inventario[], const RecetaHelado recetas[],
                             ProductoTerminado productos[], Fecha fechaActual);
void eliminarProductoTerminado(MateriaPrima inventario[], const RecetaHelado recetas[],
                               ProductoTerminado productos[], Fecha fechaActual);
void calcularCapacidadMaxima(const MateriaPrima inventario[], const RecetaHelado recetas[]);
void buscarPorProveedor(const MateriaPrima inventario[]);

// 3. FACTURACION
void menuFacturacion(Cliente clientes[], int *totalClientes,
                     ProductoTerminado productos[],
                     Factura facturas[], int *totalFacturas,
                     Fecha fechaActual);
void registrarCliente(Cliente clientes[], int *totalClientes);
void mostrarClientes(const Cliente clientes[], int totalClientes);
void registrarEgresoFactura(const Cliente clientes[], int totalClientes,
                            ProductoTerminado productos[],
                            Factura facturas[], int *totalFacturas,
                            Fecha fechaActual);
void mostrarFacturas(const Factura facturas[], int totalFacturas);

#endif