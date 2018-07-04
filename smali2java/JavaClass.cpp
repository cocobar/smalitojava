#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"


CJavaClass::CJavaClass()
{
}


CJavaClass::~CJavaClass()
{
}

CString CJavaClass::GetCppTypeFromJava(CString strType) {
	int nArrayCount = 0;
	CString strTypeString;

	if (strType.Find("[") == 0) {
		for (int i = 0; i < strType.GetLength(); i++) {
			if (strType[i] == '[') {
				nArrayCount++;
			}
			else {
				strType = strType.Right(strType.GetLength() - nArrayCount);
				break;
			}
		}
	}


	/*
	case 'B':   return "byte";
	case 'C':   return "char";
	case 'D':   return "double";
	case 'F':   return "float";
	case 'I':   return "int";
	case 'J':   return "long";
	case 'S':   return "short";
	case 'V':   return "void";
	case 'Z':   return "boolean";
	case 'L':
	*/
	if (strType.Compare("B") == 0) {
		strTypeString = CString("unsigned char");
	}
	else if (strType.Compare("C") == 0) {
		strTypeString = CString("char");
	}
	else if (strType.Compare("D") == 0) {
		strTypeString = CString("double");
	}
	else if (strType.Compare("F") == 0) {
		strTypeString = CString("float");
	}
	else if (strType.Compare("I") == 0) {
		strTypeString = CString("int");
	}
	else if (strType.Compare("J") == 0) {
		strTypeString = CString("long");
	}
	else if (strType.Compare("S") == 0) {
		strTypeString = CString("short");
	}
	else if (strType.Compare("V") == 0) {
		strTypeString = CString("void");
	}
	else if (strType.Compare("Z") == 0) {
		strTypeString = CString("bool");
	}
	else if (strType.Find("L") == 0) {
		strTypeString = strType;
#if 1
		strTypeString.Replace('/', '_');
		strTypeString.Replace('$', '_');
		strTypeString.Replace(';', ' ');
#else
		strTypeString.Remove('/');
		strTypeString.Remove('$');
		strTypeString.Remove(';');
#endif
		strTypeString.Trim();
		strTypeString = strTypeString.Right(strTypeString.GetLength() - 1);
	}
	else {
		printf("未知的类型 %s \n", strType);
		strTypeString = CString("UnknowType");
	}

	// 用Cpp的vector来嵌套替代数组
#if 1
	while (nArrayCount--) {
		strTypeString = CString("std::vector<") + strTypeString + CString(">");
	}
#else
	while (nArrayCount--) {
		strTypeString += CString("[]");
	}
#endif

	return strTypeString;
}

// 返回类型列表
std::vector<CString> CJavaClass::GetFieldSymbolList(CString strLine) {
	std::vector<CString> listSymbol;

	BOOL bInString = false;
	BOOL bChangeSymbol = false;
	CString strNote;

	listSymbol.clear();
	strLine += CString("  ");	// 优化一下下面的代码处理流程

	int nState = 0;
	for (int i = 0; i < strLine.GetLength(); i++) {
		char _c = strLine[i];

		switch (nState) {
		case 0:		// 刚开始
			strNote.Empty();

			if (
				((_c >= 'a') && (_c <= 'z')) || ((_c >= 'A') && (_c <= 'Z')) ||
				(_c == '_') || (_c == '.') || (_c == '[')
				)
			{
				strNote += _c;
				nState = 1;  // 进入标准符号
			}
			else if (
				_c == '\''
				) {
				strNote += _c;
				nState = 2;	 // 进入字节内容
			}
			else if (
				_c == '\"'
				) {
				strNote += _c;
				nState = 3;	 // 进入文本内容
			}
			else if (
				(_c == '=') || (_c == '+') || (_c == '-')
				)
			{
				strNote += _c;
				nState = 4;	 // 进入符号
			}
			else if ((_c >= '0') && (_c <= '9')) {
				strNote += _c;
				nState = 5;	 // 进入数字
			}
			else if ((_c == ' ') || (_c == '\t'))
			{
				//  这里是合法的空格
			}
			else {
				printf("Unknow Symbol %c at %d of %s\n", _c, i, strLine.GetBuffer());
				strLine.ReleaseBuffer();
			}
			break;
		case 1:	// 标准符号
			if (
				((_c >= 'a') && (_c <= 'z')) || ((_c >= 'A') && (_c <= 'Z')) || ((_c >= '0') && (_c <= '9')) ||
				(_c == '_') || (_c == '&') || (_c == '/') || (_c == ':') || (_c == ';') || (_c == '.') || (_c == '[')
				)
			{
				strNote += _c;
			}
			else {
				i--; nState = 0;
				strNote.Trim();
				listSymbol.push_back(strNote);
			}

			break;
		case 2:	// 进入字节内容

			if (bChangeSymbol) {
				strNote += _c;
				bChangeSymbol = false;
			}
			else {
				if ((_c == '\\'))
				{
					strNote += _c;
					bChangeSymbol = true;
				}
				else if ((_c == '\''))
				{
					strNote += _c;
					i--; nState = 0;
					strNote.Trim();
					listSymbol.push_back(strNote);
				}
				else {
					strNote += _c;
				}
			}

			break;

		case 3:	// 进入字串内容

			if (bChangeSymbol) {
				strNote += _c;
				bChangeSymbol = false;
			}
			else {
				if ((_c == '\\'))
				{
					strNote += _c;
					bChangeSymbol = true;
				}
				else if ((_c == '\"'))
				{
					strNote += _c;
					i--; nState = 0;
					strNote.Trim();
					listSymbol.push_back(strNote);
				}
				else {
					strNote += _c;
				}
			}
			break;

		case 4:	// 进入符号
		{
			CString strTmp = (strNote + _c);

			if ((strTmp.Compare("++") == 0) || (strTmp.Compare("+=") == 0) || (strTmp.Compare("--") == 0) || (strTmp.Compare("-=") == 0) || (strTmp.Compare("==") == 0)) {
				strNote += _c;
				nState = 0;
				strNote.Trim();
				listSymbol.push_back(strNote);
			}
			else {
				i--; nState = 0;
				strNote.Trim();
				listSymbol.push_back(strNote);
			}

		}
		break;
		case 5:	// 进入数字
			if (
				((_c >= '0') && (_c <= '9')) ||
				(_c == 'x') || (_c == 'X') || (_c == 'l') || (_c == 'L') || (_c == '.')
				)
			{
				strNote += _c;
			}
			else {
				i--; nState = 0;
				strNote.Trim();
				listSymbol.push_back(strNote);
			}

			break;
		}
	}

	return listSymbol;
}

// 开始处理单个 Smali 文件
BOOL CJavaClass::AnalyzeClassSmali(CString strSmaliFile) {

	CStdioFile cStdFile;

	if (cStdFile.Open(strSmaliFile, CFile::modeRead)) {
		CString strLine;

		while (cStdFile.ReadString(strLine)) {

			if (strLine.Trim().GetLength() > 0) {

				if (strLine.Find("#") == 0) {						// 这个是注释行
					printf("处理 %s\n", strLine);
				}
				else if (strLine.Find(".class") == 0) {				// 找到当前Class的名称
					int nFindL = strLine.Find("L");
					if (nFindL > 0) {
						strClassName = GetCppTypeFromJava(strLine.Right(strLine.GetLength() - nFindL));
						//strClassName = CJavaClass::GetCppTypeFromJava(strLine.Right(strLine.GetLength() - nFindL));
					}
					else {
						strClassName = CString("Unknow_Class_Name");
						printf("Class 名称处理错误\n");
					}
				}
				else if (strLine.Find(".super") == 0) {				// 父类的名称
					int nFindL = strLine.Find("L");
					if (nFindL > 0) {
						strSuperName = GetCppTypeFromJava(strLine.Right(strLine.GetLength() - nFindL));
					}
					else {
						strSuperName = CString("Unknow_Class_Name");
						printf("Super 名称处理错误\n");
					}
				}
				else if (strLine.Find(".implements") == 0) {		// 实现
					int nFindL = strLine.Find("L");
					CString strImplement;
					if (nFindL > 0) {
						strImplement = strLine.Right(strLine.GetLength() - nFindL);
					}
					else {
						strImplement = CString("Unknow_Class_Name");
						printf("Implements 名称处理错误\n");
					}

					listStrImplements.push_back(strImplement);
				}
				else if (strLine.Find(".field") == 0) {				// 成员变量
					unsigned int accessFlags = 0;
					BOOL bHasEq = false;
					CString strNote;
					CString valType;
					CString valName;
					CString valValue;

					std::vector<CString> listSymbol = GetFieldSymbolList(strLine);
					std::vector<CString>::iterator it;
					for (it = listSymbol.begin(); it != listSymbol.end(); it++) {
						strNote = (*it);

						if (strNote.Find(".field") == 0) {
							//
						}
						else if (strNote.Find("private") == 0) {
							accessFlags |= ACC_PRIVATE;
						}
						else if (strNote.Find("static") == 0) {
							accessFlags |= ACC_STATIC;
						}
						else if (strNote.Find("final") == 0) {
							accessFlags |= ACC_FINAL;
						}
						else if (strNote.Find("public") == 0) {
							accessFlags |= ACC_PUBLIC;
						}
						else if (strNote.Find("protected") == 0) {
							accessFlags |= ACC_PROTECTED;
						}
						else if (strNote.Find("volatile") == 0) {
							accessFlags |= ACC_VOLATILE;
						}
						else if (strNote.Find("transient") == 0) {
							accessFlags |= ACC_TRANSIENT;
						}
						else if (strNote.Find("synthetic") == 0) {
							accessFlags |= ACC_SYNTHETIC;
						}
						else if (strNote.Find("enum") == 0) {
							accessFlags |= ACC_ENUM;
						}
						else if (strNote.Find("=") == 0) {
							bHasEq = true;
						}
						else if ((strNote.Find(":") > 0) && (bHasEq == false)) {
							int nFindDot = strNote.Find(":");
							valName = strNote.Left(nFindDot);
							valType = strNote.Right(strNote.GetLength() - nFindDot - 1);
							valName.Trim();
							valType.Trim();
						}
						else if (bHasEq) {
							valValue = strNote;
						}
						else {
							printf("未知符号 strNote = %s\n", strNote.GetBuffer());
							strNote.ReleaseBuffer();
						}
					}

					// 补充成C++的代码
					CString cppString;

					if (accessFlags & ACC_PRIVATE) {
						cppString += CString("private ");
					}

					if (accessFlags & ACC_STATIC) {
						cppString += CString("static ");
					}

					if (accessFlags & ACC_FINAL) {
						cppString += CString("const ");
					}

					cppString += (GetCppTypeFromJava(valType) + CString(" ") + valName);
					if (bHasEq) {
						cppString += (CString(" = ") + valValue);
					}
					cppString += CString(";");
					printf("Cpp Code: %s\n", cppString);

					listStrFields.push_back(cppString);
				}
				else if (strLine.Find(".method") == 0) {				// 成员方法

																		// 将整个method部分的代码放入 listMethodInst 数组
					std::vector<CString> listMethodInst;
					listMethodInst.clear();
					listMethodInst.push_back(strLine);

					CJavaMethod cJavaMethod;

					printf("###################################################################################################\n");
					printf("开始处理函数 %s\n", strLine);

					while (cStdFile.ReadString(strLine)) {
						if (strLine.Trim().GetLength() > 0) {			// 过滤掉空行
							listMethodInst.push_back(strLine);
							if (strLine.Find(".end method") == 0) {		// 结束位置，处理完结束
								break;
							}
							else if (strLine.Find(".locals") == 0)
							{
							}
							else {
								if (strLine.Find(":") == 0) {
								}
								else {
								}
							}
						}
					}

					// 处理Method数组
					cJavaMethod.strSuperClass = strSuperName;
					cJavaMethod.strClassName = strClassName;
					cJavaMethod.ProcessInstList(this, listMethodInst);

					listJavaMethods.push_back(cJavaMethod);
				}
				else if (strLine.Find(".end method") == 0) {
					printf("未成对处理的 method 结束\n");
				}
				else {
					printf("未处理的行 %s\n", strLine.GetBuffer());
					strLine.ReleaseBuffer();
				}
			}
		}
		cStdFile.Close();
	}
	return TRUE;
}
