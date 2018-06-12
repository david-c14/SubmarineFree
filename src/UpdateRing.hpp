template <class T>
struct UpdateRing {
	T items[2];
	unsigned int bufCount = 0;
	T* fg() { return items + (bufCount % 2); }	
	T* bg() { return items + ((bufCount + 1) % 2); }
	void swap() { bufCount++; };
};

