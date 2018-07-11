#pragma once

#include "JavaClass.h"
#include "JavaMethod.h"

class CJavaClass
{


public:

	enum {
		ACC_PUBLIC = 0x00000001,       // class, field, method, ic
		ACC_PRIVATE = 0x00000002,       // field, method, ic
		ACC_PROTECTED = 0x00000004,       // field, method, ic
		ACC_STATIC = 0x00000008,       // field, method, ic
		ACC_FINAL = 0x00000010,       // class, field, method, ic
		ACC_SYNCHRONIZED = 0x00000020,       // method (only allowed on natives)
		ACC_SUPER = 0x00000020,       // class (not used in Dalvik)
		ACC_VOLATILE = 0x00000040,       // field
		ACC_BRIDGE = 0x00000040,       // method (1.5)
		ACC_TRANSIENT = 0x00000080,       // field
		ACC_VARARGS = 0x00000080,       // method (1.5)
		ACC_NATIVE = 0x00000100,       // method
		ACC_INTERFACE = 0x00000200,       // class, ic
		ACC_ABSTRACT = 0x00000400,       // class, method, ic
		ACC_STRICT = 0x00000800,       // method
		ACC_SYNTHETIC = 0x00001000,       // field, method, ic
		ACC_ANNOTATION = 0x00002000,       // class, ic (1.5)
		ACC_ENUM = 0x00004000,       // class, field, ic (1.5)
		ACC_CONSTRUCTOR = 0x00010000,       // method (Dalvik only)
		ACC_DECLARED_SYNCHRONIZED =
		0x00020000,       // method (Dalvik only)
		ACC_CLASS_MASK =
		(ACC_PUBLIC | ACC_FINAL | ACC_INTERFACE | ACC_ABSTRACT
			| ACC_SYNTHETIC | ACC_ANNOTATION | ACC_ENUM),
		ACC_INNER_CLASS_MASK =
		(ACC_CLASS_MASK | ACC_PRIVATE | ACC_PROTECTED | ACC_STATIC),
		ACC_FIELD_MASK =
		(ACC_PUBLIC | ACC_PRIVATE | ACC_PROTECTED | ACC_STATIC | ACC_FINAL
			| ACC_VOLATILE | ACC_TRANSIENT | ACC_SYNTHETIC | ACC_ENUM),
		ACC_METHOD_MASK =
		(ACC_PUBLIC | ACC_PRIVATE | ACC_PROTECTED | ACC_STATIC | ACC_FINAL
			| ACC_SYNCHRONIZED | ACC_BRIDGE | ACC_VARARGS | ACC_NATIVE
			| ACC_ABSTRACT | ACC_STRICT | ACC_SYNTHETIC | ACC_CONSTRUCTOR
			| ACC_DECLARED_SYNCHRONIZED),
	};

	CJavaClass();
	~CJavaClass();

	CString strClassName;								// 类名
	CString strSuperName;								// 父类名
	std::vector<CString>		listStrImplements;		// 接口类型
	std::vector<CString>		listStrFields;			// 变量
	std::vector<CJavaMethod>	listJavaMethods;		// 方法列表

														// 处理 smali 文件

	std::vector<CString>		listJavaCode;			// 翻译输出的JavaCode

	BOOL AnalyzeClassSmaliListString(std::vector<CString> listCode);

	// 专门用于处理 Field 的符号的函数
	std::vector<CString> GetFieldSymbolList(CString strLine);

	// 将Java的类型，转换成Cpp的类型
	static CString GetTypeFromJava(CString strType);

	std::vector<CString> GetClassJavaCode() {
		return listJavaCode;
	}


};

