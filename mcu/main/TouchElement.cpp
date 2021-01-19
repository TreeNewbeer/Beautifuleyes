//
// Created by genius on 12/26/20.
//

#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TouchElement.h"

TouchButton::TouchButton(const touch_button_config_t & config) {
    esp_err_t ret;
    try {
        ret = touch_button_create(&config, &handle);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchButton install failed");
        }
        channel = config.channel_num;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

TouchButton::~TouchButton() {
    touch_button_delete(handle);
}

void TouchButton::SubscribeEvent(const uint32_t& event_mask, void *arg) {
    try {
        esp_err_t ret = touch_button_subscribe_event(handle, event_mask, arg);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchButton subscribe event failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchButton::SetDispatchMethod(touch_elem_dispatch_t dispatch_method) {
    try {
        esp_err_t ret = touch_button_set_dispatch_method(handle, dispatch_method);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchButton set dispatch method failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchButton::RegisterCallback(touch_button_callback_t handler) {
    try {
        esp_err_t ret = touch_button_set_callback(handle, handler);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchButton register callback failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

uint32_t TouchButton::GetSignalValue(const te_signal_t& signalType) {
    try {
        esp_err_t ret;
        uint32_t signalValue = 0;
        if (signalType == TE_SIGNAL_RAW) {
            ret = touch_pad_read_raw_data(channel, &signalValue);
        } else if (signalType == TE_SIGNAL_SMOOTH) {
            ret = touch_pad_filter_read_smooth(channel, &signalValue);
        } else {
            ret = touch_pad_read_benchmark(channel, &signalValue);
        }
        if (ret != ESP_OK) {
            throw std::runtime_error("read signal timeout");
        }
        return signalValue;
    } catch (const std::runtime_error &e) {
        std::cout << e.what() << std::endl;
        return 0;
    }
}

TouchSlider::TouchSlider(const touch_slider_config_t& config) {
    try {
        esp_err_t ret = touch_slider_create(&config, &handle);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchSlider create failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

TouchSlider::~TouchSlider() {
    touch_slider_delete(handle);
}

void TouchSlider::SubscribeEvent(const uint32_t& event_mask, void *arg) {
    try {
        esp_err_t ret = touch_slider_subscribe_event(handle, event_mask, arg);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchSlider subscribe event failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchSlider::SetDispatchMethod(touch_elem_dispatch_t dispatch_method) {
    try {
        esp_err_t ret = touch_slider_set_dispatch_method(handle, dispatch_method);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchSlider set dispatch method failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchSlider::RegisterCallback(touch_slider_callback_t handler) {
    try {
        esp_err_t ret = touch_slider_set_callback(handle, handler);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchSlider register callback failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

TouchMatrix::TouchMatrix(const touch_matrix_config_t& config) {
    try {
        esp_err_t ret = touch_matrix_create(&config, &handle);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchMatrix create failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

TouchMatrix::~TouchMatrix() {
    touch_matrix_delete(handle);
}

void TouchMatrix::SetSubscribeEvent(const uint32_t& event_mask, void *arg) {
    try {
        esp_err_t ret = touch_matrix_subscribe_event(handle, event_mask, arg);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchMatrix subscribe event failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchMatrix::SetDispatchMethod(touch_elem_dispatch_t dispatch_method) {
    try {
        esp_err_t ret = touch_matrix_set_dispatch_method(handle, dispatch_method);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchButton set dispatch method failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void TouchMatrix::RegisterCallback(touch_matrix_callback_t handler) {
    try {
        esp_err_t ret = touch_matrix_set_callback(handle, handler);
        if (ret != ESP_OK) {
            throw std::runtime_error("TouchMatrix register callback failed");
        }
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

TouchElement::TouchElement(touch_elem_global_config_t globalConfig) {
    try {
        touch_button_global_config_t button_config = TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG();
        touch_slider_global_config_t slider_config = TOUCH_SLIDER_GLOBAL_DEFAULT_CONFIG();
        touch_matrix_global_config_t matrix_config = TOUCH_MATRIX_GLOBAL_DEFAULT_CONFIG();
        esp_err_t ret = touch_element_install(&globalConfig);;
        ret |= touch_button_install(&button_config);
        ret |= touch_slider_install(&slider_config);
        ret |= touch_matrix_install(&matrix_config);
        if (ret != ESP_OK) {
            throw std::runtime_error("Touch Sense init failed");
        }
        std::cout << "Touch Sense instantiate successful!" << std::endl;
    } catch (const std::runtime_error &e) {
        std::cout << e.what() << std::endl;
        delete this;
    }
}

TouchElement::~TouchElement() {
    esp_err_t ret = ESP_OK;
    try {
        ret |= touch_element_stop();
        if (ret != ESP_OK) {
            throw std::runtime_error("Destroy TouchSense object failed!");
        }
        touch_button_uninstall();
        touch_slider_uninstall();
        touch_matrix_uninstall();
        touch_element_uninstall();
        std::cout << "Destroy TouchElement successfully!";
    } catch (const std::runtime_error &e) {
        std::cout << e.what() << std::endl;
    }
}


uint32_t TouchElement::GetChannelValue(touch_pad_t channel_num, te_signal_t signal_type) {
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
        if (signal_type == TE_SIGNAL_RAW) {
            ret = touch_pad_read_raw_data(channel_num, &signal_value);
        } else if (signal_type == TE_SIGNAL_SMOOTH) {
            ret = touch_pad_filter_read_smooth(channel_num, &signal_value);
        } else {
            ret = touch_pad_read_benchmark(channel_num, &signal_value);
        }
        if (ret != ESP_OK) {
            throw std::runtime_error("read signal timeout");
        }
        return signal_value;
    } catch (const std::runtime_error &e) {
        std::cout << e.what() << std::endl;
        return 0;
    }
}

void TouchElement::SetupChannel(touch_pad_t channel_num, uint32_t threshold) {
    touch_pad_config(channel_num);
    if (threshold != 0) {
        touch_pad_set_thresh(channel_num, threshold);
    }
    channels.push_back(channel_num);
}

TouchButton* TouchElement::CreateButton(const touch_button_config_t &button_config) {
    auto touch_button = new TouchButton(button_config);
    return touch_button;
}

TouchSlider* TouchElement::CreateSlider(const touch_slider_config_t &slider_config) {
    auto touch_slider = new TouchSlider(slider_config);
    return touch_slider;
}

TouchMatrix* TouchElement::CreateMatrix(const touch_matrix_config_t &matrix_config) {
    auto touch_matrix = new TouchMatrix(matrix_config);
    return touch_matrix;
}
