#pragma once
/**
 * \author Lucas Kramer
 * \file  NeuralNetwork.h
 * \brief A feed-forward neural network class
 */

#include <vector>
#include <mutex>

/** \brief A representation of a neural network */
class NeuralNetwork {
  /** \brief A node in a neural network */
  struct Node {
    int id;
    bool isInput;
    bool isOutput;
    float baseline;
    std::vector<Node*> inputs;
    std::vector<float> weights;

    /**
     * The node constructor
     * \param id the id of the node
     * \param isInput is the node an input to the simulation
     * \param isOutput is the node an output to the simulation
     * \param baseline the output of the node when there is no stimulation
     * \param inputs the input nodes to the node
     * \param weights the weights by which to scale the inputs to the node
     */
    Node(int id, bool isInput, bool isOutput, float baseline,
         const std::vector<Node*> &inputs,
         const std::vector<float> &weights);
  };

public:
  /**
   * The copy constructor
   * \param other the network to copy
   */
  NeuralNetwork(const NeuralNetwork &other);

  /**
   * Constructs a network from a file description
   * \param filename the description filename
   */
  NeuralNetwork(const std::string &filename);

  /**
   * Computes the output values of a network for a vector of inputs.  
   * Throws an exception if an incorrect number of inputs is provided
   * \param inputs a vector containing the inputs
   * \return a vector containing the outputs
   */
  std::vector<float> compute(const std::vector<float> &inputs);

  /**
   * Mutates the network by changing numChanged connection strengths by
   * a random number between -amount and amount
   * \param numChanged the number of connections to mutate
   * \param amount the maximum amount to change the connections
   * \return the new network
   */
  NeuralNetwork mutate(int numChanged, float amount);

  /**
   * Combines the network with another network by constructing a new network
   * with half the connection strengths from each.  Throws an exception if
   * the networks have incompatible structures
   * \return the new network
   */
  NeuralNetwork combine(const NeuralNetwork &other);

  /**
   * Writes the network to a file
   * \param filename the file to write
   */
  void write(const std::string &filename);

  /**
   * Loads a network from a file
   * \param filename the filename to load
   * \return the new network
   */
  static NeuralNetwork load(const std::string &filename);

private:
  std::mutex computeMutex;
  std::vector<int> inputs;
  std::vector<int> outputs;
  std::vector<Node*> nodes;

  /**
   * Constructs a NeuralNetwork from vectors of inputs, outputs, and the nodes
   * \param inputs the ids of the input nodes
   * \param outputs the ids of the output nodes
   * \param nodes the nodes
   */
  NeuralNetwork(const std::vector<int> &inputs,
                const std::vector<int> &outputs,
                const std::vector<Node*> &nodes);

  // Private members and functions used for evaluation
  std::vector<float> nodeValue;
  std::vector<bool> nodeComputed;
  std::vector<int> inputIndex;

  /**
   * Computes the value of a node if it hasn't been computed
   * \param inputs the input values to the node
   * \param node the node to compute
   * \return the value of the node
   */
  float computeNode(const std::vector<float> &inputs, const Node &node);

  /**
   * Helper function for load,
   * checks if a line is all whitespace or comments
   * \param line the line to check
   * \return true if the line is all whitespace or comments
   */
  static bool isAllWhitespace(const std::string &line);
};
