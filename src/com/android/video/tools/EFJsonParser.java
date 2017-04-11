package com.android.video.tools;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class EFJsonParser {

	/** The input JSON. */
	private final String in;

	/**
	 * The index of the next character to be returned by {@link #next}. When the
	 * input is exhausted, this equals the input's length.
	 */
	private int pos;

	/**
	 * @param in
	 *            JSON encoded string. Null is not permitted and will yield a
	 *            tokener that throws {@code NullPointerExceptions} when methods
	 *            are called.
	 */
	public EFJsonParser(String in) {
		// consume an optional byte order mark (BOM) if it exists
		if (in != null && in.startsWith("\ufeff")) {
			in = in.substring(1);
		}
		this.in = in;
	}

	/**
	 * Returns the next value from the input.
	 * 
	 * @return a {@link Map<String, Object>}, {@link List<Object>}, String, Boolean,
	 *         Integer, Long, Double or {@link Map<String, Object>#NULL}.
	 * @throws Exception
	 *             if the input is malformed.
	 */
	public Object nextValue() throws Exception {
		int c = nextCleanInternal();
		switch (c) {
		case -1:
			throw new Exception("End of input");

		case '{':
			return readObject();

		case '[':
			return readArray();

		case '\'':
		case '"':
			return nextString((char) c);

		default:
			pos--;
			return readLiteral();
		}
	}

	private int nextCleanInternal() throws Exception {
		while (pos < in.length()) {
			int c = in.charAt(pos++);
			switch (c) {
			case '\t':
			case ' ':
			case '\n':
			case '\r':
				continue;

			case '/':
				if (pos == in.length()) {
					return c;
				}

				char peek = in.charAt(pos);
				switch (peek) {
				case '*':
					// skip a /* c-style comment */
					pos++;
					int commentEnd = in.indexOf("*/", pos);
					if (commentEnd == -1) {
						throw new Exception("Unterminated comment");
					}
					pos = commentEnd + 2;
					continue;

				case '/':
					// skip a // end-of-line comment
					pos++;
					skipToEndOfLine();
					continue;

				default:
					return c;
				}

			case '#':
				/*
				 * Skip a # hash end-of-line comment. The JSON RFC doesn't
				 * specify this behavior, but it's required to parse existing
				 * documents. See http://b/2571423.
				 */
				skipToEndOfLine();
				continue;

			default:
				return c;
			}
		}

		return -1;
	}

	/**
	 * Advances the position until after the next newline character. If the line
	 * is terminated by "\r\n", the '\n' must be consumed as whitespace by the
	 * caller.
	 */
	private void skipToEndOfLine() {
		for (; pos < in.length(); pos++) {
			char c = in.charAt(pos);
			if (c == '\r' || c == '\n') {
				pos++;
				break;
			}
		}
	}

	/**
	 * Returns the string up to but not including {@code quote}, unescaping any
	 * character escape sequences encountered along the way. The opening quote
	 * should have already been read. This consumes the closing quote, but does
	 * not include it in the returned string.
	 * 
	 * @param quote
	 *            either ' or ".
	 * @throws NumberFormatException
	 *             if any unicode escape sequences are malformed.
	 */
	public String nextString(char quote) throws Exception {
		/*
		 * For strings that are free of escape sequences, we can just extract
		 * the result as a substring of the input. But if we encounter an escape
		 * sequence, we need to use a StringBuilder to compose the result.
		 */
		StringBuilder builder = null;

		/* the index of the first character not yet appended to the builder. */
		int start = pos;

		while (pos < in.length()) {
			int c = in.charAt(pos++);
			if (c == quote) {
				if (builder == null) {
					// a new string avoids leaking memory
					return new String(in.substring(start, pos - 1));
				} else {
					builder.append(in, start, pos - 1);
					return builder.toString();
				}
			}

			if (c == '\\') {
				if (pos == in.length()) {
					throw new Exception("Unterminated escape sequence");
				}
				if (builder == null) {
					builder = new StringBuilder();
				}
				builder.append(in, start, pos - 1);
				builder.append(readEscapeCharacter());
				start = pos;
			}
		}

		throw new Exception("Unterminated string");
	}

	/**
	 * Unescapes the character identified by the character or characters that
	 * immediately follow a backslash. The backslash '\' should have already
	 * been read. This supports both unicode escapes "u000A" and two-character
	 * escapes "\n".
	 * 
	 * @throws NumberFormatException
	 *             if any unicode escape sequences are malformed.
	 */
	private char readEscapeCharacter() throws Exception {
		char escaped = in.charAt(pos++);
		switch (escaped) {
		case 'u':
			if (pos + 4 > in.length()) {
				throw new Exception("Unterminated escape sequence");
			}
			String hex = in.substring(pos, pos + 4);
			pos += 4;
			return (char) Integer.parseInt(hex, 16);

		case 't':
			return '\t';

		case 'b':
			return '\b';

		case 'n':
			return '\n';

		case 'r':
			return '\r';

		case 'f':
			return '\f';

		case '\'':
		case '"':
		case '\\':
		default:
			return escaped;
		}
	}

	/**
	 * Reads a null, boolean, numeric or unquoted string literal value. Numeric
	 * values will be returned as an Integer, Long, or Double, in that order of
	 * preference.
	 */
	private Object readLiteral() throws Exception {
		String literal = nextToInternal("{}[]/\\:,=;# \t\f");

		if (literal.length() == 0) {
			throw new Exception("Expected literal value");
		} else if ("null".equalsIgnoreCase(literal)) {
			return null ;
		} else if ("true".equalsIgnoreCase(literal)) {
			return Boolean.TRUE;
		} else if ("false".equalsIgnoreCase(literal)) {
			return Boolean.FALSE;
		}

		/* try to parse as an integral type... */
		if (literal.indexOf('.') == -1) {
			int base = 10;
			String number = literal;
			if (number.startsWith("0x") || number.startsWith("0X")) {
				number = number.substring(2);
				base = 16;
			} else if (number.startsWith("0") && number.length() > 1) {
				number = number.substring(1);
				base = 8;
			}
			try {
				long longValue = Long.parseLong(number, base);
				if (longValue <= Integer.MAX_VALUE
						&& longValue >= Integer.MIN_VALUE) {
					return (int) longValue;
				} else {
					return longValue;
				}
			} catch (NumberFormatException e) {
				/*
				 * This only happens for integral numbers greater than
				 * Long.MAX_VALUE, numbers in exponential form (5e-10) and
				 * unquoted strings. Fall through to try floating point.
				 */
			}
		}

		/* ...next try to parse as a floating point... */
		try {
			return Double.valueOf(literal);
		} catch (NumberFormatException ignored) {
		}

		/* ... finally give up. We have an unquoted string */
		return new String(literal); // a new string avoids leaking memory
	}

	/**
	 * Returns the string up to but not including any of the given characters or
	 * a newline character. This does not consume the excluded character.
	 */
	private String nextToInternal(String excluded) {
		int start = pos;
		for (; pos < in.length(); pos++) {
			char c = in.charAt(pos);
			if (c == '\r' || c == '\n' || excluded.indexOf(c) != -1) {
				return in.substring(start, pos);
			}
		}
		return in.substring(start);
	}

	/**
	 * Reads a sequence of key/value pairs and the trailing closing brace '}' of
	 * an object. The opening brace '{' should have already been read.
	 */
	private Map<String, Object> readObject() throws Exception {
		Map<String, Object> result = new HashMap<String, Object>();

		/* Peek to see if this is the empty object. */
		int first = nextCleanInternal();
		if (first == '}') {
			return result;
		} else if (first != -1) {
			pos--;
		}

		while (true) {
			Object name = nextValue();
			if (!(name instanceof String)) {
				if (name == null) {
					throw new Exception("Names cannot be null");
				} else {
					throw new Exception("Names must be strings, but " + name
							+ " is of type " + name.getClass().getName());
				}
			}

			/*
			 * Expect the name/value separator to be either a colon ':', an
			 * equals sign '=', or an arrow "=>". The last two are bogus but we
			 * include them because that's what the original implementation did.
			 */
			int separator = nextCleanInternal();
			if (separator != ':' && separator != '=') {
				throw new Exception("Expected ':' after " + name);
			}
			if (pos < in.length() && in.charAt(pos) == '>') {
				pos++;
			}

			result.put((String) name, nextValue());

			switch (nextCleanInternal()) {
			case '}':
				return result;
			case ';':
			case ',':
				continue;
			default:
				throw new Exception("Unterminated object");
			}
		}
	}

	/**
	 * Reads a sequence of values and the trailing closing brace ']' of an
	 * array. The opening brace '[' should have already been read. Note that
	 * "[]" yields an empty array, but "[,]" returns a two-element array
	 * equivalent to "[null,null]".
	 */
	private List<Object> readArray() throws Exception {
		List<Object> result = new ArrayList<Object>();
	 	
		/* to cover input that ends with ",]". */
		boolean hasTrailingSeparator = false;

		while (true) {
			switch (nextCleanInternal()) {
			case -1:
				throw new Exception("Unterminated array");
			case ']':
				if (hasTrailingSeparator) {
					result.add(null);
				}
				return result;
			case ',':
			case ';':
				/* A separator without a value first means "null". */
				result.add(null);
				hasTrailingSeparator = true;
				continue;
			default:
				pos--;
			}

			result.add(nextValue());

			switch (nextCleanInternal()) {
			case ']':
				return result;
			case ',':
			case ';':
				hasTrailingSeparator = true;
				continue;
			default:
				throw new Exception("Unterminated array");
			}
		}
	}

	/**
	 * Returns the current position and the entire input string.
	 */
	@Override
	public String toString() {
		// consistent with the original implementation
		return " at character " + pos + " of " + in;
	}

	/*
	 * Legacy APIs.
	 * 
	 * None of the methods below are on the critical path of parsing JSON
	 * documents. They exist only because they were exposed by the original
	 * implementation and may be used by some clients.
	 */

	/**
	 * Returns true until the input has been exhausted.
	 */
	public boolean more() {
		return pos < in.length();
	}

	/**
	 * Returns the next available character, or the null character '\0' if all
	 * input has been exhausted. The return value of this method is ambiguous
	 * for JSON strings that contain the character '\0'.
	 */
	public char next() {
		return pos < in.length() ? in.charAt(pos++) : '\0';
	}

	/**
	 * Returns the next available character if it equals {@code c}. Otherwise an
	 * exception is thrown.
	 */
	public char next(char c) throws Exception {
		char result = next();
		if (result != c) {
			throw new Exception("Expected " + c + " but was " + result);
		}
		return result;
	}

	/**
	 * Returns the next character that is not whitespace and does not belong to
	 * a comment. If the input is exhausted before such a character can be
	 * found, the null character '\0' is returned. The return value of this
	 * method is ambiguous for JSON strings that contain the character '\0'.
	 */
	public char nextClean() throws Exception {
		int nextCleanInt = nextCleanInternal();
		return nextCleanInt == -1 ? '\0' : (char) nextCleanInt;
	}

	/**
	 * Returns the next {@code length} characters of the input.
	 * 
	 * <p>
	 * The returned string shares its backing character array with this
	 * tokener's input string. If a reference to the returned string may be held
	 * indefinitely, you should use {@code new String(result)} to copy it first
	 * to avoid memory leaks.
	 * 
	 * @throws Exception
	 *             if the remaining input is not long enough to satisfy this
	 *             request.
	 */
	public String next(int length) throws Exception {
		if (pos + length > in.length()) {
			throw new Exception(length + " is out of bounds");
		}
		String result = in.substring(pos, pos + length);
		pos += length;
		return result;
	}

	/**
	 * Returns the {@link String#trim trimmed} string holding the characters up
	 * to but not including the first of:
	 * <ul>
	 * <li>any character in {@code excluded}
	 * <li>a newline character '\n'
	 * <li>a carriage return '\r'
	 * </ul>
	 * 
	 * <p>
	 * The returned string shares its backing character array with this
	 * tokener's input string. If a reference to the returned string may be held
	 * indefinitely, you should use {@code new String(result)} to copy it first
	 * to avoid memory leaks.
	 * 
	 * @return a possibly-empty string
	 */
	public String nextTo(String excluded) {
		if (excluded == null) {
			throw new NullPointerException();
		}
		return nextToInternal(excluded).trim();
	}

	/**
	 * Equivalent to {@code nextTo(String.valueOf(excluded))}.
	 */
	public String nextTo(char excluded) {
		return nextToInternal(String.valueOf(excluded)).trim();
	}

	/**
	 * Advances past all input up to and including the next occurrence of
	 * {@code thru}. If the remaining input doesn't contain {@code thru}, the
	 * input is exhausted.
	 */
	public void skipPast(String thru) {
		int thruStart = in.indexOf(thru, pos);
		pos = thruStart == -1 ? in.length() : (thruStart + thru.length());
	}

	/**
	 * Advances past all input up to but not including the next occurrence of
	 * {@code to}. If the remaining input doesn't contain {@code to}, the input
	 * is unchanged.
	 */
	public char skipTo(char to) {
		int index = in.indexOf(to, pos);
		if (index != -1) {
			pos = index;
			return to;
		} else {
			return '\0';
		}
	}

	/**
	 * Unreads the most recent character of input. If no input characters have
	 * been read, the input is unchanged.
	 */
	public void back() {
		if (--pos == -1) {
			pos = 0;
		}
	}

	/**
	 * Returns the integer [0..15] value for the given hex character, or -1 for
	 * non-hex input.
	 * 
	 * @param hex
	 *            a character in the ranges [0-9], [A-F] or [a-f]. Any other
	 *            character will yield a -1 result.
	 */
	public static int dehexchar(char hex) {
		if (hex >= '0' && hex <= '9') {
			return hex - '0';
		} else if (hex >= 'A' && hex <= 'F') {
			return hex - 'A' + 10;
		} else if (hex >= 'a' && hex <= 'f') {
			return hex - 'a' + 10;
		} else {
			return -1;
		}
	}
}