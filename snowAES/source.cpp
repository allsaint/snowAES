#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include "AES.h"
#include <unordered_set>
#include <random>
#include <utility>

std::vector<long>   bit_probability_vector(1024, 0);
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
vec_str bitFlipper(vec_str text){

	std::random_device rd1;
	std::random_device rd2;

	std::mt19937 bit(rd1());
	std::mt19937 byte(rd2());

	long scaled_byte = byte() % 128;
	long scaled_bit = bit() % 8;

	unsigned int vector_byte = text.at(scaled_byte);
	unsigned int flipped = vector_byte ^ (1 << scaled_bit);
	text.at(scaled_byte) = flipped;

	//std::cout << "Vector byte: "  <<(char)vector_byte << "; Flipped bit: " << flipped << "; Bit position to be flipped:"<<scaled_bit << std::endl;
	return text;
}


std::string randomStringGenerator(std::size_t length) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::string str("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	std::shuffle(str.begin(), str.end(), gen);
	return  str.substr(0, length);
}


// number of output bits that may flip for a single-bit change at the input
int avalancheDependence() {
	int temp = 0;
	int count = 0;
	int dependence;
	for (long x : bit_probability_vector) {
		if (x == 0) temp++;
		count++;
	}
	return count - temp;
}

int avalancheWeight(){ // Hamming Weight or hamming distance from all zero string 
	int weight= 0;
	for (long x : bit_probability_vector) {
		weight += x;
	}
	return weight;
}

// It is the uncertainty about whether output bits flip or not

 long  avalancheEntropy() {
	 long entropy=0;

	for (float x : bpv) {
		if (x == 1 || x == 0) continue;
		else entropy = entropy + (   ( - x * log2(x)) - ( (1 - x) * log2(1 - x)) );
		//std::cout <<x<< "\n";
	}
	return entropy;
}

 long entropy() {
	 long entropy = 0;

	 for (float x : bpv) {
		 if (x == 1 || x == 0) continue;
		 else entropy = entropy + (-x  * log2( x));
		 //std::cout <<x<< "\n";
	 }
	 return entropy;
 }



 template <typename T>
 void o(const std::vector<T>& in) {
	 for (const auto& x : in) {
		 std::cout << x << ' ';
	 }
	 std::cout << std::endl;
 }


 // convert vector to string 
std::string converter(std::vector<BYTE> vec){
	std::string str(vec.begin(), vec.end());
	return str;
}
// convert string to vector 
std::vector<BYTE> converter(std::string str){
	std::vector<BYTE> vec(str.begin(), str.end());
	return vec;
	// for (char x : str) vec.push_back(x);
}

// xor every 
std::vector<BYTE>xored(std::vector<BYTE>cipherA, std::vector<BYTE>cipherB) {
	int block = 0;
	std::vector<BYTE>xoredVector;
	// Count difference using XOR
	while (++block && block < 128) {
		xoredVector.push_back(cipherA[block] ^ cipherB[block]);
	}
	return xoredVector;
}


long countBits(std::vector<BYTE>vec){
	//std::pair <long, std::vector<BYTE>> hwbits;
	long count = 0;
	long tmp;
	for (long  i = 0; i < vec.size(); i++) {
		long bitsize = 7;
		while (bitsize--){
			count = vec.at(i) & (1 << bitsize);
			tmp = (i * 8) + bitsize;
			if (count%2 == 0)  bit_probability_vector.at(tmp)++;
			else  bit_probability_vector.at(tmp)+0;
		}
		//std::cout << counter<<": " << count << std::endl;
		//++counter;
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
	int x = 16;
	/*
	while (--x) {
		std::string random_string = randomStringGenerator(128); // 1024 bit
		std::vector<BYTE> tmp = bitFlipper(converter(random_string));
		std::string str = converter(tmp);
		//std::cout << "[+] " << random_string<<" \n[+] " << str << "\n\n";
	 }
	*/
	int rounds = 1024;
	int R = rounds;
	while (rounds--) {
		std::vector<std::vector<BYTE>> ciphers;
		std::vector<unsigned char>vkey = { 0x01 }; //plaintext example

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
		countBits(diff);
		system("CLS");

		//std::cin.get();
	}

	for (float x : bit_probability_vector) bpv.push_back( x/R) ;


	std::cout << " [+] Bit Probability Vector: ";  
	o(bpv);
	std::cout << "\n";
	std::cout<< " [+] Entropy: "<< avalancheEntropy()<<"\n";
	std::cout<< " [+] AES Entropy: "<< entropy()<<"\n";
	std::cout<< " [+] Weight: "<< avalancheWeight()<<"\n";
	std::cout<< " [+] Dependence: "<< avalancheDependence(); 
}
