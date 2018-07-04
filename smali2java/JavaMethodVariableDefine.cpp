#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>

// 向上查找输出寄存器，返回寄存器的类型，寄存器的full name
CRegVariablePair CJavaMethod::SegmentListRetrodictGetRegInfo(std::vector<int> listInstIndex, int nInsArryIndex, CString strRegName, int nInstIndex) {

	CRegVariablePair varPair;

	for (int i = nInsArryIndex; i > 0; i--) {
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(listInstIndex[i - 1]);
		if (it == listOriginalInstruct.end()) {
			printf("找不到序号 %4d 对应的指令\n", listInstIndex[i]);
			goto fail;
		}

		if (it->regInfo.outReg.strRegName.Compare(strRegName) == 0) {

			CString strPos;
			strPos.Format("%d-%s", nInstIndex, strRegName);
			it->regInfo.outReg.listQuotePosition.push_back(strPos);

			// 去重
			sort(it->regInfo.outReg.listQuotePosition.begin(), it->regInfo.outReg.listQuotePosition.end());
			std::vector<CString>::iterator iter = std::unique(it->regInfo.outReg.listQuotePosition.begin(), it->regInfo.outReg.listQuotePosition.end());
			it->regInfo.outReg.listQuotePosition.erase(iter, it->regInfo.outReg.listQuotePosition.end());

			return it->regInfo.outReg;
		}
	}

	// 如果没有找到，那么就需要到 listInputParam 中找
	for (unsigned int i = 0; i < listInputParam.size(); i++) {
		if (listInputParam[i].strReg.Compare(strRegName) == 0) {
			varPair.strRegName = listInputParam[i].strReg;
			varPair.strStrongType = listInputParam[i].strTypeName;
			varPair.strValName = listInputParam[i].strValueName;
			varPair.belongInstIndex = -1;
			varPair.listQuotedInstIndex.clear();
			return varPair;
		}
	}
fail:

	printf("02没有找到这个寄存器的定义\n");
	{
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(listInstIndex[nInsArryIndex]);
		printf("%4d %s\n", listInstIndex[nInsArryIndex], it->strInstString);
	}
	while (1) {};
	return varPair;
}

void ErrorNoInstructCode(CString strCode) {
	printf("未处理的指令 %s\n", strCode);
	while (1) {};
}

bool CJavaMethod::SegmentListInstSpread(std::vector<int> listSegIndex)
{
	// 获取指令清单
	std::vector<int> listInstIndex;
	listInstIndex.clear();
	for (unsigned int i = 0; i < listSegIndex.size(); i++) {
		for (unsigned int j = 0; j < this->listMethodSegment[listSegIndex[i]].listInstSn.size(); j++) {
			listInstIndex.push_back(this->listMethodSegment[listSegIndex[i]].listInstSn[j]);
		}
	}
	for (unsigned int i = 0; i < listInstIndex.size(); i++) {
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(listInstIndex[i]);
		if (it == listOriginalInstruct.end()) {
			printf("找不到序号 %4d 对应的指令\n", listInstIndex[i]);
			return false;
		}

		// 把指令解析出来
		CString strInstString = (*it).strInstString;
		int nInstIndex = listInstIndex[i];

		if (it->regInfo.listIreg.size() > 0) {
			for (unsigned int j = 0; j < it->regInfo.listIreg.size(); j++) {
				CString strInputR0 = it->regInfo.listIreg[j].strRegName;
				CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
				if (varPair.strValName.IsEmpty()) {
					printf("严重错误，这个变量的名字还没有定义\n");
					while (1) {};
				}
				else {
					if (it->regInfo.listIreg[j].strValName.IsEmpty()) {
						it->regInfo.listIreg[j].strValName = varPair.strValName;
					}
					else {
						if (it->regInfo.listIreg[j].strValName.Compare(varPair.strValName) != 0) {

							printf("寄存器名字 %s\n", it->regInfo.listIreg[j].strRegName);
							printf("已经命名 %s\n", it->regInfo.listIreg[j].strValName);
							printf("需要命名 %s\n", varPair.strValName);

							printf("%4d %s\n", it->nSerialNumber, it->strInstString);
							printf("严重错误，一个变量有多个名字\n");
							while (1) {};
						}
					}
				}
			}
		}
	}

	return true;
}

// 立刻重组整个指令
bool CJavaMethod::SegmentListInstRebuild(std::vector<int> listSegIndex) {

	// 获取指令清单
	std::vector<int> listInstIndex;
	listInstIndex.clear();
	for (unsigned int i = 0; i < listSegIndex.size(); i++) {
		for (unsigned int j = 0; j < this->listMethodSegment[listSegIndex[i]].listInstSn.size(); j++) {
			listInstIndex.push_back(this->listMethodSegment[listSegIndex[i]].listInstSn[j]);
		}
	}

	// 开始从上到下分析
	for (unsigned int i = 0; i < listInstIndex.size(); i++) {
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(listInstIndex[i]);
		if (it == listOriginalInstruct.end()) {
			printf("找不到序号 %4d 对应的指令\n", listInstIndex[i]);
			return false;
		}

		// 把指令解析出来
		CString strInstString = (*it).strInstString;
		int nInstIndex = listInstIndex[i];
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
				CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
				it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
				it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);
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
				}
				else {
					// 有寄存器
					if (listI.size() == 2) {
						CString strInputR0 = listI[1];
						CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
						it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

						// 更新一次强类型
						it->CheckInputRegStrongType(strInputR0, this->strReturnType);
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
				}
			}
			else if (strCmd.Find("const") == 0) {
				if (listI.size() == 3) {
					CString strType;
					if (strCmd.Find("const/4") == 0) {						//const/4 vx, lit4					存入4位常量到vx。
						strType = CString("char");
					}
					else if (strCmd.Find("const/16") == 0) {				//const/16 vx, lit16				存入16位常量到vx。
						strType = CString("short");
					}
					else if (strCmd.Find("const/high16") == 0) {			//const/high16 v0, lit16			存入16位常量到最高位寄存器，用于初始化float值。
						strType = CString("float");
					}
					else if (strCmd.Find("const-wide/16") == 0) {			//const-wide/16 vx, lit16			存入int常量到vx, vx + 1寄存器，扩展int型常量为long常量。
						strType = CString("long");
					}
					else if (strCmd.Find("const-wide/32") == 0) {			//const - wide / 32 vx, lit32		存入32位常量到vx, vx + 1寄存器，扩展int型常量到long常量。
						strType = CString("long");
					}
					else if (strCmd.Find("const-wide") == 0) {				//const-wide vx, lit64				存入64位常量到vx, vx + 1寄存器
						strType = CString("long");
					}
					else if (strCmd.Find("const-wide/high16") == 0) {		//const-wide/high16 vx, lit16		存入16位常量到最高16位的vx, vx + 1寄存器，用于初始化double 值。
						strType = CString("double");
					}
					else if (strCmd.Find("const-string") == 0) {			//const-string vx, 字符串ID			存入字符串常量引用到vx，通过字符串ID或字符串。
						strType = CJavaClass::GetTypeFromJava(CString("Ljava/lang/String;"));
					}
					else if (strCmd.Find("const-string/jumbo") == 0) {		//同上，字串ID的数值可以超过65535
						strType = CJavaClass::GetTypeFromJava(CString("Ljava/lang/String;"));
					}
					else if (strCmd.Find("const-class") == 0) {				//const-class vx, 类型ID			存入类对象常量到vx，通过类型ID或类型（如Object.class）。
						strType = CJavaClass::GetTypeFromJava(listI[2]);
					}
					else if (strCmd.Find("const") == 0) {					//const vx, lit32					存入int 型常量到vx。
						strType = CString("int");
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}

					CString strOutputR0 = listI[1];
					it->SetOutputRegInfo(strOutputR0, "", strType, "");
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
			}
			else if (strCmd.Find("monitor-") == 0) {
				//printf("%s 指令未解析\n", strInstString);
				CString strInputR0 = listI[1];
				CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
				it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
			}
			else if (strCmd.Find("check-cast") == 0) {					//check-cast vx, 类型ID	检查vx寄存器中的对象引用是否可以转换成类型ID对应类型的实例。如不可转换，抛出ClassCastException异常，否则继续执行。
				if (listI.size() == 3) {
					CString strInputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

					CString strType = CJavaClass::GetTypeFromJava(listI[2]);
					it->CheckInputRegStrongType(strInputR0, strType);
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
						CString strOutputR0 = listI[1];
						CString strType = CJavaClass::GetTypeFromJava(listI[2]);
						it->SetOutputRegInfo(strOutputR0, strType, "", "");
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
				}
				else if (strCmd.Find("new-array") == 0) {
					if (listI.size() == 4) {
						CString strOutputR0 = listI[1];		CString strInputR0 = listI[2];
						CString strType = CJavaClass::GetTypeFromJava(listI[3]);

						CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
						it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
						it->SetOutputRegInfo(strOutputR0, strType, "", "");
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
				}
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
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
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
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
				}
				else if (listI.size() == 3) {
					CString strInputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
				}
				else {
					printf("%s 指令解析有错误 %d %s\n", strInstString, __LINE__, __FILE__);
				}
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

					CString strOutputR0 = listI[1];		CString strInputR0 = listI[2];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, strFieldType, "", "");
					it->CheckInputRegStrongType(strInputR0, strClassName);
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
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
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

					it->CheckInputRegStrongType(strInputR0, strFieldType);
					it->CheckInputRegStrongType(strInputR1, strClassName);
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
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

					CString strOutputR0 = listI[1];
					it->SetOutputRegInfo(strOutputR0, strFieldType, "", "");
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
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->CheckInputRegStrongType(strInputR0, strFieldType);
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
					int nIndexParam = 0;

					if (strInstString.Find("invoke-static") == 0) {		// 这里缺一个参数
						for (itReg = listReg.begin(), nIndexParam = 0; itReg != listReg.end(); itReg++, nIndexParam++) {
							if (it->GetTotalSizeOfParam(listParamType) == listReg.size()) {
								CString strTypeName = listParamType[nIndexParam];

								CString strInputR0 = (*itReg);
								CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
								it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

								it->CheckInputRegStrongType(strInputR0, strTypeName);


								if (strTypeName.Compare("double") == 0) {
									itReg++;
								}
								else if (strTypeName.Compare("long") == 0) {
									itReg++;
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
									CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
									it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
								}
								else {
									CString strTypeName = listParamType[nIndexParam - 1];

									CString strInputR0 = (*itReg);
									CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
									it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
									it->CheckInputRegStrongType(strInputR0, strTypeName);


									if (strTypeName.Compare("double") == 0) {
										itReg++;
									}
									else if (strTypeName.Compare("long") == 0) {
										itReg++;
									}
								}
							}
							else {
								printf("%s 指令解析有错误\n", strInstString);
							}
						}
					}

					// 默认的返回变量
					strReturnName.Trim();
					if (!strReturnName.IsEmpty()) {
						if (strReturnName.Compare("void") != 0) {
							CString strOutputR0 = CString("r0");
							it->SetOutputRegInfo(strOutputR0, strReturnName, "", "");
						}
					}
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
				if (listI.size() == 3) {
					if (listI[0].Compare("int-to-long") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("int-to-float") == 0) {
						CString strInputR0 = listI[1];
						CString strOutputR0 = listI[1];
						CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
						it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

						it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);

						it->CheckInputRegStrongType(strInputR0, CString("int"));
					}
					else if (listI[0].Compare("int-to-double") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("int-to-byte") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("int-to-char") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("int-to-short") == 0) {
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
			else if (strCmd.Find("long-to-") == 0) {
				//printf("%s 指令未解析\n", strInstString);
				//  "long-to-int",			//long-to-int vx, vy	转换vy,vy+1寄存器中的long型值为int型值存入vx。
				//	"long-to-float",		//long-to-float vx, vy	转换vy,vy+1寄存器中的long型值为float型值存入vx。
				//	"long-to-double",		//long-to-double vx, vy	转换vy,vy+1寄存器中的long型值为double型值存入vx,vx+1。
				if (listI.size() == 3) {
					CString strInputR0 = listI[1];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);
					it->CheckInputRegStrongType(strInputR0, CString("long"));
				}
				else {
					printf("%s 指令解析有错误\n", strInstString);
				}
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

					CString strInputR0 = listI[1];		CString strInputR1 = listI[2];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);

					if (listI[0].Compare("add-int/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("int"));
						it->CheckInputRegStrongType(strInputR1, CString("int"));
					}
					else if (listI[0].Compare("add-long/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("long"));
						it->CheckInputRegStrongType(strInputR1, CString("long"));
					}
					else if (listI[0].Compare("add-float/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("float"));
						it->CheckInputRegStrongType(strInputR1, CString("float"));
					}
					else if (listI[0].Compare("add-double/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("double"));
						it->CheckInputRegStrongType(strInputR1, CString("double"));

					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
				}
				else if (listI.size() == 4) {

					CString strInputR0 = listI[2];		CString strInputR1 = listI[3];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);

					if (listI[0].Compare("add-int") == 0) {
						varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
						it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
						it->CheckInputRegStrongType(strInputR0, CString("int"));
						it->CheckInputRegStrongType(strInputR1, CString("int"));
					}
					else if (listI[0].Compare("add-long") == 0) {
						varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
						it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
						it->CheckInputRegStrongType(strInputR0, CString("long"));
						it->CheckInputRegStrongType(strInputR1, CString("long"));
					}
					else if (listI[0].Compare("add-float") == 0) {
						varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
						it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

						it->CheckInputRegStrongType(strInputR0, CString("float"));
						it->CheckInputRegStrongType(strInputR1, CString("float"));
					}
					else if (listI[0].Compare("add-int/lit16") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("short"));
					}
					else if (listI[0].Compare("add-int/lit8") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("char"));
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
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

					CString strInputR0 = listI[1];		CString strInputR1 = listI[2];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);

					if (listI[0].Compare("sub-int/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("int"));
						it->CheckInputRegStrongType(strInputR1, CString("int"));
					}
					else if (listI[0].Compare("sub-long/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("long"));
						it->CheckInputRegStrongType(strInputR1, CString("long"));
					}
					else if (listI[0].Compare("sub-float/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("float"));
						it->CheckInputRegStrongType(strInputR1, CString("float"));
					}
					else if (listI[0].Compare("sub-double/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("double"));
						it->CheckInputRegStrongType(strInputR1, CString("double"));
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
				}
				else if (listI.size() == 4) {

					CString strInputR0 = listI[2];		CString strInputR1 = listI[3];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);
					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

					if (listI[0].Compare("sub-int") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("int"));
						it->CheckInputRegStrongType(strInputR1, CString("int"));
					}
					else if (listI[0].Compare("sub-long") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("long"));
						it->CheckInputRegStrongType(strInputR1, CString("long"));
					}
					else if (listI[0].Compare("sub-float") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("float"));
						it->CheckInputRegStrongType(strInputR1, CString("float"));
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
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

					CString strInputR0 = listI[1];		CString strInputR1 = listI[2];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);

					if (listI[0].Compare("mul-int/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("int"));
						it->CheckInputRegStrongType(strInputR1, CString("int"));
					}
					else if (listI[0].Compare("mul-long/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("long"));
						it->CheckInputRegStrongType(strInputR1, CString("long"));
					}
					else if (listI[0].Compare("mul-float/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("float"));
						it->CheckInputRegStrongType(strInputR1, CString("float"));
					}
					else if (listI[0].Compare("mul-double/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("double"));
						it->CheckInputRegStrongType(strInputR1, CString("double"));
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
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

					CString strInputR0 = listI[1];		CString strInputR1 = listI[2];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);

					if (listI[0].Compare("div-int/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("int"));
						it->CheckInputRegStrongType(strInputR1, CString("int"));
					}
					else if (listI[0].Compare("div-long/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("long"));
						it->CheckInputRegStrongType(strInputR1, CString("long"));
					}
					else if (listI[0].Compare("div-float/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("float"));
						it->CheckInputRegStrongType(strInputR1, CString("float"));
					}
					else if (listI[0].Compare("div-double/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("double"));
						it->CheckInputRegStrongType(strInputR1, CString("double"));
					}
					else {
						printf("%s 指令解析有错误\n", strInstString);
					}
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
			}
			else if (strCmd.Find("catch") == 0) {
				//e寄存器
				CString strOutputR0 = listI[1];
				it->SetOutputRegInfo(strOutputR0, "", listI[2], "");
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

	return true;
}

bool CJavaMethod::SegmentVariableRecursion(std::vector<int> listSegIndex, int nIndex) {

	if (find(listSegIndex.begin(), listSegIndex.end(), nIndex) != listSegIndex.end()) {
		// 这里是判断进入了循环
		// 这个是最后一个序号：listSegIndex[listSegIndex.size() - 1];
		// nIndex 这个是循环开始
		CLoopPair loopPair;
		loopPair.nLoopStart = nIndex;
		loopPair.nLoopEnd = listSegIndex[listSegIndex.size() - 1];

		printf("循环节 %d --> %d\n", loopPair.nLoopStart, loopPair.nLoopEnd);

		// 下次开始调试
		while (1) {};
		return true;
	}

	// 没有出现循环就添加进去
	listSegIndex.push_back(nIndex);

	switch (listMethodSegment[nIndex].SegmentNextType) {
	case CJavaMethodSegment::nextNormal:		// 通过顺延过去
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("next->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			SegmentVariableRecursion(listSegIndex, SSI);
		}
		else {
			printf("12无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextGoto:			// 通过跳转过去
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("goto->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			SegmentVariableRecursion(listSegIndex, SSI);
		}
		else {
			printf("13无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextIf:			// 通过if过去, 不一定是if过去的，可能是 while for 循环
	{
		CString strIfyLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("if-y->"));
		CString strIfnLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("if-n->"));

		int SSI = GetSegmentIndexFromName(strIfyLabel);
		if (SSI >= 0) {
			SegmentVariableRecursion(listSegIndex, SSI);
		}
		else {
			printf("14无法找到函数中的起始位置\n");
		}
		SSI = GetSegmentIndexFromName(strIfnLabel);
		if (SSI >= 0) {
			SegmentVariableRecursion(listSegIndex, SSI);
		}
		else {
			printf("41无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextSwitch:		// 通过switch结构过去
	{

		CString strSwitchLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("switch->"));
		CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("default->"));

		// 遍历 Switch 分支
		std::vector<CJavaPackedSwitchStruct>::iterator itpss;
		for (itpss = listPackedSwitch.begin(); itpss != listPackedSwitch.end(); itpss++) {
			std::vector<CJavaSwitchItem>::iterator itsit;
			if (strSwitchLabel.Compare((*itpss).strSwitchName) == 0) {
				for (itsit = (*itpss).listItem.begin(); itsit != (*itpss).listItem.end(); itsit++) {
					int SSI = GetSegmentIndexFromName((*itsit).strSegmentName);
					if (SSI >= 0) {
						SegmentVariableRecursion(listSegIndex, SSI);
					}
					else {
						printf("16无法找到函数中的起始位置\n");
					}
				}
				break;
			}
		}

		// default 分支
		int SSI = GetSegmentIndexFromName(strDefaultLabel);
		if (SSI >= 0) {
			SegmentVariableRecursion(listSegIndex, SSI);
		}
		else {
			printf("17无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextReturn:		// 通过return返回的段
	{
		return SegmentListInstRebuild(listSegIndex);
	}
	break;
	case CJavaMethodSegment::nextTryCatch:		// 通过TryCatch分支
	{
		CString strTryLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("try->"));
		CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("default->"));

		std::vector<CTryCatchNode>::iterator ittcn;
		for (ittcn = listCatchData.begin(); ittcn != listCatchData.end(); ittcn++) {
			std::vector<CCatchItemNode>::iterator itcit;
			if (strTryLabel.Compare((*ittcn).strName) == 0) { // 找到对应的Try结构体

															  // try分支
				int SsiStart = GetSegmentIndexFromName((*ittcn).strStartLabel);
				if (SsiStart >= 0) {
					SegmentVariableRecursion(listSegIndex, SsiStart);
				}
				else {
					printf("181无法找到函数中的起始位置\n");
				}


				// catch分支
				for (itcit = (*ittcn).listCatch.begin(); itcit != (*ittcn).listCatch.end(); itcit++) {
					int SSI = GetSegmentIndexFromName((*itcit).strCatchLabel);
					if (SSI >= 0) {
						SegmentVariableRecursion(listSegIndex, SSI);
					}
					else {
						printf("18无法找到函数中的起始位置\n");
					}
				}
				// final分支
				if (!(*ittcn).strFinallyLabel.IsEmpty()) {
					int SSI = GetSegmentIndexFromName((*ittcn).strFinallyLabel);
					if (SSI >= 0) {
						SegmentVariableRecursion(listSegIndex, SSI);
					}
					else {
						printf("19无法找到函数中的起始位置\n");
					}
				}

				break;
			}
		}

		// default 分支
		int SSI = GetSegmentIndexFromName(strDefaultLabel);
		if (SSI >= 0) {
			SegmentVariableRecursion(listSegIndex, SSI);
		}
		else {
			printf("20无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextThrow:			// 这个也是结束代码
	{
		// 调用到尾巴上
		return SegmentListInstRebuild(listSegIndex);
	}
	break;
	default:
	{
		printf("没有使用过的类型\n");
	}
	}

	return true;
}

CRegVariablePair CJavaMethod::GetRegVariablePair(CString strRegInfo) {

	int nFindDot = strRegInfo.Find("-");

	if (nFindDot > 0) {
		CString strInstIndex = strRegInfo.Left(nFindDot).Trim();
		CString strRegName = strRegInfo.Right(strRegInfo.GetLength() - nFindDot - 1).Trim();

		int nOutInst = atoi(strInstIndex);
		std::vector<CJavaInstructItem>::iterator itInst = GetOriginalInstruct(nOutInst);
		if (itInst == this->listOriginalInstruct.end()) {
			printf("找不到的指令\n");
			while (1) {};
		}

		for (unsigned int i = 0; i < itInst->regInfo.listIreg.size(); i++) {
			if (itInst->regInfo.listIreg[i].strRegName == strRegName) {
				return itInst->regInfo.listIreg[i];
			}
		}
		printf("从输入寄存器列表上找不到的指令\n");
		while (1) {};
	}
	else {
		int nOutInst = atoi(strRegInfo);
		std::vector<CJavaInstructItem>::iterator itInst = GetOriginalInstruct(nOutInst);
		if (itInst == this->listOriginalInstruct.end()) {
			printf("找不到的指令\n");
			while (1) {};
		}
		return itInst->regInfo.outReg;
	}
}

// 通过递归方式反演出所有寄存器可能的类型
void CJavaMethod::SegmentVariableDefine(void) {

	std::vector<int> listSegIndex;

	// 下面是扫描完所有变量的依赖关系，还有类型
	listSegIndex.clear();
	int SSI = GetSegmentIndexFromName(CString("main"));
	if (SSI >= 0) {
		SegmentVariableRecursion(listSegIndex, SSI);
	}
	else {
		printf("21无法找到函数中的起始位置\n");
	}


	// 所有输入变量都有 依赖关系
	// 所有输出变量都有输入类型
	// 1、扫描所有指令的输入寄存器列表，按输入寄存器的引用列表分组
	printf("显示寄存器引用关系， 表示这条指令的输入寄存器需要引自哪条指令\n");
	for (unsigned int i = 0; i < this->listOriginalInstruct.size(); i++) {
		printf("%4d --> ", this->listOriginalInstruct[i].nSerialNumber);
		for (unsigned int j = 0; j < this->listOriginalInstruct[i].regInfo.listIreg.size(); j++) {
			printf("%s(%s):", this->listOriginalInstruct[i].regInfo.listIreg[j].strRegName, this->listOriginalInstruct[i].regInfo.listIreg[j].strGetVarType());
			for (unsigned int k = 0; k < this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex.size(); k++) {
				printf("%d,", this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex[k]);
			}
			if (this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex.size() == 0) {
				printf("这个指令的输入寄存器找不到出处？\n");
				while (1) {};
			}

			printf("; ");
		}
		printf("\n");
	}

	// 输出寄存器分组
	std::vector<std::vector<int>> listArrayOutregType;
	listArrayOutregType.clear();

	// 把所有输出寄存器合并
	for (unsigned int i = 0; i < this->listOriginalInstruct.size(); i++) {
		if (this->listOriginalInstruct[i].regInfo.listIreg.size() > 0) {	// 有数据寄存器，那么就有资格对输出寄存器分组
			for (unsigned int j = 0; j < this->listOriginalInstruct[i].regInfo.listIreg.size(); j++) {
				// this->listOriginalInstruct[i].regInfo.listIreg[j].strRegName 这个是寄存器名称
				// this->listOriginalInstruct[i].regInfo.listIreg[j].listRefedInstIndex 这个是引用寄存器列表，这个里面肯定属于同一类

				bool bFindOverlap = false;
				for (unsigned int m = 0; m < listArrayOutregType.size(); m++) {
					for (unsigned int k = 0; k < this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex.size(); k++) {
						if (find(listArrayOutregType[m].begin(), listArrayOutregType[m].end(), this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex[k]) != listArrayOutregType[m].end()) {
							listArrayOutregType[m].insert(listArrayOutregType[m].begin(),
								this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex.begin(),
								this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex.end());
							bFindOverlap = true;
							break;
						}
					}
					if (bFindOverlap) {
						break;
					}
				}
				// 没有找到就新增一个
				if (!bFindOverlap) {
					listArrayOutregType.push_back(this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex);
				}
			}
		}
	}

	// 排序消重
	printf("显示应该定义了新变量的位置，同一行表示这个变量为同一组\n");
	for (unsigned int m = 0; m < listArrayOutregType.size(); m++) {
		// 排序去重
		sort(listArrayOutregType[m].begin(), listArrayOutregType[m].end());
		std::vector<int>::iterator iter = std::unique(listArrayOutregType[m].begin(), listArrayOutregType[m].end());
		listArrayOutregType[m].erase(iter, listArrayOutregType[m].end());
#if 1
		for (unsigned int s = 0; s < listArrayOutregType[m].size(); s++) {
			printf("%d, ", listArrayOutregType[m][s]);
		}
		printf("\n");
#endif
	}

	// 对变量进行赋值
	int nVarIndexStart = 1;
	for (unsigned int m = 0; m < listArrayOutregType.size(); m++) {

		if (listArrayOutregType[m].size() > 0) {
			if (listArrayOutregType[m][0] < 0) {
				// 这里使用函数的输入变量，不需要重新定义，因为函数的是p寄存器和通用寄存器都没有关系
			}
			else {

				CString strValueName;

				strValueName.Format("var%d", nVarIndexStart++);

				// 统计一个变量被引用了多少次
				int nValueBerefCount = 0;
				// outReg.listQuotePosition 这个里面保存的是输出寄存器被哪些输入指令引用过

				// 这个变量多少次作为输出使用？ 也就是被赋值多少次？
				int nValueOutputCount = 0;

				// 对一组相同的输出寄存器进行
				for (unsigned int s = 0; s < listArrayOutregType[m].size(); s++) {
					std::vector<CJavaInstructItem>::iterator itInst = GetOriginalInstruct(listArrayOutregType[m][s]);
					if (itInst == this->listOriginalInstruct.end()) {
						printf("找不到的指令\n");
						while (1) {};
					}
					nValueBerefCount += itInst->regInfo.outReg.listQuotePosition.size();
					nValueOutputCount++;
				}

				// 对所有输出寄存器上都赋值
				if (listArrayOutregType[m].size() > 0) {

					CString strType;

					for (unsigned int s = 0; s < listArrayOutregType[m].size(); s++) {
						std::vector<CJavaInstructItem>::iterator itInst = GetOriginalInstruct(listArrayOutregType[m][s]);
						if (itInst == this->listOriginalInstruct.end()) {
							printf("找不到的指令\n");
							while (1) {};
						}
						// 变量赋值
						itInst->regInfo.outReg.strValName = strValueName;
						itInst->regInfo.outReg.nOutregBeQuoteCount = nValueBerefCount;
						itInst->regInfo.outReg.nOutregAssignedCount = nValueOutputCount;

						if (strType.IsEmpty()) {
							strType = itInst->regInfo.outReg.strGetVarType();
						}
					}

					// 放入将变量放入全局列表
					CVariableItem cVariableItem;
					cVariableItem.clear();
					cVariableItem.strName = strValueName;
					cVariableItem.strTypeName = strType;
					listVariableItem.push_back(cVariableItem);
				}
			}
		}
		else {
			printf("严重错误\n");
			while (1) {};
		}
	}

	// 打印出所有变量的名字
	// 检测是否所有变量都有名字
	printf("输出所有指令的输出寄存器的 寄存器名 变量名 被引用的次数\n");
	for (unsigned int i = 0; i < this->listOriginalInstruct.size(); i++) {

		if ((this->listOriginalInstruct[i].regInfo.outReg.listQuotePosition.size() != 0) && (this->listOriginalInstruct[i].regInfo.outReg.nOutregBeQuoteCount != 0)) {
			printf("%4d %s %s(%s) used %d {",
				this->listOriginalInstruct[i].nSerialNumber,
				this->listOriginalInstruct[i].regInfo.outReg.strRegName,
				this->listOriginalInstruct[i].regInfo.outReg.strValName,
				this->listOriginalInstruct[i].regInfo.outReg.strGetVarType(),
				this->listOriginalInstruct[i].regInfo.outReg.nOutregBeQuoteCount);

			for (unsigned int j = 0; j < this->listOriginalInstruct[i].regInfo.outReg.listQuotePosition.size(); j++) {
				printf("%s, ", this->listOriginalInstruct[i].regInfo.outReg.listQuotePosition[j]);
			}
			printf("}\n");
		}
		else {
			printf("%4d %s\n", this->listOriginalInstruct[i].nSerialNumber, this->listOriginalInstruct[i].regInfo.outReg.strRegName);
		}
	}

	printf("输出同组类型寄存器的 输出 输入 寄存器的类型；这里显示的每个横向的组肯定是同一个类型的\n");
	printf("xx-xx 这个是在指令的输入寄存器上，另外一个在输出寄存器上，[强制类型]，(弱类型)\n");
	for (unsigned int m = 0; m < listArrayOutregType.size(); m++) {
		// 排序去重
		sort(listArrayOutregType[m].begin(), listArrayOutregType[m].end());
		std::vector<int>::iterator iter = std::unique(listArrayOutregType[m].begin(), listArrayOutregType[m].end());
		listArrayOutregType[m].erase(iter, listArrayOutregType[m].end());

		std::vector<CString> listSameTypeRegInfo;
		listSameTypeRegInfo.clear();

		for (unsigned int s = 0; s < listArrayOutregType[m].size(); s++) {
			//printf("%d, ", listArrayOutregType[m][s]);
			if (listArrayOutregType[m][s] >= 0) {	// 这个是输出寄存器同名的组
				int nCurrentInstIndex = listArrayOutregType[m][s];

				CString strInstIndex;
				strInstIndex.Format("%d", nCurrentInstIndex);
				listSameTypeRegInfo.push_back(strInstIndex);

				std::vector<CJavaInstructItem>::iterator itInst = GetOriginalInstruct(nCurrentInstIndex);
				if (itInst == this->listOriginalInstruct.end()) {
					printf("找不到的指令\n");
					while (1) {};
				}
				for (unsigned int j = 0; j < itInst->regInfo.outReg.listQuotePosition.size(); j++) {
					listSameTypeRegInfo.push_back(itInst->regInfo.outReg.listQuotePosition[j]);
				}

			}
		}

		// 下面显示出来强制类型，然后我们来决策一下到底用什么类型，决定完成之后，要把所有寄存器的强制类型修复成功
		if (listSameTypeRegInfo.size() > 0) {
			for (unsigned int k = 0; k < listSameTypeRegInfo.size(); k++) {

				CString strLabel = listSameTypeRegInfo[k];
				CRegVariablePair tmpRegInfo;
				if (strLabel.Find(CString("-")) > 0) {
					tmpRegInfo = GetRegVariablePair(strLabel);
					printf("%s[%s](", strLabel, tmpRegInfo.strStrongType);
					for (unsigned int d = 0; d < tmpRegInfo.listWeakType.size(); d++) {
						printf("%s,", tmpRegInfo.listWeakType[d]);
					}
					printf(");");
				}
				else {
					tmpRegInfo = GetRegVariablePair(strLabel);
					tmpRegInfo = GetRegVariablePair(strLabel);
					printf("%s[%s](", strLabel, tmpRegInfo.strStrongType);
					for (unsigned int d = 0; d < tmpRegInfo.listWeakType.size(); d++) {
						printf("%s,", tmpRegInfo.listWeakType[d]);
					}
					printf(");");
				}
			}
			printf("\n");
		}
	}

}
