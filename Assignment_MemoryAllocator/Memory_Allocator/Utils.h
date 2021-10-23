#pragma once
#ifndef UTILS_H
#define UTILS_H

class Utils {
public:
	static const size_t CalculatePadding(const size_t baseAddress, const size_t alignment) {
		const size_t multiplier = (baseAddress / alignment) + 1;
		const size_t alignedAddress = multiplier * alignment;
		const size_t padding = alignedAddress - baseAddress;
		return padding;
	}

	static const size_t CalculatePaddingWithHeader(const size_t baseAddress, const size_t alignment, const size_t headerSize) {
		size_t padding = CalculatePadding(baseAddress, alignment);
		size_t neededSpace = headerSize;

		if (padding < neededSpace) {
			// Header does not fit - Calculate next aligned address that header fits
			neededSpace -= padding;

			// How many alignments I need to fit the header        
			if (neededSpace % alignment > 0) {
				padding += alignment * (1 + (neededSpace / alignment));
			}
			else {
				padding += alignment * (neededSpace / alignment);
			}
		}

		return padding;
	}
};

#endif /* UTILS_H */