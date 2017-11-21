#ifndef LIB_FUNCIONESMS_H_
#define LIB_FUNCIONESMS_H_

#include "estructurasMS.h"

Tmaster * obtenerConfiguracionMaster(char * ruta);
void mostrarConfiguracion(Tmaster *master);

void inicioEjecucionTransformacion(int id);
void finEjecucionTransformacion(int id,int segundosDuracion);
void inicioEjecucionRL(int id);
void finEjecucionRL(int id,int segundosDuracion);
void removerTransformacionDeLista(int id);
void removerReduccionDeLista(int id);
void removerTransformacionFallida(int id);

void mostrarMetricasJob();

double getPromedioTransformaciones();
double getPromedioRL();

TpackInfoBloque *recibirInfoBloque(int sockYama);
int enviarScript(char * rutaScript,int sockDestino);

TreduccionLocal *recibirInfoReduccionLocal(int sockYama);


//-----Funciones Hilo Worker Transf//
int conectarseAWorkersTransformacion(t_list * bloquesTransformacion,int sockYama);
int conectarseAWorkerParaReplanificarTransformacion(TpackInfoBloque *infoBloque,int sockYama);
void hiloWorkerTransformacion(void *info);

int conectarseAWorkerParaReduccionLocal(TreduccionLocal *infoReduccion,int sockYama);
int conectarseAWorkerParaReduccionGlobal(TreduccionGlobal *infoReduccion,int sockYama);

TreduccionGlobal *recibirInfoReduccionGlobal(int sockYama);

TinfoAlmacenadoFinal *recibirInfoAlmacenadoFinal(int sockYama);

int conectarseAWorkerParaAlmacenamientoFinal(TinfoAlmacenadoFinal *infoAlmacenamiento,int sockYama);
void hiloWorkerAlmacenamientoFinal(void *info);

#endif
