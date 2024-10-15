#include <glog/logging.h>
#include <memory>
#include "base/alloc.h"
#include "model/llama2.h"
int main(int argc, char* argv[]) {
  if (argc != 3) {
    LOG(INFO) << "Usage: ./demo checkpoint_path tokenizer_path ";
    return -1;
  }
  std::shared_ptr<base::CPUDeviceAllocator> alloc =
      std::make_shared<base::CPUDeviceAllocator>();

  const char* checkpoint_path = argv[1];  // e.g. out/model.bin
  const char* tokenizer_path = argv[2];
  model::LLama2Model model(tokenizer_path, checkpoint_path);
  model.init(base::DeviceType::kDeviceCPU);
  std::string sentence = "Hi, this is ";
  const auto& tokens = model.encode(sentence);
  const auto s = model.forward(tokens, 32);
  LOG(INFO) << s;
  return 0;
}