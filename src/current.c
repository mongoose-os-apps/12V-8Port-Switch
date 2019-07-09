#include "mgos.h"
#include "mgos_config.h"
#include "mgos_ina219.h"
#include "mgos_prometheus_metrics.h"

struct mgos_ina219 *s_ina219 = NULL;

static void current_metrics(struct mg_connection *nc, void *user_data) {
  if (s_ina219) {
    float bus, shunt, current, res;

    mgos_ina219_get_bus_voltage(s_ina219, &bus);
    mgos_ina219_get_shunt_resistance(s_ina219, &res);
    mgos_ina219_get_shunt_voltage(s_ina219, &shunt);
    mgos_ina219_get_current(s_ina219, &current);

    mgos_prometheus_metrics_printf(
        nc, GAUGE, "voltage", "Voltage (Volts)",
        "{type=\"ina219\", unit=\"0\", chan=\"0\"} %f", bus);

    mgos_prometheus_metrics_printf(
        nc, GAUGE, "current", "Current (Amperes)",
        "{type=\"ina219\", unit=\"0\", chan=\"0\"} %f", current);
  }

  (void) user_data;
}

static void ina219_timer_cb(void *user_data) {
  float bus, shunt, current, res;

  if (!s_ina219) return;

  mgos_ina219_get_bus_voltage(s_ina219, &bus);
  mgos_ina219_get_shunt_resistance(s_ina219, &res);
  mgos_ina219_get_shunt_voltage(s_ina219, &shunt);
  mgos_ina219_get_current(s_ina219, &current);
  LOG(LL_INFO, ("Vbus=%.3f V Vshunt=%.0f uV Rshunt=%.3f Ohm Ishunt=%.1f mA",
                bus, shunt * 1e6, res, current * 1e3));

  (void) user_data;
}

bool current_init(void) {
  s_ina219 = mgos_ina219_create(mgos_i2c_get_global(),
                                mgos_sys_config_get_ina219_i2caddr());

  if (!s_ina219) {
    LOG(LL_ERROR, ("Could not create INA219 sensor"));
    return false;
  }
  if (!mgos_ina219_set_shunt_resistance(s_ina219, 0.010)) {
    return false;
  }

  mgos_set_timer(1000, true, ina219_timer_cb, NULL);
  mgos_prometheus_metrics_add_handler(current_metrics, NULL);

  return true;
}
