#include "channel.h"
#include "mgos_config.h"
#include "mgos_mcp23xxx.h"

static struct mgos_mcp23xxx *s_gpio = NULL;

int channel_get_total(void) {
  return 8;
}

bool channel_is_valid(int channel) {
  if (channel < 0) return false;
  if (channel >= channel_get_total()) return false;
  return true;
}

bool channel_get(int channel, bool *state) {
  if (!state) return false;
  if (!channel_is_valid(channel)) return false;
  *state = mgos_mcp23xxx_gpio_read(s_gpio, channel);
  return true;
}

bool channel_set(int channel, bool state) {
  if (!channel_is_valid(channel)) return false;
  mgos_mcp23xxx_gpio_write(s_gpio, channel, state);
  return true;
}

bool channel_toggle(int channel) {
  bool state;

  if (!channel_is_valid(channel)) return false;
  if (!channel_get(channel, &state)) return false;

  return channel_set(channel, !state);
}

static void channel_status(void *arg) {
  mgos_mcp23xxx_print_state(s_gpio);
  (void)arg;
}

bool channel_init(void) {
  int i;

  if (!(s_gpio = mgos_mcp23008_create(
            mgos_i2c_get_global(), mgos_sys_config_get_mcp23xxx_i2caddr(),
            mgos_sys_config_get_mcp23xxx_int_gpio()))) {
    LOG(LL_ERROR, ("Could not create MCP23008"));
    return false;
  }

  for (i = 0; i < 8; i++)
    mgos_mcp23xxx_gpio_set_mode(s_gpio, i, MGOS_GPIO_MODE_OUTPUT);

  mgos_set_timer(5000, true, channel_status, NULL);

  return true;
}
