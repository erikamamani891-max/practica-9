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
#include <cmath> // Necesario para sqrt

// Usamos el namespace std para evitar el prefijo std::
using namespace std;

// ============ JERARQUÍA DE EXCEPCIONES PERSONALIZADAS ============

class MathException : public runtime_error {
public:
    MathException(const string& msg) : runtime_error(msg) {}
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

class InvalidInputException : public runtime_error {
public:
    InvalidInputException()
        : runtime_error("Error: Entrada no numérica detectada.") {}
};

// ============ SISTEMA DE LOGGING AVANZADO ============

class Logger {
private:
    ofstream logfile;
    string filename;

    string getCurrentTimestamp() {
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        stringstream ss;
        // Se usa std::put_time porque put_time no es parte del namespace global de C++ y requiere el prefijo
        // Aunque el using namespace std; está activo, algunos elementos de <iomanip> como put_time
        // pueden requerir calificación si la implementación del compilador es estricta o si se usan junto a localtime.
        ss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
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

    Logger(const string& fname) : filename(fname) {
        logfile.open(filename, ios::app);
        if (!logfile.is_open()) {
            throw runtime_error("No se pudo abrir el archivo de log: " + filename);
        }
        log(INFO, "Sistema iniciado");
    }

    ~Logger() {
        log(INFO, "Sistema finalizado");
        if (logfile.is_open()) logfile.close();
    }

    void log(LogLevel level, const string& message) {
        string levelStr;
        switch(level) {
            case INFO:     levelStr = "INFO"; break;
            case WARNING:  levelStr = "WARNING"; break;
            case ERROR:    levelStr = "ERROR"; break;
            case CRITICAL: levelStr = "CRITICAL"; break;
            case DEBUG:    levelStr = "DEBUG"; break;
        }

        logfile << "[" << getCurrentTimestamp() << "] "
                << "[" << levelStr << "] "
                << message << endl;
        logfile.flush();
    }

    void logException(const exception& ex) {
        log(ERROR, string("Excepción capturada: ") + ex.what());
    }

    void logMetrics(int totalOps, int successOps, int failedOps) {
        stringstream ss;
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
        cout << "\n========== MÉTRICAS DEL SISTEMA ==========" << endl;
        cout << "Total de operaciones: " << totalOperations << endl;
        cout << "Operaciones exitosas: " << successfulOperations << endl;
        cout << "Operaciones fallidas: " << failedOperations << endl;
        if (totalOperations > 0) {
            double successRate = (successfulOperations * 100.0) / totalOperations;
            cout << "Tasa de éxito: " << fixed << setprecision(2)
                      << successRate << "%" << endl;
        }
        cout << "==========================================" << endl;

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

void procesarListaNumeros(const vector<pair<double, double>>& pares,
                          Logger& logger, SystemMonitor& monitor) {
    cout << "\n===== PROCESAMIENTO EN TIEMPO REAL =====" << endl;
    logger.log(Logger::INFO, "Iniciando procesamiento de lista de números");

    for (size_t i = 0; i < pares.size(); i++) {
        double a = pares[i].first;
        double b = pares[i].second;

        cout << "\nOperación #" << (i + 1) << ": " << a << " / " << b << endl;
        logger.log(Logger::DEBUG, "Procesando operación: " + to_string(a) + " / " + to_string(b));

        try {
            double resultado = dividir(a, b);
            cout << "✓ Resultado: " << resultado << endl;
            logger.log(Logger::INFO, "Operación exitosa. Resultado: " + to_string(resultado));
            monitor.recordSuccess();
        }
        catch (const DivisionByZeroException& ex) {
            cerr << "✗ " << ex.what() << endl;
            logger.logException(ex);
            monitor.recordFailure();
        }
        catch (const NegativeNumberException& ex) {
            cerr << "✗ " << ex.what() << endl;
            logger.logException(ex);
            monitor.recordFailure();
        }
        catch (const exception& ex) {
            cerr << "✗ Excepción inesperada: " << ex.what() << endl;
            logger.logException(ex);
            monitor.recordFailure();
        }

        // Simular procesamiento en tiempo real
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    logger.log(Logger::INFO, "Procesamiento de lista completado");
}

// ============ FUNCIÓN PRINCIPAL ============

int main() {
    try {
        Logger logger("system.log");
        SystemMonitor monitor(logger);

        cout << "========================================" << endl;
        cout << "  SISTEMA DE MONITOREO Y LOGGING" << endl;
        cout << "========================================" << endl;

        // PRUEBA 1: División básica con error
        cout << "\n--- PRUEBA 1: División entre cero ---" << endl;
        try {
            logger.log(Logger::INFO, "Intentando dividir 10 / 0");
            double resultado = dividir(10, 0);
            cout << "Resultado: " << resultado << endl;
            monitor.recordSuccess();
        }
        catch (const DivisionByZeroException& ex) {
            cerr << "✗ " << ex.what() << endl;
            logger.logException(ex);
            monitor.recordFailure();
        }

        // PRUEBA 2: Números negativos
        cout << "\n--- PRUEBA 2: Números negativos ---" << endl;
        try {
            logger.log(Logger::INFO, "Intentando dividir -5 / 2");
            double resultado = dividir(-5, 2);
            cout << "Resultado: " << resultado << endl;
            monitor.recordSuccess();
        }
        catch (const NegativeNumberException& ex) {
            cerr << "✗ " << ex.what() << endl;
            logger.logException(ex);
            monitor.recordFailure();
        }

        // PRUEBA 3: Operación exitosa
        cout << "\n--- PRUEBA 3: División válida ---" << endl;
        try {
            logger.log(Logger::INFO, "Intentando dividir 100 / 5");
            double resultado = dividir(100, 5);
            cout << "✓ Resultado: " << resultado << endl;
            logger.log(Logger::INFO, "Operación exitosa: 100 / 5 = 20");
            monitor.recordSuccess();
        }
        catch (const exception& ex) {
            cerr << "✗ " << ex.what() << endl;
            logger.logException(ex);
            monitor.recordFailure();
        }

        // PRUEBA 4: Monitoreo en tiempo real con lista de operaciones
        vector<pair<double, double>> listaOperaciones = {
            {100, 5},    // Válida
            {50, 0},     // Error: división por cero
            {81, 9},     // Válida
            {-10, 2},    // Error: número negativo
            {200, 10},   // Válida
            {7, 0},      // Error: división por cero
            {144, 12},   // Válida
            {-50, -5}    // Error: números negativos
        };

        procesarListaNumeros(listaOperaciones, logger, monitor);

        // Mostrar métricas finales
        monitor.showMetrics();

        cout << "\n✓ Verifica el archivo 'system.log' para ver los registros completos." << endl;
        cout << "\n========================================" << endl;
        cout << "  EJECUCIÓN COMPLETADA" << endl;
        cout << "========================================" << endl;
    }
    catch (const exception& ex) {
        cerr << "Error crítico del sistema: " << ex.what() << endl;
        return 1;
    }

    return 0;
}
