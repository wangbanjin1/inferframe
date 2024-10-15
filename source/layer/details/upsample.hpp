//
// Created by fss on 22-12-25.
//

#ifndef KUIPER_INFER_SOURCE_LAYER_DETAILS_UPSAMPLE_HPP_
#define KUIPER_INFER_SOURCE_LAYER_DETAILS_UPSAMPLE_HPP_
#include "layer/abstract/layer.hpp"

namespace kuiper_infer {
enum class UpSampleMode {
  kModeNearest = 0,  // 目前上采样层只支持邻近采样
};

class UpSampleLayer : public Layer {
 public:
  explicit UpSampleLayer(uint32_t scale_h, uint32_t scale_w,
                         UpSampleMode mode = UpSampleMode::kModeNearest);

  InferStatus Forward(
      const std::vector<std::shared_ptr<Tensor<float>>>& inputs,
      std::vector<std::shared_ptr<Tensor<float>>>& outputs) override;

  static ParseParameterAttrStatus GetInstance(
      const std::shared_ptr<RuntimeOperator>& op,
      std::shared_ptr<Layer>& upsample_layer);

 private:
  uint32_t scale_h_ = 0.f;
  uint32_t scale_w_ = 0.f;
  UpSampleMode mode_ = UpSampleMode::kModeNearest;
};
}  // namespace kuiper_infer
#endif  // KUIPER_INFER_SOURCE_LAYER_DETAILS_UPSAMPLE_HPP_
