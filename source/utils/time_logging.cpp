//
// Created by fss on 23-4-27.
//

#include "utils/time_logging.hpp"

namespace kuiper_infer {
namespace utils {
PtrLayerTimeStates LayerTimeStatesSingleton::SingletonInstance() {
  std::lock_guard<std::mutex> lock_(mutex_);
  if (layer_time_states_ == nullptr) {
    layer_time_states_ = std::make_shared<
        std::map<std::string, std::shared_ptr<LayerTimeState>>>();
    auto registry = LayerRegisterer::Registry();
    for (const auto& layer_creator : registry) {
      const std::string& layer_type = layer_creator.first;
      layer_time_states_->insert(
          {layer_type, std::make_shared<LayerTimeState>(0, layer_type)});
    }
  }
  return layer_time_states_;
}

void LayerTimeStatesSingleton::LayerTimeStatesInit() {
  if (layer_time_states_ != nullptr) {
    std::lock_guard<std::mutex> lock_(mutex_);
    layer_time_states_ = nullptr;
  }
  layer_time_states_ = LayerTimeStatesSingleton::SingletonInstance();
}

std::mutex LayerTimeStatesSingleton::mutex_;
PtrLayerTimeStates LayerTimeStatesSingleton::layer_time_states_;

LayerTimeLogging::LayerTimeLogging(const std::string& layer_type)
    : layer_type_(layer_type), start_time_(Time::now()) {
  layer_time_states_ = LayerTimeStatesSingleton::SingletonInstance();
  CHECK(layer_time_states_ != nullptr);
}

LayerTimeLogging::~LayerTimeLogging() {
  if (layer_time_states_ == nullptr) {
    layer_time_states_ = LayerTimeStatesSingleton::SingletonInstance();
  }
  CHECK(layer_time_states_ != nullptr);
  if (layer_time_states_->find(layer_type_) != layer_time_states_->end()) {
    auto& layer_state = layer_time_states_->at(layer_type_);
    CHECK(layer_state != nullptr);
    std::lock_guard<std::mutex> lock_guard(layer_state->time_mutex_);
    const auto end_time = Time::now();

    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                              end_time - start_time_)
                              .count();
    layer_state->duration_time_ += duration;
  } else {
    if (!layer_type_.empty())
      LOG(ERROR) << "Unknown layer type: " << layer_type_;
  }
}

void LayerTimeLogging::SummaryLogging() {
  CHECK(layer_time_states_ != nullptr);
  long total_time_costs = 0;
  if (layer_time_states_ != nullptr) {
    for (const auto& layer_time_state_pair : *(layer_time_states_.get())) {
      auto layer_time_state = layer_time_state_pair.second;
      CHECK(layer_time_state != nullptr);

      std::lock_guard<std::mutex> lock(layer_time_state->time_mutex_);
      auto time_cost = layer_time_state->duration_time_;
      total_time_costs += time_cost;
      if (layer_time_state->duration_time_ != 0) {
        LOG(INFO) << "Layer type: " << layer_time_state_pair.first
                  << " time cost: " << time_cost << "ms";
      }
    }
    LOG(INFO) << "Total time: " << total_time_costs << "ms";
  }
}

}  // namespace utils
}  // namespace kuiper_infer