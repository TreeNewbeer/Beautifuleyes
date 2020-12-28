//
// Created by genius on 12/26/20.
//

#ifndef TOUCH_SENSE_SENSITIVITY_DETECT_TOUCHSENSE_H
#define TOUCH_SENSE_SENSITIVITY_DETECT_TOUCHSENSE_H

#include <vector>
#include "touch/touch_sense_button.h"
#include "touch/touch_sense_slider.h"
#include "touch/touch_sense_matrix.h"

typedef struct {
    touch_pad_t channel;
    float sensitivity;
} ts_channel_meta_t;

class TouchButton {
public:
    explicit TouchButton(const ts_btn_config_t& config, bool is_need_waterproof);
    ~TouchButton();
    void SetSubscribeEvent(const uint32_t& event_mask);
    void RegisterCallback(ts_btn_cb_t callback);
private:
    ts_btn_handle_t handle = nullptr;
};

class TouchSlider {
public:
    explicit TouchSlider(const ts_sld_config_t& config, bool is_need_waterproof);
    ~TouchSlider();
    void SetSubscribeEvent(const uint32_t& event_mask);
    void RegisterCallback(ts_sld_cb_t callback);
private:
    ts_sld_handle_t handle = nullptr;
};

class TouchMatrix {
public:
    explicit TouchMatrix(const ts_mat_config_t& config, bool is_need_waterproof);
    ~TouchMatrix();
    void SetSubscribeEvent(const uint32_t& event_mask);
    void RegisterCallback(ts_mat_cb_t callback);
private:
    ts_mat_handle_t handle = nullptr;
};

class TouchSense {
public:
    explicit TouchSense(ts_init_t *ts_init = nullptr, ts_waterproof_config_t *ts_waterproof = nullptr);
    ~TouchSense();
    uint32_t GetChannelValue(touch_pad_t channel_num, ts_channel_signal_t signal_type = TS_SIGNAL_RAW);
    TouchButton* CreateButton(const ts_btn_config_t& button_config, bool is_need_waterproof = false);
    TouchSlider* CreateSlider(const ts_sld_config_t& slider_config, bool is_need_waterproof = false);
    TouchMatrix* CreateMatrix(const ts_mat_config_t& matrix_config, bool is_need_waterproof = false);

private:
    std::vector<touch_pad_t> channels;
    void SetupChannel(touch_pad_t channel_num, uint32_t threshold = 0);
    void TouchSenseStart();
    void TouchSenseStop();
};


#endif //TOUCH_SENSE_SENSITIVITY_DETECT_TOUCHSENSE_H
