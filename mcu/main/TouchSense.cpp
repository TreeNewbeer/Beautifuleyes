//
// Created by genius on 12/26/20.
//

#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TouchSense.h"

TouchButton::TouchButton(const ts_btn_config_t& config, bool is_need_waterproof) {
    try {
        esp_err_t ret = ts_button_create(&handle, &config);
        if (is_need_waterproof) {
            ret |= ts_waterproof_add(handle);
        }
        if (ret != ESP_OK) {
            throw std::runtime_error("Button init failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

TouchButton::~TouchButton() {
    try {
        esp_err_t ret = ts_button_delete(handle);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchButton delete failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchButton::SetSubscribeEvent(const uint32_t& event_mask) {
    try {
        esp_err_t ret = ts_button_subscribe_event(handle, event_mask, nullptr);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchButton init failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchButton::RegisterCallback(ts_btn_cb_t callback) {
    try {
        esp_err_t ret = ts_button_set_dispatch_method(handle, TS_DISP_CALLBACK);
        ret |= ts_button_set_callback(handle, callback);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchButton init failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

TouchSlider::TouchSlider(const ts_sld_config_t& config, bool is_need_waterproof) {
    try {
        esp_err_t ret = ts_slider_create(&handle, &config);
        if (is_need_waterproof) {
            ret |= ts_waterproof_add(handle);
        }
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchSlider init failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

TouchSlider::~TouchSlider() {
    try {
        esp_err_t ret = ts_slider_delete(handle);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchSlider delete failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchSlider::SetSubscribeEvent(const uint32_t& event_mask) {
    try {
        esp_err_t ret = ts_slider_subscribe_event(handle, event_mask, nullptr);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchSlider init failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchSlider::RegisterCallback(ts_sld_cb_t callback) {
    try {
        esp_err_t ret = ts_slider_set_dispatch_method(handle, TS_DISP_CALLBACK);
        ret |= ts_slider_set_callback(handle, callback);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchSlider init failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

TouchMatrix::TouchMatrix(const ts_mat_config_t& config, bool is_need_waterproof) {
    try {
        esp_err_t ret = ts_matrix_create(&handle, &config);
        if (is_need_waterproof) {
            ret |= ts_waterproof_add(handle);
        }
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchMatrix init failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

TouchMatrix::~TouchMatrix() {
    try {
        esp_err_t ret = ts_matrix_delete(handle);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchMatrix delete failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchMatrix::SetSubscribeEvent(const uint32_t& event_mask) {
    try {
        esp_err_t ret = ts_matrix_subscribe_event(handle, event_mask, nullptr);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchMatrix init failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchMatrix::RegisterCallback(ts_mat_cb_t callback) {
    try {
        esp_err_t ret = ts_matrix_set_dispatch_method(handle, TS_DISP_CALLBACK);
        ret |= ts_matrix_set_callback(handle, callback);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchMatrix init failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

TouchSense::TouchSense(ts_init_t *ts_init, ts_waterproof_config_t *ts_waterproof) {
    ts_init_t default_init;
    default_init = TS_GENERAL_DEFAULT_INIT();
    if (ts_init == nullptr) {
        ts_init = &default_init;
    }
    try {
        ts_btn_init_t button_default_init = TS_BTN_GENERAL_DEFAULT_INIT();
        ts_sld_init_t slider_default_init = TS_SLD_GENERAL_DEFAULT_INIT();
        ts_mat_init_t matrix_default_init = TS_MAT_GENERAL_DEFAULT_INIT();
        esp_err_t ret = touch_sense_init(ts_init);
        if (ts_waterproof != nullptr) {
            ret |= ts_waterproof_init(ts_waterproof);
        }
        ret |= ts_button_init(&button_default_init);
        ret |= ts_slider_init(&slider_default_init);
        ret |= ts_matrix_init(&matrix_default_init);
        if (ret != ESP_OK) {
            throw std::runtime_error("Touch Sense init failed");
        }
        std::cout << "Touch Sense instantiate successful!" << std::endl;
    } catch (const std::runtime_error &e) {
        std::cout << e.what() << std::endl;
        delete this;
    }
}

TouchSense::~TouchSense() {
    esp_err_t ret = ESP_OK;
    try {
        ret |= touch_sense_stop();
        ret |= ts_button_deinit();
        ret |= ts_slider_deinit();
        ret |= ts_matrix_deinit();
        ret |= touch_sense_deinit();
        if (ret != ESP_OK) {
            throw std::runtime_error("Destroy TouchSense object failed!");
        }
        std::cout << "Destroy TouchSense object successful!";
    } catch (const std::runtime_error &e) {
        std::cout << e.what() << std::endl;
    }
}


uint32_t TouchSense::GetChannelValue(touch_pad_t channel_num, ts_channel_signal_t signal_type) {
    bool is_channel_init = false;
    for (const auto& channel : channels) {
        if (channel_num == channel) {
            is_channel_init = true;
        }
    }
    if (!is_channel_init) {
        SetupChannel(channel_num);
    }
    uint32_t signal_value = 0;
    try {
        esp_err_t ret;
        uint32_t retry_count = 0;
        do {
            ret = ts_channel_read_signal(channel_num, signal_type, &signal_value);
            if (ret != ESP_OK) {
                vTaskDelay(pdTICKS_TO_MS(1));
            } else {
                return signal_value;
            }
        } while (ret != ESP_OK && retry_count < 100);
        throw std::runtime_error("read signal timeout");
    } catch (const std::runtime_error &e) {
        std::cout << e.what() << std::endl;
        return 0;
    }
}

void TouchSense::TouchSenseStart() {
    try {
        esp_err_t ret = touch_sense_start();
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchSense has been started");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchSense::TouchSenseStop() {
    try {
        esp_err_t ret = touch_sense_stop();
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchSense has been stopped");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchSense::SetupChannel(touch_pad_t channel_num, uint32_t threshold) {
    TouchSenseStop();
    touch_pad_config(channel_num);
    if (threshold != 0) {
        touch_pad_set_thresh(channel_num, threshold);
    }
    channels.push_back(channel_num);
    TouchSenseStart();
}

TouchButton* TouchSense::CreateButton(const ts_btn_config_t &button_config, bool is_need_waterproof) {
    TouchSenseStop();
    auto touch_button = new TouchButton(button_config, is_need_waterproof);
    TouchSenseStart();
    return touch_button;
}

TouchSlider* TouchSense::CreateSlider(const ts_sld_config_t &slider_config, bool is_need_waterproof) {
    TouchSenseStop();
    auto touch_slider = new TouchSlider(slider_config, is_need_waterproof);
    TouchSenseStart();
    return touch_slider;
}

TouchMatrix* TouchSense::CreateMatrix(const ts_mat_config_t &matrix_config, bool is_need_waterproof) {
    TouchSenseStop();
    auto touch_matrix = new TouchMatrix(matrix_config, is_need_waterproof);
    TouchSenseStart();
    return touch_matrix;
}
