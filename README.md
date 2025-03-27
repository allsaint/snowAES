

# Avalanche Effect Analyzer for AES

A C++ tool to measure the **avalanche effect** in AES-128 encryption. It quantifies how small changes in plaintext (single-bit flips) propagate to ciphertext changes, evaluating cryptographic strength through statistical metrics.

## Features
- **Metrics**: 
  - **Avalanche Dependence**: % of bits flipped.
  - **Avalanche Weight**: Total flipped bits (Hamming weight).
  - **Entropy**: Uncertainty in bit-flip distribution.
  - **Bit Probability Vector**: Per-bit flip probability (0-1).
- **Dynamic Inputs**: Random plaintext and key generation.
- **CSV Export**: Export metrics for visualization.
- **Timing Benchmarks**: Measure encryption performance.

## Installation
1. **Dependencies**:
   - C++17 compiler (GCC/Clang)
   - CMake (optional)
2. **Build**:
   ```bash
   git clone https://github.com/your-username/aes-avalanche.git
   cd aes-avalanche
   g++ -std=c++17 main.cpp BPV.cpp AES.cpp -o avalanche
   ```
   
## Usage
   ```
   ./avalanche
   ```
- Outputs:
	- Terminal: Metrics (entropy, weight, dependence).
	- bit_probability.csv: Bit-flip probabilities for plotting.
		
## Results Interpretation

- Ideal Values:
	- Bit Probability: ~50% for all bits.
	- Entropy: Close to 1024 (max uncertainty).
	- Dependence: 100% of bits flipped.
	
## AES Modes:
    - Current Mode: ECB (for isolated avalanche analysis).
    - Future Work: CBC/CTR mode comparison (see Contributing).
	
## Security Considerations
	⚠️ This is a research tool. ECB mode is insecure for real-world applications—use only for analysis.
	
## Contributing
    1. Fork the repo.
    2. Add features (e.g., CBC mode, improved stats).
    3. Submit a PR. See Issues for ideas.
	


