#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <vector>

class Neuron;

class Axon {
    
    public:
        double weight;
        Neuron* from;
        Neuron* to;

};

class Neuron {
    
    public:
        double charge;
        double err;
        bool is_bias = false;
        std::vector<Axon *> connections_in;
        std::vector<Axon *> connections_out; 


    private:

};

class NeuralNetwork {

    public:

        enum PrintType {
            CHARGE,
            ERROR,
        };

        int inputs;
        int outputs;
        int cols;
        double learning_rate = 0.01;
        std::vector<std::vector<Neuron>> neurons;
        std::vector<Neuron>* input_layer;
        std::vector<Neuron>* output_layer;

        NeuralNetwork(int, int, std::vector<int>);
        std::vector<double> feed_forward(std::vector<double>);
        double back_prop(std::vector<double>);
        void print(PrintType);
        void convert(NeuralNetwork*);

};


#endif
