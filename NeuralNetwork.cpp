#include "NeuralNetwork.h"
#include <assert.h>
#include <math.h>
#include <iostream>
#include <iomanip>

NeuralNetwork::NeuralNetwork(int inputs, int outputs, std::vector<int> hiddens) {
    cols = 2 + hiddens.size();
    this->inputs = inputs;
    this->outputs = outputs;

    // construct neurons
    for (int i = 0; i < cols; i++) {
        neurons.push_back(std::vector<Neuron>());
    }
    for (int i = 0; i < cols; i++) {
        int n_this_col = (i == 0 ? inputs : i == cols - 1 ? outputs : (hiddens[i - 1]) + 1);
        for (int j = 0; j < n_this_col; j++) {
            auto n = Neuron();
            n.charge = (double)rand() / RAND_MAX;
            if (i > 0 && i < cols - 1 && j == n_this_col - 1) {
                n.is_bias = true;
            }
            neurons[i].push_back(std::move(n));        
        }
    }

    // link neurons
    for (int i = 1; i < cols; i++) {
        auto& prev_col = neurons[i - 1];
        auto& at_col = neurons[i];
        for (auto& neuron_link: prev_col) {
            // -1 to ignore the bias neuron
            for (int j = 0; j < at_col.size() - (i == cols - 1 ? 0 : 1); j++) {
                Axon* a = new Axon;
                a->from = &neuron_link;
                a->to = &at_col[j];
                a->weight = (double)rand() / (double)RAND_MAX; 
                at_col[j].connections_in.push_back(a);
                neuron_link.connections_out.push_back(a);
            }
        } 
    }

    // establish pointers
    input_layer = &neurons[0];
    output_layer = &neurons[cols - 1];

}

std::vector<double>
NeuralNetwork::feed_forward(std::vector<double> data) {
    assert(data.size() == neurons[0].size());

    std::vector<double> output;

    for (int i = 0; i < data.size(); i++) {
        (*input_layer)[i].charge = data[i];
    }

    for (int i = 1; i < cols; i++) {
        // -1 to ignore the bias neuron
        int num_in_col = (i < cols - 1) ? neurons[i].size() - 1 : neurons[i].size();
        for (int j = 0; j < num_in_col; j++) {
            auto& neuron = neurons[i][j];
            double sum = 0.0;
            for (auto axon: neuron.connections_in) {
                sum += axon->weight * axon->from->charge;
            }
            neuron.charge = 1 / (1 + exp(-sum*0.5));
        }
    }

    for (auto& neuron: *output_layer) {
        output.push_back(neuron.charge);
    }

    return output;
}

void
NeuralNetwork::convert(NeuralNetwork* other) {
    assert(inputs == other->inputs);
    assert(outputs == other->outputs);
    assert(cols == other->cols);
    for (int i = 0; i < neurons.size(); i++) {
        assert(other->neurons[i].size() == neurons[i].size());
    }
    
    for (int i = 0; i < other->neurons.size(); i++) {
        std::vector<Neuron>& col = other->neurons[i];
        for (int j = 0; j < col.size(); j++) { 
            std::vector<Axon*>& connections = col[j].connections_out;
            for (int k = 0; k < connections.size(); k++) {
                connections[k]->weight = neurons[i][j].connections_out[k]->weight;
                double roll = (double)rand() / RAND_MAX;
                // 30% chance for mutation
                if (roll < 0.1) {
                    roll *= (1.0 / 0.1);
                    if (roll < 0.33) {
                        double d_weight = ((double)rand() / RAND_MAX) + 0.5;
                        connections[k]->weight *= d_weight;
                    } else if (roll < 0.66) {
                        double d_weight = (((double)rand() / RAND_MAX) - 0.5) * 3;
                        connections[k]->weight += d_weight;
                    } else {
                        connections[k]->weight = (double)rand() / RAND_MAX;
                    }
                }
            }
        }
    }
}

double
NeuralNetwork::back_prop(std::vector<double> expected) {
    double cost = 0.0;

    // calcualte errors for output neurons
    int index = 0;
    for (auto& neuron: *output_layer) {
        double neuron_err = expected[index] - neuron.charge;
        neuron.err = neuron_err;
        cost += 0.5 * (neuron_err*neuron_err); 
        index++;
    }

    // back propagate and calculate the errors for the rest of the neurons
    for (int i = cols - 2; i > 0; i--) {
        for (auto& neuron: neurons[i]) {
            double my_err = 0.0;
            for (auto axon: neuron.connections_out) {
                my_err += axon->weight * axon->to->err;
            }
            neuron.err = my_err;
        }
    }

    auto sig = [](double x) -> double {
        return 1.0 / (1.0 + exp(-x));
    };

    // change weights
    for (int i = 1; i < cols; i++) {
        for (auto& neuron: neurons[i]) {
            for (auto axon: neuron.connections_in) {
                double delta_w = learning_rate * neuron.err * (sig(neuron.charge) * (1 - sig(neuron.charge))) * axon->from->charge;

                axon->weight += delta_w;
            }
            if (neuron.is_bias) {
                //neuron.charge += learning_rate * neuron.err * (sig(neuron.charge) * (1 - sig(neuron.charge)));
            }
        }
    }
    
    return cost;
}

void
NeuralNetwork::print(NeuralNetwork::PrintType type) {
    int largest_index = 0;
    for (int i = 0; i < cols; i++) {
        int at_size = neurons[i].size();
        if (at_size > largest_index) {
            largest_index = at_size;
        }
    }
    
    std::cout << std::setprecision(5) << std::fixed;
    for (int i = 0; i < largest_index; i++) {
        for (int j = 0; j < cols; j++) {
            if (neurons[j].size() > i) {
                if (type == PrintType::CHARGE) {
                    std::cout << neurons[j][i].charge << " ";
                } else if (type == PrintType::ERROR) {
                    std::cout << neurons[j][i].err << " ";
                }
            } else {
                std::cout << "        ";
            }
        }
        std::cout << std::endl;
    }
}
