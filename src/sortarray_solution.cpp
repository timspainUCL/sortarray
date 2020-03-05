#include <iostream>
#include <sstream>
#include <chrono>

int* fill_array(int n);
void insertion_sort(int array[], int length);
void merge_sort(int array[], int length);
void array_to_cout(int *a, int n);
void write_diagnostics(const char sort_type[], std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point stop);

//
// Create an unsorted list, then sort it using an O(n^2) insertion sort
// and an O(n log n) merge sort
//
int main(int argc, char *argv[]) {
	int *array = 0;

	int length = 256;
	// Get the length of the array from the command line
	if (argc > 1) {
		int x;
		if (!(std::istringstream(argv[1]) >> x)) {
			std::cerr << "Invalid number: " << argv[1] << ", using default length = 256" << std::endl;
		} else {
			length = x;
		}
	}

	array = fill_array(length);
	if (0 == array)
		return 1;

	// Start the timer
	auto start = std::chrono::steady_clock::now();
	// Insertion sort
	insertion_sort(array, length);
	// Stop the timer
	auto stop = std::chrono::steady_clock::now();

	write_diagnostics("Insertion", start, stop);

	delete[] array;
	array = fill_array(length);
	if (0 == array)
		return 1;
  
	// Start the timer
	start = std::chrono::steady_clock::now();
	// Insertion sort
	merge_sort(array, length);
	// Stop the timer
	stop = std::chrono::steady_clock::now();

	write_diagnostics("Merge", start, stop);

	return 0;
}

void insertion_sort(int array[], int length) {
	int working_value;
	for (int i = 0; i < length; i++) {
		working_value = array[i];
		// In the sorted portion of the array, find the index of the first
		// value greater than working_value
		int insert_at = i;
		for (int j = 0; j < i; j++) {
			if (array[j] > working_value) {
				insert_at = j;
				break;
			}
		}
		// shift values from the insertion point to the new end of the
		// sorted portion of the array
		for (int j = i; j > insert_at; j--) {
			array[j] = array[j-1];
		}
		// write a into the correct point in the sorted portion of the array
		array[insert_at] = working_value;
	}
	return;
}

// Merge two sorted arrays, and return an allocated array containing the sorted values
int* merge(int array_1[], int length_1, int array_2[], int length_2) {
	// indices for the current element in each half
	int first = 0, second = 0;
	// Auxiliary copy array
	int merged_length = length_1 + length_2;
	int *merged = new int[length_1 + length_2];

	for (auto i = 0; i < merged_length; i++) {
		// Pick an element, copy a value, increment the index
		if ( (first < length_1 && array_1[first] < array_2[second]) || second >= length_2 ) {
			merged[i] = array_1[first];
			first++;
		} else {
			merged[i] = array_2[second];
			second++;
		}
	}

	return merged;
}

// Perform a merge sort on array
void merge_sort(int array[], int length) {

	int sublist_length = 1;

	while (sublist_length < length) {
		for (auto i = 0; i < length; i += 2*sublist_length) {
			int length_1 = sublist_length, length_2 = sublist_length;
			if (i + sublist_length > length) {
				// does sublist a run off the end of the array?
				length_1 = length - i;
				length_2 = 0;
			} else if (i + 2 * sublist_length > length) {
				// does sublist b run off the end of the array?
				length_2 = length - (i + sublist_length);
			}

			// merge the two sublists
			int* merged = merge(&array[i], length_1, &array[i + length_1], length_2);
			// Write back the merged results
			for (auto j = 0; j < length_1 + length_2; j++) {
				array[j + i] = merged[j];
			}
		}
		sublist_length <<= 1;
	}
}

// Calculate a good step length to get the integer entries
// out-of-order. Any value which has the generated prime factors is >
// 33 billion entries in length.
int what_step(int n) {
	int step = 7;
	int stepstep = 30; // 7 + 30n generates prime numbers for 0 <= n < 6
	int maxstep = 157; // 187 = 11*17
	while ( (n % step == 0) && (step <= maxstep) )
		step += stepstep;

	return (step > maxstep) ? -1 : step;
}

// Fill an n-element array with the integers 0 to n-1, not in order
int* fill_array(int n) {
	// Allocate space for the integer array
	int *a = new int[n];

	int step = what_step(n);
	if (-1 == step) {
		delete[] a;
		a = 0;
	}

	// Assign the integers 0 to n-1 inclusive to the array, out of order
	int i = 0;
	for (auto filled_values = 0; filled_values < n; filled_values++) {
		a[i] = filled_values;
		i += step;
		i %= n;
	}

	return a;
}

void array_to_cout(int *a, int n) {
	int line_len = 20;

	std::cout << "[";
	if (0 == n) {
		std::cout << "]" << std::endl;
		return;
	}
	for (int i = 0; i < n; i++) {
		std::cout << a[i];
		if (i == (n-1)) {
			std::cout << "]" << std::endl;
		} else {
			std::cout << ",";
			if (i % line_len == (line_len-1))
				std::cout << std::endl;
		}
	}
	return;
}

void write_diagnostics(const char sort_type[], std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point stop) {
  std::cout << sort_type << " sort took " <<  std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << " microseconds" << std::endl;
}
