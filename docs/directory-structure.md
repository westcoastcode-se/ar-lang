# Directory Structure

All source code needed for the virtual machine, builder and compiler can be found in the `src` directory. All tests 
used to verify that the source code works can be found in the `test` directory.

- `src`: Contains the source code
	- `utils`: Utility functions and types used by all parts of the language. 
			   Functions, structs etc has the prefix `arU`.
	- `builder`: Contains the builder-pattern C-API that helps ut generate bytecode. All builder
	             functions and types have the prefix `arB`.
	- `compiler`: The actual compiler which transforms ar-lang source code into bytecode. 
				  All compiler-related code have the prefix `arC`.
- `test`: Contains the tests verifying the source code