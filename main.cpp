#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <queue>

std::string read_main_parameters(int argc, char* argv[])
{
	if (argc != 2)
		throw std::exception();

	return argv[1];
}

void add_counts(std::vector<int64_t>& counts, const char buffer[], int buffer_size)
{
	for (int i = 0; i < buffer_size; i++)
	{
		int temp = static_cast<int>(static_cast<unsigned char>(buffer[i]));
		counts[static_cast<size_t>(temp)] += 1;
	}
}

std::vector<int64_t> load_bite_counts(std::ifstream& input_file)
{
	std::vector<int64_t> bite_counts(256);
	int BUFFER_SIZE = 128;

	while (true)
	{
		char buffer[128];
		input_file.read(buffer, BUFFER_SIZE);
		if (input_file.gcount() < BUFFER_SIZE)
		{
			add_counts(bite_counts, buffer, static_cast<int>(input_file.gcount()));
			break;
		}
		add_counts(bite_counts, buffer, BUFFER_SIZE);
	}
	return bite_counts;
}

struct HuffmanNode {
	int64_t count;
	std::string values;
	HuffmanNode* left_child;
	HuffmanNode* right_child;
};

class Compare {
public:
	bool operator()(HuffmanNode* first, HuffmanNode* second) {
		return (first->count > second->count);
	}
};

HuffmanNode* create_tree(std::vector<int64_t> bite_counts)
{
	std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> heap;
	for (size_t i = 0; i < bite_counts.size(); i++)
	{
		if (bite_counts[i] > 0)
		{
			HuffmanNode* newNode = new HuffmanNode();
			newNode->count = bite_counts[i];
			newNode->values = static_cast<char>(static_cast<int>(i));
			newNode->left_child = NULL;
			newNode->right_child = NULL;
			heap.push(newNode);
		}
	}
	while (heap.size() > 1)
	{
		HuffmanNode* min1 = heap.top();
		heap.pop();
		HuffmanNode* min2 = heap.top();
		heap.pop();

		int64_t new_count = min1->count + min2->count;
		std::string new_values = min1->values + min2->values;
		HuffmanNode* sum = new HuffmanNode();
		sum->count = new_count;
		sum->values = new_values;
		sum->left_child = min1;
		sum->right_child = min2;
		heap.push(sum);
	}
	if (heap.size() == 1) {
		HuffmanNode* root = heap.top();
		return root;
	}
	else {
		//std::cout << "ERROR: NO ROOT FOUND\n";
		exit(0);
		return NULL;
	}
}

void printCodes(HuffmanNode* root, const std::string& current_code, std::vector<std::string>& result)
{
	if (root->left_child != NULL) {
		std::string new_code = current_code + '0';
		printCodes(root->left_child, new_code, result);
	}

	if (root->right_child != NULL) {
		std::string new_code = current_code + '1';
		printCodes(root->right_child, new_code, result);
	}

	if (root->left_child == NULL && root->right_child == NULL) {
		result[static_cast<int>(static_cast<unsigned char>(root->values[0]))] = current_code;
	}
}

void free_cascade(HuffmanNode* root)
{
	if (root == NULL)
		return;

	free_cascade(root->left_child);
	free_cascade(root->right_child);

	delete root;
}

int main(int argc, char* argv[])
{
	std::string filepath;
	try {
		filepath = read_main_parameters(argc, argv);
	}
	catch (...) {
		//std::cout << "Error: Invalid number of arguments\n";
		return 1;
	}

	std::ifstream input_file(filepath, std::ios::binary);
	if (!input_file.good())
	{
		///		std::cout << "Error: file doesnt exist or is unreadable!\n";
		return 1;
	}

	std::vector<int64_t> bite_counts = load_bite_counts(input_file);
	HuffmanNode* root = create_tree(bite_counts);
	std::string current_code;
	std::vector <std::string> huffman_codes(256);
	printCodes(root, current_code, huffman_codes);
	free_cascade(root);
	for (size_t i = 0; i < 256; i++)
	{
		if (!huffman_codes[i].empty())
			std::cout << i << ": " << huffman_codes[i] << '\n';
	}
	return 0;
}