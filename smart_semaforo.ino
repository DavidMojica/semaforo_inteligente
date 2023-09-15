//--------------------BLOQUE DE INFORMACIÓN----------------------//
//***************************************************************//
//                  .                        *                *  //
//                *          MADE BY            .                //
//        *           _____,    _..-=-=-=-=-====--,  *   .       //
//       _         _.'a   /  .-',___,..=--=--==-'`               //
//      ((        ( _     \ /  //___/-=---=----'                 //
//       `         ` `\    /  //---/--==----=-'                  //
//             ,-.    | / \_//-_.'==-==---='       *             //
//      *     (.-.`\  | |'../-'=-=-=-=--'                   .    //
//      .      (' `\`\| //_|-\.`;-~````~,        _               //
//                   \ | \_,_,_\.'        \     .'_`\            //
//     *             `\            ,    , \    || `\\            //
//                 .   \    /   _.--\    \ '._.'/  / |         * //
//                     /  /`---'   \ \   |`'---'   \/            //
//          *         / /'          \ ;-. \                      //
//                 __/ /      *    __) \ ) `|            *       //
//    .           ((='--;)      .  (,___/(,_/                    //
//                *                                 *      .   . //
//       *                -David Mojica S.E                      //
//          GitHub: https://github.com/DavidMojicaDev     .      //
//                     -Jhojan Florez Lopez S.E                  //
//     .    GitHub:  https://github.com/Jhojan006      *         //
//-------------------------------------------------------------- //
//                         15/09/23 (Viernes)                    //
//************************************************************** //

/*------------------------QUÉ ES ESTO----------------------------//
Este semaforo inteligente es capaz de detectar cuando algo está muy próximo a él mediante un sensor de ultrasonido
y en base a esto toma desiciones.

/------------------------MODO DE USO-----------------------------//
Usted mismo puede interactuar con el sensor de ultrasonido (haciéndo click sobre el HC-SR04 durante la ejecución) para variar su proximidad al semáforo.
El programa también irá generando aleatoriamente peatones, simulando una calle con un tránsito peatonal normal y el sensor los irá detectando.
-->Si los peatones o usted (a través de la barra del sensor) se acercan demasiado (50cm o menos actualmente) el semaforo empezará a cambiar hacia el rojo
para permitir el paso peatonal a través de la calle/avenida.
-->El semaforo despues de haber cambiado entrará en enfriamiento impidiendo volver a cambiar hacia el rojo inmediatamente acabó de cambiar, 
porque también se tiene que permitir el flujo vehicular y también debemos de prevenir ataques de "gente graciosa" evitando así que el semaforo esté permanentemente en rojo para los carros (actualmente el enfriamiento es de 3000ms). */

/*-------------------Información del codigo---------------------//
Nos arriesgamos a resolver el semaforo a través del paradigma programación orientada objetos, usando conceptos como:
Clases, herencia, polimorfismo, sobrecargas, métodos, variables volátiles.
Lo hicimos motivados principalmente por aprender a manejar bien este nuevo lenguaje de programación (arduino, que es como c++ pero no es igual).

Ventajas:
-Es una opción mucho más escalable, ya que con este código se puede poner a funcionar correctamente uno o 5 o 20 semaforos simplemente instanciando la clase semaforo sin tener que desarrollar codigo adicional.
-Control total sobre los dispositivos, cuando vuelves un objeto físico en un objeto se puede controlar totalmente, desde su inicializacion hasta el fin de su ejecución en pocas líneas.
-Bacano 👍

Desventajas:
-Código más extenso, pero más optimizado.

/*Índice//            Línea
---->Objetos<----    : 64
Sensor de ultrasonido: 68
Peatón               : 99
Semaforo             : 112
Semaforo Grande      : 138

---->Codigo del programa<----
Variables            : 221
Inicio del programa  : 258

/*------------------------------------------------------------------//
//-----------------------------Objetos------------------------------//
//------------------------------------------------------------------*/
//Sensor de ultrasonido HC-SR04.
class SensorUltrasonido{
    // Constructor
  public:
    SensorUltrasonido(int trigger, int echo) : trigger_(trigger), echo_(echo){
        pinMode(trigger_, OUTPUT);
        pinMode(echo_, INPUT);
    }

    // Metodos
    float medir_distancia_CM(){
      digitalWrite(trigger_, LOW);
      delayMicroseconds(2);
      digitalWrite(trigger_, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigger_, LOW);
      return pulseIn(echo_, HIGH) * (0.034317 / 2);
    }
    //Sobrecarga
    float medir_distancia_CM(int peaton_distancia){
      Serial.print("SENSOR: Peatón (generado por el sistema) detectado a ");
      Serial.print(peaton_distancia);
      Serial.print(" cms ->");
      return 0;
    }
    
  protected:
    int trigger_;
    int echo_;
};

//Un peatón cualquiera que camina por la acera y es detectado por el sensor de ultrasonido.
class Peaton{
    // Constructor
  public:
    Peaton(int proximidad_al_sensor) : proximidad_al_sensor_(proximidad_al_sensor) {}
    // Metodos:
    void cruzar(){
        Serial.println("PEATÓN: El peatón cruzó la calle");
    }
  public:
    int proximidad_al_sensor_;
};

//La clase base semaforo. En nuestro contexto, se ajusta al semaforo peatonal.
class Semaforo{
    // Constructor
  public:
    Semaforo(int ledRojo, int ledVerde) : ledRojo_(ledRojo), ledVerde_(ledVerde){
        pinMode(ledRojo_, OUTPUT);
        pinMode(ledVerde_, OUTPUT);
    }

    // Métodos:
    virtual void inicializar(){ // Método apto para que se le aplique polimorfismo
        digitalWrite(ledRojo_, HIGH);
        digitalWrite(ledVerde_, LOW);
    }

    virtual bool ciclo(int x = 0, int y = 0){
        digitalWrite(ledRojo_, LOW);
        digitalWrite(ledVerde_, HIGH);
        return true;
    }

  protected:
    int ledRojo_;
    int ledVerde_;
};

//Semaforo vehicular. Hereda los rasgos de semaforo pero tiene muchas características adicionales.
class SemaforoGrande : Semaforo{
    
  public:
    SemaforoGrande(int ledRojo, int ledAmarillo, int ledVerde,
                   int tiempo_en_rojo, int tiempo_en_amarillo, int tiempo_en_verde, Semaforo& semaforo_peatones) : Semaforo(ledRojo, ledVerde), ledAmarillo_(ledAmarillo),
                                                                                                                   tiempo_en_rojo_(tiempo_en_rojo), tiempo_en_amarillo_(tiempo_en_amarillo), tiempo_en_verde_(tiempo_en_verde), semaforo_peatones_(semaforo_peatones), ciclo_activo_(false), tiempo_ultimo_ciclo_(0){
        pinMode(ledAmarillo_, OUTPUT);
        pinMode(ledRojo_, OUTPUT);
        pinMode(ledVerde_, OUTPUT);
    }
    void initMsj(){ Serial.println("SEMAFORO: [Encendido y funcionando]."); }
    // Metodos:
    void inicializar() override
    { // Polimorfismo en c++
        digitalWrite(ledVerde_, HIGH);
        digitalWrite(ledAmarillo_, LOW);
        digitalWrite(ledRojo_, LOW);
        ciclo_activo_ = false; 
    }

    bool ciclo(int tiempo, int tiempo_de_enfriamiento) override
    {
      // Verificar si ha pasado suficiente tiempo desde el último ciclo
      int judge = tiempo + tiempo_de_enfriamiento;
        if (judge >= 0){
          ciclo_activo_ = true;
          tiempo_ultimo_ciclo_ = tiempo;
          Serial.println(" SEMAFORO: Cambiando luces...");
          Serial.print("Tiempos de cambio - [Verde]:");
          Serial.print(tiempo_en_verde_/1000.0);
          Serial.print("s");
          delay(tiempo_en_verde_);
          // En amarillo
          digitalWrite(ledVerde_, LOW);
          digitalWrite(ledAmarillo_, HIGH);
          Serial.print(" - [Amarillo]: ");
          Serial.print(tiempo_en_amarillo_/1000.0);
          Serial.print("s");
          delay(tiempo_en_amarillo_);
          // En rojo
          digitalWrite(ledAmarillo_, LOW);
          digitalWrite(ledRojo_, HIGH);
          Serial.print("- [Rojo]: ");
          Serial.print(tiempo_en_rojo_/1000.0);
          Serial.println("s");
          semaforo_peatones_.ciclo();
          delay(tiempo_en_rojo_);
          // Reset a verde
          semaforo_peatones_.inicializar();
          inicializar();
          return true;
        } 
        else{
          Serial.print("SEMAFORO: [En enfriamiento] ");
          Serial.print(judge/1000.0);
          Serial.println("s restantes.");
          return false;
        }
    }

    bool evaluar(int distancia_persona, int distancia_cerca, Peaton &peaton, int tiempo_ejecucion, int tiempo_de_enfriamiento){
      if(distancia_persona <= distancia_cerca && !ciclo_activo_){
        if(ciclo(tiempo_ejecucion, tiempo_de_enfriamiento)){
          peaton.cruzar();
          return true;
        } else return false;
      } else {
        Serial.println(" SEMAFORO: Sin cambios...");
        return false;
      }
    }

  private:
    int ledAmarillo_;
    int tiempo_en_rojo_;
    int tiempo_en_amarillo_;
    int tiempo_en_verde_;
    Semaforo& semaforo_peatones_;
    unsigned long tiempo_ultimo_ciclo_;
  public:
    bool ciclo_activo_;
};

/*------------------------------------------------------------------//
//-----------------------Variables globales-------------------------//
//------------------------------------------------------------------*/
/*--Conexiones al Arduíno--*/
const int conn_led_rojo_2         = 3;
const int conn_led_verde_2        = 4;

const int conn_led_rojo_1         = 7;
const int conn_led_amarillo_1     = 6;
const int conn_led_verde_1        = 5;

const int conn_ultrasonic_trigger = 8;
const int conn_ultrasonic_echo    = 9; 

/*--Variables de control--*/
//--Semaforo--//
const int tiempo_en_rojo          = 2000; //ms
const int tiempo_en_amarillo      = 1000; //ms
const int tiempo_en_verde         = 2000; //ms
const int tiempo_de_enfriamiento  = -3000; //ms. Tiempo que tiene que pasar antes de que el semaforo pueda volver a cambiar de verde hacia rojo despues de haber cambiado.

//--Ejecución--//
const int cerca                        = 50; //cm
const int tiempo_delay                 = 200;
volatile unsigned long tiempo_contador = 3000;
const long tiempo_contador_            = 3000;
const int valor_permitido              = 10000;

//--Peatones--//
const int probabilidad_generacion = 5;  //(%)
const int rango_maximo_sensor     = 400; //cm

/*--Instancias de los objetos--*/
Semaforo semaforo_peatones(conn_led_rojo_2, conn_led_verde_2);
SensorUltrasonido sensor_ultrasonido(conn_ultrasonic_trigger, conn_ultrasonic_echo);
SemaforoGrande semaforo_carros(conn_led_rojo_1, conn_led_amarillo_1, conn_led_verde_1, tiempo_en_rojo, tiempo_en_amarillo, tiempo_en_verde, semaforo_peatones);

/*------------------------------------------------------------------//
//------------------------------Código------------------------------//
//------------------------------------------------------------------*/
/*--Configuración--*/
void setup(){
    // Configurar la velocidad de transmisión de datos en 9600bps.
    Serial.begin(9600);

    // Inicializamos los semaforos. Ya no es necesario inicializar los pines del sensor porque estos se inicializan cuando el sensor es construido.
    semaforo_carros.initMsj();
    semaforo_carros.inicializar();
    semaforo_peatones.inicializar();   
}

void loop(){
  //Detector de ultrasonido MANUAL.
  int distancia_usuario = sensor_ultrasonido.medir_distancia_CM();
  if(distancia_usuario <= cerca){
    Serial.print("Fuiste detectado muy cerca del sensor a ");
    Serial.print(distancia_usuario);
    Serial.print(" cms. -->");
    if(semaforo_carros.ciclo(tiempo_contador, tiempo_de_enfriamiento)){
      Serial.println("Tú: *Cruzaste la calle*");
      tiempo_contador = 0;
    } 
  }

  //Codigo adicional para "spawnear" otros peatones y medir su proximidad automáticamente, simulando una acera en una calle normal.
  //Actualmente un peatón tiene un 5% de chance de spawnear a una distancia entre 0 y 400 cm con cada iteración (200ms).
  if(random(100) <= probabilidad_generacion){
    Peaton peaton(random(rango_maximo_sensor));
    sensor_ultrasonido.medir_distancia_CM(peaton.proximidad_al_sensor_);
    tiempo_contador = (semaforo_carros.evaluar(peaton.proximidad_al_sensor_, cerca, peaton, tiempo_contador, tiempo_de_enfriamiento)) ? 0 : tiempo_contador;
  }
  //Expresión ternaria para reiniciar el tiempo del contador a su valor original si este excede el valor permitido ms. Esto se hace para evitar que resulte en valores muy grandes, es innecesario.
  tiempo_contador = (tiempo_contador >= valor_permitido) ? tiempo_contador_ : tiempo_contador += tiempo_delay;
  delay(tiempo_delay);
} //End