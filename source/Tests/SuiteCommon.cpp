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

	void ArrayWithInheritence()
	{
		Array<MyClass1*, 2> arr;
		arr[0] = new MyClass1();
		arr[1] = new MyClass2();
		ReadOnlyArray<MyInterface*> readOnly = arr;
		AssertEquals(arr.Size(), 2);
		AssertEquals(readOnly.Size(), 2);
		AssertEquals(readOnly[0], (MyInterface*)arr[0]);
		AssertEquals(readOnly[1], (MyInterface*)arr[1]);
		AssertEquals(readOnly[0]->Id(), MyClass1::Value);
		AssertEquals(readOnly[1]->Id(), MyClass2::Value);
		delete arr[1];
		delete arr[0];
	}

	void ArrayWithVarargs()
	{
		Array<I32, 3> items(1, 2, 3);
		AssertEquals(items.Size(), 3);
		AssertEquals(items[0], 1);
		AssertEquals(items[1], 2);
		AssertEquals(items[2], 3);
	}

	void EmptyVector()
	{
		Vector<MyInterface*> arr;
		ReadOnlyArray<MyInterface*> readOnly = arr;
		AssertEquals(arr.Size(), 0);
		AssertEquals(readOnly.Size(), 0);
	}

	void EmptyVectorWithInheritence()
	{
		Vector<MyClass1*> arr;
		ReadOnlyArray<MyInterface*> readOnly = arr;
		AssertEquals(arr.Size(), 0);
		AssertEquals(readOnly.Size(), 0);
	}

	void VectorWithInheritence()
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

	void VectorWithVarargs()
	{
		Vector<I32> items(1, 2, 3);
		AssertEquals(items.Size(), 3);
		AssertEquals(items[0], 1);
		AssertEquals(items[1], 2);
		AssertEquals(items[2], 3);
	}

	void VectorInsertAtEnd()
	{
		Vector<I32> items(1, 2, 3);
		items.Add(Vector<I32>(4, 5));
		AssertEquals(items.Size(), 5);
		AssertEquals(items[0], 1);
		AssertEquals(items[1], 2);
		AssertEquals(items[2], 3);
		AssertEquals(items[3], 4);
		AssertEquals(items[4], 5);
	}

	void VectorInsertAtStart()
	{
		Vector<I32> items(1, 2, 3);
		items.InsertAt(Vector<I32>(4, 5), 0);
		AssertEquals(items.Size(), 5);
		AssertEquals(items[0], 4);
		AssertEquals(items[1], 5);
		AssertEquals(items[2], 1);
		AssertEquals(items[3], 2);
		AssertEquals(items[4], 3);
	}

	void VectorInsertAtMiddle()
	{
		Vector<I32> items(1, 2, 3);
		items.InsertAt(Vector<I32>(4, 5), 2);
		AssertEquals(items.Size(), 5);
		AssertEquals(items[0], 1);
		AssertEquals(items[1], 2);
		AssertEquals(items[2], 4);
		AssertEquals(items[3], 5);
		AssertEquals(items[4], 3);
	}

	void operator()()
	{
		TEST(ArrayWithInheritence());
		TEST(ArrayWithVarargs());

		TEST(EmptyVector());
		TEST(EmptyVectorWithInheritence());
		TEST(VectorWithInheritence());
		TEST(VectorWithVarargs());
		TEST(VectorInsertAtEnd());
		TEST(VectorInsertAtStart());
		TEST(VectorInsertAtMiddle());
	}
};


// Run all compilation tests
void SuiteCommon()
{
	SUITE(SuiteCommonArray);
}
