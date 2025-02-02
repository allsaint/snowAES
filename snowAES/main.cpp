#include "IncludeMe.hpp"

std::vector<long> bit_probability_vector(1024, 0);
std::vector<float> bpv;

std::vector<BYTE> vplaintext;
std::vector<BYTE> vciphertext;
std::vector<BYTE> vkey;
std::string splaintext;
std::string skey;

/*
	M iterations are performed for each of the k bits.
	( k is the number of bits in the input).
	ie; First bit of all the random inputs are changed for M iterations. 
	All 3 avalanche metrics are calculated.
	
	Compare the calculated avalanche metrics 
	and check if it is lower than the existing metrics.
	(All the metric values are initialized to maximum in the beginning). 
	The minimum value is kept.
	
	Then the second bit of all the random inputs is changed
	for another M iterations. And the process of calculating
	the avalanche metrics and taking the minimum is repeated.
	( So, a total of k*M iterations).
*/

template <typename vec_str>

// Flip 1 random bit from 1024b bits
vec_str bitFlipper(vec_str text) {
	std::random_device rd;

	std::mt19937 gen(rd());
	long scaled_byte = gen() % 128;
	long scaled_bit = gen() % 8;

	unsigned int vector_byte = text.at(scaled_byte);
	unsigned int flipped = vector_byte ^ (1 << scaled_bit);
	text.at(scaled_byte) = flipped;
	//std::cout << "Vector byte: "  <<(char)vector_byte << "; Flipped bit: " << flipped << "; Bit position to be flipped:"<<scaled_bit << std::endl;
	return text;
}

std::string randomStringGenerator(size_t length) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, 61); // 0-9, A-Z, a-z
	const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string str;
	for (size_t i = 0; i < length; ++i) {
		str += charset[dist(gen)];
	}
	return str;
}


// Number of output bits that may flip for a single-bit change at the input
template <typename BPVIN>
int avalancheDependence(BPVIN bpv_in) {
	int temp = 0;
	int count = 0;
	int dependence; 
	for (long x : bpv_in ) {
		if (x == 0) temp++;
		count++;
	}
	return count - temp;
}

// Hamming Weight or hamming distance from all zero string
template <typename BPVIN>
int avalancheWeight(BPVIN bpv_in){
	int weight= 0;
	for (long x : bpv_in) {
		weight += x;
	}
	return weight;
}

// It is the uncertainty about whether output bits flip or not
template <typename BPVOUT>
double avalancheEntropy(BPVOUT bpvout) {
	double entropy = 0.0;
	for (float x : bpvout) {
		if (x == 0 || x == 1) continue;
			entropy += -x * log2(x) - (1 - x) * log2(1 - x);
	}
	return entropy;
}

template <typename BPVOUT>
double aesEntropy(BPVOUT bpvout) {
	double entropy = 0.0;
	for (float x : bpvout) {
		if (x == 0 || x == 1) continue;
		entropy += -x * log2(x);
	}
	return entropy;
}

  template <typename T>
 void printBits(const std::vector<T>& in) {
	 for (const auto& x : in) {
		 std::cout << "[*] Bit probability: " << x << '\n';
	 }
	 std::cout << std::endl;
 }

 // Convert vector to string 
std::string converter(std::vector<BYTE> vec){
	std::string str(vec.begin(), vec.end());
	return str;
}

// Convert string to vector 
std::vector<BYTE> converter(std::string str){
	std::vector<BYTE> vec(str.begin(), str.end());
	return vec;
	// for (char x : str) vec.push_back(x);
}

// xor every 
std::vector<BYTE> xored(std::vector<BYTE> cipherA, std::vector<BYTE> cipherB) {
	std::vector<BYTE> xoredVector;
	for (int block = 0; block < 128; block++) {
		xoredVector.push_back(cipherA[block] ^ cipherB[block]);
	}
	return xoredVector;
}

long countBits(std::vector<BYTE> vec , std::vector<long> & bpvpin) {
	for (size_t i = 0; i < vec.size(); i++) {
		for (int bitsize = 0; bitsize < 8; bitsize++) { // LSB (bit 0) to MSB (bit 7)
			size_t tmp = (i * 8) + bitsize;
			if ((vec[i] & (1 << bitsize)) != 0) { // Check if bit flipped
				bpvpin[tmp]++;
			}
		}
	}

	//std::cout << "Humming weight: \n";
	int counter = 0;
	long hw = 0;
	for (int x : vec) {
		while (x) {
			hw += x & 1;
			x >>= 1;
		}
		/*std::cout << counter<<": " << count << std::endl;
		++counter;*/
	}
	return hw;
}

std::vector<BYTE> encryptor(std::vector<BYTE> vplaintext, std::vector<BYTE> vkey){
	AES aes = AES::AES(AESKeyLength::AES_128);
	vciphertext = aes.EncryptECB(vplaintext, vkey);
	//vplaintext = aes.DecryptECB(vciphertext, vkey);
	return vciphertext;
}

int main(){
	auto start = std::chrono::high_resolution_clock::now();
	BitProbability bp (1024, 0);

	int rounds = 1024;
	int R = rounds;
	while (rounds--) {
		std::vector<std::vector<BYTE>> ciphers;
		std::vector<unsigned char>vkey = { 0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41 }; //plaintext example

		std::string str = randomStringGenerator(128);   // string
		std::string f_str = bitFlipper <std::string>(str); // string with flipped byte

		//std::cout << "[+] " << str<<" \n[+] " << f_str << "\n\n";

		// Convert 2 strings to BYTE vectors
		std::vector <BYTE>  vplaintext = converter(str);
		std::vector <BYTE>  f_vplaintext = converter(f_str);

		// Encrypt 2 BYTE vectors 
		std::vector<BYTE> origtext = encryptor(vplaintext, vkey);
		std::vector<BYTE> f_origtext = encryptor(f_vplaintext, vkey);

		std::vector<BYTE> diff;
		diff = xored(origtext, f_origtext);
		countBits(diff, bp.bpv_in);

		//std::cin.get();

	}

	for (float x : bp.bpv_in) bp.bpv_out.push_back( x/R) ;

		std::cout << " [+] Bit Probability Vector: ";  
	printBits(bp.bpv_out);
	std::cout << "\n";
	std::cout<< " [+] Entropy: "<< avalancheEntropy(bp.bpv_out)<<"\n";
	std::cout<< " [+] AES Entropy: "<< aesEntropy(bp.bpv_out)<<"\n";
	std::cout<< " [+] Weight: "<< avalancheWeight(bp.bpv_in)<<"\n";
	std::cout<< " [+] Dependence: "<< avalancheDependence(bp.bpv_in); auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";

}
