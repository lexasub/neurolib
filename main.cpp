#include "mnist/include/mnist/mnist_reader.hpp"
#include "neuro.cpp"

int main() {
  mnist::MNIST_dataset<std::vector, std::vector<uint8_t>, uint8_t> dataset =
      mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>(
          "./");
  std::vector<double> in;
  for (unsigned long i = 0; i < dataset.training_images[0].size(); ++i)
    in.push_back(dataset.training_images[0][i] / 255.0);
  std::vector<unsigned long> n = {dataset.training_images[0].size(), 10, 10};
  neuro<neuron<double, double>, double, double, unsigned long> t(in, n);
  for (long long i = 0; i < 1000000; ++i) {
    in.clear();
    for (unsigned long j = 0; j < dataset.training_images[i].size(); ++j)
      in.push_back(dataset.training_images[i][j] / 255.0);
    std::vector<double> vec;
    for (unsigned long j = 0; j < 10; ++j)
      vec.push_back(0.0);
    vec[dataset.training_labels[i]] = 1;
    t.setin(in);
    t.run(vec);
  }
}
