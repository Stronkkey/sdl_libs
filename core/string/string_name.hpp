#pragma once

#include <core/string/string_def.hpp>

#include <memory>
#include <unordered_set>

namespace sdl {

class StringName {
public:
	using value_type = StringView;
	using traits_type = value_type::traits_type;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;
	using CharT = value_type::value_type;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

public:
	struct __String_Storer__ {
		const CharT *string;
		StringView view;
		mutable size_type uses;

		constexpr __String_Storer__(): string(), view(), uses(0) {
		}

		constexpr __String_Storer__(const CharT *string): string(string), view(this->string), uses(0) {
		}

		constexpr bool operator==(const __String_Storer__ &string_storer) const {
			return view == string_storer.view;
		}
	};

	struct __String_Saver__ {
		const __String_Storer__ *stored_string;

		constexpr __String_Saver__(): stored_string(nullptr) {
		}

		constexpr __String_Saver__(const __String_Storer__ *string_storer): stored_string(string_storer) {
		}

		constexpr size_type get_use_count() const {
			return stored_string->uses;
		}

		constexpr bool is_used() const {
			return stored_string->uses > 0;
		}

		constexpr void decrement() {
			if (stored_string)
				--stored_string->uses;
		}

		constexpr void increment() {
			if (stored_string)
				++stored_string->uses;
		}

		constexpr void delete_stored_string() {
			if (stored_string && stored_string->string)
				delete stored_string->string;
		}

		constexpr bool operator==(const __String_Storer__ &string_saver) const {
			return stored_string->string == string_saver.string;
		}

		constexpr const_reference operator*() const {
			return stored_string->view;
		}

		constexpr const_pointer operator->() const {
			return &stored_string->view;
		}

		constexpr bool operator==(const_reference string) const {
			return StringView(stored_string->string) == string;
		}
	};

	struct ConstIterator {
    	using iterator_category = std::bidirectional_iterator_tag;
		using value_type = StringName::CharT;
		using difference_type = std::ptrdiff_t;
		using reference = const value_type&;
		using pointer = const value_type*;

		constexpr ConstIterator(pointer ptr): ptr(ptr) {}

		constexpr reference operator*() const {
			return *ptr;
		}

		constexpr pointer operator->() const {
			return ptr;
		}

		constexpr ConstIterator &operator++() {
			ptr++;
			return *this;
		}

		constexpr ConstIterator operator++(int) {
			return ptr++;
		}

		constexpr ConstIterator &operator--() {
			ptr--;
			return *this;
		}

		constexpr ConstIterator operator--(int) {
			return --ptr;
		}

		constexpr friend bool operator==(const ConstIterator &left, const ConstIterator &right) {
			return left.ptr == right.ptr;
		}

		constexpr friend bool operator!=(const ConstIterator &left, const ConstIterator &right) {
			return left.ptr != right.ptr;
		}

		private:
			pointer ptr;
	};

public:
	using const_iterator = ConstIterator;
	using iterator = ConstIterator;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	static constexpr size_type npos = size_type(-1);

private:
	static std::unordered_set<__String_Storer__> stored_strings;
	__String_Saver__ saved_string;

	void _set_string(const_reference string, const bool copy = true);
	void _set_string(const CharT *string, const bool copy = true);
	void _reset();
	const __String_Storer__ *_allocate_str(const CharT *string, const bool copy = true) const;
	const __String_Storer__ *_allocate_str(const_reference string, const bool copy = true) const;
	const __String_Storer__ *_allocate_string(const CharT *string, const bool copy = true) const;
	const __String_Storer__ *_allocate_string(const_reference string, const bool copy = true) const;
public:
	StringName();
	StringName(const CharT *string);
	StringName(const_reference string);

	constexpr StringName(const StringName &string_name) {
		saved_string = string_name.saved_string;
		saved_string.increment();
	}

	constexpr StringName(StringName &&string_name) {
		saved_string = string_name.saved_string;
	}

	~StringName();

	constexpr iterator begin() const noexcept {
		return saved_string->begin();
	}

	constexpr const_iterator cbegin() const noexcept {
		return saved_string->cbegin();
	}

	constexpr iterator end() const noexcept {
		return saved_string->end();
	}

	constexpr const_iterator cend() const noexcept {
		return saved_string->cend();
	}

	constexpr reverse_iterator rbegin() const noexcept {
		return saved_string->rbegin();
	}

	constexpr const_reverse_iterator crbegin() const noexcept {
		return saved_string->crbegin();
	}

	constexpr reverse_iterator rend() const noexcept {
		return saved_string->rend();
	}

	constexpr const_reverse_iterator crend() const noexcept {
		return saved_string->crend();
	}

	constexpr const CharT &operator[](const size_type index) const {
		return (*saved_string)[index];
	}

	constexpr const CharT &at(const size_type index) const {
		return saved_string->at(index);
	}

	operator String() const;
	constexpr operator StringView() const {
		return StringView(saved_string.stored_string->string);
	}

	[[nodiscard]] constexpr const_reference get_string() const {
		return saved_string.stored_string->view;
	}

	[[nodiscard]] constexpr const CharT *get_string_ptr() const {
		return saved_string.stored_string ? saved_string.stored_string->string : nullptr;
	}

	[[nodiscard]] constexpr const CharT &front() const {
		return saved_string->front();
	}

	[[nodiscard]] constexpr const CharT &back() const {
		return saved_string->back();
	}

	[[nodiscard]] constexpr const CharT *data() const {
		return saved_string->data();
	}

	[[nodiscard]] constexpr size_type size() const {
		return saved_string->size();
	}

	[[nodiscard]] constexpr size_type length() const {
		return saved_string->length();
	}

	[[nodiscard]] constexpr size_type max_size() const {
		return saved_string->max_size();
	}

	[[nodiscard]] constexpr bool empty() const {
		return saved_string->empty();
	}

	void remove_prefix(const size_type n);
	void remove_suffix(const size_type n);

	constexpr void swap(StringName &string_name) {
		__String_Saver__ temp = std::move(saved_string);
		saved_string = std::move(string_name.saved_string);
		string_name.saved_string = std::move(temp);
	}

	size_type copy(CharT *dest, const size_type count, const size_type pos = 0) const;

	[[nodiscard]] StringName substr(const size_type pos = 0, const size_type count = npos) const;

	[[nodiscard]] constexpr int compare(const StringName &string_name) const {
		return saved_string->compare(*string_name.saved_string);
	}

	[[nodiscard]] constexpr size_type find(const StringName &string_name, const size_type pos = 0) const {
		return saved_string->find(*string_name.saved_string, pos);
	}

	[[nodiscard]] constexpr size_type find(const CharT character, const size_type pos = 0) const {
		return saved_string->find(character, pos);
	}

	[[nodiscard]] constexpr size_type find(const CharT *string, const size_type pos, const size_type count) const {
		return saved_string->find(string, pos, count);
	}

	[[nodiscard]] constexpr size_type find(const CharT *string, const size_type pos = 0) const {
		return saved_string->find(string, pos);
	}

	[[nodiscard]] constexpr size_type rfind(const StringName &string_name, const size_type pos = npos) const {
		return saved_string->rfind(*string_name.saved_string, pos);
	}

	[[nodiscard]] constexpr size_type rfind(const CharT character, const size_type pos = npos) const {
		return saved_string->rfind(character, pos);
	}

	[[nodiscard]] constexpr size_type rfind(const CharT *string, const size_type pos, const size_type count) const {
		return saved_string->rfind(string, pos, count);
	}

	[[nodiscard]] constexpr size_type rfind(const CharT *string, const size_type pos = npos) const {
		return saved_string->rfind(string, pos);
	}

	[[nodiscard]] constexpr size_type find_first_of(const StringName &string_name, const size_type pos = 0) const {
		return saved_string->find_first_of(*string_name.saved_string, pos);
	}

	[[nodiscard]] constexpr size_type find_first_of(const CharT character, const size_type pos = 0) const {
		return saved_string->find_first_of(character, pos);
	}

	[[nodiscard]] constexpr size_type find_first_of(const CharT *string, const size_type pos, const size_type count) const {
		return saved_string->find_first_of(string, pos, count);
	}

	[[nodiscard]] constexpr size_type find_first_of(const CharT *string, const size_type pos = 0) const {
		return saved_string->find_first_of(string, pos);
	}

	[[nodiscard]] constexpr size_type find_last_of(const StringName &string_name, const size_type pos = npos) const {
		return saved_string->find_last_of(*string_name.saved_string, pos);
	}

	[[nodiscard]] constexpr size_type find_last_of(const CharT character, const size_type pos = npos) const {
		return saved_string->find_last_of(character, pos);
	}

	[[nodiscard]] constexpr size_type find_last_of(const CharT *string, const size_type pos, const size_type count) const {
		return saved_string->find_last_of(string, pos, count);
	}

	[[nodiscard]] constexpr size_type find_last_of(const CharT *string, const size_type pos = npos) const {
		return saved_string->find_last_of(string, pos);
	}

	[[nodiscard]] constexpr size_type find_first_not_of(const StringName &string_name, const size_type pos = 0) const {
		return saved_string->find_first_not_of(*string_name.saved_string, pos);
	}

	[[nodiscard]] constexpr size_type find_first_not_of(const CharT character, const size_type pos = 0) const {
		return saved_string->find_first_not_of(character, pos);
	}

	[[nodiscard]] constexpr size_type find_first_not_of(const CharT *string, const size_type pos, const size_type count) const {
		return saved_string->find_first_not_of(string, pos, count);
	}

	[[nodiscard]] constexpr size_type find_first_not_of(const CharT *string, const size_type pos = 0) const {
		return saved_string->find_first_not_of(string, pos);
	}

	[[nodiscard]] constexpr size_type find_last_not_of(const StringName &string_name, const size_type pos = npos) const {
		return saved_string->find_last_not_of(*string_name.saved_string, pos);
	}

	[[nodiscard]] constexpr size_type find_last_not_of(const CharT character, const size_type pos = npos) const {
		return saved_string->find_last_not_of(character, pos);
	}

	[[nodiscard]] constexpr size_type find_last_not_of(const CharT *string, const size_type pos, const size_type count) const {
		return saved_string->find_last_not_of(string, pos, count);
	}

	[[nodiscard]] constexpr size_type find_last_not_of(const CharT *string, const size_type pos = npos) const {
		return saved_string->find_last_not_of(string, pos);
	}

	constexpr void operator=(const StringName &string_name) {
		saved_string = string_name.saved_string;
		saved_string.increment();
	}

	constexpr void operator=(StringName &&string_name) {
		saved_string = string_name.saved_string;
		saved_string.increment();
	}

	void operator=(const_reference string);

	void operator+=(const_reference string);
	void operator+=(const StringName &string_name);

	[[nodiscard]] StringName operator+(const_reference string) const;
	[[nodiscard]] StringName operator+(const StringName &string_name) const;

	[[nodiscard]] constexpr bool operator==(const StringName &right) const {
		return (*saved_string) == (*right.saved_string);
	}

	[[nodiscard]] constexpr bool operator!=(const StringName &right) const {
		return (*saved_string) != (*right.saved_string);
	}

	[[nodiscard]] constexpr bool operator<(const StringName &right) const {
		return (*saved_string) < (*right.saved_string);
	}

	[[nodiscard]] constexpr bool operator>(const StringName &right) const {
		return (*saved_string) > (*right.saved_string);
	}

	[[nodiscard]] constexpr bool operator<=(const StringName &right) const {
		return (*saved_string) <= (*right.saved_string);
	}

	[[nodiscard]] constexpr bool operator>=(const StringName &right) const {
		return (*saved_string) >= (*right.saved_string);
	}

	typedef std::basic_ostream<String::value_type, String::traits_type> __os_type__;
	friend __os_type__ &operator<<(__os_type__ &left, const StringName &right);
};

namespace literals {

inline StringName operator""_sn(const char *string, const size_t length) {
	return StringName(String(string, length));
}

}
}

template<>
struct std::hash<sdl::StringName::__String_Storer__> {
	size_t operator()(const sdl::StringName::__String_Storer__ &string_storer) const noexcept {
		return hash<sdl::StringView>()(string_storer.view);
	}
};

template<>
struct std::hash<sdl::StringName::__String_Saver__> {
	size_t operator()(const sdl::StringName::__String_Saver__ &string_saver) const noexcept {
		return hash<sdl::StringView>()(*string_saver);
	}
};

template<>
struct std::hash<sdl::StringName> {
	size_t operator()(const sdl::StringName &string_name) const noexcept {
		return hash<sdl::StringView>()(string_name.get_string());
	}
};
