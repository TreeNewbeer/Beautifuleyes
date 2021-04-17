//
// Created by genius on 12/26/20.
//

#ifndef TOUCH_SENSE_SENSITIVITY_DETECT_TOUCHSENSE_H
#define TOUCH_SENSE_SENSITIVITY_DETECT_TOUCHSENSE_H

#include <vector>
#include "touch_element/touch_button.h"
#include "touch_element/touch_slider.h"
#include "touch_element/touch_matrix.h"

typedef enum {
    TE_SIGNAL_RAW,
    TE_SIGNAL_SMOOTH,
    TE_SIGNAL_BENCHMARK
} te_signal_t;

class TouchButton {
public:
    explicit TouchButton(const touch_button_config_t& config);
    ~TouchButton();
    void SubscribeEvent(const uint32_t& event_mask, void *arg = nullptr);
    void SetDispatchMethod(touch_elem_dispatch_t dispatch_method);
    void RegisterCallback(touch_button_callback_t handler);
    uint32_t GetSignalValue(const te_signal_t& signalType = TE_SIGNAL_RAW);
    touch_pad_t GetChannelNum();

private:
    touch_button_handle_t handle = nullptr;
    touch_pad_t channel = TOUCH_PAD_MAX;
};

class TouchSlider {
public:
    explicit TouchSlider(const touch_slider_config_t& config);
    ~TouchSlider();
    void SubscribeEvent(const uint32_t& event_mask, void *arg = nullptr);
    void SetDispatchMethod(touch_elem_dispatch_t dispatch_method);
    void RegisterCallback(touch_slider_callback_t handler);
private:
    touch_slider_handle_t handle = nullptr;
};

class TouchMatrix {
public:
    explicit TouchMatrix(const touch_matrix_config_t& config);
    ~TouchMatrix();
    void SetSubscribeEvent(const uint32_t& event_mask, void *arg = nullptr);
    void SetDispatchMethod(touch_elem_dispatch_t dispatch_method);
    void RegisterCallback(touch_matrix_callback_t handler);
private:
    touch_matrix_handle_t handle = nullptr;
};

class TouchElement {
public:
    explicit TouchElement(touch_elem_global_config_t globalConfig = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG());
    ~TouchElement();
    uint32_t GetChannelValue(touch_pad_t channel_num, te_signal_t signal_type = TE_SIGNAL_RAW);
    TouchButton* CreateButton(const touch_button_config_t& button_config);
    TouchSlider* CreateSlider(const touch_slider_config_t& slider_config);
    TouchMatrix* CreateMatrix(const touch_matrix_config_t& matrix_config);

private:
    std::vector<touch_pad_t> channels;
    void SetupChannel(touch_pad_t channel_num, uint32_t threshold = 0);
};

#endif //TOUCH_SENSE_SENSITIVITY_DETECT_TOUCHSENSE_H
