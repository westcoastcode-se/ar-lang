#include "Utils.hpp"

struct SuiteCommonArray : TestUtils
{
	void BeforeEach()
	{
	}

	void AfterEach(const std::exception* e)
	{
	}

	class MyInterface
	{
	public:
		virtual ~MyInterface() {}

		static const int Value = 1;
		virtual int Id() { return Value; }
	};

	class MyClass1 : public MyInterface
	{
	public:
		static const int Value = 2;
		virtual int Id() { return Value; }
	};
	
	class MyClass2 : public MyClass1
	{
	public:
		static const int Value = 3;
		virtual int Id() { return Value; }
	};

	void EmptyArray()
	{
		Vector<MyInterface*> arr;
		ReadOnlyArray<MyInterface*> readOnly = arr;
		AssertEquals(arr.Size(), 0);
		AssertEquals(readOnly.Size(), 0);
	}

	void EmptyArrayWithInheritence()
	{
		Vector<MyClass1*> arr;
		ReadOnlyArray<MyInterface*> readOnly = arr;
		AssertEquals(arr.Size(), 0);
		AssertEquals(readOnly.Size(), 0);
	}

	void ArrayWithInheritence()
	{
		Vector<MyClass1*> arr;
		arr.Add(new MyClass1());
		arr.Add(new MyClass2());
		ReadOnlyArray<MyInterface*> readOnly = arr;
		AssertEquals(arr.Size(), 2);
		AssertEquals(readOnly.Size(), 2);
		AssertEquals(readOnly[0], (MyInterface*)arr[0]);
		AssertEquals(readOnly[1], (MyInterface*)arr[1]);
		AssertEquals(readOnly[0]->Id(), MyClass1::Value);
		AssertEquals(readOnly[1]->Id(), MyClass2::Value);
	}

	void ArrayWithVarargs()
	{
		Array<I32, 3> items(1, 2, 3);
		AssertEquals(items.Size(), 3);
		AssertEquals(items[0], 1);
		AssertEquals(items[1], 2);
		AssertEquals(items[2], 3);
	}

	void VectorWithVarargs()
	{
		Vector<I32> items(1, 2, 3);
		AssertEquals(items.Size(), 3);
		AssertEquals(items[0], 1);
		AssertEquals(items[1], 2);
		AssertEquals(items[2], 3);
	}

	void RangeFrom()
	{
		Vector<MyClass1*> arr;
		ReadOnlyArray<MyInterface*> readOnly = arr;
		AssertEquals(arr.Size(), 0);
		AssertEquals(readOnly.Size(), 0);
	}

	void operator()()
	{
		TEST(EmptyArray());
		TEST(EmptyArrayWithInheritence());
		TEST(ArrayWithInheritence());
		TEST(ArrayWithVarargs());
		TEST(VectorWithVarargs());
		//TEST(RangeFrom());
		//TEST(RangeTo());
		//TEST(Range());
	}
};


// Run all compilation tests
void SuiteCommon()
{
	SUITE(SuiteCommonArray);
}
