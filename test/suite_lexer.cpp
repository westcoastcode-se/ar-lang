#include "test.hpp"

// Base class for all vm tests
struct suite_lexer_utils : test_utils
{
	void verify_token(vmc_lexer& lexer, vmc_lexer_token& token, vmc_lexer_token_type type) {
		vmc_lexer_next_newline(&lexer, &token);
		if (type != token.type)
			throw_(error() << "error token: " << type << " but was : " << token.type);
	}

	void verify_token(vmc_lexer& lexer, vmc_lexer_token& token, vmc_lexer_token_type type, const char* str) {
		vmc_lexer_next_newline(&lexer, &token);
		if (type != token.type)
			throw_(error() << "error token: " << type << " but was : " << token.type);
		if (!vm_string_cmpsz(&token.string, str, strlen(str)))
			throw_("expected \"'%s'\" but was \"'%.*s'\"",
				str,
				vm_string_length(&token.string), token.string.start);
	}

	void verify_whitespace_until_eof(vmc_lexer& lexer, vmc_lexer_token& token) {
		vmc_lexer_next_newline(&lexer, &token);
		while(token.type == VMC_LEXER_TYPE_NEWLINE)
			vmc_lexer_next_newline(&lexer, &token);

		if (token.type != VMC_LEXER_TYPE_EOF)
			throw_(error() << "expected eof but was " << token.type);
	}

	void verify_modifier(vmc_lexer_token& t, vm_int32 bit) {
		if ((t.modifier & bit) != bit)
			throw_(error() << "expected modifier " << bit << " to be set in value " << t.modifier);
	}
	
	void verify_successful(vmc_lexer& l) {
		if (vmc_lexer_success(&l))
			return;
		error_string_stream e;
		e << "could parse source code: [";
		auto message = l.messages_first;
		while (message != nullptr) {
			e << "\n" << message->message;
			message = message->next;
		}
		e << "\n]";
		vmc_lexer_release(&l);
		throw_(e);
	}
};

// All test functions
struct suite_lexer_tests : suite_lexer_utils
{
	void empty_string()
	{
		const auto source = R"()";
		vmc_lexer lexer;
		vmc_lexer_init(&lexer, source);

		// Verify lexer
		vmc_lexer_token token;
		verify_token(lexer, token, VMC_LEXER_TYPE_EOF);
		verify_successful(lexer);
	}

	void simple_function()
	{
		const auto source = R"(
		fn Calc(lhs int32, rhs int32) (int32) {
			return lhs + rhs
		}
)";
		vmc_lexer lexer;
		vmc_lexer_init(&lexer, source);

		// Verify lexer
		vmc_lexer_token token;
		verify_token(lexer, token, VMC_LEXER_TYPE_NEWLINE);

		verify_token(lexer, token, VMC_LEXER_TYPE_KEYWORD_FN);
		verify_token(lexer, token, VMC_LEXER_TYPE_KEYWORD, "Calc");

		verify_token(lexer, token, VMC_LEXER_TYPE_PARAN_L);
		verify_token(lexer, token, VMC_LEXER_TYPE_KEYWORD, "lhs");
		verify_token(lexer, token, VMC_LEXER_TYPE_KEYWORD, "int32");
		verify_token(lexer, token, VMC_LEXER_TYPE_COMMA);
		verify_token(lexer, token, VMC_LEXER_TYPE_KEYWORD, "rhs");
		verify_token(lexer, token, VMC_LEXER_TYPE_KEYWORD, "int32");
		verify_token(lexer, token, VMC_LEXER_TYPE_PARAN_R);

		verify_token(lexer, token, VMC_LEXER_TYPE_PARAN_L);
		verify_token(lexer, token, VMC_LEXER_TYPE_KEYWORD, "int32");
		verify_token(lexer, token, VMC_LEXER_TYPE_PARAN_R);

		verify_token(lexer, token, VMC_LEXER_TYPE_BRACKET_L);
		verify_token(lexer, token, VMC_LEXER_TYPE_NEWLINE);

		verify_token(lexer, token, VMC_LEXER_TYPE_KEYWORD_RETURN);
		verify_token(lexer, token, VMC_LEXER_TYPE_KEYWORD, "lhs");
		verify_token(lexer, token, VMC_LEXER_TYPE_PLUS);
		verify_token(lexer, token, VMC_LEXER_TYPE_KEYWORD, "rhs");
		verify_token(lexer, token, VMC_LEXER_TYPE_NEWLINE);

		verify_token(lexer, token, VMC_LEXER_TYPE_BRACKET_R);

		verify_whitespace_until_eof(lexer, token);
		verify_successful(lexer);
	}

	void single_line_comment()
	{
		const auto source = R"(// Test)";
		vmc_lexer lexer;
		vmc_lexer_init(&lexer, source);

		// Verify lexer
		vmc_lexer_token token;
		verify_token(lexer, token, VMC_LEXER_TYPE_COMMENT, " Test");
		verify_token(lexer, token, VMC_LEXER_TYPE_EOF);
		verify_successful(lexer);
	}

	void multi_line_comment1()
	{
		const auto source = "/* Test */";
		vmc_lexer lexer;
		vmc_lexer_init(&lexer, source);

		// Verify lexer
		vmc_lexer_token token;
		verify_token(lexer, token, VMC_LEXER_TYPE_COMMENT, " Test ");
		verify_modifier(token, VMC_LEXER_TOKEN_MODIFIER_MULTILINE);
		verify_token(lexer, token, VMC_LEXER_TYPE_EOF);
		verify_successful(lexer);
	}

	void multi_line_comment2()
	{
		const auto source = R"(/* 
Test */)";
		vmc_lexer lexer;
		vmc_lexer_init(&lexer, source);

		// Verify lexer
		vmc_lexer_token token;
		verify_token(lexer, token, VMC_LEXER_TYPE_COMMENT, " \nTest ");
		verify_modifier(token, VMC_LEXER_TOKEN_MODIFIER_MULTILINE);
		verify_token(lexer, token, VMC_LEXER_TYPE_EOF);
		verify_successful(lexer);
	}

	void multi_line_comment3()
	{
		const auto source = R"(/* 
Test
*/)";
		vmc_lexer lexer;
		vmc_lexer_init(&lexer, source);

		// Verify lexer
		vmc_lexer_token token;
		verify_token(lexer, token, VMC_LEXER_TYPE_COMMENT, " \nTest\n");
		verify_modifier(token, VMC_LEXER_TOKEN_MODIFIER_MULTILINE);
		verify_token(lexer, token, VMC_LEXER_TYPE_EOF);
		verify_successful(lexer);
	}

	void verify_string1() {
		const auto source = "\"Test\"";
		vmc_lexer lexer;
		vmc_lexer_init(&lexer, source);

		// Verify lexer
		vmc_lexer_token token;
		verify_token(lexer, token, VMC_LEXER_TYPE_STRING, "Test");
		verify_token(lexer, token, VMC_LEXER_TYPE_EOF);
		verify_successful(lexer);
	}

	void verify_string2() {
		const auto source = "\"\\\"Test\\\"\"";
		vmc_lexer lexer;
		vmc_lexer_init(&lexer, source);

		// Verify lexer
		vmc_lexer_token token;
		verify_token(lexer, token, VMC_LEXER_TYPE_STRING, "\\\"Test\\\"");
		verify_modifier(token, VMC_LEXER_TOKEN_MODIFIER_ESCAPED);
		verify_token(lexer, token, VMC_LEXER_TYPE_EOF);
		verify_successful(lexer);
	}

	void verify_string3() {
		const auto source = R"(`Test`)";
		vmc_lexer lexer;
		vmc_lexer_init(&lexer, source);

		// Verify lexer
		vmc_lexer_token token;
		verify_token(lexer, token, VMC_LEXER_TYPE_STRING, "Test");
		verify_modifier(token, VMC_LEXER_TOKEN_MODIFIER_MULTILINE);
		verify_token(lexer, token, VMC_LEXER_TYPE_EOF);
		verify_successful(lexer);
	}

	void verify_int() {
		const auto source = "123";
		vmc_lexer lexer;
		vmc_lexer_init(&lexer, source);

		// Verify lexer
		vmc_lexer_token token;
		verify_token(lexer, token, VMC_LEXER_TYPE_INT, "123");
		verify_token(lexer, token, VMC_LEXER_TYPE_EOF);
		verify_successful(lexer);
	}

	void operator()()
	{
		TEST(empty_string);

		TEST(simple_function);

		TEST(single_line_comment);

		TEST(multi_line_comment1);
		TEST(multi_line_comment2);
		TEST(multi_line_comment3);

		TEST(verify_string1);
		TEST(verify_string2);
		TEST(verify_string3);

		TEST(verify_int);
	}
};

void suite_lexer()
{
	SUITE(suite_lexer_tests);
}
