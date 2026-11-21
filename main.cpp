#include <sqlite3.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Excepciones propias
class ArchivoNoEncontrado : public std::runtime_error {
public:
    explicit ArchivoNoEncontrado(const std::string& ruta)
        : std::runtime_error("No se encontro el archivo '" + ruta + "'") {}
};

class ErrorBaseDatos : public std::runtime_error {
public:
    explicit ErrorBaseDatos(const std::string& msg) : std::runtime_error(msg) {}
};

// Lee las tareas de un archivo de texto
class LectorTareas {
public:
    static std::vector<std::string> leer(const std::string& ruta) {
        std::ifstream archivo(ruta);
        if (!archivo) throw ArchivoNoEncontrado(ruta);
        std::vector<std::string> tareas;
        std::string linea;
        while (std::getline(archivo, linea))
            if (!linea.empty()) tareas.push_back(linea);
        return tareas;
    }
};

// Conexion a SQLite que lanza excepciones si algo falla
class BaseDatos {
public:
    explicit BaseDatos(const std::string& ruta) {
        // Si la ruta no es valida (ej. una carpeta que no existe) la conexion falla
        int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        if (sqlite3_open_v2(ruta.c_str(), &db, flags, nullptr) != SQLITE_OK) {
            std::string msg = sqlite3_errmsg(db);
            sqlite3_close(db);
            throw ErrorBaseDatos("No se pudo conectar a '" + ruta + "': " + msg);
        }
        ejecutar("CREATE TABLE IF NOT EXISTS tareas (id INTEGER PRIMARY KEY, titulo TEXT)");
    }
    ~BaseDatos() { sqlite3_close(db); }

    void insertar(const std::string& titulo) {
        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db, "INSERT INTO tareas (titulo) VALUES (?)", -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, titulo.c_str(), -1, SQLITE_TRANSIENT);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE) throw ErrorBaseDatos(sqlite3_errmsg(db));
    }

    int contar() {
        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM tareas", -1, &stmt, nullptr);
        sqlite3_step(stmt);
        int total = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return total;
    }

private:
    sqlite3* db = nullptr;

    void ejecutar(const std::string& sql) {
        char* error = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error) != SQLITE_OK) {
            std::string msg = error;
            sqlite3_free(error);
            throw ErrorBaseDatos(msg);
        }
    }
};

int main(int argc, char* argv[]) {
    std::string rutaArchivo = argc > 1 ? argv[1] : "tareas.txt";
    std::string rutaBD = argc > 2 ? argv[2] : "tareas.db";

    try {
        std::vector<std::string> tareas = LectorTareas::leer(rutaArchivo);
        std::cout << "Se leyeron " << tareas.size() << " tareas de " << rutaArchivo << "\n";

        BaseDatos db(rutaBD);
        for (const auto& t : tareas) db.insertar(t);
        std::cout << "Guardadas en la BD. Total de tareas en la tabla: " << db.contar() << "\n";
    } catch (const ArchivoNoEncontrado& e) {
        std::cout << "[Error de archivo] " << e.what() << ". Revisa la ruta e intenta de nuevo.\n";
        return 1;
    } catch (const ErrorBaseDatos& e) {
        std::cout << "[Error de base de datos] " << e.what() << "\n";
        return 2;
    } catch (const std::exception& e) {
        std::cout << "[Error inesperado] " << e.what() << "\n";
        return 3;
    }

    std::cout << "El programa termino correctamente.\n";
    return 0;
}
