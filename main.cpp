#include <iostream>

#include <fstream>

#include <stdexcept>

#include <ctime>

#include <vector>

#include <string>

#include <sstream>

#include <iomanip>

#include <chrono>

#include <thread>



// ============ JERARQUÍA DE EXCEPCIONES PERSONALIZADAS ============



class MathException : public std::runtime_error {

public:

MathException(const std::string& msg) : std::runtime_error(msg) {}

};



class DivisionByZeroException : public MathException {

public:

DivisionByZeroException()

: MathException("Error: División entre cero detectada.") {}

};



class NegativeNumberException : public MathException {

public:

NegativeNumberException()

: MathException("Error: Número negativo no permitido en esta operación.") {}

};



class InvalidInputException : public std::runtime_error {

public:

InvalidInputException()

: std::runtime_error("Error: Entrada no numérica detectada.") {}

};



// ============ SISTEMA DE LOGGING AVANZADO ============



class Logger {

private:

std::ofstream logfile;

std::string filename;


std::string getCurrentTimestamp() {

auto now = std::chrono::system_clock::now();

auto time = std::chrono::system_clock::to_time_t(now);

std::stringstream ss;

ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");

return ss.str();

}


public:

enum LogLevel {

INFO,

WARNING,

ERROR,

CRITICAL,

DEBUG

};


Logger(const std::string& fname) : filename(fname) {

logfile.open(filename, std::ios::app);

if (!logfile.is_open()) {

throw std::runtime_error("No se pudo abrir el archivo de log: " + filename);

}

log(INFO, "Sistema iniciado");

}


~Logger() {

log(INFO, "Sistema finalizado");

if (logfile.is_open()) logfile.close();

}


void log(LogLevel level, const std::string& message) {

std::string levelStr;

switch(level) {

case INFO: levelStr = "INFO"; break;

case WARNING: levelStr = "WARNING"; break;

case ERROR: levelStr = "ERROR"; break;

case CRITICAL: levelStr = "CRITICAL"; break;

case DEBUG: levelStr = "DEBUG"; break;

}


logfile << "[" << getCurrentTimestamp() << "] "

<< "[" << levelStr << "] "

<< message << std::endl;

logfile.flush();

}


void logException(const std::exception& ex) {

log(ERROR, std::string("Excepción capturada: ") + ex.what());

}


void logMetrics(int totalOps, int successOps, int failedOps) {

std::stringstream ss;

ss << "Métricas - Total: " << totalOps

<< " | Exitosas: " << successOps

<< " | Fallidas: " << failedOps

<< " | Tasa de éxito: " << (totalOps > 0 ? (successOps * 100.0 / totalOps) : 0) << "%";

log(INFO, ss.str());

}

};



// ============ SISTEMA DE MONITOREO ============



class SystemMonitor {

private:

Logger& logger;

int totalOperations;

int successfulOperations;

int failedOperations;


public:

SystemMonitor(Logger& log)

: logger(log), totalOperations(0), successfulOperations(0), failedOperations(0) {}


void recordSuccess() {

totalOperations++;

successfulOperations++;

}


void recordFailure() {

totalOperations++;

failedOperations++;

}


void showMetrics() {

std::cout << "\n========== MÉTRICAS DEL SISTEMA ==========" << std::endl;

std::cout << "Total de operaciones: " << totalOperations << std::endl;

std::cout << "Operaciones exitosas: " << successfulOperations << std::endl;

std::cout << "Operaciones fallidas: " << failedOperations << std::endl;

if (totalOperations > 0) {

double successRate = (successfulOperations * 100.0) / totalOperations;

std::cout << "Tasa de éxito: " << std::fixed << std::setprecision(2)

<< successRate << "%" << std::endl;

}

std::cout << "==========================================" << std::endl;


logger.logMetrics(totalOperations, successfulOperations, failedOperations);

}

};



// ============ FUNCIONES MATEMÁTICAS ============



double dividir(double a, double b) {

if (b == 0) throw DivisionByZeroException();

if (a < 0 || b < 0) throw NegativeNumberException();

return a / b;

}



double raizCuadrada(double num) {

if (num < 0) throw NegativeNumberException();

return sqrt(num);

}



// ============ SIMULACIÓN DE MONITOREO EN TIEMPO REAL ============



void procesarListaNumeros(const std::vector<std::pair<double, double>>& pares,

Logger& logger, SystemMonitor& monitor) {

std::cout << "\n===== PROCESAMIENTO EN TIEMPO REAL =====" << std::endl;

logger.log(Logger::INFO, "Iniciando procesamiento de lista de números");


for (size_t i = 0; i < pares.size(); i++) {

double a = pares[i].first;

double b = pares[i].second;


std::cout << "\nOperación #" << (i + 1) << ": " << a << " / " << b << std::endl;

logger.log(Logger::DEBUG, "Procesando operación: " + std::to_string(a) + " / " + std::to_string(b));


try {

double resultado = dividir(a, b);

std::cout << "✓ Resultado: " << resultado << std::endl;

logger.log(Logger::INFO, "Operación exitosa. Resultado: " + std::to_string(resultado));

monitor.recordSuccess();

}

catch (const DivisionByZeroException& ex) {

std::cerr << "✗ " << ex.what() << std::endl;

logger.logException(ex);

monitor.recordFailure();

}

catch (const NegativeNumberException& ex) {

std::cerr << "✗ " << ex.what() << std::endl;

logger.logException(ex);

monitor.recordFailure();

}

catch (const std::exception& ex) {

std::cerr << "✗ Excepción inesperada: " << ex.what() << std::endl;

logger.logException(ex);

monitor.recordFailure();

}


// Simular procesamiento en tiempo real

std::this_thread::sleep_for(std::chrono::milliseconds(500));

}


logger.log(Logger::INFO, "Procesamiento de lista completado");

}



// ============ FUNCIÓN PRINCIPAL ============



int main() {

try {

Logger logger("system.log");

SystemMonitor monitor(logger);


std::cout << "========================================" << std::endl;

std::cout << " SISTEMA DE MONITOREO Y LOGGING" << std::endl;

std::cout << "========================================" << std::endl;


// PRUEBA 1: División básica con error

std::cout << "\n--- PRUEBA 1: División entre cero ---" << std::endl;

try {

logger.log(Logger::INFO, "Intentando dividir 10 / 0");

double resultado = dividir(10, 0);

std::cout << "Resultado: " << resultado << std::endl;

monitor.recordSuccess();

}

catch (const DivisionByZeroException& ex) {

std::cerr << "✗ " << ex.what() << std::endl;

logger.logException(ex);

monitor.recordFailure();

}


// PRUEBA 2: Números negativos

std::cout << "\n--- PRUEBA 2: Números negativos ---" << std::endl;

try {

logger.log(Logger::INFO, "Intentando dividir -5 / 2");

double resultado = dividir(-5, 2);

std::cout << "Resultado: " << resultado << std::endl;

monitor.recordSuccess();

}

catch (const NegativeNumberException& ex) {

std::cerr << "✗ " << ex.what() << std::endl;

logger.logException(ex);

monitor.recordFailure();

}


// PRUEBA 3: Operación exitosa

std::cout << "\n--- PRUEBA 3: División válida ---" << std::endl;

try {

logger.log(Logger::INFO, "Intentando dividir 100 / 5");

double resultado = dividir(100, 5);

std::cout << "✓ Resultado: " << resultado << std::endl;

logger.log(Logger::INFO, "Operación exitosa: 100 / 5 = 20");

monitor.recordSuccess();

}

catch (const std::exception& ex) {

std::cerr << "✗ " << ex.what() << std::endl;

logger.logException(ex);

monitor.recordFailure();

}


// PRUEBA 4: Monitoreo en tiempo real con lista de operaciones

std::vector<std::pair<double, double>> listaOperaciones = {

{100, 5}, // Válida

{50, 0}, // Error: división por cero

{81, 9}, // Válida

{-10, 2}, // Error: número negativo

{200, 10}, // Válida

{7, 0}, // Error: división por cero

{144, 12}, // Válida

{-50, -5} // Error: números negativos

};


procesarListaNumeros(listaOperaciones, logger, monitor);


// Mostrar métricas finales

monitor.showMetrics();


std::cout << "\n✓ Verifica el archivo 'system.log' para ver los registros completos." << std::endl;

std::cout << "\n========================================" << std::endl;

std::cout << " EJECUCIÓN COMPLETADA" << std::endl;

std::cout << "========================================" << std::endl;

}

catch (const std::exception& ex) {

std::cerr << "Error crítico del sistema: " << ex.what() << std::endl;

return 1;

}


return 0;

} 
