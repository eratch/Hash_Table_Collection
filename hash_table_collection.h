/*
Author: Erik Ratchford
Assignment: hw7
Date: 10/31/2019
file: hash_table_collection.h
*/

#ifndef HASH_TABLE_COLLECTION_H
#define HASH_TABLE_COLLECTION_H

#include <vector>
#include <algorithm>
#include <functional>
#include "collection.h"

template <typename K, typename V>
class HashTableCollection : public Collection<K,V>
{
	public:

	// create an empty linked list
	HashTableCollection();															

	// copy a linked list
	HashTableCollection(const HashTableCollection<K,V>& rhs);						

	// assign a linked list
	HashTableCollection<K,V>& operator=(const HashTableCollection<K,V>& rhs);		

	// delete a linked list
	~HashTableCollection();															

	// insert a key - value pair into the collection
	void insert(const K& key, const V& val);										

	// remove a key - value pair from the collection
	void remove(const K& key);														

	// find the value associated with the key
	bool find(const K& key, V& val) const;											

	// find the keys associated with the range
	void find(const K& k1, const K& k2, std::vector<K>& keys) const;				

	// return all keys in the collection
	void keys(std::vector<K>& keys) const;											

	// return collection keys in sorted order
	void sort(std::vector<K>& keys) const;											

	// return the number of keys in collection
	int size() const;																

	private:

	// helper to empty entire hash table
	void make_empty();																

	// resize and rehash the hash table
	void resize_and_rehash();														

	// linked list node structure
	struct Node {
		K key;
		V value;
		Node* next;
	};

	// number of k-v pairs in the collection
	int collection_size;

	// number of hash table buckets (default is 16)
	int table_capacity;

	// hash table array load factor (set at 75% for resizing)
	const double load_factor_threshold;

	// hash table array
	Node** hash_table;
};


template <typename K, typename V>
HashTableCollection<K,V>::HashTableCollection() :
collection_size(0), table_capacity(16), load_factor_threshold(0.75) {
	// dynamically allocate the hash table array
	hash_table = new Node*[table_capacity];
	// initialize the hash table chains
	for (int i = 0; i < table_capacity; ++i)
		hash_table[i] = nullptr;
}

template <typename K, typename V>
HashTableCollection<K,V>::HashTableCollection(const HashTableCollection<K,V>& rhs)
: hash_table(nullptr) {
	*this = rhs;
}

template <typename K, typename V>
HashTableCollection<K,V>&
HashTableCollection<K,V>::operator=(const HashTableCollection<K,V>& rhs) {
	Node* rhsbucketptr;
	
	// check if rhs is current object and return current object
	if (this == &rhs) {
		return *this;
	} else {
		make_empty();			// delete current object
		table_capacity = rhs.table_capacity;
		collection_size = 0;
		hash_table = new Node*[table_capacity];
		
		for (int i = 0; i < table_capacity; ++i) {
			hash_table[i] = nullptr;
		}
		for (int i = 0; i < table_capacity; i++) {
			rhsbucketptr = rhs.hash_table[i];
			while (rhsbucketptr != nullptr) {
				insert(rhsbucketptr -> key, rhsbucketptr -> value);
				rhsbucketptr = rhsbucketptr -> next;
			}
		}
		rhsbucketptr = nullptr;
	}		
	return *this;
}

template <typename K, typename V>
HashTableCollection<K,V>::~HashTableCollection() {
	make_empty();
}

template <typename K, typename V>
void HashTableCollection<K,V>::insert(const K& key, const V& val) {
	std::hash<K> hash_fun;
	size_t value;
	size_t index;
	double load_factor = 0;
	Node* insertptr;
	
	
	load_factor = collection_size / table_capacity;
	if (load_factor >= load_factor_threshold) {
		resize_and_rehash();
	}
	value = hash_fun(key);
	index = value % table_capacity;
	
	insertptr = new Node;
	insertptr -> key = key;
	insertptr -> value = val;
	insertptr -> next = nullptr;
	
	if (hash_table[index] == nullptr) {
		hash_table[index] = insertptr;
		collection_size++;
		insertptr = nullptr;
	} else {
		insertptr -> next = hash_table[index];
		hash_table[index] = insertptr;
		collection_size++;
		insertptr = nullptr;
	}
}

template <typename K, typename V>
void HashTableCollection<K,V>::remove(const K& key) {
	V tmp_val;
	std::hash<K> hash_fun;						// hash function 
	size_t value = hash_fun(key);				// hash value
	size_t index = value % table_capacity;		// hash table index
	
	if (collection_size == 0) {
		return;
	} else if (!find(key, tmp_val)) {
		return;
	} else if (hash_table[index] -> next == nullptr) {		// one element list
		delete hash_table[index];
		hash_table[index] = nullptr;
		collection_size--;
	} else {
		Node* delptr;
		Node* prevptr;
		delptr = hash_table[index];
		
		while (delptr != nullptr) {
			if (hash_table[index] -> key == key && hash_table[index] -> next != nullptr) {
				hash_table[index] = hash_table[index] -> next;
				delptr -> next = nullptr;
				delete delptr;
				delptr = nullptr;
				collection_size--;
			} else if (delptr -> key == key && delptr -> next != nullptr) {
				prevptr = hash_table[index];
				while (prevptr -> next != delptr) {
					prevptr = prevptr -> next;
				}
				if (prevptr == hash_table[index]) {
					hash_table[index] -> next = delptr -> next;
					delptr -> next = nullptr;
					delete delptr;
					delptr = nullptr;
					collection_size--;
				} else {
					prevptr -> next = delptr -> next;
					delptr -> next = nullptr;
					delete delptr;
					delptr = nullptr;
					collection_size--;
				}
			} else if (delptr -> key == key && delptr -> next == nullptr) {
				prevptr = hash_table[index];
				while (prevptr -> next != delptr) {
					prevptr = prevptr -> next;
				}
				prevptr -> next = nullptr;
				delete delptr;
				delptr = nullptr;
				collection_size--;
			} else {
				delptr = delptr -> next;
			}
		}
	}
}
		

template <typename K, typename V>
bool HashTableCollection<K,V>::find(const K& key, V& val) const {
	std::hash<K> hash_fun;
	size_t value = hash_fun(key);
	size_t index = value % table_capacity;
	Node* searchptr = hash_table[index];
	bool is_found = false;
	
	if (searchptr == nullptr) {
		return is_found;
	} else if (searchptr -> key == key) {
		val = searchptr -> value;
		is_found = true;
		return is_found;
	} else {
		while (searchptr != nullptr && !is_found) {
			if (searchptr -> key == key) {
				val = searchptr -> value;
				is_found = true;
				return is_found;
			} else {
				searchptr = searchptr -> next;
			}
		}
	}
	return is_found;
}

template <typename K, typename V>
void HashTableCollection<K,V>::find(const K& k1, const K& k2, std::vector<K>& keys) const {
	std::hash<K> hash_fun;
	size_t value_1 = hash_fun(k1);
	size_t value_2 = hash_fun(k2);
	size_t index_1 = value_1 % table_capacity;
	size_t index_2 = value_1 % table_capacity;
	Node* searchptr;
	
	for (int i = 0; i < table_capacity; i++) {
		if (hash_table[i] != nullptr) {
			searchptr = hash_table[i];
			while (searchptr != nullptr) {
				if (searchptr -> key >= k1 && searchptr -> key <= k2) {
					keys.push_back(searchptr -> key);
				}
				searchptr = searchptr -> next;
			}
		}
	}
}

template <typename K, typename V>
void HashTableCollection<K,V>::keys(std::vector<K>& keys) const {
	Node* ptr;
	
	for (int i = 0; i < table_capacity; i++) {
		ptr = hash_table[i];
		while (ptr != nullptr) {
			keys.push_back(ptr -> key);
			ptr = ptr -> next;
		}
	}	
}

template <typename K, typename V>
void HashTableCollection<K,V>::sort(std::vector<K>& ks) const {
	keys(ks);
	std::sort(ks.begin(), ks.end());
}

template <typename K, typename V>
int HashTableCollection<K,V>::size() const {
 return collection_size;
}

template <typename K, typename V>
void HashTableCollection<K,V>::make_empty() {
	Node* delptr;
	Node* nextptr;
	
	if (hash_table == nullptr) {
		return;
	}
	for (int i = 0; i < table_capacity; i++) {
		if (hash_table[i] == nullptr) {
			continue;
		} else {
			delptr = hash_table[i];
			while (delptr != nullptr) {
				nextptr = delptr -> next;
				delete delptr;
				delptr = nextptr;
				collection_size--;
			}
			hash_table[i] = nullptr;
		}
	}
	delete hash_table;
}


template <typename K, typename V>
void HashTableCollection<K,V>::resize_and_rehash() {
	std::hash<K> hash_fun;
	size_t value;
	size_t index;
	V val;
	
	// setup new table
	int new_capacity = table_capacity * 2;
	int new_collection_size = collection_size;
	// dynamically allocate the new table
	Node** new_table = new Node*[new_capacity];
	// initialize new table
	for (int i = 0; i < new_capacity; ++i) {
		new_table[i] = nullptr;
	}
	// insert key values
	std::vector<K> ks;
	keys(ks);

	for (K key : ks) {
		// hash the key 
		value = hash_fun(key);
		index = value % new_capacity;
		
		// create a new node in new table
		find(key, val);
		Node* rehashptr = new Node;
		rehashptr -> key = key;
		rehashptr -> value = val;
		rehashptr -> next = new_table[index];
		new_table[index] = rehashptr;		
	}
	// clear the current data
	make_empty();
	// update to the new settings
	hash_table = new_table;
	table_capacity = new_capacity;
	collection_size = new_collection_size;
	// ... update remaining vars ...
	
	//delete new_table;
}

#endif