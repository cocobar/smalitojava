#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>


static int TestCount = 0;

CString CJavaMethod::GetValueNameExpression(std::vector<CValueEqString> & listValueEqString, CString strValueName) {

	for (unsigned int i = 0; i < listValueEqString.size(); i++) {
		if (listValueEqString[i].strValueName.Compare(strValueName) == 0) {
			return listValueEqString[i].strExpression;
		}
	}
	return strValueName;
}

extern void ErrorNoInstructCode(CString strCode);


//SegmentListRetrodictGetRegInfo

// 向上查找输出寄存器，返回寄存器的类型，寄存器的full name
CString CJavaMethod::SegmentListRetrodictGetExpression(std::vector<int> listInstIndex, int nInsArryIndex, CString strRegName, int nInstIndex) {

	for (int i = nInsArryIndex; i > 0; i--) {
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(listInstIndex[i - 1]);
		if (it == listOriginalInstruct.end()) {
			printf("找不到序号 %4d 对应的指令\n", listInstIndex[i]);
			goto fail;
		}

		// 找到匹配的寄存器，然后需要决策这个是要输出变量还是表达式？
		if (it->regInfo.outReg.strRegName.Compare(strRegName) == 0) {
			return it->GetOutExpression();
		}
	}

	// 如果没有找到，那么就需要到 listInputParam 中找
	for (unsigned int i = 0; i < listInputParam.size(); i++) {
		if (listInputParam[i].strReg.Compare(strRegName) == 0) {
			return listInputParam[i].strValueName;
		}
	}
fail:

	printf("01没有找到这个寄存器的定义\n");
	{
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(listInstIndex[nInsArryIndex]);
		printf("%4d %s\n", listInstIndex[nInsArryIndex], it->strInstString);
	}
	while (1) {};
	return CString("Unknow!");
}

bool strCheckIsNumber(CString strNumber) {
	if ((strNumber.Find("0x") == 0) || (strNumber.Find("0X") == 0)) {
		return true;
	}
	else {
		// 10 进制
		for (int i = 0; i < strNumber.GetLength(); i++) {
			char _c = strNumber[i];
			if ((_c < '0') || (_c > '9')) {
				return false;
			}
		}
	}
	return true;
}

unsigned int strGetNumberData(CString strNumber) {
	strNumber.Trim();
	if ((strNumber.Find("0x") == 0) || (strNumber.Find("0X") == 0)) {
		return CJavaMethod::GetDataFromHex(strNumber);
	}
	else {
		// 10 进制
		unsigned int nData = 0;

		for (int i = 0; i < strNumber.GetLength(); i++) {
			char _c = strNumber[i];

			nData *= 10;
			if ((_c >= '0') && (_c <= '9')) {
				nData += (_c - '0');
			}
		}

		return nData;
	}
}


// 开始翻译指令
void CJavaMethod::SegmentCodeTranslate(int nLevel, std::vector<int> listInstIndex, std::vector<int> listNewInst) {
	unsigned int nStartIndex = listInstIndex.size();
	std::vector<int>::iterator itIndex = listInstIndex.insert(listInstIndex.end(), listNewInst.begin(), listNewInst.end());

	// 开始从上到下分析
	for (unsigned int i = nStartIndex; itIndex < listInstIndex.end(); itIndex++, i++) {
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct((*itIndex));
		if (it == listOriginalInstruct.end()) {
			printf("找不到序号 %4d 对应的指令\n", (*itIndex));
			return;
		}

		// 把指令解析出来
		CString strInstString = (*it).strInstString;
		int nInstIndex = (*itIndex);
		std::vector<CString> listI = CJavaInstructItem::GetListInstructionSymbol(strInstString);
		if (listI.size() > 0) {
			CString strCmd = listI[0];

			if (strCmd.Find("nop") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("move") == 0) {
				//  "move",					//move vx, vy	移动vy的内容到vx。两个寄存器都必须在最初的256寄存器范围以内。
				//	"move/from16",			//move/from16 vx, vy	移动vy的内容到vx。vy可能在64K寄存器范围以内，而vx则是在最初的256寄存器范围以内。
				//	"move/16",
				//	"move-wide",			//move-wide v4, v2   J-->J
				//	"move-wide/from16",		//move-wide/from16 vx, vy	移动一个long/double值，从vy到vx。vy可能在64K寄存器范围以内，而vx则是在最初的256寄存器范围以内。
				//	"move-wide/16",
				//	"move-object",			//move-object vx, vy	移动对象引用，从vy到vx。
				//	"move-object/from16",	//move-object/from16 vx, vy	移动对象引用，从vy到vx。vy可以处理64K寄存器地址，vx可以处理256寄存器地址。
				//	"move-object/16",

				//	"move-result",			//move-result vx	移动上一次方法调用的返回值到vx。
				//	"move-result-wide",		//move-result-wide vx	移动上一次方法调用的long/double型返回值到vx,vx+1。
				//	"move-result-object",	//move-result-object vx	移动上一次方法调用的对象引用返回值到vx。
				//	"move-exception",		//move-exception vx	当方法调用抛出异常时移动异常对象引用到vx。

				CString strOutputR0;  CString strInputR0;
				if (strCmd.Find("move-result") == 0) {
					strOutputR0 = listI[1];		strInputR0 = CString("r0");
				}
				else if (strCmd.Find("move-exception") == 0) {
					strOutputR0 = listI[1];		strInputR0 = CString("e0");
				}
				else {
					strOutputR0 = listI[1];		strInputR0 = listI[2];
				}
				CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
				it->strCppExpression = strExpression;

				if (it->GetOutputCanBeShow()) {
					it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
					this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
					this->TouchTheVariableItem(it->regInfo.outReg.strValName);
					this->TouchTheVariableItem(it->strCppExpression);
				}
				else {
					it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);
			}
			else if (strCmd.Find("return") == 0) {
				//"return-void",			//return-void	返回空值。
				//"return",					//return vx	返回在vx寄存器的值。
				//"return-wide",			//return-wide vx	返回在vx,vx+1寄存器的double/long值。
				//"return-object",			//return-object vx	返回在vx寄存器的对象引用。
				if (strCmd.Find("return-void") == 0) {
					// 无寄存器依赖
					if (this->strReturnType.Compare("void")) {
						printf("返回为 void 的类型不匹配\n");
						while (1) {};
					}

					// 这里可能还存在特殊算法
					it->bForceOutputCode = true;

					if (it->GetOutputCanBeShow()) {
						if (nLevel != 0) {
							it->strCppCodeOutput.Format("return;");
							this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						}
					}
					else {
						it->strCppCodeOutput.Format("//return");
					}
				}
				else {
					// 有寄存器
					if (listI.size() == 2) {
						CString strInputR0 = listI[1];
						CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
						it->strCppExpression = strExpression;

						it->bForceOutputCode = true;

						if (it->GetOutputCanBeShow()) {
							it->strCppCodeOutput.Format("return %s;", it->strCppExpression);
							this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
							this->TouchTheVariableItem(it->strCppExpression);
						}
						else {
							it->strCppCodeOutput.Format("//return %s", it->strCppExpression);
						}
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);
			}
			else if (strCmd.Find("const") == 0) {
				if (listI.size() == 3) {
					CString strType;
					if (strCmd.Find("const/4") == 0) {						//const/4 vx, lit4					存入4位常量到vx。
						strType = CString("char");
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const/16") == 0) {				//const/16 vx, lit16				存入16位常量到vx。
						strType = CString("short");
						it->strCppExpression = listI[2];

						if (strCheckIsNumber(it->strCppExpression)) {
							unsigned int nNumber = strGetNumberData(it->strCppExpression);
							int iNumber = 0;
							memcpy(&iNumber, &nNumber, sizeof(unsigned int));
							it->strCppExpression.Format("%d", iNumber);
						}
					}
					else if (strCmd.Find("const/high16") == 0) {			//const/high16 v0, lit16			存入16位常量到最高位寄存器，用于初始化float值。
						strType = CString("float");
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-wide/16") == 0) {			//const-wide/16 vx, lit16			存入int常量到vx, vx + 1寄存器，扩展int型常量为long常量。
						strType = CString("long");
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-wide/32") == 0) {			//const - wide / 32 vx, lit32		存入32位常量到vx, vx + 1寄存器，扩展int型常量到long常量。
						strType = CString("long");
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-wide") == 0) {				//const-wide vx, lit64				存入64位常量到vx, vx + 1寄存器
						strType = CString("long");
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-wide/high16") == 0) {		//const-wide/high16 vx, lit16		存入16位常量到最高16位的vx, vx + 1寄存器，用于初始化double 值。
						strType = CString("double");
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-string") == 0) {			//const-string vx, 字符串ID			存入字符串常量引用到vx，通过字符串ID或字符串。
						strType = CJavaClass::GetTypeFromJava(CString("Ljava/lang/String;"));
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-string/jumbo") == 0) {		//同上，字串ID的数值可以超过65535
						strType = CJavaClass::GetTypeFromJava(CString("Ljava/lang/String;"));
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-class") == 0) {				//const-class vx, 类型ID			存入类对象常量到vx，通过类型ID或类型（如Object.class）。
						strType = CJavaClass::GetTypeFromJava(listI[2]);
						it->strCppExpression = CString("Unknow!");
					}
					else if (strCmd.Find("const") == 0) {					//const vx, lit32					存入int 型常量到vx。
						strType = CString("int");
						it->strCppExpression = listI[2];

						if (strCheckIsNumber(it->strCppExpression)) {
							unsigned int nNumber = strGetNumberData(it->strCppExpression);
							int iNumber = 0;
							memcpy(&iNumber, &nNumber, sizeof(unsigned int));
							it->strCppExpression.Format("%d", iNumber);
						}
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}

				// 属于常量复制
				it->bConstValue = true;

				if (it->GetOutputCanBeShow()) {
					it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
					this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
					this->TouchTheVariableItem(it->regInfo.outReg.strValName);
					this->TouchTheVariableItem(it->strCppExpression);
				}
				else {
					it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);

			}
			else if (strCmd.Find("monitor-") == 0) {
				//printf("%s 指令未解析\n", strInstString);

				CString strInputR0 = listI[1];
				CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
				it->strCppExpression = strExpression + CString("->monitor()");
				it->bForceOutputCode = true;

			}
			else if (strCmd.Find("check-cast") == 0) {					//check-cast vx, 类型ID	检查vx寄存器中的对象引用是否可以转换成类型ID对应类型的实例。如不可转换，抛出ClassCastException异常，否则继续执行。
				if (listI.size() == 3) {

					CString strType = CJavaClass::GetTypeFromJava(listI[2]);
					CString strInputR0 = listI[1];
					CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					it->strCppExpression = strExpression + CString("->checkType(\"") + strType + CString("\")");
					it->bForceOutputCode = true;
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
			}
			else if (strCmd.Find("instance-of") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("array-length") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("new-") == 0) {

				//"new-instance",				//new-instance vx, 类型ID	根据类型ID或类型新建一个对象实例，并将新建的对象的引用存入vx。
				//"new-array",					//new-array vx, vy, 类型ID	根据类型ID或类型新建一个数组，vy存入数组的长度，vx存入数组的引用。
				if (strCmd.Find("new-instance") == 0)
				{
					if (listI.size() == 3) {
						CString strType = CJavaClass::GetTypeFromJava(listI[2]);
						it->strCppExpression = CString("new ") + strType;
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}

					// 被引用的计数强制设置为1
					it->regInfo.outReg.nOutregBeQuoteCount = 1;
				}
				else if (strCmd.Find("new-array") == 0) { // 使用vector的resize方法
					if (listI.size() == 4) {
						CString strOutputR0 = listI[1];		CString strInputR0 = listI[2];
						CString strType = CJavaClass::GetTypeFromJava(listI[3]);
						it->strCppExpression = strType + CString(".resize(") + strInputR0 + CString(")");
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
					it->bForceOutputCode = true;
				}

				it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);
			}
			else if (strCmd.Find("fill") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("throw") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("goto") == 0) {
				//printf("%s 指令未解析\n", strInstString);
				//"goto",			//goto 目标	通过短偏移量注2无条件跳转到目标。
				//"goto/16",		//goto/16目标	通过16位偏移量注2无条件跳转到目标。
				//"goto/32",		//goto/32目标	通过32位偏移量注2无条件跳转到目标。
			}
			else if ((strCmd.Find("packed-switch") == 0) || (strCmd.Find("sparse-switch") == 0)) {
				//packed-switch vx, 索引表偏移量	实现一个switch 语句，case常量是连续的。这个指令使用索引表，vx是在表中找到具体case的指令偏移量的索引，如果无法在表中找到vx对应的索引将继续执行下一个指令（即default case）。
				if (listI.size() == 3) {
					CString strInputR0 = listI[1];
					CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					it->strCppExpression = CString("switch (") + strExpression + CString(")");
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
				it->bForceOutputCode = true;
			}
			else if (strCmd.Find("cmp") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("if-") == 0) {
				//printf("%s 指令未解析\n", strInstString);
				//  "if-eq",	//if-eq vx,vy, 目标	如果vx == vy注2，跳转到目标。vx和vy是int型值。	
				//	"if-ne",	//if-ne vx,vy, 目标	如果vx != vy注2，跳转到目标。vx和vy是int型值。
				//	"if-lt",	//if-lt vx,vy, 目标	如果vx < vy注2，跳转到目标。vx和vy是int型值。
				//	"if-ge",	//if-ge vx, vy, 目标	如果vx >= vy注2，跳转到目标。vx和vy是int型值。
				//	"if-gt",	//if-gt vx,vy, 目标	如果vx > vy注2，跳转到目标。vx和vy是int型值。
				//	"if-le",	//if-le vx,vy, 目标	如果vx <= vy注2，跳转到目标。vx和vy是int型值。
				//	"if-eqz",	//if-eqz vx, 目标	如果vx == 0注2，跳转到目标。vx是int型值。
				//	"if-nez",	//if-nez vx, 目标	如果vx != 0注2，跳转到目标
				//	"if-ltz",	//if-ltz vx, 目标	如果vx < 0注2，跳转到目标。
				//	"if-gez",	//if-gez vx, 目标	如果vx >= 0注2，跳转到目标。
				//	"if-gtz",	//if-gtz vx, 目标	如果vx > 0注2，跳转到目标。
				//	"if-lez",	//if-lez vx, 目标	如果vx <= 0注2，跳转到目标。
				if (listI.size() == 4) {
					CString strInputR0 = listI[1];		CString strInputR1 = listI[2];

					CString strExpression0 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR1, nInstIndex);

					if (strCmd.Compare("if-eq") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("==") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("!=") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-ne") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("!=") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("==") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-lt") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("<") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString(">=") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-ge") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString(">=") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("<") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-gt") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString(">") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("<=") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-le") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("<=") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString(">") + strExpression1 + CString(")");
					}
				}
				else if (listI.size() == 3) {
					CString strInputR0 = listI[1];
					CString strExpression0 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression1 = CString("0");

					if (strCmd.Compare("if-eqz") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("==") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString(")");
					}
					else if (strCmd.Compare("if-nez") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("!=") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("==") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-ltz") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("<") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString(">=") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-gez") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString(">=") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("<") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-gtz") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString(">") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("<=") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-lez") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("<=") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString(">") + strExpression1 + CString(")");
					}
				}
				else {
					printf("%s 指令解析有错误 %d %s\n", strInstString, __LINE__, __FILE__);
				}

				it->bForceOutputCode = true;
			}
			else if (strCmd.Find("aget") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("aput") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("iget") == 0) {
				//printf("%s 指令未解析\n", strInstString);
				//"iget",				//iget vx, vy, 字段ID	根据字段ID读取实例的int型字段到vx，vy寄存器中是该实例的引用。
				//"iget-wide",			//iget-wide vx, vy, 字段ID	根据字段ID读取实例的double/long型字段到vx,vx+1注1，vy寄存器中是该实例的引用。
				//"iget-object",		//iget-object vx, vy, 字段ID	根据字段ID读取一个实例的对象引用字段到vx，vy寄存器中是该实例的引用。
				//"iget-boolean",		//iget-boolean vx, vy, 字段ID	根据字段ID读取实例的boolean型字段到vx，vy寄存器中是该实例的引用。
				//"iget-byte",			//iget-byte vx, vy, 字段ID	根据字段ID读取实例的byte型字段到vx，vy寄存器中是该实例的引用。
				//"iget-char",			//iget-char vx, vy, 字段ID	根据字段ID读取实例的char型字段到vx，vy寄存器中是该实例的引用。
				//"iget-short",			//iget-short vx, vy, 字段ID	根据字段ID读取实例的short型字段到vx，vy寄存器中是该实例的引用。
				if (listI.size() == 4) {
					CString strClassName = it->GetClassNameFromInstIput(listI[3]);
					CString strFieldName = it->GetFieldNameFromInstIput(listI[3]);
					CString strFieldType = it->GetFieldTypeFromInstIput(listI[3]);

					CString strInputR0 = listI[2];
					CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);

					if (strExpression == CString("this")) {
						it->strCppExpression = strFieldName;
					}
					else {
						it->strCppExpression = strExpression + CString(".") + strFieldName;
					}

				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}

				if (it->GetOutputCanBeShow()) {
					it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
					this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
					this->TouchTheVariableItem(it->regInfo.outReg.strValName);
					this->TouchTheVariableItem(it->strCppExpression);
				}
				else {
					it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);

			}
			else if (strCmd.Find("iput") == 0) {
				//"iput",				// iput vx, vy, 字段ID	根据字段ID将vx寄存器的值存入实例的int型字段，vy寄存器中是该实例的引用。
				//"iput-wide",			// iput-wide vx, vy, 字段ID	根据字段ID将vx,vx+1寄存器的值存入实例的double/long型字段，vy寄存器中是该实例的引用。
				//"iput-object",		// iput-object vx, vy, 字段ID	根据字段ID将vx寄存器的值存入实例的对象引用字段，vy寄存器中是该实例的引用。
				//"iput-boolean",		// iput-boolean vx, vy, 字段ID	根据字段ID将vx寄存器的值存入实例的boolean型字段，vy寄存器中是该实例的引用。
				//"iput-byte",			// iput-byte vx, vy, 字段ID	根据字段ID将vx寄存器的值存入实例的byte型字段，vy寄存器中是该实例的引用。
				//"iput-char",			// iput-char vx, vy, 字段ID	根据字段ID将vx寄存器的值存入实例的char型字段，vy寄存器中是该实例的引用。
				//"iput-short",			// iput-short vx, vy, 字段ID	根据字段ID将vx寄存器的值存入实例的short型字段，vy寄存器中是该实例的引用。
				//printf("%s 指令未解析\n", strInstString);
				if (listI.size() == 4) {
					CString strClassName = it->GetClassNameFromInstIput(listI[3]);
					CString strFieldName = it->GetFieldNameFromInstIput(listI[3]);
					CString strFieldType = it->GetFieldTypeFromInstIput(listI[3]);

					CString strInputR0 = listI[1];		CString strInputR1 = listI[2];

					CString strExpression0 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR1, nInstIndex);

					// 判断是不是bool类型
					if (strCmd.Compare("iput-boolean") == 0) {
						// 判断是不是数字，如果是数字，那么就转换成 true, false;
						if (strCheckIsNumber(strExpression0)) {
							unsigned int nNumber = strGetNumberData(strExpression0);

							if (nNumber > 0) {
								strExpression0 = CString("true");
							}
							else {
								strExpression0 = CString("false");
							}
						}
					}
					else if (strCmd.Compare("iput") == 0) {
						if (strCheckIsNumber(strExpression0)) {
							unsigned int nNumber = strGetNumberData(strExpression0);
							int iNumber = 0;
							memcpy(&iNumber, &nNumber, sizeof(unsigned int));
							strExpression0.Format("%d", iNumber);
						}
					}

					if (strExpression1.Compare("this") == 0) {
						if (this->strMethodName.Compare(CString("<init>")) == 0) {
							it->strCppExpression = strFieldName + CString(" = ") + strExpression0;
						}
						else {
							it->strCppExpression = strFieldName + CString(" = ") + strExpression0;
						}
					}
					else {
						it->strCppExpression = strExpression1 + CString(".") + strFieldName + CString(" = ") + strExpression0;
					}

					it->bForceOutputCode = true;
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}

				if (it->GetOutputCanBeShow()) {
					it->strCppCodeOutput.Format("%s;", it->strCppExpression);
					this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
				}
				else {
					it->strCppCodeOutput.Format("//%s", it->strCppExpression);
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);

			}
			else if (strCmd.Find("sget") == 0) {
				//  "sget",				//	sget vx, 字段ID	根据字段ID读取静态int型字段到vx。
				//	"sget-wide",		//  sget-wide vx, 字段ID	根据字段ID读取静态double/long型字段到vx,vx+1。
				//	"sget-object",		//  sget-object vx, 字段ID	根据字段ID读取静态对象引用字段到vx。
				//	"sget-boolean",		//  sget-boolean vx, 字段ID	根据字段ID读取静态boolean型字段到vx。
				//	"sget-byte",		//  sget-byte vx, 字段ID	根据字段ID读取静态byte型字段到vx。
				//	"sget-char",		//  sget-char vx, 字段ID	根据字段ID读取静态char型字段到vx。
				//	"sget-short",		//	sget-short vx, 字段ID	根据字段ID读取静态short型字段到vx。
				if (listI.size() == 3) {
					CString strClassName = it->GetClassNameFromInstIput(listI[2]);
					CString strFieldName = it->GetFieldNameFromInstIput(listI[2]);
					CString strFieldType = it->GetFieldTypeFromInstIput(listI[2]);

					it->strCppExpression = strClassName + CString("::") + strFieldName;
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
			}
			else if (strCmd.Find("sput") == 0) {
				//  "sput",				// sput vx, 字段ID	根据字段ID将vx寄存器中的值赋值到int型静态字段。
				//	"sput-wide",		// sput-wide vx, 字段ID	根据字段ID将vx,vx+1寄存器中的值赋值到double/long型静态字段。
				//	"sput-object",		// sput-object vx, 字段ID	根据字段ID将vx寄存器中的对象引用赋值到对象引用静态字段。
				//	"sput-boolean",		// sput-boolean vx, 字段ID	根据字段ID将vx寄存器中的值赋值到boolean型静态字段。
				//	"sput-byte",		// sput-byte vx, 字段ID	根据字段ID将vx寄存器中的值赋值到byte型静态字段。
				//	"sput-char",		// sput-char vx, 字段ID	根据字段ID将vx寄存器中的值赋值到char型静态字段。
				//	"sput-short",		// sput-short vx, 字段ID	根据字段ID将vx寄存器中的值赋值到short型静态字段。
				if (listI.size() == 3) {
					CString strClassName = it->GetClassNameFromInstIput(listI[2]);
					CString strFieldName = it->GetFieldNameFromInstIput(listI[2]);
					CString strFieldType = it->GetFieldTypeFromInstIput(listI[2]);

					CString strInputR0 = listI[1];
					CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					it->strCppExpression = strClassName + CString("::") + strFieldName + CString("=") + strExpression;
					it->bForceOutputCode = true;
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
			}
			else if (strCmd.Find("invoke-") == 0) {

				if (listI.size() == 3) {
					//"invoke-virtual/range",
					//"invoke-super/range",
					//"invoke-direct/range",
					//"invoke-static/range",
					//"invoke-interface/range",
					//"invoke-virtual",
					//"invoke-super",
					//"invoke-direct",
					//"invoke-static",
					//"invoke-interface",
					std::vector<CString> listReg;
					std::vector<CString>::iterator itReg;

					if (strInstString.Find("/range") > 0) {
						listReg = it->GetMethodCallRegListRange(listI[1]);
					}
					else {
						listReg = it->GetMethodCallRegList(listI[1]);
					}

					CString strClassName = it->GetClassNameFromInstInvoke(listI[2]);
					std::vector<CString> listParamType = it->GetSignatureFromInstInvoke(listI[2]);
					CString strReturnName = it->GetReturnTypeFromInstInvoke(listI[2]);
					CString strFunctionName = it->GetMethodNameFromInstInvoke(listI[2]);
					int nIndexParam = 0;

					CString strCppTotalExpression;

					if (strInstString.Find("invoke-static") == 0) {		// 这里缺一个参数

						strCppTotalExpression = strClassName + CString("::") + strFunctionName + CString("(");

						for (itReg = listReg.begin(), nIndexParam = 0; itReg != listReg.end(); itReg++, nIndexParam++) {
							if (it->GetTotalSizeOfParam(listParamType) == listReg.size()) {
								CString strTypeName = listParamType[nIndexParam];

								CString strInputR0 = (*itReg);
								CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);

								if (strTypeName.Compare("double") == 0) {
									itReg++;
								}
								else if (strTypeName.Compare("long") == 0) {
									itReg++;
								}

								if ((itReg + 1) == listReg.end()) {
									strCppTotalExpression += strExpression;
								}
								else {
									strCppTotalExpression += (strExpression + CString(","));
								}
							}
							else {
								printf("%s 指令解析有错误\n", strInstString);
							}
						}
					}
					else {
						for (itReg = listReg.begin(), nIndexParam = 0; itReg != listReg.end(); itReg++, nIndexParam++) {
							if (it->GetTotalSizeOfParam(listParamType) == (listReg.size() - 1)) {
								if (nIndexParam == 0) {

									CString strInputR0 = (*itReg);
									CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);

									if ((strFunctionName.Compare("<init>") == 0) && (strExpression.Find("new ") == 0)) {
										strCppTotalExpression = strExpression + CString("(");

										// 重新继承输出变量，当前寄存器作为变量
										CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
										it->regInfo.outReg.strValName = varPair.strValName;
										it->regInfo.outReg.strRegName = varPair.strRegName;

										// 强制设置多数引用
										//it->regInfo.outReg.nOutregBeQuoteCount = 100;
										it->regInfo.outReg.nOutregBeQuoteCount = varPair.nOutregBeQuoteCount - 1;
									}
									else {

										if (strExpression == CString("this")) {
											strCppTotalExpression = strFunctionName + CString("(");
										}
										else {
											strCppTotalExpression = strExpression + CString(".") + strFunctionName + CString("(");
										}
									}
								}
								else {
									CString strTypeName = listParamType[nIndexParam - 1];

									CString strInputR0 = (*itReg);
									CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);

									if (strTypeName.Compare("double") == 0) {
										itReg++;
									}
									else if (strTypeName.Compare("long") == 0) {
										itReg++;
									}
									if ((itReg + 1) == listReg.end()) {
										strCppTotalExpression += strExpression;
									}
									else {
										strCppTotalExpression += (strExpression + CString(","));
									}
								}
							}
							else {
								printf("%s 指令解析有错误\n", strInstString);
							}
						}
					}

					strCppTotalExpression += CString(")");

					it->strCppExpression = strCppTotalExpression;

					// 默认的返回变量
					strReturnName.Trim();
					if (!strReturnName.IsEmpty()) {	// 有返回值
						if (strReturnName.Compare("void") != 0) {
							CString strOutputR0 = CString("r0");
							it->SetOutputRegInfo(strOutputR0, strReturnName, "", "");

							// 没有人引用
							if (it->regInfo.outReg.listQuotePosition.size() == 0) {
								it->bForceOutputCode = true;
							}
						}
						else {
							it->bForceOutputCode = true;
						}
					}
					else {
						it->bForceOutputCode = true;
					}



					if (this->strMethodName.Compare("<init>") == 0) {
						if (strFunctionName.Compare("<init>") == 0) {
							// 还要判断Supper Class strClassName
							//if (this->strClassName.Compare())
							it->bForceOutputCode = false;
							it->bForceHideIt = true;
						}
					}

					if (it->GetOutputCanBeShow()) {
						if (it->regInfo.outReg.strValName.IsEmpty()) {
							it->strCppCodeOutput.Format("%s;", it->strCppExpression);
						}
						else {
							it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
							this->TouchTheVariableItem(it->regInfo.outReg.strValName);
							this->TouchTheVariableItem(it->strCppExpression);
						}
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
					}
					else {
						if (it->regInfo.outReg.strValName.IsEmpty()) {
							it->strCppCodeOutput.Format("//%s", it->strCppExpression);
						}
						else {
							it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
						}
					}

					printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
					printf("%s\n", it->strCppCodeOutput);

				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
			}
			else if (strCmd.Find("neg-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("not-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("int-to-") == 0) {
				//printf("%s 指令未解析\n", strInstString);
				//"int-to-long"				//int-to-long vx, vy	转换vy寄存器中的int型值为long型值存入vx,vx+1。
				//"int-to-float",			//int-to-float vx, vy	转换vy寄存器中的int型值为float型值存入vx。
				//"int-to-double",			//int-to-double vx, vy	转换vy寄存器中的int型值为double型值存入vx,vx+1。
				//"int-to-byte",			//int-to-byte vx, vy	转换vy寄存器中的int型值为byte型值存入vx。
				//"int-to-char",			//int-to-char vx, vy	转换vy寄存器中的int型值为char型值存入vx。
				//"int-to-short",			//int-to-short vx, vy	转换vy寄存器中的int型值为short型值存入vx。

				CString strOutputR0;  CString strInputR0;

				strOutputR0 = listI[1];		strInputR0 = listI[2];

				CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);

				if (listI.size() == 3) {
					if (listI[0].Compare("int-to-long") == 0) {
						it->strCppExpression.Format("((long)%s)", strExpression1);
					}
					else if (listI[0].Compare("int-to-float") == 0) {
						it->strCppExpression.Format("((float)%s)", strExpression1);
					}
					else if (listI[0].Compare("int-to-double") == 0) {
						it->strCppExpression.Format("((double)%s)", strExpression1);
					}
					else if (listI[0].Compare("int-to-byte") == 0) {
						it->strCppExpression.Format("((byte)%s)", strExpression1);
					}
					else if (listI[0].Compare("int-to-char") == 0) {
						it->strCppExpression.Format("((char)%s)", strExpression1);
					}
					else if (listI[0].Compare("int-to-short") == 0) {
						it->strCppExpression.Format("((short)%s)", strExpression1);
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}

				if (it->GetOutputCanBeShow()) {
					it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
					this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
					this->TouchTheVariableItem(it->regInfo.outReg.strValName);
					this->TouchTheVariableItem(it->strCppExpression);
				}
				else {
					it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);

			}
			else if (strCmd.Find("long-to-") == 0) {
				//printf("%s 指令未解析\n", strInstString);
				//  "long-to-int",			//long-to-int vx, vy	转换vy,vy+1寄存器中的long型值为int型值存入vx。
				//	"long-to-float",		//long-to-float vx, vy	转换vy,vy+1寄存器中的long型值为float型值存入vx。
				//	"long-to-double",		//long-to-double vx, vy	转换vy,vy+1寄存器中的long型值为double型值存入vx,vx+1。

				CString strOutputR0;  CString strInputR0;

				strOutputR0 = listI[1];		strInputR0 = listI[2];

				CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);

				if (listI.size() == 3) {
					if (listI[0].Compare("long-to-int") == 0) {
						it->strCppExpression.Format("((int)%s)", strExpression1);
					}
					else if (listI[0].Compare("long-to-float") == 0) {
						it->strCppExpression.Format("((float)%s)", strExpression1);
					}
					else if (listI[0].Compare("long-to-double") == 0) {
						it->strCppExpression.Format("((double)%s)", strExpression1);
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}

				if (it->GetOutputCanBeShow()) {
					it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
					this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
					this->TouchTheVariableItem(it->regInfo.outReg.strValName);
					this->TouchTheVariableItem(it->strCppExpression);
				}
				else {
					it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);

			}
			else if (strCmd.Find("float-to-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("double-to-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("add-") == 0) {
				//printf("%s 指令未解析\n", strInstString);
				//  "add-int",				//add-int vx, vy, vz	计算vy + vz并将结果存入vx。
				//  "add-long",				//add-long vx, vy, vz	计算vy,vy+1 + vz,vz+1并将结果存入vx,vx+1注1。
				//	"add-float",			//add-long vx, vy, vz	计算vy,vy+1 + vz,vz+1并将结果存入vx,vx+1注1。
				//	"add-double",			//add-double vx, vy, vz	计算vy,vy+1 + vz,vz+1并将结果存入vx,vx+1注1。
				//	"add-int/2addr",		//add-int/2addr vx, vy	计算vx + vy并将结果存入vx。
				//	"add-long/2addr",		//add-long/2addr vx, vy	计算vx,vx+1 + vy,vy+1并将结果存入vx,vx+1注1。
				//	"add-float/2addr",		//add-float/2addr vx, vy	计算vx + vy并将结果存入vx。
				//	"add-double/2addr",		//add-double/2addr vx, vy	计算vx,vx+1 + vy,vy+1并将结果存入vx,vx+1注1。
				//	"add-int/lit16",		//add-int/lit16 vx, vy, lit16	计算vy + lit16并将结果存入vx。
				//	"add-int/lit8",			//add-int/lit8 vx, vy, lit8	计算vy + lit8并将结果存入vx
				if ((listI.size() == 3)) {

					CString strOutputR0;  CString strInputR0;

					strOutputR0 = listI[1];		strInputR0 = listI[2];

					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression2 = SegmentListRetrodictGetExpression(listInstIndex, i, strOutputR0, nInstIndex);

					it->strCppExpression.Format("(%s + %s)", strExpression2, strExpression1);

					if (it->GetOutputCanBeShow()) {
						it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						this->TouchTheVariableItem(it->regInfo.outReg.strValName);
						this->TouchTheVariableItem(it->strCppExpression);
					}
					else {
						it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
					}
#if 0
					if (listI[0].Compare("add-int/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-long/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-float/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-double/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
#endif
				}
				else if (listI.size() == 4) {

					// add-int/lit16 vx, vy, lit16	计算vy + lit16并将结果存入vx。
					// "add-float",			//add-long vx, vy, vz	计算vy,vy+1 + vz,vz+1并将结果存入vx,vx+1注1。

					CString strOutputR0;  CString strInputR0; CString strInputR1;
					strOutputR0 = listI[1];		strInputR0 = listI[2];	strInputR1 = listI[3];

					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression2;
					if ((listI[0].Compare("add-int/lit16") == 0) || (listI[0].Compare("add-int/lit8") == 0)) {
						strExpression2 = strInputR0;
					}
					else {
						strExpression2 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR1, nInstIndex);
					}

					it->strCppExpression.Format("(%s + %s)", strExpression2, strExpression1);

					if (it->GetOutputCanBeShow()) {
						it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						this->TouchTheVariableItem(it->regInfo.outReg.strValName);
						this->TouchTheVariableItem(it->strCppExpression);
					}
					else {
						it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
					}
#if 0
					if (listI[0].Compare("add-int") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-long") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-float") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-int/lit16") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-int/lit8") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
#endif
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
			}
			else if (strCmd.Find("sub-") == 0) {
				//printf("%s 指令未解析\n", strInstString);
				//  "sub-int",				//sub-int vx, vy, vz	计算vy - vz并将结果存入vx。
				//	"sub-long",				//sub-long vx, vy, vz	计算vy,vy+1 - vz,vz+1并将结果存入vx,vx+1注1。
				//	"sub-float",			//sub-float vx, vy, vz	计算vy - vz并将结果存入vx。
				//	"sub-double",			//sub-double vx, vy, vz	计算vy,vy+1 - vz,vz+1并将结果存入vx,vx+1注1。
				//	"sub-int/2addr",		//sub-int/2addr vx, vy	计算vx - vy并将结果存入vx。
				//	"sub-long/2addr",		//sub-long/2addr vx, vy	计算vx,vx+1 - vy,vy+1并将结果存入vx,vx+1注1。
				//	"sub-float/2addr",		//sub-float/2addr vx, vy	计算vx - vy并将结果存入vx。
				//	"sub-double/2addr",		//sub-double/2addr vx, vy	计算vx,vx+1 - vy,vy+1并将结果存入vx,vx+1注1。
				if ((listI.size() == 3)) {

					CString strOutputR0;  CString strInputR0;

					strOutputR0 = listI[1];		strInputR0 = listI[2];

					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression2 = SegmentListRetrodictGetExpression(listInstIndex, i, strOutputR0, nInstIndex);

					it->strCppExpression.Format("(%s - %s)", strExpression2, strExpression1);

					if (it->GetOutputCanBeShow()) {
						it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						this->TouchTheVariableItem(it->regInfo.outReg.strValName);
						this->TouchTheVariableItem(it->strCppExpression);
					}
					else {
						it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
					}

#if 0
					if (listI[0].Compare("sub-int/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("sub-long/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("sub-float/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("sub-double/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
#endif
				}
				else if (listI.size() == 4) {

					CString strOutputR0;  CString strInputR0;

					strOutputR0 = listI[2];		strInputR0 = listI[3];

					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression2 = SegmentListRetrodictGetExpression(listInstIndex, i, strOutputR0, nInstIndex);

					it->strCppExpression.Format("(%s - %s)", strExpression2, strExpression1);

					if (it->GetOutputCanBeShow()) {
						it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						this->TouchTheVariableItem(it->regInfo.outReg.strValName);
						this->TouchTheVariableItem(it->strCppExpression);
					}
					else {
						it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
					}
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}

			}
			else if (strCmd.Find("mul-") == 0) {
				//printf("%s 指令未解析\n", strInstString);
				//  "mul-int",				// mul-int vx, vy, vz	计算vy * vz并将结果存入vx。
				//	"mul-long",				// mul-long vx, vy, vz	计算vy,vy+1 * vz,vz+1并将结果存入vx,vx+1注1。
				//	"mul-float",			// mul-float vx, vy, vz	计算vy * vz并将结果存入vx。
				//	"mul-double",			// mul-double vx, vy, vz	计算vy,vy+1 * vz,vz+1并将结果存入vx,vx+1注1。
				//	"mul-int/2addr"			// mul-int/2addr vx, vy	计算vx * vy并将结果存入vx。
				//	"mul-long/2addr",		// mul-long/2addr vx, vy	计算vx,vx+1 * vy,vy+1并将结果存入vx,vx+1注1。
				//	"mul-float/2addr",		// mul-float/2addr vx, vy	计算vx * vy并将结果存入vx。
				//	"mul-double/2addr",		// mul-double/2addr vx, vy	计算vx,vx+1 * vy,vy+1并将结果存入vx,vx+1注1。
				//	"mul-int/lit16",		// mul-int/lit16 vx, vy, lit16	计算vy * lit16并将结果存入vx。
				//	"mul-int/lit8",			// mul-int/lit8 vx, vy, lit8	计算vy * lit8并将结果存入vx。
				if ((listI.size() == 3)) {

					CString strOutputR0;  CString strInputR0;

					strOutputR0 = listI[1];		strInputR0 = listI[2];

					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression2 = SegmentListRetrodictGetExpression(listInstIndex, i, strOutputR0, nInstIndex);

					it->strCppExpression.Format("(%s * %s)", strExpression2, strExpression2, strExpression1);

					if (it->GetOutputCanBeShow()) {
						it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						this->TouchTheVariableItem(it->regInfo.outReg.strValName);
						this->TouchTheVariableItem(it->strCppExpression);
					}
					else {
						it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
					}

					printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
					printf("%s\n", it->strCppCodeOutput);
				}
				else if (listI.size() == 4) {
					if (listI[0].Compare("mul-int") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("mul-long") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("mul-float") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("mul-int/lit16") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("mul-int/lit8") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
			}
			else if (strCmd.Find("div-") == 0) {
				//printf("%s 指令未解析\n", strInstString);
				//  "div-int",					//div-int vx, vy, vz	计算vy / vz并将结果存入vx。
				//	"div-long",					//div-long vx, vy, vz	计算vy,vy+1 / vz,vz+1并将结果存入vx,vx+1注1。
				//	"div-float",				//div-float vx, vy, vz	计算vy / vz并将结果存入vx。
				//	"div-double",				//div-double vx, vy, vz	计算vy,vy+1 / vz,vz+1并将结果存入vx,vx+1注1。
				//	"div-int/2addr",			//div-int/2addr vx, vy	计算vx / vy并将结果存入vx。
				//	"div-long/2addr",			//div-long/2addr vx, vy	计算vx,vx+1 / vy,vy+1并将结果存入vx,vx+1注1。
				//	"div-float/2addr",			//div-float/2addr vx, vy	计算vx / vy并将结果存入vx。
				//	"div-double/2addr",			//div-double/2addr vx, vy	计算vx,vx+1 / vy,vy+1并将结果存入vx,vx+1注1。
				//	"div-int/lit16",			//div-int/lit16 vx, vy, lit16	计算vy / lit16并将结果存入vx。
				//	"div-int/lit8",				//div-int/lit8 vx, vy, lit8	计算vy / lit8并将结果存入vx。
				if ((listI.size() == 3)) {

					CString strOutputR0;  CString strInputR0;

					strOutputR0 = listI[1];		strInputR0 = listI[2];

					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression2 = SegmentListRetrodictGetExpression(listInstIndex, i, strOutputR0, nInstIndex);

					it->strCppExpression.Format("(%s / %s)", strExpression2, strExpression2, strExpression1);

					if (it->GetOutputCanBeShow()) {
						it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						this->TouchTheVariableItem(it->regInfo.outReg.strValName);
						this->TouchTheVariableItem(it->strCppExpression);
					}
					else {
						it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
					}
#if 0
					if (listI[0].Compare("div-int/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-long/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-float/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-double/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
#endif
				}
				else if (listI.size() == 4) {
					if (listI[0].Compare("div-int") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-long") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-float") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-int/lit16") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-int/lit8") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
			}
			else if (strCmd.Find("rem-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("and-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("or-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("xor-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("shl-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("shr-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("ushr-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("rsub-int") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("+") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("^breakpoint") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("^throw-verification-error") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("try") == 0) {
				// 虚拟指令不需要翻译
				it->strCppExpression = CString("try");
				it->bForceOutputCode = true;
			}
			else if (strCmd.Find("catch") == 0) {
				//e寄存器
				it->strCppExpression = CString("catch");
				it->bForceOutputCode = true;
			}
			else {
				printf("%s 指令解析有错误\n", strInstString);
				while (1) {};
			}
		}
		else {
			printf("%s 指令解析有错误\n", strInstString);
		}
	}

	return;
}