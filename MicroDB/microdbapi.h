#ifndef MICRODB_H
#define MICRODB_H

#include <stddef.h> // Required for size_t

#define STATION_LENGTH 32
#define TIMESTAMP 10

struct DatabaseHeader {
    size_t num_stations;
    size_t num_readings;
};

struct SensorStation {
    int id;
    char name[STATION_LENGTH];
    int status;
};

struct TempReading {
    int id;
    char timestamp[TIMESTAMP];
    double temperature;
    int station_id;
};

struct MicroDB {
    struct DatabaseHeader header;

    struct SensorStation *stations;
    size_t max_stations;

    struct TempReading *readings;
    size_t max_readings;
};

/**
 * @brief Initializes a fresh, empty database in RAM with default capacities.
 * @return A pointer to the newly allocated MicroDB struct.
 */
struct MicroDB *db_create(void);

/**
 * @brief Loads the database from 'micro.db' on disk into RAM.
 * @return A pointer to the populated MicroDB struct, or NULL if no file exists.
 */
struct MicroDB *db_load(void);

/**
 * @brief Performs a Full Flush, overwriting 'micro.db' with the current RAM state.
 * @param db Pointer to the active MicroDB instance.
 * @return void
 */
void db_save(struct MicroDB *db);

/**
 * @brief Safely frees all memory associated with the database (arrays and the struct).
 * @param db Pointer to the active MicroDB instance.
 * @return void
 */
void db_free(struct MicroDB *db);

/**
 * @brief Adds a new SensorStation to the database in RAM.
 * @param db Pointer to the active MicroDB instance.
 * @param station_name The string name of the station.
 * @param status Integer representing operational status (e.g., 1 for active).
 * @return void
 */
void add_station(struct MicroDB *db, const char *station_name, int status);

/**
 * @brief Adds a new TempReading to the database in RAM, linked to a station.
 * @param db Pointer to the active MicroDB instance.
 * @param station_id The Foreign Key ID of the parent SensorStation.
 * @param timestamp The string representation of the reading time.
 * @param temp The double precision temperature value.
 * @return void
 */
void add_reading(struct MicroDB *db, int station_id, const char *timestamp, double temp);

#endif
