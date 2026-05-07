#pragma once

typedef struct string_t {
	char* data;
	size_t len;
	//func:
	size_t			   (*Length)      (struct string_t* self);
	size_t		       (*IndexOf)     (struct string_t* self, char* string, int from, int count);
	bool			   (*StartsWith)  (struct string_t* self, char* string);
	bool			   (*EndsWith)    (struct string_t* self, char* string);
	bool			   (*Contains)    (struct string_t* self, char* string);
	bool               (*Equals)      (struct string_t* self, const char* other);
	void               (*Clear)       (struct string_t* self);
	struct string_t    (*Clone)       (struct string_t* self);
	struct string_t    (*Replace)     (struct string_t* self, char* oldstring, char* newstring);
	void               (*AddChars)    (struct string_t* self, char* string);
	void               (*SetChars)    (struct string_t* self, int index, char* string);
	struct string_t    (*Skip)        (struct string_t* self, int count);
	struct string_t    (*Take)        (struct string_t* self, int count);
	struct string_t    (*SubString)   (struct string_t* self, int from, int count);
	char*              (*ToChars)     (struct string_t* self);
	void               (*Dispose)	  (struct string_t* self);
	String             (*FromInt)     (int value);
}String;

static size_t String_Length(String* self);
static size_t String_IndexOf(String* self, char* string, int from, int count);
static bool String_StartsWith(String* self, char* string);
static bool String_EndsWith(String* self, char* string);
static bool String_Contains(String* self, char* string);
static void String_Clear(String* self);
static String String_Clone(String* self);
static String String_Replace(String* self, char* oldstring, char* newstring);
static void String_AddChars(String* self, char* string);
static void String_SetChars(String* self, int index, char* string);
static String String_Skip(String* self, int count);
static String String_Take(String* self, int count);
static String String_SubString(String* self, int from, int count);
static char* String_ToChars(String* self);
static void String_Dispose(String* self);
static bool String_Equals(String* self, const char* other);
static String FromInt(int value);

static void String_Init(String* s) {
	s->Length = String_Length;
	s->IndexOf = String_IndexOf;
	s->StartsWith = String_StartsWith;
	s->EndsWith = String_EndsWith;
	s->Contains = String_Contains;
	s->Clear = String_Clear;
	s->Clone = String_Clone;
	s->Replace = String_Replace;
	s->AddChars = String_AddChars;
	s->SetChars = String_SetChars;
	s->Skip = String_Skip;
	s->Take = String_Take;
	s->SubString = String_SubString;
	s->ToChars = String_ToChars;
	s->Dispose = String_Dispose;
	s->Equals = String_Equals;
	s->FromInt = FromInt;
}
String String_FromInt(int value) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%d", value);
	return GetString(buf);   // 复用 GetString
}
static bool String_Equals(struct string_t* self, const char* other) {
	if (!self || !self->data || !other) return false;
	return strcmp(self->data, other) == 0;
}

String GetString(const char* source){
	String ParseredString;
	if (!source) source = "";
	ParseredString.len = strlen(source);
	ParseredString.data = (char*)malloc(ParseredString.len + 1);
	if (ParseredString.data) {
		memcpy(ParseredString.data, source, ParseredString.len);
		ParseredString.data[ParseredString.len] = '\0';
	}
	else {
		ParseredString.len = 0;
		ParseredString.data = NULL;
	}
	String_Init(&ParseredString);
	return ParseredString;
}

static size_t String_Length(struct string_t* self) {
	if (!self || !self->data) return 0;
	return self->len;
}

static size_t String_IndexOf(struct string_t* self, const char* str, int from, int count) {
	if (!self || !self->data || !str || !*str) return (size_t)-1;  // 空串返回 -1 (size_t 的最大值)
	if (from < 0) from = 0;
	if (from >= (int)self->len) return (size_t)-1;

	// 计算实际要搜索的结尾位置
	size_t end;
	if (count < 0 || from + count >(int)self->len) {
		end = self->len;
	}
	else {
		end = from + count;
	}

	// 从 from 开始，在 [from, end) 范围内查找
	for (size_t i = from; i + strlen(str) <= end; i++) {
		if (memcmp(self->data + i, str, strlen(str)) == 0) {
			return i;
		}
	}
	return (size_t)-1;
}

static bool String_StartsWith(struct string_t* self, const char* prefix) {
	if (!self || !self->data || !prefix) return false;
	size_t pre_len = strlen(prefix);
	if (pre_len > self->len) return false;
	return memcmp(self->data, prefix, pre_len) == 0;
}

static bool String_EndsWith(struct string_t* self, const char* suffix) {
	if (!self || !self->data || !suffix) return false;
	size_t suf_len = strlen(suffix);
	if (suf_len > self->len) return false;
	return memcmp(self->data + self->len - suf_len, suffix, suf_len) == 0;
}


static bool String_Contains(struct string_t* self, const char* str) {
	size_t pos = String_IndexOf(self, str, 0, -1);
	return pos != (size_t)-1;
}

static void String_Clear(struct string_t* self) {
	if (!self) return;
	free(self->data);  // 释放旧数据
	self->len = 0;
	self->data = (char*)malloc(1);
	if (self->data) {
		self->data[0] = '\0';
	}
}


static struct string_t String_Clone(struct string_t* self) {
	String clone;
	if (!self || !self->data) {
		// 如果自己就是空的，克隆空串
		clone.len = 0;
		clone.data = (char*)malloc(1);
		if (clone.data) clone.data[0] = '\0';
	}
	else {
		clone.len = self->len;
		clone.data = (char*)malloc(clone.len + 1);
		if (clone.data) {
			memcpy(clone.data, self->data, clone.len);
			clone.data[clone.len] = '\0';
		}
	}
	String_BindMethods(&clone);
	return clone;
}



static struct string_t String_Replace(struct string_t* self, const char* oldstr, const char* newstr) {
	String result;
	if (!self || !self->data || !oldstr || !*oldstr) {
		// 如果原串为空或旧串为空，就返回原串的副本
		return String_Clone(self);
	}
	if (!newstr) newstr = "";  // 空替换

	size_t old_len = strlen(oldstr);
	size_t new_len = strlen(newstr);

	// 先计算有多少处替换
	size_t count = 0;
	size_t pos = 0;
	while (pos <= self->len - old_len) {
		if (memcmp(self->data + pos, oldstr, old_len) == 0) {
			count++;
			pos += old_len;
		}
		else {
			pos++;
		}
	}

	// 计算结果串长度
	size_t result_len = self->len + count * (new_len - old_len);
	result.data = (char*)malloc(result_len + 1);
	result.len = result_len;
	if (!result.data) {
		result.len = 0;
		String_BindMethods(&result);
		return result;
	}

	// 构建新字符串
	char* dst = result.data;
	const char* src = self->data;
	while (*src) {
		if (memcmp(src, oldstr, old_len) == 0) {
			memcpy(dst, newstr, new_len);
			dst += new_len;
			src += old_len;
		}
		else {
			*dst++ = *src++;
		}
	}
	*dst = '\0';

	String_BindMethods(&result);
	return result;
}


static void String_AddChars(struct string_t* self, const char* str) {
	if (!self || !str) return;
	if (!self->data) {
		// 如果自身 data 为空，变成新字符串
		self->len = strlen(str);
		self->data = (char*)malloc(self->len + 1);
		if (self->data) {
			memcpy(self->data, str, self->len);
			self->data[self->len] = '\0';
		}
		return;
	}
	size_t add_len = strlen(str);
	char* new_data = (char*)realloc(self->data, self->len + add_len + 1);
	if (!new_data) return;  // 分配失败，原数据不变
	memcpy(new_data + self->len, str, add_len);
	self->len += add_len;
	new_data[self->len] = '\0';
	self->data = new_data;
}





static void String_SetChars(struct string_t* self, int index, const char* str) {
	if (!self || !self->data || !str) return;
	if (index < 0) index = 0;
	if (index > (int)self->len) index = self->len; // 如果超过长度，就当追加

	size_t new_len = index + strlen(str);
	char* new_data = (char*)malloc(new_len + 1);
	if (!new_data) return;

	// 拷贝前半部分
	memcpy(new_data, self->data, index);
	// 拷贝新字符串
	memcpy(new_data + index, str, strlen(str));
	new_data[new_len] = '\0';

	free(self->data);
	self->data = new_data;
	self->len = new_len;
}


static struct string_t String_Skip(struct string_t* self, int count) {
	if (!self || !self->data || count < 0) count = 0;
	if (count >= (int)self->len) {
		// 返回空串
		return String_From("");
	}
	// 直接利用 SubString 实现，避免重复代码
	return String_SubString(self, count, (int)(self->len - count));
}


static struct string_t String_Take(struct string_t* self, int count) {
	if (!self || !self->data || count <= 0) {
		return String_From("");
	}
	if (count > (int)self->len) count = self->len;
	return String_SubString(self, 0, count);
}

static struct string_t String_SubString(struct string_t* self, int from, int count) {
	String sub;
	if (!self || !self->data || from < 0 || count <= 0 || from >= (int)self->len) {
		return String_From("");
	}
	if (from + count > (int)self->len) {
		count = self->len - from;  // 截取到末尾
	}
	sub.len = count;
	sub.data = (char*)malloc(count + 1);
	if (sub.data) {
		memcpy(sub.data, self->data + from, count);
		sub.data[count] = '\0';
	}
	else {
		sub.len = 0;
	}
	String_BindMethods(&sub);
	return sub;
}

static char* String_ToChars(struct string_t* self) {
	if (!self) return NULL;
	return self->data;  // 可能为 NULL
}

static void String_Dispose(struct string_t* self) {
	if (!self) return;
	free(self->data);
	self->data = NULL;
	self->len = 0;
}
