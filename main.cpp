#include "mnist/include/mnist/mnist_reader.hpp"
#include "neuro.cpp"

int main() {
  typedef unsigned long CNT;
  typedef double DATA_TYPE;
  typedef double W_TYPE;
  const CNT MAX_EPOCH = 1000000;
  mnist::MNIST_dataset<std::vector, std::vector<uint8_t>, uint8_t> dataset =
      mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>(
          "mnist/");//connect image set
  std::vector<DATA_TYPE> in;//vector of input data
  for (CNT i = 0; i < dataset.training_images[0].size(); ++i)
    in.push_back(dataset.training_images[0][i] / 255.0);

  std::vector<CNT> num_neurons_each_lay = {dataset.training_images[0].size(), 100,  10};
 	
  //create singletone object for neural network
  neuro<neuron<DATA_TYPE, W_TYPE>, W_TYPE, DATA_TYPE, CNT> t(in, num_neurons_each_lay);
  
  for (CNT epoch = 0; epoch < MAX_EPOCH; ++epoch) {//for each epoch
    for (CNT img = 0; img < dataset.training_images.size(); ++img){//for each image
      in.clear();
      for (CNT pixel = 0; pixel < dataset.training_images[img].size(); ++pixel)//for each pixel
        in.push_back(dataset.training_images[img][pixel] / 255.0);//fill input data
      std::vector<DATA_TYPE> out(10, 0);//vector of output data
      out[dataset.training_labels[img]] = 1;//mark needed didgit on out
      t.setin(in);//set input data
      t.run(out);//do one epoch
    }
  }
}
