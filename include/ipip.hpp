
#pragma once

#include <vector>
#include <string>
#include <arpa/inet.h>
#include <fstream>
#include <algorithm>

/**
 * IPIP.HPP, C++ Access library to ipip.net IP database.
 */
class ipip
{
	struct ipip_idx
	{
		std::uint32_t ipblock_end_addr; // network ending
		std::uint32_t offset_and_len;
	};

	struct ipip_dat
	{
		// 文件大小.
		std::uint32_t index_size; // big ending
		// 第一级 索引. IP 的第一个字节.
		std::uint32_t level1_index[256]; // host ending
		ipip_idx level2_index[0];
	};

	// little to host
	static inline std::uint32_t ltohl(std::uint32_t n)
	{
#ifdef __BIG_ENDIAN__
		const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&n);
		return (unsigned)(bytes[0]) + ((unsigned)(bytes[1]) << 8) + ((unsigned)(bytes[2]) << 16) + ((unsigned)(bytes[3]) << 24);
#else
		return n;
#endif
	}

public:
	ipip(const char* datafile)
	{
		std::ifstream inputfile(datafile, std::ios::binary);
		if (inputfile.is_open() == false)
		{
			throw std::runtime_error("open ipip data failed");
		}
		// std vector reserve
		data.assign(std::istreambuf_iterator<char>(inputfile), std::istreambuf_iterator<char>());

		const ipip_dat* datex_ptr = reinterpret_cast<const ipip_dat*>(data.data());
		index_size = ntohl(datex_ptr->index_size);
		number_of_index = (index_size - 1028) / 8;

		if (index_size > data.size() - 1024)
		{
			throw std::runtime_error("ipip: invalid ip data file");
		}
	}

	std::string find(std::uint32_t ip) const
	{
		std::string result;
		std::uint32_t ip_in_host_ending = ntohl(ip);

		unsigned ip_first_octent = (ip_in_host_ending & 0xFF000000) >> 24;

		const ipip_dat* datex_ptr = reinterpret_cast<const ipip_dat*>(data.data());

		std::uint32_t start =  ltohl(datex_ptr->level1_index[ip_first_octent]);

		const ipip_idx* search_range_start = &datex_ptr->level2_index[start];

		const ipip_idx* search_range_end = &datex_ptr->level2_index[number_of_index];

		if (ip_first_octent < 255){
			std::uint32_t end = ltohl(datex_ptr->level1_index[ip_first_octent+1]);
			search_range_end = &datex_ptr->level2_index[end];
		}

		const ipip_idx* it = std::lower_bound(
			search_range_start,
			search_range_end,
			ip_in_host_ending,
			[](const ipip_idx& a, std::uint32_t b)
			{
				return ntohl(a.ipblock_end_addr) < b;
			}
		);

		if (it != search_range_end)
		{
			const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&(it->offset_and_len));

			unsigned offset = (unsigned)(bytes[0]) + ((unsigned)(bytes[1]) << 8) + ((unsigned)(bytes[2]) << 16);

			result = std::string(&data[index_size + offset - 1024], bytes[3]);
		}

		return result;
	}

private:
	std::vector<char> data;
	int index_size;
	int number_of_index;
};
