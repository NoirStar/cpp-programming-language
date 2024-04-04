#include <iostream>
#include <type_traits>
#include <vector>

template<int A, int B> 
struct GCD {
	static const int value = GCD<B, A % B>::value;
};

// ���ø� Ư��ȭ
template<int A>
struct GCD<A, 0> {
	static const int value = A;
};

// N ���� D �и�
template<int N, int D = 1>
struct Ratio {
	using type = Ratio<N, D>;
	static const int num = N;
	static const int den = D;
};



template<int N>
struct Int {
	static const int num = N;
};

template<typename A, typename B>
struct add {
	using result = Int<A::num + B::num>;
};

//

template<typename T>
struct is_void {
	static constexpr bool value = false;
};

template<> 
struct is_void<void> {
	static constexpr bool value = true;
};

template<typename T>
void TellType() {
	if (is_void<T>::value) {
		std::cout << "void\n";
	}
	else std::cout << "not void \n";
}

template<typename T>
void OnlyInt(const T& t) {
	static_assert(std::is_integral<T>::value);
	std::cout << "T is Int\n";
}

template<typename T>
void OnlyClass(const T& t) {
	static_assert(std::is_class<T>::value);
	std::cout << "T is Class\n";
}



namespace detail {
	template <class T>
	char test(int T::*);
	struct two {
		char c[2];
	};
	template <class T>
	two test(...);
}  // namespace detail

template <class T>
struct is_class
	: std::integral_constant<bool, sizeof(detail::test<T>(0)) == 1 &&
	!std::is_union<T>::value> {};


template<typename T, 
	typename = typename std::enable_if<std::is_integral<T>::value>::type>
	void test(const T& t) {
	std::cout << "t : " << t << std::endl;
}

class Move {
public:
	// �⺻ ������
	// ������
	explicit Move(int value) : value_(value) {};

	// �Ҹ���
	virtual ~Move() {};

	// ���������
	Move(const Move& other) = delete;
	// ������� ������
	Move& operator=(const Move& other) = delete;

	// �̵�������
	Move(Move&& ohter) noexcept;
	// �̵����� ������
	Move& operator=(Move&& other) noexcept;


private:
	mutable int value_;

};

Move::Move(Move&& other) noexcept : value_(other.value_) {
	std::cout << "�̵� ������" << std::endl;
	other.value_ = 0;
}

Move& Move::operator=(Move&& other) noexcept {
	if (this != &other) {
		value_ = other.value_;
		other.value_ = 0;
		std::cout << "�̵����� ������ ȣ��" << std::endl;
	}
	return *this;
}


int main() {


	return 0;
}
