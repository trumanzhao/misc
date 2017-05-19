#pragma once

#include <stddef.h>
#include <algorithm>
#include <type_traits>
#include <functional>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include "var_int.h"

template <typename T>
bool io_save_s(char*& buffer, size_t& buffer_size, T v)
{
	size_t len = encode_s64((unsigned char*)buffer, buffer_size, v);
	if (len > 0)
	{
		buffer += len;
		buffer_size -= len;
		return true;
	}
	return false;
}

template <typename T>
bool io_save_u(char*& buffer, size_t& buffer_size, T v)
{
	size_t len = encode_u64((unsigned char*)buffer, buffer_size, v);
	if (len > 0)
	{
		buffer += len;
		buffer_size -= len;
		return true;
	}
	return false;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, bool>::type
io_save(char*& buffer, size_t& buffer_size, T v) { return io_save_s(buffer, buffer_size, v); }

template <typename T>
typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, bool>::type 
io_save(char*& buffer, size_t& buffer_size, T v) { return io_save_u(buffer, buffer_size, v); }

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type io_save(char*& buffer, size_t& buffer_size, T v)
{
	if (buffer_size >= sizeof(v))
	{
		memcpy(buffer, &v, sizeof(v));
		buffer += sizeof(v);
		buffer_size -= sizeof(v);
		return true;
	}
	return false;
}

inline bool io_save(char*& buffer, size_t& buffer_size, const char* v)
{
	size_t len = strlen(v) + 1;
	if (buffer_size >= len)
	{
		memcpy(buffer, v, len);
		buffer += len;
		buffer_size -= len;
		return true;
	}
	return false;
}

template <int C>
bool io_save(char*& buffer, size_t& buffer_size, const unsigned char (&v)[C])
{
	if (buffer_size >= C)
	{
		memcpy(buffer, v, C);
		buffer += C;
		buffer_size -= C;
		return true;
	}
	return false;
}

template <int C>
bool io_save(char*& buffer, size_t& buffer_size, const bool (&v)[C])
{
	if (buffer_size >= C)
	{
		memcpy(buffer, v, C);
		buffer += C;
		buffer_size -= C;
		return true;
	}
	return false;
}

template <typename T, int C>
bool io_save(char*& buffer, size_t& buffer_size, const T (&v)[C])
{
	for (int i = 0; i < C; i++)
	{
		if (!io_save(buffer, buffer_size, v[i]))
			return false;
	}
	return true;
}

inline bool io_save(char*& buffer, size_t& buffer_size, const std::string& v)
{
	size_t len = v.size() + 1;
	if (buffer_size >= len)
	{
		memcpy(buffer, v.c_str(), len);
		buffer += len;
		buffer_size -= len;
		return true;
	}
	return false;
}

template<typename T, typename... O>
bool io_save(char*& buffer, size_t& buffer_size, const std::vector<T, O...>& v)
{
	return io_save(buffer, buffer_size, v.size()) && std::all_of(v.begin(), v.end(), [&](auto& node) { return io_save(buffer, buffer_size, node); });
}

template<typename T, typename... O>
bool io_save(char*& buffer, size_t& buffer_size, const std::list<T, O...>& v)
{ 
	return io_save(buffer, buffer_size, v.size()) && std::all_of(v.begin(), v.end(), [&](auto& node) { return io_save(buffer, buffer_size, node); });
}

template<typename T, typename... O>
bool io_save(char*& buffer, size_t& buffer_size, const std::set<T, O...>& v)
{
	return io_save(buffer, buffer_size, v.size()) && std::all_of(v.begin(), v.end(), [&](auto& node) { return io_save(buffer, buffer_size, node); });
}

template<typename T, typename... O>
bool io_save(char*& buffer, size_t& buffer_size, const std::unordered_set<T, O...>& v)
{
	return io_save(buffer, buffer_size, v.size()) && std::all_of(v.begin(), v.end(), [&](auto& node) { return io_save(buffer, buffer_size, node); });
}

template<typename K, typename V, typename... O>
bool io_save(char*& buffer, size_t& buffer_size, const std::map<K, V, O...>& v)
{
	return io_save(buffer, buffer_size, v.size()) && std::all_of(v.begin(), v.end(), [&](auto& node) {
		return io_save(buffer, buffer_size, node.first) && io_save(buffer, buffer_size, node.second);
	});
	return false;
}

template<typename K, typename V, typename... O>
bool io_save(char*& buffer, size_t& buffer_size, const std::unordered_map<K, V, O...>& v)
{
	return io_save(buffer, buffer_size, v.size()) && std::all_of(v.begin(), v.end(), [&](auto& node) {
		return io_save(buffer, buffer_size, node.first) && io_save(buffer, buffer_size, node.second);
	});
	return false;
}

template<typename T, typename C = int>
bool io_save(char*& buffer, size_t& buffer_size, const T* table, C count)
{
	return io_save(buffer, buffer_size, count) && std::all_of(table, table + count, [&](auto& node) { return io_save(buffer, buffer_size, node); });
}

template<typename T, const int C>
bool io_save(char*& buffer, size_t& buffer_size, T (&table)[C])
{
	return io_save(buffer, buffer_size, C) && std::all_of(table, table + C, [&](auto& node) { return io_save(buffer, buffer_size, node); });
}

template<typename T>
struct has_member_save
{
	template<typename U> static auto check(int) ->
		decltype(std::declval<U>().io_save(std::declval<char*&>(), std::declval<size_t&>()), std::true_type());
	template<typename U> static std::false_type check(...);
	enum { value = std::is_same<decltype(check<T>(0)), std::true_type>::value };
};

template <class T>
typename std::enable_if<has_member_save<T>::value, bool>::type
io_save(char*& buffer, size_t& buffer_size, const T& v) { return v.io_save(buffer, buffer_size); }

template <typename first_type, typename... other_types>
bool io_save(char*& buffer, size_t& buffer_size, const first_type& first_arg, const other_types&... other_args)
{
	return io_save(buffer, buffer_size, first_arg) && io_save(buffer, buffer_size, other_args...);
}

template <typename T>
bool io_load_s(const char*& data, size_t& data_len, T& v)
{
	int64_t v64 = 0;
	size_t len = decode_s64(&v64, (const unsigned char*)data, data_len);
	if (len > 0)
	{
		v = (T)v64;
		data += len;
		data_len -= len;
		return true;
	}
	return false;
}

template <typename T>
bool io_load_u(const char*& data, size_t& data_len, T& v)
{
	uint64_t v64 = 0;
	size_t len = decode_u64(&v64, (const unsigned char*)data, data_len);
	if (len > 0)
	{
		v = (T)v64;
		data += len;
		data_len -= len;
		return true;
	}
	return false;
}

inline bool io_load(const char*& data, size_t& data_len, bool& v)
{
	unsigned char vchar;
	if (io_load_u(data, data_len, vchar))
	{
		v = !!vchar;
		return true;
	}
	return false;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, bool>::type
io_load(const char*& data, size_t& data_len, T& v) { return io_load_s(data, data_len, v); }

template <typename T>
typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, bool>::type
io_load(const char*& data, size_t& data_len, T& v) { return io_load_u(data, data_len, v); }

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type io_load(const char*& data, size_t& data_len, T& v)
{
	if (data_len >= sizeof(v))
	{
		memcpy(&v, data, sizeof(v));
		data += sizeof(v);
		data_len -= sizeof(v);
		return true;
	}
	return false;
}

inline bool io_load(const char*& data, size_t& data_len, const char*& v)
{
	auto* end = data + data_len;
	for (auto* pos = data; pos < end; pos++)
	{
		if (*pos == '\0')
		{
			v = data;
			data = pos + 1;
			data_len = (size_t)(end - data);
			return true;
		}
	}
	return false;
}

template <int C>
bool io_load(const char*& data, size_t& data_len, char (&v)[C])
{
	auto* end = data + data_len;
	for (auto* pos = data; pos < end; pos++)
	{
		if (*pos == '\0')
		{
			int len = (int)(pos - data) + 1;
			if (len <= C)
			{
				memcpy(v, data, len);
				data = pos + 1;
				data_len = (size_t)(end - data);
				return true;
			}
			return false;
		}
	}
	return false;
}

template <int C>
bool io_load(const char*& data, size_t& data_len, unsigned char (&v)[C])
{
	if (data_len >= C)
	{
		memcpy(v, data, C);
		data += C;
		data_len -= C;
		return true;
	}
	return false;
}

template <int C>
bool io_load(const char*& data, size_t& data_len, bool (&v)[C])
{
	if (data_len >= C)
	{
		memcpy(v, data, C);
		data += C;
		data_len -= C;
		return true;
	}
	return false;
}

template <typename T, int C>
bool io_load(const char*& data, size_t& data_len, T (&v)[C])
{
	for (int i = 0; i < C; i++)
	{
		if (!io_load(data, data_len, v[i]))
			return false;
	}
	return true;
}

inline bool io_load(const char*& data, size_t& data_len, std::string& v)
{
	const char* str = nullptr;
	if (io_load(data, data_len, str))
	{
		v = str;
		return true;
	}
	return false;
}

template<typename T, typename... O>
bool io_load(const char*& data, size_t& data_len, std::vector<T, O...>& v)
{
	size_t count = 0;
	if (!io_load(data, data_len, count))
		return false;

	for (size_t i = 0; i < count; i++)
	{
		T tmp;
		if (!io_load(data, data_len, tmp))
			return false;
		v.push_back(std::move(tmp));
	}
	return true;
}

template<typename T, typename... O>
bool io_load(const char*& data, size_t& data_len, std::list<T, O...>& v)
{
	size_t count = 0;
	if (!io_load(data, data_len, count))
		return false;

	for (size_t i = 0; i < count; i++)
	{
		T tmp;
		if (!io_load(data, data_len, tmp))
			return false;
		v.push_back(std::move(tmp));
	}
	return true;
}

template<typename T, typename... O>
bool io_load(const char*& data, size_t& data_len, std::set<T, O...>& v)
{
	size_t count = 0;
	if (!io_load(data, data_len, count))
		return false;

	for (size_t i = 0; i < count; i++)
	{
		T tmp;
		if (!io_load(data, data_len, tmp))
			return false;
		v.insert(std::move(tmp));
	}
	return true;
}

template<typename KType, typename VType, typename... O>
bool io_load(const char*& data, size_t& data_len, std::map<KType, VType, O...>& v)
{
	size_t count = 0;
	if (!io_load(data, data_len, count))
		return false;

	for (size_t i = 0; i < count; i++)
	{
		KType tk;
		VType tv;
		if (!io_load(data, data_len, tk) || !io_load(data, data_len, tv))
			return false;
		v[std::move(tk)] = std::move(tv);
	}
	return true;
}

template<typename T, typename... O>
bool io_load(const char*& data, size_t& data_len, std::unordered_set<T, O...>& v)
{
	size_t count = 0;
	if (!io_load(data, data_len, count))
		return false;

	for (size_t i = 0; i < count; i++)
	{
		T tmp;
		if (!io_load(data, data_len, tmp))
			return false;
		v.insert(std::move(tmp));
	}
	return true;
}

template<typename KType, typename VType, typename... O>
bool io_load(const char*& data, size_t& data_len, std::unordered_map<KType, VType, O...>& v)
{
	size_t count = 0;
	if (!io_load(data, data_len, count))
		return false;

	for (size_t i = 0; i < count; i++)
	{
		KType tk;
		VType tv;
		if (!io_load(data, data_len, tk) || !io_load(data, data_len, tv))
			return false;
		v[std::move(tk)] = std::move(tv);
	}
	return true;
}

template<typename T>
struct has_member_load
{
	template<typename U> static auto check(int) -> 
		decltype(std::declval<U>().io_load(std::declval<const char*&>(), std::declval<size_t&>()), std::true_type());
	template<typename U> static std::false_type check(...);
	enum { value = std::is_same<decltype(check<T>(0)), std::true_type>::value };
};

template <class T>
typename std::enable_if<has_member_load<T>::value, bool>::type
io_load(const char*& data, size_t& data_len, T& v) { return v.io_load(data, data_len); }

template <typename first_type, typename... other_types>
bool io_load(const char*& data, size_t& data_len, first_type& first_arg, other_types&... other_args)
{
	return io_load(data, data_len, first_arg) && io_load(data, data_len, other_args...);
}

#define MEMBER_IO(...)	\
bool io_save(char*& buffer, size_t& buffer_size) const { return ::io_save(buffer, buffer_size, __VA_ARGS__);} \
bool io_load(const char*& data, size_t& data_len) { return ::io_load(data, data_len, __VA_ARGS__); }

struct msg_archiver
{
	msg_archiver() { m_buffer.resize(1024); }
	~msg_archiver() { }

	template<typename... arg_types>
	const char* save(size_t& data_len, arg_types... args)
	{
		char* buffer = &m_buffer[0];
		size_t buffer_size = m_buffer.size();
		bool rets[] = { io_save(buffer, buffer_size, args)... };
		if (std::all_of(std::begin(rets), std::end(rets), [](auto e) { return e; }))
		{
			data_len = (size_t)(m_buffer.size() - buffer_size);
			return &m_buffer[0];
		}
		return nullptr;
	}

	void resize(size_t size) { m_buffer.resize(size); }

private:
	std::vector<char> m_buffer;
};

template <typename T>
struct msg_table
{
	T m_id;
	std::unordered_map<T, std::function<bool(const char*, size_t)>> m_msg_table;
};

template<>
struct msg_table<const char*>
{
	std::string m_id;
	std::unordered_map<std::string, std::function<bool(const char*, size_t)>> m_msg_table;
};

// T用来表示消息ID, 可以是enum, integer, const char*,std::string
template <typename T>
struct msg_dispatcher : msg_table<typename std::decay<T>::type>
{
	bool dispatch(const char* data, size_t data_len)
	{
		if (io_load(data, data_len, m_id))
		{
			auto it = m_msg_table.find(m_id);
			if (it != m_msg_table.end())
			{
				return it->second(data, data_len);
			}
		}
		return false;
	}

	// 绑定全局函数
	template <typename... arg_types>
	void bind(T id, void(*func)(arg_types...))
	{
		m_msg_table[id] = [func](const char* data, size_t data_len)
		{
			return invoke(data, data_len, func, std::make_index_sequence<sizeof...(arg_types)>());
		};
	}

	// 绑定成员函数
	template <typename class_type, typename... arg_types>
	void bind(T id, class_type* object, void(class_type::*func)(arg_types...))
	{
		m_msg_table[id] = [object, func](const char* data, size_t data_len)
		{
			return invoke(data, data_len, func, object, std::make_index_sequence<sizeof...(arg_types)>());
		};
	}

	template<size_t... Integers, typename... arg_types>
	static bool invoke(const char*& data, size_t& data_len, void(*func)(arg_types...), std::index_sequence<Integers...>&&)
	{
		std::tuple<typename std::decay<arg_types>::type...> vars;
		bool rets[] = { true, (io_load(data, data_len, std::get<Integers>(vars)))... };
		if (data_len == 0 && std::all_of(std::begin(rets), std::end(rets), [](auto e) {return e;}))
		{
			(*func)(std::get<Integers>(vars)...);
			return true;
		}
		return false;
	}

	template<size_t... Integers, typename class_type, typename... arg_types>
	static bool invoke(const char*& data, size_t& data_len, void(class_type::*func)(arg_types...), class_type* object, std::index_sequence<Integers...>&&)
	{
		std::tuple<typename std::decay<arg_types>::type...> vars;
		bool rets[] = { true, (io_load(data, data_len), std::get<Integers>(vars))... };
		if (data_len == 0 && std::all_of(std::begin(rets), std::end(rets), [](auto e) {return e;}))
		{
			(object->*func)(std::get<Integers>(vars)...);
			return true;
		}
		return false;
	}
};
