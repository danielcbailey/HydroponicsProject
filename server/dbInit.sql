CREATE TABLE IF NOT EXISTS sensor_readings (
    time DateTime,
    air_temp Float32,
    air_rel_humidity Float32,
    air_co2 Float32,
    water_level Float32,
    water_flow Float32,
    water_acc_flow Float32,
    light_level Float32,
    ph Float32,
    electrical_conductivity Float32
)
ENGINE = MergeTree()
PRIMARY KEY time
ORDER BY time;