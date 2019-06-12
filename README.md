Include this header to your C++ code to parse texts from a file,

Example command:

	string str;

	bool status = parse("file1.txt","foo",str); 

	// String corresponding to 'foo' in file 'file1.txt' is parsed to variable 'str'. 
	// If parsing is successful, then 'status' is set true, otherwise false.
