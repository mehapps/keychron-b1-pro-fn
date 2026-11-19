/*
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_fn_ptt

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#define FN_LAYER DT_INST_PROP(0, layer)
#define TAP_KEY DT_INST_PROP(0, tap_key)
#define HOLD_KEY DT_INST_PROP(0, hold_key)
#define CANCEL_KEY DT_INST_PROP(0, cancel_key)
#define TAPPING_TERM_MS DT_INST_PROP(0, tapping_term_ms)

enum fn_state { FN_IDLE, FN_UNDECIDED, FN_LAYER_ONLY, FN_PTT };

static enum fn_state state = FN_IDLE;
static uint32_t fn_position;
static struct k_work_delayable ptt_work;

static void send_key(uint32_t keycode, bool pressed) {
    ZMK_EVENT_RAISE(zmk_keycode_state_changed_from_encoded(keycode, pressed, k_uptime_get()));
}

static void ptt_work_handler(struct k_work *work) {
    if (state != FN_UNDECIDED) {
        return;
    }
    state = FN_PTT;
    zmk_keymap_layer_activate(FN_LAYER);
    send_key(HOLD_KEY, true);
}

static int on_fn_pressed(struct zmk_behavior_binding *binding,
                         struct zmk_behavior_binding_event event) {
    fn_position = event.position;
    state = FN_UNDECIDED;
    k_work_schedule(&ptt_work, K_MSEC(TAPPING_TERM_MS));
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_fn_released(struct zmk_behavior_binding *binding,
                          struct zmk_behavior_binding_event event) {
    k_work_cancel_delayable(&ptt_work);
    switch (state) {
    case FN_UNDECIDED:
        send_key(TAP_KEY, true);
        send_key(TAP_KEY, false);
        break;
    case FN_PTT:
        send_key(HOLD_KEY, false);
        zmk_keymap_layer_deactivate(FN_LAYER);
        break;
    case FN_LAYER_ONLY:
        zmk_keymap_layer_deactivate(FN_LAYER);
        break;
    default:
        break;
    }
    state = FN_IDLE;
    return ZMK_BEHAVIOR_OPAQUE;
}

static int fn_ptt_position_listener(const zmk_event_t *eh) {
    struct zmk_position_state_changed *ev = as_zmk_position_state_changed(eh);
    if (ev == NULL || !ev->state || ev->position == fn_position) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (state == FN_UNDECIDED) {
        k_work_cancel_delayable(&ptt_work);
        state = FN_LAYER_ONLY;
        zmk_keymap_layer_activate(FN_LAYER);
    } else if (state == FN_PTT) {
        if (CANCEL_KEY) {
            send_key(CANCEL_KEY, true);
            send_key(CANCEL_KEY, false);
        }
        send_key(HOLD_KEY, false);
        state = FN_LAYER_ONLY;
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(behavior_fn_ptt, fn_ptt_position_listener);
ZMK_SUBSCRIPTION(behavior_fn_ptt, zmk_position_state_changed);

static const struct behavior_driver_api behavior_fn_ptt_driver_api = {
    .binding_pressed = on_fn_pressed,
    .binding_released = on_fn_released,
};

static int behavior_fn_ptt_init(const struct device *dev) {
    k_work_init_delayable(&ptt_work, ptt_work_handler);
    return 0;
}

DEVICE_DT_INST_DEFINE(0, behavior_fn_ptt_init, NULL, NULL, NULL, APPLICATION,
                      CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_fn_ptt_driver_api);

#endif
