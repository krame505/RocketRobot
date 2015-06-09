/**
 * \author Lucas Kramer
 * \file  NeuralNetwork.cpp
 * \brief A feed-forward neural network class
 */

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
using namespace std;
using namespace boost;

#include "NeuralNetwork.h"

NeuralNetwork::Node::Node(int id, bool isInput, bool isOutput, float baseline,
                          const vector<Node*> &inputs,
                          const vector<float> &weights) : 
  id(id),
  isInput(isInput),
  isOutput(isOutput),
  baseline(baseline),
  inputs(inputs),
  weights(weights),
  numReferences(0) {
  for (Node *n : inputs) {
    n->attatch();
  }
}

NeuralNetwork::Node::~Node() {
  for (Node *n : inputs) {
    n->detatch();
  }
}

void NeuralNetwork::Node::attatch() {
  numReferences++;
}

void NeuralNetwork::Node::detatch() {
  numReferences--;
  if (numReferences == 0)
    delete this;
}

NeuralNetwork::NeuralNetwork(const vector<int> &inputs,
                             const vector<int> &outputs,
                             const vector<Node*> &nodes) :
  inputs(inputs),
  outputs(outputs),
  nodes(nodes),
  nodeValue(nodes.size()),
  nodeComputed(nodes.size()),
  inputIndex(inputs.size()) {
  for (unsigned i = 0; i < inputs.size(); i++) {
    inputIndex[nodes[inputs[i]]->id] = i;
  }
  for (Node *n : nodes) {
    n->attatch();
  }
}

NeuralNetwork::NeuralNetwork(const string &filename) {
  NeuralNetwork loadRes = load(filename);
  inputs = loadRes.inputs;
  outputs = loadRes.outputs;
  nodes = loadRes.nodes;
  nodeValue = loadRes.nodeValue;
  nodeComputed = loadRes.nodeComputed;
  inputIndex = loadRes.inputIndex;
  for (unsigned i = 0; i < inputs.size(); i++) {
    inputIndex[nodes[inputs[i]]->id] = i;
  }
  for (Node *n : nodes) {
    n->attatch();
  }
}

NeuralNetwork::NeuralNetwork(const NeuralNetwork &other) :
  NeuralNetwork(other.inputs, other.outputs, other.nodes) {}

NeuralNetwork::~NeuralNetwork() {
  for (Node *n : nodes) {
    n->detatch();
  }
}

vector<float> NeuralNetwork::compute(const vector<float> &inputs) {
  if (inputs.size() != this->inputs.size())
    throw new runtime_error("Incorrect number of inputs to neural network");

  computeMutex.lock();
  fill(nodeComputed.begin(), nodeComputed.end(), 0);
  vector<float> outputs(this->outputs.size());

  for (unsigned i = 0; i < outputs.size(); i++) {
    outputs[i] = computeNode(inputs, *nodes[this->outputs[i]]);
  }
  computeMutex.unlock();
  return outputs;
}

float NeuralNetwork::computeNode(const vector<float> &inputs, const Node &node) {
  int id = node.id;
  if (!nodeComputed[id]) {
    if (node.isInput)
      nodeValue[id] = inputs[inputIndex[id]];
    else {
      nodeValue[id] = node.baseline;
      for (unsigned i = 0; i < node.inputs.size(); i++)
        nodeValue[id] += computeNode(inputs, *node.inputs[i]) * node.weights[i];
    }
    nodeComputed[id] = true;
  }
  //  cout << "(" << id << ", " << nodeValue[id] << ")" << endl;
  return nodeValue[id];
}

NeuralNetwork NeuralNetwork::mutate(int numChanged, float amount) {
  vector<Node*> newNodes;
  for (unsigned i = 0; i < nodes.size(); i++) {
    vector<Node*> newInputs;
    for (Node *node : nodes[i]->inputs) {
      newInputs.push_back(newNodes[node->id]);
    }
    newNodes.push_back(new Node(nodes[i]->id,
                                nodes[i]->isInput,
                                nodes[i]->isOutput,
                                nodes[i]->baseline,
                                newInputs,
                                nodes[i]->weights));
  }
  for (int i = 0; i < numChanged; i++) {
    Node *changeNode = newNodes[rand() % newNodes.size()];
    if (changeNode->weights.size() == 0) {
      i--; // Add an extra cycle if there aren't any connections to change
      break;
    }
    changeNode->weights[rand() % changeNode->weights.size()] +=
      ((float)rand()) / ((float)RAND_MAX) * amount * 2 - amount;
  }
  return NeuralNetwork(inputs, outputs, newNodes);
}

NeuralNetwork NeuralNetwork::combine(const NeuralNetwork &other) {
  if (nodes.size() != other.nodes.size()) {
    throw new runtime_error("Combined networks must have the same structure");
  }
  vector<Node*> newNodes;
  for (unsigned i = 0; i < nodes.size(); i++) {
    vector<Node*> newInputs;
    for (Node *node : nodes[i]->inputs) {
      if (i < nodes.size() / 2)
        newInputs.push_back(newNodes[node->id]);
      else
        newInputs.push_back(newNodes[node->id]);
    }
    if (i < nodes.size() / 2)
      newNodes.push_back(new Node(nodes[i]->id,
                                  nodes[i]->isInput,
                                  nodes[i]->isOutput,
                                  nodes[i]->baseline,
                                  newInputs,
                                  nodes[i]->weights));
    else
      newNodes.push_back(new Node(nodes[i]->id,
                                  nodes[i]->isInput,
                                  nodes[i]->isOutput,
                                  other.nodes[i]->baseline,
                                  newInputs,
                                  other.nodes[i]->weights));
  }
  return NeuralNetwork(inputs, outputs, newNodes);
}

void NeuralNetwork::write(const string &filename) {
  ofstream output(filename);
  if (output.is_open()) {
    for (Node *node : nodes) {
      if (node->isInput)
        output << " input";
      else if (node->isOutput)
        output << "output";
      else
        output << "      ";
      output << " node " << node->id;
      if (node->baseline != 0)
        output << " default " << node->baseline;
      if (node->inputs.size() != 0) {
        output << " connected to";
        for (unsigned i = 0; i < node->inputs.size(); i++) {
          output << " " << node->inputs[i]->id << ": " << node->weights[i];
        }
      }
      output << endl;
    }
    output.close();
  }
  else {
    cerr << "Failed to write neural network description file " << filename << endl;
    exit(1);
  }
}

NeuralNetwork NeuralNetwork::load(const string &filename) {
  vector<Node*> nodes;
  vector<int> netInputs;
  vector<int> netOutputs;
  ifstream input(filename);

  // Check if the file can be opened
  if (input.is_open()) {
    string line;
    int checkId = 0;
    // Iterate over all lines and update the line number
    for (int lineNum = 1; getline(input, line); lineNum++) {
      if (!isAllWhitespace(line)) {
        // Parse line into type, name, and value.  
        regex lineParse("[ ]*(input|output|)[ ]?node[ ]+([0-9]+)(?:[ ]+default[ ]+(-?[0-9]+(?:\\.[0-9]+)?))?(?:[ ]+connected to[ ]+([-0-9\\.e: ]+))?(?:#.*)?");
        smatch parseResult; // parseResult[0] is the whole string
        if (!regex_match(line, parseResult, lineParse)) {
          cerr << "Syntax error when parsing neural network description file " << filename << " at line " << lineNum << endl;
          exit(1);
        }
        string type = parseResult[1];
        int id = stoi(parseResult[2]);
        float baseline =
          ((string)parseResult[3]).size() > 0?
          stof(parseResult[3]) : 0;
        string connections = parseResult[4];
        vector<Node*> inputs;
        vector<float> weights;

        regex connectionParse("[ ]*([0-9]+)[ ]*\\:[ ]*(-?[0-9]+(?:\\.[0-9]+)?(?:e-?[0-9]+)?)([ ]*[0-9][-0-9\\.e: ]+)?");
        while (connections != "") {
          if (!regex_match(connections, parseResult, connectionParse)) {
            cerr << "Syntax error when parsing connections in neural network description file " << filename << " at line " << lineNum << endl;
            exit(1);
          }
          int newId = stoi(parseResult[1]);
          if (newId >= (int)nodes.size()) {
            cerr << "Error when parsing neural network description file " << filename << " at line " << lineNum <<
              ": Undefined node id " << newId << endl;
            exit(1);
          }
          inputs.push_back(nodes[newId]);
          weights.push_back(stof(parseResult[2]));
          connections = parseResult[3];
        }

        if (id != checkId++) {
          cerr << "Error when parsing neural network description file " << filename << " at line " << lineNum <<
            ": Node ids must be sequential and start at 0" << endl;
          exit(1);
        }
        Node *newNode = new Node(id, type == "input", type == "output", baseline, inputs, weights);
        nodes.push_back(newNode);
        if (type == "input")
          netInputs.push_back(id);
        else if (type == "output")
          netOutputs.push_back(id);
      }
    }
    input.close();
  }
  else {
    cerr << "Could not find neural network description file " << filename << endl;
    exit(1);
  }

  return NeuralNetwork(netInputs, netOutputs, nodes);
}

bool NeuralNetwork::isAllWhitespace(const string &line) {
  // Ignore comments and empty lines
  bool allWhitespace = true;
  for (int i = 0, line_len = line.length(); i < line_len; i++) {
    if (line[i] == '#') {
      break;
    }
    else allWhitespace &= line[i] == ' ';
  }
  return allWhitespace;
}
