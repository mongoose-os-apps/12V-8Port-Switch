#include "rpc.h"
#include "channel.h"

static void rpc_log(struct mg_rpc_request_info *ri, struct mg_str args) {
  LOG(LL_INFO,
      ("tag=%.*s src=%.*s method=%.*s args='%.*s'", ri->tag.len, ri->tag.p,
       ri->src.len, ri->src.p, ri->method.len, ri->method.p, args.len, args.p));
  // TODO(pim): log to MQTT
}

static void rpc_channel_toggle_handler(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {
  int channel;
  bool state;

  rpc_log(ri, args);

  if (1 != json_scanf(args.p, args.len, ri->args_fmt, &channel)) {
    mg_rpc_send_errorf(ri, 400, "channel is a required argument");
    ri = NULL;
    return;
  }

  if (!channel_is_valid(channel)) {
    mg_rpc_send_errorf(ri, 400, "channel must be between 0 and %d",
                       channel_get_total() - 1);
    ri = NULL;
    return;
  }

  channel_toggle(channel);

  channel_get(channel, &state);
  mg_rpc_send_responsef(ri, "{channel: %d, state: %d}", channel, state);
  ri = NULL;

  (void) cb_arg;
  (void) fi;
}

static void rpc_channel_get_handler(struct mg_rpc_request_info *ri,
                                    void *cb_arg, struct mg_rpc_frame_info *fi,
                                    struct mg_str args) {
  int channel;
  bool state;

  rpc_log(ri, args);

  if (1 != json_scanf(args.p, args.len, ri->args_fmt, &channel)) {
    mg_rpc_send_errorf(ri, 400, "channel is a required argument");
    ri = NULL;
    return;
  }

  if (!channel_is_valid(channel)) {
    mg_rpc_send_errorf(ri, 400, "channel must be between 0 and %d",
                       channel_get_total() - 1);
    ri = NULL;
    return;
  }

  channel_get(channel, &state);
  mg_rpc_send_responsef(ri, "{channel: %d, state: %d}", channel, state);
  ri = NULL;

  (void) cb_arg;
  (void) fi;
}

static void rpc_channel_set_handler(struct mg_rpc_request_info *ri,
                                    void *cb_arg, struct mg_rpc_frame_info *fi,
                                    struct mg_str args) {
  int channel;
  bool state;

  rpc_log(ri, args);

  if (2 != json_scanf(args.p, args.len, ri->args_fmt, &channel, &state)) {
    mg_rpc_send_errorf(ri, 400, "channel and state are required arguments");
    ri = NULL;
    return;
  }

  if (!channel_is_valid(channel)) {
    mg_rpc_send_errorf(ri, 400, "channel must be between 0 and %d",
                       channel_get_total() - 1);
    ri = NULL;
    return;
  }

  if (state != 0 && state != 1) {
    mg_rpc_send_errorf(ri, 400, "state must be 0 or 1");
    ri = NULL;
    return;
  }

  channel_set(channel, state);

  channel_get(channel, &state);
  mg_rpc_send_responsef(ri, "{channel: %d, state: %d}", channel, state);
  ri = NULL;

  (void) cb_arg;
  (void) fi;
}

void rpc_init() {
  struct mg_rpc *c = mgos_rpc_get_global();

  mg_rpc_add_handler(c, "Channel.Toggle", "{channel: %d}",
                     rpc_channel_toggle_handler, NULL);
  mg_rpc_add_handler(c, "Channel.Get", "{channel: %d}", rpc_channel_get_handler,
                     NULL);
  mg_rpc_add_handler(c, "Channel.Set", "{channel: %d, state: %d}",
                     rpc_channel_set_handler, NULL);
}
